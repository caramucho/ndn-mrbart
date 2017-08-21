#!/bin/zsh
. ./sync-clion.sh
DIRECTORY=data/mean

if [[ ! -d "$DIRECTORY" ]]; then
  mkdir $DIRECTORY
fi


for (( i = 0.7; i <= 1.2; i=i+0.1 )); do
  #statements
  # echo `echo "$i" | awk '{printf("%4.1f",$1)}'`
  rm -f out.txt
  touch out.txt
  waf --run="dash-simple --mean=${i} --delay=10" >> out.txt
  mv out.txt $DIRECTORY/${i}.txt
  # waf --run="dash-simple --mean=${i}"

done
