#!/bin/bash
# 315314930 Shay Tzirin


grep "$1" "$2" | awk '{total=null; total+=$3; print $0} END{if(total!=null){print "Total balance: ",total}}'
