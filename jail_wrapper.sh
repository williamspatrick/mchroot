#!/bin/sh

SCRIPT=$0
SCRIPTPATH=`dirname ${SCRIPT}`

export PATH=${PATH/$SCRIPTPATH:/}

${SCRIPTPATH}/mchroot ${SCRIPTPATH/\/wrappers/} $*
