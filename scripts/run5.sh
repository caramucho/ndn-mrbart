#!/bin/bash
# run scenario#2
. ./scripts/sync-clion.sh
# rm -f out.txt
# touch out.txt
# waf --run="test-scenario-multiProducers --mean=0.9 --delay=10"
DIRECTORY=data/simu5/
rm -f out.txt
touch out.txt
waf --run="simu3-10consumers" >> out.txt
if [ ! -d "$DIRECTORY" ]; then
  mkdir "$DIRECTORY"
fi
mv out.txt "$DIRECTORY"/100Mbps-aaash.txt
