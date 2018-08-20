#!/bin/bash
# 315314930 Shay Tzirin
 
#set variable with the number of arguments recieved
arguments=$#

#check number of arguments
if [ $arguments -eq 0 ]||[ $arguments -gt 1 ]; then
    echo error:‬‬ ‫‪only‬‬ ‫‪one‬‬ ‫‪argument‬‬ ‫‪is‬‬ ‫‪allowed‬‬
    exit
fi

#check if file exists in folder
if [ -e $1 ]; then
        #check if folder doesnt exist
        if [ ! -d "safe_rm_dir" ]; then
        	mkdir safe_rm_dir
	fi
        cp $1 safe_rm_dir/
        rm -f $1
	echo "done!"
        exit 
else
    echo ‫‪"error:‬‬ ‫‪there‬‬ ‫‪is‬‬ ‫‪no‬‬ ‫‪such‬‬ ‫‪file‬‬"
fi

                                                                               



