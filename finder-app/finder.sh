#!/bin/bash
# finder filesdir searchstr

if [ $# -ne 2 ] ; then
   exit 1
fi

filesrid=$1

if ! [ -d $filesdir ] ; then
    exit 1
fi

X=`find $filesdir -type f | wc -l`
Y=`find $filesdir -type f | while read FILE ; do cat $FILE; done | grep $2 | wc -l`
echo "The number of files are $X and the number of matching lines are $Y"
