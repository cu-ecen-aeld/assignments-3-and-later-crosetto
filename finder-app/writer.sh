#!/bin/bash
# writer writefile writestr

if [ $# -ne 2 ] ; then
   exit 1
fi

writedir=`dirname $1`
mkdir -p $writedir

if [ $? -ne 0 ]; then
   exit $?
fi
echo $2 > $1

if [ $? -ne 0 ]; then
   exit $?
fi
