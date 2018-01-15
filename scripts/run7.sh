#!/bin/bash
#available bandwidth estimation
DIRECTORY=data/simu7/
rm -f out.txt
touch out.txt
waf --run="bwe-6-nodes" >> out.txt
if [ ! -d "$DIRECTORY" ]; then
  mkdir "$DIRECTORY"
fi
mv out.txt "$DIRECTORY"/uips-sample.txt
