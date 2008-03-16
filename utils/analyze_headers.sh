#! /bin/bash

if [[ `gcc -dumpversion` != 4.2.3 ]] ; then
    cat <<'GCC_ERROR'
ERROR:
  This script expects to be run on a GCC 4.2.3 machine.
  Please adjust it to your environment and remove this check after.'
GCC_ERROR
    exit 1
fi

INCLUDE_DIRS=`find . -name '*.h' | sed 's/^.\/\(.\+\)\/[^\/]\+.h$/-I\1/' | sort -u`'
-I/usr/include/c++/4.2.3
-I/usr/include/c++/4.2/i486-linux-gnu
-I/usr/include/c++/4.2/tr1
-I/usr/include/linux
-I/usr/include/qt3
-I/usr/include
'

TMP=`mktemp /tmp/tfile.XXXXXXXXXX`
if [ "x${TMP}" = x ]; then
    exit 1
fi

find . -name '*.h' -o -name '*.cc' | while read FILE; do
    # Replace leading "./"
    FILE=`echo ${FILE} | sed 's/^.\///'`
    DIRNAME=`dirname ${FILE}`

    # Skip examples for now
    if [[ ${FILE} == *examples* ]]; then
        continue
    fi

    # Skip utils folder
    if [[ ${DIRNAME} == utils* ]]; then
        continue
    fi

    # Find all files that
    # - this file is including
    # - excluding system headers
    # - excluding files from the same folder
    touch "${TMP}"
    echo "[${FILE}]"
    makedepend -f "${TMP}" -w1000000 -D__MAKEDEPEND__ ${INCLUDE_DIRS} ${FILE}

    if [[ `cat "${TMP}" | grep ':' | wc -w` -gt 1 ]]; then 
        echo '  Files'
    fi
    DEPDIRS=
    for DEP in `cat "${TMP}" | grep ':'`; do
        # Skip target
        if [[ ${DEP} == *: ]]; then
            continue
        fi

        # Skip system header
        if [[ ${DEP} == /usr/include/* ]]; then
            continue
        fi

        # Skip same dir headers
        if [ `dirname ${DEP}` = ${DIRNAME} ]; then
            continue
        fi

        # (1) Print header file
        echo "    F ${DEP}"

        DEPDIRS="${DEPDIRS} "`dirname "${DEP}"`
    done

    UNIQUEDEPDIRS=`echo ${DEPDIRS} | sed 's/\s/\n/g' | sort -u`
    if [[ `echo "${UNIQUEDEPDIRS}" | wc -w` -gt 0 ]]; then 
        echo '  Directories'
    fi
    for UDD in ${UNIQUEDEPDIRS}; do
        # (2) Print header dir
        echo "    D ${UDD}"
    done

    echo
    rm "${TMP}" 2>/dev/null
done

rm "${TMP}" 2>/dev/null

