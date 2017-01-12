#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"
#include <iostream>
// #include "dash-name.h"
// #include "dash-client.h"
// #include "model/ndn-parser.h"
#include "model/dash-content.h"

using namespace ns3;
using namespace ns3::ndn;
using namespace std;


int
main(int argc, char *argv[])
{
  DashContent dashcontent;
  Data data = dashcontent.getDataPacket(1000000,0);
  cout << data.getContent().value_size() << endl;

}
