#! /bin/sh
# Calls bootstrap.sh && ./configure --prefix=$PREFIX $OPTIONS && make && make install

PREFIX=${PWD}/../../relacs_test_install
OPTIONS="--disable-doxygen-dot"
# --disable-linktests
# --with-gsl=/usr

# find . -name '*.so*' -print -delete
# find . -name '*.la' -print -delete
# make clean

./bootstrap.sh || exit 1

./configure "--prefix=$PREFIX" $OPTIONS || exit 1 

if [ "x${MAKE}" = x ]; then
    MAKE=make
fi
"$MAKE" || exit 1
"$MAKE" install || exit 1

echo DONE
exit 0
