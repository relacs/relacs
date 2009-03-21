#!/bin/bash
echo "please wait..."

BOARD="$(comedi_test -t info -f /dev/comedi0 -t info | awk '{print $4;}')"

if test "x$BOARD" = "xMSERIES"; then
    comedi_soft_calibrate -f /dev/comedi0
else
    comedi_calibrate --calibrate -f /dev/comedi0
cat > /dev/null <<EOF
    The following probably doesn't make sense!
    ComedAnalogInput and ComediAnalogOutput are doing this anyways!
    echo "apply calibration to analog input ..."
    CHANNELS=$(comedi_test -t info -f /dev/comedi0 | grep 'analog input' -A 1 | tail -n 1 | awk '{print $4;}')
    let CHANNELS-=1
    RANGES=$(comedi_test -t info -f /dev/comedi0 | grep 'analog input' -A 4 | tail -n 1 | awk '{print NF}')
    let RANGES-=3
    for C in $(seq 0 $CHANNELS); do       # all channels
	for A in 0 1 2; do                # all references
	    for R in $(seq 0 $RANGES); do # all ranges
		comedi_calibrate -reset -calibrate -f /dev/comedi0 -s 0 -r $R -a $A -c $C
	    done
	done
    done
    echo
    echo "apply calibration to analog output ..."
    CHANNELS=$(comedi_test -t info -f /dev/comedi0 | grep 'analog output' -A 1 | tail -n 1 | awk '{print $4;}')
    let CHANNELS-=1
    for C in $(seq 0 $CHANNELS); do      # all channels
	for A in 0; do                   # all references
	    for R in 0; do               # all ranges
		comedi_calibrate -reset -calibrate -f /dev/comedi0 -s 1 -r $R -a $A -c $C
	    done
	done
    done
EOF
fi
