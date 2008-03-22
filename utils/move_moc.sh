#! /usr/bin/env bash
if false; then
TEMPFILE=`mktemp`
while  read header ; do
    if [[ `moc-qt3 -o "${TEMPFILE}" "${header}" 2>&1 | head -n 1` != *'No relevant classes found.'* ]] ; then
        header=${header#./}
        echo "${header} "
        FILE=`basename "${header}"`
        DIR=`dirname "${header}"`
        if [[ "${header}" = hardware* ]]; then
            HOST_SOURCE="${DIR}/../../${FILE%.h}.cc"
        else
            HOST_SOURCE="${DIR}/../../src/${FILE%.h}.cc"
        fi
        echo "  Host: ${HOST_SOURCE}"
        MOC_SOURCE="moc_${FILE%.h}.cc"
        echo "  Moc: ${MOC_SOURCE}"
        {
            if [ -f "${HOST_SOURCE}" ]; then
                echo
            fi
            echo "#include \"${MOC_SOURCE}\"" ;
        } >> "${HOST_SOURCE}"
    fi
done < <(find . -type f -name '*.h')
rm "${TEMPFILE}"
fi

EXTRA_FILES='
auditory/speaker/calibspeakers.cc
auditory/base/spikeprecision.cc
auditory/base/ficurve.cc
auditory/base/auditoryneuron.cc
auditory/sascha/isoresponse.cc
auditory/base/singlestimulus.cc
auditory/base/fifield.cc
auditory/base/syslatency.cc
auditory/base/auditorysearch.cc
auditory/base/auditorysession.cc
auditory/sascha/oneclick.cc
common/controls/spectrumanalyzer.cc
common/models/simplemodel.cc
common/detectors/spikedetector.cc
common/base/stimulusdelay.cc
common/base/restartdelay.cc
common/models/neuronmodels.cc
common/controls/motorcontrol.cc
common/filter/highpass.cc
common/filter/lowpass.cc
common/base/pause.cc
'

for HOST_SOURCE in $EXTRA_FILES ; do
    HOST_SOURCE=${HOST_SOURCE#./}
    echo "${HOST_SOURCE} "
    echo "  Host: ${HOST_SOURCE}"
    FILE=`basename "${HOST_SOURCE}"`
    DIR=`dirname "${HOST_SOURCE}"`
    MOC_SOURCE="moc_${FILE%.cc}.cc"
    echo "  Moc: ${MOC_SOURCE}"
    {
        if [ -f "${HOST_SOURCE}" ]; then
            echo
        fi
        echo "#include \"${MOC_SOURCE}\"" ;
    } >> "${HOST_SOURCE}"
done

