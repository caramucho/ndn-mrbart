#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"
#include <iostream>
// #include "dash-name.h"
// #include "dash-client.h"
// #include "model/ndn-parser.h"
#include "model/dash-content.h"
// #include "ns3/packet.h"


// #include "ns3/ndnSIM/apps/ndn-consumer.hpp"





// using namespace ns3;
using namespace ns3::ndn;
using namespace std;
// using namespace ndn;


int
main(int argc, char *argv[])
{
  MPEGHeader mpeg_header;
  ndn::Buffer buf = ndn::Buffer(1000000);
  DashContent dashcontent;
  Data data = dashcontent.getDataPacket(1000000,0);
  for(auto i = data.getContent().value_begin();i!= data.getContent().value_end();i++){
    buf.push_back(*i);
    // cout << *i << endl;
  }

  uint32_t headersize = mpeg_header.GetSerializedSize();
  cout << headersize << endl;
  ns3::Packet headerPacket(&buf[0], headersize);
  headerPacket.RemoveHeader(mpeg_header);
  cout << "content size:" << mpeg_header.GetSize() << endl;

  return 0;
}
