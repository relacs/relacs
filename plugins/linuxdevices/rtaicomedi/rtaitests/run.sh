INSM=true
RMM=true
SL=false
if test "x$1" = "xi"; then
    RMM=false
fi
if test "x$1" = "xr"; then
    INSM=false
fi
if test "x$1" = "xn"; then
    INSM=false
    RMM=false
fi
if test "x$2" = "xs"; then
    SL=true
fi
#RUNS=true
#if test "x$2" = "xr"; then
#    RUNS=false
#fi


if $INSM; then
    echo "insmod rtai modules"
    insmod /usr/realtime/modules/rtai_hal.ko
    insmod /usr/realtime/modules/rtai_sched.ko
    #insmod /usr/realtime/modules/rtai_fifos.ko
fi
echo "insmod rt_process"
insmod rt_process.ko

#if $RUNS; then
#    echo "run scope ..."
#    ./scope
#    echo "scope finished"
#else
    echo "do sleep 1 ..."
    sleep 1
    echo "sleep finished"
#fi

if $SL; then
    echo "do sleep 1 ..."
    sleep 1
    echo "sleep finished"
fi

rmmod rt_process
echo "rmmod rt_process"
if $RMM; then
    #rmmod rtai_fifos
    rmmod rtai_sched
    rmmod rtai_hal
    echo "rmmod rtai modules"
fi
