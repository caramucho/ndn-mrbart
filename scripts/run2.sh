#!/bin/bash
# run scenario#2
. ./sync-clion.sh
# rm -f out.txt
# touch out.txt
waf --run="dash-congestion#2 --mean=0.9 --delay=10"
