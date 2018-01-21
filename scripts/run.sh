#!/bin/bash
#available bandwidth estimation
DIRECTORY=data/issue21/
rm -f out.txt
touch out.txt

waf --run="bwe-6-nodes" >> out.txt
if [ ! -d "$DIRECTORY" ]; then
  mkdir "$DIRECTORY"
fi
mv out.txt "$DIRECTORY"/svaa-buffer.txt
