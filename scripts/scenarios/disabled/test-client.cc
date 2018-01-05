#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"
#include <iostream>
// #include "dash-name.h"
#include "model/dash-client.h"
// #include "model/ndn-parser.h"

using namespace ns3;
using namespace ns3::ndn;
using namespace std;


int
main(int argc, char *argv[])
{
    DashClient client;
    Name("/caida/DashOverICN/");
    client.SetInterestName
}
