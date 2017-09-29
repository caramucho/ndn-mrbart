#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"
#include <iostream>
#include "model/dash-client.h"
// #include "model/ndn-net-device-face.hpp"
#include "dash-parameters.h"
#include "./scenario-parameters.h"

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
  for (auto itr = producerList.begin();  itr != producerList.end(); itr++) {
    Ptr<Node> producerNode = Names::Find<Node>(*itr);
    AppHelper producerHelper("ns3::ndn::DashServer");
    // producerHelper.SetAttribute("DashServerPayloadSize", StringValue("8000"));
    string prefix = "/" + *itr;
    // std::cout << prefix << '\n';

    ndnGlobalRoutingHelper.AddOrigins(prefix, producerNode);
    producerHelper.SetPrefix(prefix);
    ApplicationContainer producer = producerHelper.Install(producerNode);
    producer.Start(Seconds(0));
    producer.Stop(Seconds(SCENARIOTIME));
  }



  //Consumer application
  Ptr<Node> consumerNode = Names::Find<Node>("urjc");
  // AppHelper consumerHelper("ns3::ndn::DashClient");
  AppHelper consumerHelper(DASH_CLIENT_TYPE);

  consumerHelper.SetAttribute("VideoId", StringValue("1"));
  consumerHelper.SetAttribute("NumberOfContents", StringValue(CONTENT_NUMBER_STR));
  // consumerHelper.SetPrefix("/caida/dash/MovieID/Period/AdaptationSet/1080p");

  ApplicationContainer consumer0 = consumerHelper.Install(consumerNode);
  consumer0.Start(Seconds(0));
  consumer0.Stop(Seconds(SCENARIOTIME));

  ApplicationContainer consumer1 = consumerHelper.Install(consumerNode);
  consumer1.Start(Seconds(0));
  consumer1.Stop(Seconds(SCENARIOTIME));

  ApplicationContainer consumer2 = consumerHelper.Install(consumerNode);
  consumer2.Start(Seconds(0));
  consumer2.Stop(Seconds(SCENARIOTIME));

  ApplicationContainer consumer3 = consumerHelper.Install(consumerNode);
  consumer3.Start(Seconds(0));
  consumer3.Stop(Seconds(SCENARIOTIME));

  ApplicationContainer consumer4 = consumerHelper.Install(consumerNode);
  consumer4.Start(Seconds(0));
  consumer4.Stop(Seconds(SCENARIOTIME));

  ApplicationContainer consumer5 = consumerHelper.Install(consumerNode);
  consumer5.Start(Seconds(0));
  consumer5.Stop(Seconds(SCENARIOTIME));

  ApplicationContainer consumer6 = consumerHelper.Install(consumerNode);
  consumer6.Start(Seconds(0));
  consumer6.Stop(Seconds(SCENARIOTIME));

  ApplicationContainer consumer7 = consumerHelper.Install(consumerNode);
  consumer7.Start(Seconds(0));
  consumer7.Stop(Seconds(SCENARIOTIME));


  // ndn::GlobalRoutingHelper::CalculateAllPossibleRoutes();
  GlobalRoutingHelper::CalculateRoutes();
  Simulator::Stop(Seconds(SCENARIOTIME));
  // Simulator::Schedule (Seconds (0.0), PrintTime, Seconds (10.0), "");

  // L3RateTracer::InstallAll("/Users/zhaoliang/ndnSIM/my-simulations/results/rate-trace.txt", Seconds(0.5));
  // CsTracer::InstallAll("/home/zhaoliang/Documents/ndnSIM/ndnSIM-scenario-template/results/cs-trace.txt", Seconds(1));
  // L2RateTracer::InstallAll("drop-trace.txt", Seconds(0.5));
  // ndn::AppDelayTracer::InstallAll("app-delays-trace.txt");

  Simulator::Run();
  for (auto itr = producerList.begin();  itr != producerList.end(); itr++) {
    std::cout << *itr << ": ";
    Ptr<Node> producerNode = Names::Find<Node>(*itr);
    auto ndn = producerNode->GetObject<L3Protocol>();
    double hitrate = ndn->getForwarder()->getCs().getHitrate();
    std::cout << hitrate << '\n';
  }
  Simulator::Destroy();
  // uint32_t k;
  // for (k = 0; k < users; k++)
  //   {

  Ptr<DashClient> app = DynamicCast<DashClient>(consumer0.Get(0));
 // std::cout << protocols[k % protoNum] << "-Node: " << k;
 app->GetStats();
 app->GetContentPopularity();


   app = DynamicCast<DashClient>(consumer1.Get(0));
  // std::cout << protocols[k % protoNum] << "-Node: " << k;
  app->GetStats();
  app->GetContentPopularity();

   app = DynamicCast<DashClient>(consumer2.Get(0));
  // std::cout << protocols[k % protoNum] << "-Node: " << k;
  app->GetStats();
  app->GetContentPopularity();

   app = DynamicCast<DashClient>(consumer3.Get(0));
  // std::cout << protocols[k % protoNum] << "-Node: " << k;
  app->GetStats();
  app->GetContentPopularity();

   app = DynamicCast<DashClient>(consumer4.Get(0));
  // std::cout << protocols[k % protoNum] << "-Node: " << k;
  app->GetStats();
  app->GetContentPopularity();

   app = DynamicCast<DashClient>(consumer5.Get(0));
  // std::cout << protocols[k % protoNum] << "-Node: " << k;
  app->GetStats();
  app->GetContentPopularity();

   app = DynamicCast<DashClient>(consumer6.Get(0));
  // std::cout << protocols[k % protoNum] << "-Node: " << k;
  app->GetStats();
  app->GetContentPopularity();

   app = DynamicCast<DashClient>(consumer7.Get(0));
  // std::cout << protocols[k % protoNum] << "-Node: " << k;
  app->GetStats();
  app->GetContentPopularity();
    // }
  //Get Hit rate


  return 0;
}



// int
// main(int argc, char* argv[])
// {
//   return ns3::main(argc, argv);
// }
