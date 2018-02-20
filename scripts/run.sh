#!/bin/bash
#available bandwidth estimation
ISSUE=issue27
SIMUTAG=simu6
DIRECTORY=data/$ISSUE/$SIMUTAG

# SIMUNAME=bn2M-bitrate
# rm -f out.txt
# touch out.txt
if [ ! -d "$DIRECTORY" ]; then
  mkdir "$DIRECTORY"
fi
waf --run='bwe-6-nodes --targetDt=0 --ipswindow=4s --Issue='$ISSUE' --Simutag='$SIMUTAG''

# mv out.txt "$DIRECTORY"/"$SIMUNAME".txt
# mv data/L3Rate.txt "$DIRECTORY"/L3rate.txt
