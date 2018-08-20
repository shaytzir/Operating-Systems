#!/bin/bash
# 315314930 Shay Tzirin

cd $1
TXTF=$(ls |grep ".txt"| wc -w)
echo "‫‪Number‬‬ ‫‪of‬‬ ‫‪files‬‬ ‫‪in‬‬ ‫‪the‬‬ ‫‪directory‬‬ ‫‪that‬‬ ‫‪end‬‬ ‫‪with‬‬ ‫‪.txt‬‬ ‫‪is‬‬ ${TXTF}"
