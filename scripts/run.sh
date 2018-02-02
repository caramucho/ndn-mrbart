#!/bin/bash
#available bandwidth estimation
DIRECTORY=data/issue23/bn3M-5rates
# SIMUNAME=bn2M-bitrate
# rm -f out.txt
# touch out.txt
if [ ! -d "$DIRECTORY" ]; then
  mkdir "$DIRECTORY"
fi
waf --run="bwe-6-nodes"

# mv out.txt "$DIRECTORY"/"$SIMUNAME".txt
mv data/L3Rate.txt "$DIRECTORY"/L3rate.txt
