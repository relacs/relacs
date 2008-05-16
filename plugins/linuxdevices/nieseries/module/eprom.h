#ifndef _EPROM_H_
#define _EPROM_H_


#define uint8 u8
#define int16 s16
#define uint16 u16
#define int32 s32
#define uint32 u32

   enum RegOffsets{
      kSerialCommandReg = 0x0D,
      kSerialStatusReg  = 0x01
      };

   enum SerialCommBits{
      kSerialClock      =  (1 << 0),
      kSerialData       =  (1 << 1),
      kEpromChipSel     =  (1 << 2),
      kSerialDacLd0     =  (1 << 3),
      kSerialDacLd1     =  (1 << 4),
      kSerialDacLd2     =  (1 << 5)
      };

   enum SerialStatusBits{
      kEpromDataOut     =  (1 << 0)
      };

   enum EPROMItemLengths{
      kEEAddressLength  = 8,
      kEECommandLength  = 8,
      kEEDataLength     = 8
      };

   enum Commands{
      kEEReadCommand          = 0x03,
      kEEReadStatusCommand    = 0x05,
      kEEWriteCommand         = 0x02,
      kEEWriteEnableCommand   = 0x06,
      kEEWriteStatusCommand   = 0x01
      };

   enum ProtectionCodes{
      kEEWriteProtect1_4      = 0x04,
      kEEWriteProtect1_2      = 0x08,
      kEEWriteProtectAll      = 0x0C,
      kEEWriteProtectOff      = 0x00
      };

   enum DACLengths {
      kSerialDacIdLength = 4,
      kSerialDacDataLength = 8,
      kSerialDacDataLength16x = 16
      };
   
   enum DACTypes  {
      kXe50,
      k6110,
      kf2Eseries,
      k8804
      };

int loadcalibration(bp dev);

int16 serialDacWrite(uint32 calClass, uint32 dac, uint16 value);
int16 eEpromRead(uint32 address, uint16 *binaryValue);
int16 eEpromWrite(uint32 address, uint16 binaryValue);
int16 eEpromProtect(uint32 value);
int16 eEpromUnProtect(void);

int16 sDacWritef2Eseries(int16 dac_number, uint16 value);
int16 sDacWriteXe50(int16 dac_number, uint16 value);
int16 sDacWrite6110(int16 dac_number, uint16 value);
int16 sDacWrite8804(int16 dac_number, uint16 value);

void check_eeprom_busy(void);
void toggle_chip_select(void);
void remove_chip_select(void);
void data_out_msb_first(uint16 number_of_bits, uint16 the_bits);
void data_out_lsb_first(uint16 number_of_bits, uint16 the_bits);
void read_status_register(uint16* value);
void get_bits(uint16 number_of_bits,  uint16* the_bits);
void strobe_serial_dac(uint8 value);

#endif
