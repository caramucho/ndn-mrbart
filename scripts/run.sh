#!/bin/bash
#available bandwidth estimation
<<<<<<< HEAD
DIRECTORY=data/issue24/simu4
=======
ISSUE=issue28
SIMUTAG=simu4
DIRECTORY=data/$ISSUE/$SIMUTAG

>>>>>>> simulation_base
# SIMUNAME=bn2M-bitrate
# rm -f out.txt
# touch out.txt
if [ ! -d "$DIRECTORY" ]; then
  mkdir "$DIRECTORY"
fi
waf --run='bwe-6-nodes --targetDt=4.0 --ipswindow=1s --Issue='$ISSUE' --Simutag='$SIMUTAG''

# mv out.txt "$DIRECTORY"/"$SIMUNAME".txt
# mv data/L3Rate.txt "$DIRECTORY"/L3rate.txt
