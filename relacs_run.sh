#! /bin/sh
RELACSEXECPATH="$PWD/src" \
	RELACSPLUGINPATHES="\
auditory/.libs\
:common/.libs\
:hardware/.libs\
:patch/.libs" \
	exec ./utils/relacsscript $@

