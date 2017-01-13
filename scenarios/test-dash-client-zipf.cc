#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"
#include <iostream>

// #include "dash-client.h"
#include "model/dash-client-zipf.h"

using namespace ns3;
using namespace ns3::ndn;
using namespace std;


int
main(int argc, char *argv[])
{
  std::vector<int> counts(200,0);
  DashClientZipf dcz;
  dcz.SetNumberOfContents(200);
  for (size_t i = 0; i < 10000; i++) {
    counts[dcz.GetNextContentId()]++;
  }
  std::cout << counts[0] << '\n';
  // std::vector<int>::iterator itr = counts.begin();
  for (auto itr = counts.begin(); itr != counts.end(); itr++) {
    std::cout << *itr << " ";
  }
}
