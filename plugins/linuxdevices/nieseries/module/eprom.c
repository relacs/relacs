/*
* EEProm and Serial DAC Example 1
*
* Read/Write EEProm and Write Serial DAC
*
* *** CAUTION *** Writing to the EEProm can overwrite values which
* were written at the factory
*
* This example shows reading and writing to E-series EEProm
* and writing to CALDACs.
* This example is written for all E-series devices
* The write cycle for 8-bit DAC and 12 bit DAC is illustrated in \
* chapter 5 in Register Level Programming Manual under title
* "Calibration DACs".
*/

#include "core.h"
#include "eprom.h"


#ifdef NIDAQ_EPROMDEBUG
#define DPRINT( x ) printk x
#else
#define DPRINT( x )
#endif


#define noError                 1000
#define invalidValueError       1001

static bp cdev;
#define Board_Write_8bit(a,x) Board_Write_Byte(cdev,a,x)
#define Board_Read8(a) Board_Read_Byte(cdev, a)

static uint8  serialCommandRegCopy;

/*
static void testee(void)
{
  int16 i, value;
  if(cdev->type&NI_PCI)return;
  printk("TESTEEPROM\n");
  for(i=0; i<500; i+=10){
    eEpromRead(i, &value);
    printk("%d: %d\n",i, value);
  }
}
*/

static void eepr2dac(uint32 epraddr, uint32 dac, uint32 calclass)
{
  uint16 value;
  eEpromRead(epraddr, &value);
  serialDacWrite(calclass, dac, value);
#ifdef DAQ_DEBUG  
  printk( "read %d from %d\n", value, epraddr );
#endif
}
static void eepr2dac2(uint32 epraddrm, uint32 epraddrl, uint32 dac, uint32 calclass)
{
  uint16 valuem, valuel;
  eEpromRead(epraddrm, &valuem);
  eEpromRead(epraddrl, &valuel);
  serialDacWrite(calclass, dac, (valuem<<8)|valuel);
  // printk("read %d from %d\n",value, epraddr);
}

int loadcalibration(bp dev)
{
  cdev=dev;
  serialCommandRegCopy=0;

  /* read AI & AO calibration constants from EEProm and write to DACs*/

  /* the following will load factory cal constants on E-4 */
  switch(dev->type & 0xFFFF){
  case 0x6011: /* MI0-XE-50 */
    eepr2dac2(436,435, 8, kXe50);
    eepr2dac(434, 2, kXe50);
    eepr2dac(433, 0, kXe50);
    eepr2dac(432, 1, kXe50);
    eepr2dac(426, 6, kXe50);
    eepr2dac(425, 4, kXe50);
    eepr2dac(424, 7, kXe50);
    eepr2dac(423, 5, kXe50);
    break;
  case 0x6030: case 0x6031: case 0x6032: case 0x6033: /* XE-10 */
    eepr2dac2(429,428, 8, kXe50);
    eepr2dac(427, 2, kXe50);
    eepr2dac(426, 3, kXe50);
    eepr2dac(425, 0, kXe50);
    eepr2dac(424, 1, kXe50);
    eepr2dac(417, 6, kXe50);
    eepr2dac(416, 4, kXe50);
    eepr2dac(415, 7, kXe50);
    eepr2dac(414, 5, kXe50);
    break;
  case 0x6020: case 0x6021: /* AT-MIO-16{D}E-10 */
    eepr2dac(422, 4, kf2Eseries);
    eepr2dac(421,11, kf2Eseries);
    eepr2dac(420, 1, kf2Eseries);
    eepr2dac(419, 3, kf2Eseries);
    eepr2dac(418, 2, kf2Eseries);
    eepr2dac(417, 5, kf2Eseries);
    eepr2dac(416, 7, kf2Eseries);
    eepr2dac(415, 6, kf2Eseries);
    eepr2dac(414, 8, kf2Eseries);
    eepr2dac(413,10, kf2Eseries);
    eepr2dac(412, 9, kf2Eseries);
    break;
  default:
    //testee();
    eepr2dac(424, 4, kf2Eseries);
    eepr2dac(423, 1, kf2Eseries);
    eepr2dac(422, 3, kf2Eseries);
    eepr2dac(421, 2, kf2Eseries);
    eepr2dac(420, 5, kf2Eseries);
    eepr2dac(419, 7, kf2Eseries);
    eepr2dac(418, 6, kf2Eseries);
    eepr2dac(417, 8, kf2Eseries);
    eepr2dac(416,10, kf2Eseries);
    eepr2dac(415, 9, kf2Eseries);
  }
  return 0;
}

/*********************************************************************

serial DAC Write

*********************************************************************/
int16 serialDacWrite (uint32 calibrationClass, uint32 dacNumber, uint16 dacValue)
{
   int16 status = noError;

   switch (calibrationClass){
      case kXe50:
         status = sDacWriteXe50((int16) dacNumber, (uint16) dacValue);
         break;
      case k6110:
         status = sDacWrite6110((int16) dacNumber, (uint16) dacValue);
         break;
      case kf2Eseries:
         status = sDacWritef2Eseries((int16) dacNumber, (uint16) dacValue);
         break;
      case k8804:
         status = sDacWrite8804((int16) dacNumber, (uint16) dacValue);
         break;
   }

   return status;
}

/*********************************************************************

EEPROM Write

*********************************************************************/
int16 eEpromWrite (uint32 eeprom_addr, uint16 value)
{
   int16 status = noError;
   uint16   command_modifier=0;

   /* For the 512 word memory, you need to modify READ and WRITE commands
    * so that MSB in the lower nibble indicates MSB of the address;
    * e.g., if address is 0x0113, then this bit should be 1. */
   if (eeprom_addr > 0xFF)
      command_modifier = 0x08;

   toggle_chip_select();
   data_out_msb_first(kEECommandLength, kEEWriteEnableCommand);
   toggle_chip_select();
   data_out_msb_first(kEECommandLength, (kEEWriteCommand | command_modifier));
   data_out_msb_first(kEEAddressLength, (uint16)eeprom_addr);
   data_out_msb_first(kEEDataLength, value);
   remove_chip_select();

   check_eeprom_busy();

   return status;
}
/**********************************************************************

EEPROM Read

**********************************************************************/
int16 eEpromRead (uint32 eeprom_addr, uint16 *value)
{
   int16 status = noError;
   uint16   command_modifier=0;

   /* For the 512 word memory, you need to modify READ and WRITE commands
    * so that MSB in the lower nibble indicates MSB of the address;
    * e.g., if address is 0x0113, then this bit should be 1. */
   if (eeprom_addr > 0xFF)
      command_modifier = 0x08;

   toggle_chip_select();
   data_out_msb_first(kEECommandLength, (kEEReadCommand | command_modifier));
   data_out_msb_first(kEEAddressLength, (uint16)eeprom_addr);
   get_bits(kEEDataLength, value);
   remove_chip_select();

   return status;
}

/*********************************************************************

EEPROM Unprotect

*********************************************************************/
int16 eEpromUnProtect ()
{
   int16 status = noError;

   // write to eeprom to unprotect region
   // currently unprotects entire eeprom
   toggle_chip_select();
   data_out_msb_first(kEECommandLength, kEEWriteEnableCommand);
   toggle_chip_select();
   data_out_msb_first(kEECommandLength, kEEWriteStatusCommand);
   data_out_msb_first(kEEDataLength, kEEWriteProtectOff);
   remove_chip_select();
   check_eeprom_busy();

   return status;
}

/*********************************************************************

EEPROM Protect

*********************************************************************/
int16 eEpromProtect (uint32 value)
{
   int16 status = noError;
   uint16 protect;

   // write to eeprom to protect region
   switch (value){
         case 0:
            protect = kEEWriteProtectAll;
            break;
         case 256:
            protect = kEEWriteProtect1_2;
            break;
         case 384:
            protect = kEEWriteProtect1_4;
            break;
         default:
	   protect=0;
	   status = invalidValueError;
   }

   if (status == noError){
      toggle_chip_select();
      data_out_msb_first(kEECommandLength, kEEWriteEnableCommand);
      toggle_chip_select();
      data_out_msb_first(kEECommandLength, kEEWriteStatusCommand);
      data_out_msb_first(kEEDataLength, protect);
      remove_chip_select();
      check_eeprom_busy();
   }

   return status;
}

/*********************************************************************

Serial DAC Write for everything not specifically covered below.

*********************************************************************/
int16 sDacWritef2Eseries(int16 dac_number, uint16 value)
{
   int16 status = noError;

   remove_chip_select();
   data_out_lsb_first(kSerialDacIdLength, dac_number);
   data_out_msb_first(kSerialDacDataLength, value);
   strobe_serial_dac(kSerialDacLd0);

   /**************************************************************************8
   Code added for new CALDAC that replaced the old Fujitsu part. The new Analog
   Devices 8804 CalDAC does not exactly match the old part.  When writing to DAC
   3 one must also write to DAC 14.  When writing to DAC 7 one must
   also write to DAC 13 and when writing to DAC 11 one must also write
   to DAC 0 in order for the new part to function like the old part.
   *******************************************************************/

   if ((dac_number == 3)||(dac_number == 7)||(dac_number == 11)) {
      remove_chip_select();
      switch(dac_number){
         case 3:
            data_out_lsb_first(kSerialDacIdLength, 14);
            break;
         case 7:
            data_out_lsb_first(kSerialDacIdLength, 13);
            break;
         case 11:
            data_out_lsb_first(kSerialDacIdLength, 0);
            break;
      }
      data_out_msb_first(kSerialDacDataLength, value);
      strobe_serial_dac(kSerialDacLd0);
   }

   return status;
}

/*********************************************************************

Serial DAC Write for XE-50 and XE-10

*********************************************************************/
int16 sDacWriteXe50(int16 dac_number, uint16 value)
{
   int16 status = noError;
   int16            length=12;
   uint16            bitMask;
   int16            index;

   if(dac_number < 8) /* using 8-bit 8800 dacs */
   {
      value &= 0x00FF;
      value |= dac_number << 8;
      length = 11;
   }

   /* if dac_number == 8 value is unchanged (using 8043 dac) and length=12 */

   if(dac_number > 8) /* handling XE-10 caldacs */
   {
      value &= 0x0FFF;                  // Make sure control bits are clear
      value |= ((dac_number-8) << 13);  // select DAC - dac_number is 9 or 10
      value |= 0x8000;                  // software decode
      length   = 16;
   }

   /* Write out CalDac value. */
   bitMask = 1 << (length-1);

   /* Initially Clock is low. */
   /* Clear Clock and calDAC load signals.   */
   serialCommandRegCopy &= ~(kSerialClock | kSerialDacLd0 | kSerialDacLd1 | kSerialDacLd2);
   Board_Write_8bit(kSerialCommandReg, serialCommandRegCopy);

   for(index=length-1;index>=0;index--)
   {
      if (value & bitMask)
         serialCommandRegCopy |= kSerialData;
      else
         serialCommandRegCopy &= ~kSerialData;
      Board_Write_8bit(kSerialCommandReg, serialCommandRegCopy);

      serialCommandRegCopy |= kSerialClock;
      Board_Write_8bit(kSerialCommandReg, serialCommandRegCopy);
      Board_Write_8bit(kSerialCommandReg, serialCommandRegCopy);

      serialCommandRegCopy &= ~kSerialClock;
      Board_Write_8bit(kSerialCommandReg, serialCommandRegCopy);
      Board_Write_8bit(kSerialCommandReg, serialCommandRegCopy);

      bitMask >>=1;
   }

   /* Send appropriate load signal to update caldac. */
   if (dac_number < 8)
   {
      strobe_serial_dac(kSerialDacLd0);
   }
   else if (dac_number > 8)
   {
      strobe_serial_dac(kSerialDacLd2);
   }
   else
   {
      strobe_serial_dac(kSerialDacLd1);
   }

   return status;
}

/*********************************************************************

Serial DAC Write for 6110

*********************************************************************/
int16 sDacWrite6110(int16 dac_number, uint16 value)
{
   int16 status = noError;
   int16            length;
   uint16            bitMask;
   int16            index;


   value &= 0x00FF;
   value |= ((dac_number%8)+1) << 8;
   length = 12;

   /* Write out CalDac value. */
   bitMask = 1 << (length-1);

   /* Initially Clock is low. */
   /* Clear Clock and calDAC load signals.   */
   serialCommandRegCopy &= ~(kSerialClock | kSerialDacLd0 | kSerialDacLd1 | kSerialDacLd2);
   Board_Write_8bit(kSerialCommandReg, serialCommandRegCopy);

   for(index=length-1;index>=0;index--)
   {
      if (value & bitMask)
         serialCommandRegCopy |= kSerialData;
      else
         serialCommandRegCopy &= ~kSerialData;
      Board_Write_8bit(kSerialCommandReg, serialCommandRegCopy);

      serialCommandRegCopy |= kSerialClock;
      Board_Write_8bit(kSerialCommandReg, serialCommandRegCopy);
      Board_Write_8bit(kSerialCommandReg, serialCommandRegCopy);

      serialCommandRegCopy &= ~kSerialClock;
      Board_Write_8bit(kSerialCommandReg, serialCommandRegCopy);
      Board_Write_8bit(kSerialCommandReg, serialCommandRegCopy);

      bitMask >>=1;
   }

   /* Send appropriate load signal to update caldac. */

   if (dac_number < 8)
      strobe_serial_dac(kSerialDacLd0);
   else
      strobe_serial_dac(kSerialDacLd1);

   return status;
}

/*********************************************************************

Serial DAC Write for 8804 DACs on 67xx boards

*********************************************************************/
int16 sDacWrite8804(int16 dac_number, uint16 value)
{
   int16 status = noError;

   remove_chip_select();

   // DAC 24, 25 address used for analog trigger DACs on 6052E
   if(dac_number > 23){
      value &= 0x0FFF;                   // Make sure control bits are clear
      value |= ((dac_number-23) << 13);  // select DAC - dac_number is 9 or 10
      value |= 0x8000;                   // software decode
      data_out_msb_first(kSerialDacDataLength16x, value);
      strobe_serial_dac(kSerialDacLd2);
   }
   else{
      data_out_msb_first(kSerialDacIdLength, (dac_number%12));
      data_out_msb_first(kSerialDacDataLength, value);
      if (dac_number < 12)
         strobe_serial_dac(kSerialDacLd0);
      else
         strobe_serial_dac(kSerialDacLd1);
   }
   return status;
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

EEPROM send bits utility functions

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
void data_out_msb_first(uint16 number_of_bits, uint16 the_bits)
{
   uint16            this_bit;
   int16            index;

   for (index=number_of_bits-1; index>=0; index--)
   {
      serialCommandRegCopy &= ~kSerialClock;
      Board_Write_8bit(kSerialCommandReg, serialCommandRegCopy);
      Board_Write_8bit(kSerialCommandReg, serialCommandRegCopy);

      this_bit = (the_bits >> index) & 0x01;

      if (this_bit)
         serialCommandRegCopy |= kSerialData;
      else
         serialCommandRegCopy &= ~kSerialData;

      Board_Write_8bit(kSerialCommandReg, serialCommandRegCopy);

      serialCommandRegCopy |= kSerialClock;
      Board_Write_8bit(kSerialCommandReg, serialCommandRegCopy);
      Board_Write_8bit(kSerialCommandReg, serialCommandRegCopy);
   }
}

void data_out_lsb_first(uint16 number_of_bits, uint16 the_bits)
{
   uint16             this_bit;
   uint16             index;

   for (index=0; index<=number_of_bits-1; index++)
   {
      serialCommandRegCopy &= ~kSerialClock;
      Board_Write_8bit(kSerialCommandReg, serialCommandRegCopy);
      Board_Write_8bit(kSerialCommandReg, serialCommandRegCopy);

      this_bit = (the_bits >> index) & 0x01;

      if (this_bit)
         serialCommandRegCopy |= kSerialData;
      else
         serialCommandRegCopy &= ~kSerialData;

      Board_Write_8bit(kSerialCommandReg, serialCommandRegCopy);

      serialCommandRegCopy |= kSerialClock;
      Board_Write_8bit(kSerialCommandReg, serialCommandRegCopy);
      Board_Write_8bit(kSerialCommandReg, serialCommandRegCopy);
   }
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

Strobe the serial DAC utility function

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
void strobe_serial_dac(uint8 value)
{
   serialCommandRegCopy &= ~value;
   Board_Write_8bit(kSerialCommandReg, serialCommandRegCopy);

   serialCommandRegCopy |= value;
   Board_Write_8bit(kSerialCommandReg, serialCommandRegCopy);

   serialCommandRegCopy &= ~value;
   Board_Write_8bit(kSerialCommandReg, serialCommandRegCopy);
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

EEPROM deactivate chip select line utility function

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
void remove_chip_select ()
{
   serialCommandRegCopy &= ~kEpromChipSel;
   Board_Write_8bit(kSerialCommandReg, serialCommandRegCopy);
}

void check_eeprom_busy()
{
   uint16   write_in_progress=1;
   uint16   time_out;

    /** The EEPROM write time is up to 10ms.
    ** Byte access on the AT bus is at least 500ns.
    ** 20000 * 500ns = 10ms.
    ** 60000 * 300ns for PCI
    ** Write In Progress (WIP) is bit 0 in the EEPROM
    ** status register.  It is 1 while write is in progress,
    ** and 0 at other times.
    **
    ** Therefore, the following wait loop is correct. **/

   time_out = 60000;

   while( (0 < time_out) && (write_in_progress == 1))
   {
      read_status_register(&write_in_progress);
      write_in_progress &= 0x01;
      time_out--;
   }
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

EEPROM toggle chip select line utility function

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
void toggle_chip_select ()
{
   serialCommandRegCopy &= ~kEpromChipSel;
   Board_Write_8bit(kSerialCommandReg, serialCommandRegCopy);

   serialCommandRegCopy |= kEpromChipSel;
   Board_Write_8bit(kSerialCommandReg, serialCommandRegCopy);
}

/**********************************************************************

EEPROM Read status register

**********************************************************************/
void read_status_register(uint16* value)
{
   toggle_chip_select();
   data_out_msb_first(kEECommandLength, kEEReadStatusCommand);
   get_bits(kEEDataLength, value);
   remove_chip_select();
}

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

EEPROM get bits utility function

*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
void get_bits(uint16 number_of_bits,  uint16* the_bits)
{
   uint16            this_bit;
   int16            index;

   /* Clk is high, and it is ready to start   */
   /* outputing data when the clk goes low  */

   *the_bits = 0;

   for (index = number_of_bits-1; index >= 0; index--)
   {
      serialCommandRegCopy &= ~kSerialClock;
      Board_Write_8bit(kSerialCommandReg, serialCommandRegCopy);
      Board_Write_8bit(kSerialCommandReg, serialCommandRegCopy);

      this_bit = Board_Read8(kSerialStatusReg);
      this_bit &= kEpromDataOut;

      *the_bits |= this_bit << index;

      serialCommandRegCopy |= kSerialClock;
      Board_Write_8bit(kSerialCommandReg, serialCommandRegCopy);
      Board_Write_8bit(kSerialCommandReg, serialCommandRegCopy);
   }
}

