#!/bin/bash
. ./sync-clion.sh
rm -f out.txt
touch out.txt
waf --run=dash-simple >> out.txt
