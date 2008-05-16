#ifndef _MISC_H_
#define _MISC_H_

#include "core.h"


void misc_init( bp dev );
void misc_cleanup( bp dev );
int misc_open( bp dev, struct file *file );
void misc_release( bp dev );
int misc_ioctl( bp dev, struct file *file, unsigned int cmd, unsigned long arg );

void MSC_Clock_Configure( bp dev );
void MSC_IO_Pin_Configure( bp dev, int pin, int output );
void MSC_IrqGroupEnable( bp dev, int grp );
void Analog_Trigger_Control( bp dev, int enable );

#endif
