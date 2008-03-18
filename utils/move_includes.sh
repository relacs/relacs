#! /usr/bin/env bash
# Run from ../ through./utils/<self>
# Used to do larger include files move

LIBRARIES="config daq datafile mplot numerics options widgets"

recursive_add() {
    PWD_BACKUP=$PWD
    while read part ; do
        echo svn add "${part}"
        svn add "${part}"

        echo cd "${part}"
        cd "${part}"
    done < <(sed 's/\//\n/g' <<< "$1")
    cd "${PWD_BACKUP}"
}

local_to_incpath() {
    TEMPFILE=`mktemp`
    sed 's/^\(# *include \+\)"\(.\+\)" *$/\1<relacs\/\2>/' "$1" > "${TEMPFILE}"
    mv "${TEMPFILE}" "$1"
}

for dir in $LIBRARIES ; do
    while read header ; do
        ## Fix includes in source files
        source="${header%.h}.cc"
        if [ -f "${source}" ]; then
            local_to_incpath "${source}"
        fi

        header_after=`sed 's/\/src\//\/include\/relacs\//' <<<"${header}"`
        header_after_dir=`dirname "${header_after}"`

        ## Add move location to repository
        echo mkdir -p "${header_after_dir}"
        mkdir -p "${header_after_dir}"
        recursive_add "${header_after_dir}"

        ## Move header
        echo svn mv "${header}" "${header_after}"
        svn mv "${header}" "${header_after}"

        ## Fix includes in headers
        local_to_incpath "${header_after}"

        ## Fix header locations in Automake file
        makefile=`dirname "${header}"`'/Makefile.am'
        TEMPFILE=`mktemp`
        sed 's/^\( *\)\('`basename "${header}"`'.*\)/\1..\/include\/relacs\/\2/' < "${makefile}" > "${TEMPFILE}"
        mv "${TEMPFILE}" "${makefile}"
    done < <(find $dir -type f -wholename '*/src/*.h')
done

## Fix include dirs in Automake files
ANY_LIB=`sed 's/ /\\\\|/g' <<< "${LIBRARIES}"`
while read makefile ; do
    TEMPFILE=`mktemp`
    sed -e 's/^\( *-I.\+\('"${ANY_LIB}"'\)\/\)src\(.*\)/\1include\3/' -e 's/\(%-moc.cpp *: *\)\(%.h\)/\1..\/include\/relacs\/\2/'< "${makefile}" > "${TEMPFILE}"
    mv "${TEMPFILE}" "${makefile}"
done < <(find . -name 'Makefile.am')

## Fix includes in relacs/*
while read file ; do
    local_to_incpath "${file}"
done < <(find relacs -type f -name '*.h' -o -name '*.cc')



PLUGINS="common hardware auditory"
for dir in $PLUGINS ; do
    ## Fix library includes in plugin code
    while read file ; do
        TEMPFILE=`mktemp`
        KEEP_LOCAL=$(find $(dirname "$file") -name '*.h' -printf '%f ' | sed 's/ /\\|/g')
        sed -e 's/^\(# *include \+\)"\(.\+\)" *$/\1<relacs\/\2>/' -e 's/^\(# *include \+\)<relacs\/\('"${KEEP_LOCAL}"'\)> *$/\1"\2"/' "${file}" > "${TEMPFILE}"
        mv "${TEMPFILE}" "${file}"
    done < <(find $dir -type f -name '*.h' -o -name '*.cc')

    ## Fix librelacs includes in plugin code
    while read makefile ; do
        TEMPFILE=`mktemp`
        sed -e 's/include\/relacs/include/' -e 's/..\/include\/%.h/%.h/' < "${makefile}" > "${TEMPFILE}"
        mv "${TEMPFILE}" "${makefile}"
    done < <(find $dir -name 'Makefile.am')
done

