#!/bin/bash
cd /home/zhaoliang/Documents/ndnSIM/ndnSIM-scenario-template
rsync -ahv --delete  ~/CLionProjects/ndnSIM-dash-test/extensions/ extensions/
rsync -ahv --delete  ~/CLionProjects/ndnSIM-dash-test/scenarios/ scenarios/
