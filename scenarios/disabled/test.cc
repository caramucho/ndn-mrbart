#include <string>
#include <fstream>
#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "helper/dash-client-helper.h"
#include "helper/dash-server-helper.h"
#include "model/dash-client.h"

using namespace ns3;

int
main(int argc, char *argv[])
{
