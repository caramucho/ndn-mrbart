#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"
#include <iostream>
#include "model/dash-client.h"
// #include "model/ndn-net-device-face.hpp"
#include "dash-parameters.h"

using namespace ns3;
using namespace ns3::ndn;
using namespace std;


// void PrintTime (Time next, const string name)
// {
//   cerr << " === " << name << " " << Simulator::Now ().ToDouble (Time::S) << "s" << endl;
//   Simulator::Schedule (next, PrintTime, next, name);
// }

int
main(int argc, char* argv[])
{
  cout << "scenario13 initilizing" << endl;


  AnnotatedTopologyReader topologyReader("", 1);
  // topologyReader.SetFileName("src/ndnSIM/examples/topologies/testbed.txt");
  topologyReader.SetFileName("/home/zhaoliang/Documents/ndnSIM/my-simulations/topo/testbed.txt");
  topologyReader.Read();
  topologyReader.ApplyOspfMetric();

  // Install NDN stack on all nodes
  StackHelper ndnHelper;
  ndnHelper.setCsSize(CACHE_SIZE);
  ndnHelper.setPolicy("nfd::cs::lru");
  // ndnHelper.SetOldContentStore("ns3::ndn::cs::Lru", "MaxSize",
  // cacheSize); // ! Attention ! If set to 0, then MaxSize is infinite
  ndnHelper.InstallAll();


  StrategyChoiceHelper::InstallAll("/", "/localhost/nfd/strategy/best-route");

  GlobalRoutingHelper ndnGlobalRoutingHelper;
  ndnGlobalRoutingHelper.InstallAll();

  //Movie Producer
  const std::vector<string> producerList = {"remap","byu","ua","csu","wu","um","uiuc","mich","verisign","neu","lip6","urjc","orange","systemx","ntnu","basel","padua","bupt","pku","tongji","anyang","kisti","waseda","caida","ucla","uci"};
  Ptr<Node> producerNode = Names::Find<Node>("caida");
  AppHelper producerHelper("ns3::ndn::DashServer");
  // producerHelper.SetAttribute("DashServerPayloadSize", StringValue("8000"));
  ndnGlobalRoutingHelper.AddOrigins("/Caida", producerNode);
  producerHelper.SetPrefix("/Caida");
  ApplicationContainer producer = producerHelper.Install(producerNode);
  producer.Start(Seconds(0));
  producer.Stop(Seconds(SCENARIOTIME));

  //Consumer application
  Ptr<Node> consumerNode = Names::Find<Node>("urjc");
  // AppHelper consumerHelper("ns3::ndn::DashClient");
  AppHelper consumerHelper("ns3::ndn::SftmClient");

  consumerHelper.SetAttribute("VideoId", StringValue("1"));
  consumerHelper.SetAttribute("NumberOfContents", StringValue(CONTENT_NUMBER_STR));
  // consumerHelper.SetPrefix("/caida/dash/MovieID/Period/AdaptationSet/1080p");

  ApplicationContainer consumer = consumerHelper.Install(consumerNode);
  consumer.Start(Seconds(0));
  consumer.Stop(Seconds(SCENARIOTIME));



  // ndn::GlobalRoutingHelper::CalculateAllPossibleRoutes();
  GlobalRoutingHelper::CalculateRoutes();
  Simulator::Stop(Seconds(SCENARIOTIME));
  // Simulator::Schedule (Seconds (0.0), PrintTime, Seconds (10.0), "");

  // L3RateTracer::InstallAll("/Users/zhaoliang/ndnSIM/my-simulations/results/rate-trace.txt", Seconds(0.5));
  // CsTracer::InstallAll("/home/zhaoliang/Documents/ndnSIM/ndnSIM-scenario-template/results/cs-trace.txt", Seconds(1));
  // L2RateTracer::InstallAll("drop-trace.txt", Seconds(0.5));
  // ndn::AppDelayTracer::InstallAll("app-delays-trace.txt");

  Simulator::Run();
  Simulator::Destroy();
  // uint32_t k;
  // for (k = 0; k < users; k++)
  //   {
  Ptr<DashClient> app = DynamicCast<DashClient>(consumer.Get(0));
  // std::cout << protocols[k % protoNum] << "-Node: " << k;
  app->GetStats();
  app->GetContentPopularity();
    // }


  return 0;
}



// int
// main(int argc, char* argv[])
// {
//   return ns3::main(argc, argv);
// }
