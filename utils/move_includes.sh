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

fix_includes_in_source_file() {
    # BEFORE: #include "header.h"
    # AFTER:  #include <relacs/header.h>
    TEMPFILE=`mktemp`
    KEEP_LOCAL=$(find $(dirname "$1") -name '*.h' -printf '%f ' | sed 's/ /\\|/g')
    sed -e 's/^\(# *include \+\)"\(.\+\)" *$/\1<relacs\/\2>/' -e 's/^\(# *include \+\)<relacs\/\('"${KEEP_LOCAL}"'\)> *$/\1"\2"/' "$1" > "${TEMPFILE}"
    mv "${TEMPFILE}" "$1"
}

update_header_location_in_makefile() {
    # BEFORE:     header.h
    # AFTER:      ../include/relacs/header.h
    TEMPFILE=`mktemp`
    sed 's/^\( *\)\('`basename "${header}"`'.*\)/\1..\/include\/relacs\/\2/' < "$1" > "${TEMPFILE}"
    mv "${TEMPFILE}" "$1"
}

ANY_LIB=`sed 's/ /\\\\|/g' <<< "${LIBRARIES}"`
update_incpaths_in_makefile() {
    # BEFORE: -Ifolder/src
    # AFTER:  -Ifolder/include/relacs
    TEMPFILE=`mktemp`
    sed -e 's/^\( *-I.\+\('"${ANY_LIB}"'\)\/\)src\(.*\)/\1include\3/' < "$1" > "${TEMPFILE}"
    mv "${TEMPFILE}" "$1"
}

update_moc_rule_in_makefile() {
    TEMPFILE=`mktemp`
    sed -e 's/\(%-moc.cpp *: *\)\(%.h\)/\1..\/include\/relacs\/\2/' < "$1" > "${TEMPFILE}"
    mv "${TEMPFILE}" "$1"
}

move_header() {
    header=$1
    header_after=`sed 's/\/src\//\/include\/relacs\//' <<<"${header}"`
    header_after_dir=`dirname "${header_after}"`

    ## Add move location to repository
    echo mkdir -p "${header_after_dir}"
    mkdir -p "${header_after_dir}"
    recursive_add "${header_after_dir}"

    ## Move header
    echo svn mv "${header}" "${header_after}"
    svn mv "${header}" "${header_after}"
}



# Fix all includes in source files
while read file ; do
    fix_includes_in_source_file "${file}"
done < <(find . -type f -name '*.h' -o -name '*.cc')

