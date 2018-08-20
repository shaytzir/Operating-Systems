#!/bin/bash
# 315314930 Shay Tzirin
cd $1
for i in *
do
	if [[ -f $i ]];
	then
		echo "$i is a file"
	fi
	if [[ -d $i ]];
	then
		echo "$i is a directory"
	fi
done
