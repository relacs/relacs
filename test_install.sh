#! /bin/sh
./bootstrap.sh || exit 1
# find . -name '*.so*' -print -delete
# find . -name '*.la' -print -delete
# --disable-linktests
./configure "--prefix=${PWD}/../../relacs_test_install" --with-gsl=/usr || exit 1 
if [ "x${MAKE}" = x ]; then
    MAKE=make
fi
"$MAKE" || exit 1
# "$MAKE" install || exit 1

echo DONE
exit 0
