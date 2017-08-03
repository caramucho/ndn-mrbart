#!/bin/bash
. ./sync-clion.sh
for (( i = 10; i <= 50; i+=10 )); do
  #statements
  touch out.txt
  waf --run="dash-simple --delay=${i}" >> out.txt
  mv out.txt data/delays/${i}.txt
done
