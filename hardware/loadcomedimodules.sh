/sbin/insmod /usr/src/kernels/rtai/modules/rtai_hal.ko
/sbin/insmod /usr/src/kernels/rtai/modules/rtai_ksched.ko
/sbin/insmod /usr/src/kernels/rtai/modules/rtai_fifos.ko
/sbin/insmod /usr/src/kernels/rtai/modules/rtai_math.ko
/sbin/insmod /usr/src/kernels/rtai/modules/rtai_shm.ko
/sbin/modprobe kcomedilib
/sbin/modprobe ni_pcimio
sleep 5
/usr/src/comedilib/comedi_config/comedi_config /dev/comedi0 ni_pcimio
sleep 1
/usr/src/comedilib/comedi_calibrate/comedi_calibrate /dev/comedi0
chmod 666 /dev/comedi*
#mknod -m 666 /dev/rtf0 c 150 0      # for RTAI fifos
#mknod -m 666 /dev/rtf1 c 150 1      # for RTAI fifos
#mknod -m 666 /dev/rtai_shm c 10 254 # for RTAI shared memory
#mknod -m 666 /dev/rttest c 227 0    # create own driver node

