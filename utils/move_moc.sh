#! /usr/bin/env bash
TEMPFILE=`mktemp`
while  read header ; do
    if [[ `moc-qt3 -o "${TEMPFILE}" "${header}" 2>&1 | head -n 1` = *'No relevant classes found.'* ]] ; then
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
        { echo '' ; echo "#include \"${MOC_SOURCE}\"" ; } >> "${HOST_SOURCE}"
    fi
done < <(find . -type f -name '*.h')
rm "${TEMPFILE}"

