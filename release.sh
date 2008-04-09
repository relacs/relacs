#! /usr/bin/env bash
if [ x${MAKE} = x ]; then
    MAKE=make
fi

find . -name '*.la' -print -delete
./bootstrap.sh || exit 1
./configure # Also creates configure.ac files of libraries
${MAKE} || exit 1
${MAKE} distcheck || exit 1

for in in config daq datafile mplot numerics options widgets ; do
    cd $i || exit 1
    ./bootstrap.sh || exit 1
    ./configure || exit 1
    ${MAKE} || exit 1
    ${MAKE} distcheck || exit 1
    cd - || exit 1
done

echo DONE.
exit 0

