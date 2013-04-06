#!/bin/sh

SCRIPT=`realpath $0`
SCRIPTPATH=`dirname ${SCRIPT}`

mchroot ${SCRIPTPATH/\/wrappers/} /usr/bin/`basename $0` $*
