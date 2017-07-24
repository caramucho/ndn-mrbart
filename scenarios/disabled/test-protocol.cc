#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"
#include <iostream>
#include "model/dash-name.h"
// #include "dash-client.h"
#include "model/ndn-parser.h"

using namespace ns3;
using namespace ns3::ndn;
using namespace std;


int
main(int argc, char *argv[])
{
  DashName dashname;
  dashname.parseName(Name("/caida/DashOverICN/"));
  // std::string str2 = "caida";
  // auto check = dashname.GetProducerDomain().compare("caida");
  // std::cout << check << '\n';
}
