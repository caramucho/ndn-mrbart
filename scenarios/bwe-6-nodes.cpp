#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"
#include "ns3/point-to-point-module.h"

#include <iostream>
#include "dash-parameters.h"
#include "scenario-parameters.h"
#include "model/ndn-dash-mrbart.hpp"


using namespace ns3;
using namespace ns3::ndn;
using namespace std;



int
main(int argc, char* argv[])
{

    CommandLine cmd;
    cmd.Parse (argc, argv);

    AnnotatedTopologyReader topologyReader("", 25);
    topologyReader.SetFileName("topo/topo-6-node.txt");
    topologyReader.Read();
    // topologyReader.ApplySettings();
    // topologyReader.ApplyOspfMetric();

    // Install NDN stack on all nodes
    StackHelper ndnHelper;
    ndnHelper.setCsSize(CACHE_SIZE);
    ndnHelper.setPolicy("nfd::cs::lru");
    // ndnHelper.SetOldContentStore("ns3::ndn::cs::Lru", "MaxSize",
    // cacheSize); // ! Attention ! If set to 0, then MaxSize is infinite
//    ndnHelper.SetDefaultRoutes(true);

    ndnHelper.InstallAll();

    StrategyChoiceHelper::InstallAll("/", "/localhost/nfd/strategy/best-route");

    GlobalRoutingHelper ndnGlobalRoutingHelper;
    ndnGlobalRoutingHelper.InstallAll();

    // Find consumer nodes
    Ptr<Node> consumers[3];
    for(int i=0;i<3;i++){
        consumers[i] = Names::Find<Node>("Src" + to_string(i+1));
    }

//    DashClient::RegisterProducerDomain(producerList);
    const std::vector<string> producerList = {"Dst1","Dst2","Dst3"};

    for (auto itr = producerList.begin();  itr != producerList.end(); itr++) {
        Ptr<Node> producerNode = Names::Find<Node>(*itr);
        AppHelper producerHelper("ns3::ndn::Producer");
        producerHelper.SetAttribute("PayloadSize", StringValue("8000"));
        string prefix = "/" + *itr;
//         std::cout << prefix << '\n';

        ndnGlobalRoutingHelper.AddOrigins(prefix, producerNode);
        producerHelper.SetPrefix(prefix);
        ApplicationContainer producer = producerHelper.Install(producerNode);
        producer.Start(Seconds(0));
        producer.Stop(Seconds(SCENARIOTIME));
    }
    //Consumer application
    ns3::ndn::AppHelper consumerHelper("ns3::ndn::FdashClient");
    consumerHelper.SetPrefix("/Dst1");
    consumerHelper.SetAttribute("ConsumerID" , StringValue("0"));
    ApplicationContainer consumerapp = consumerHelper.Install(consumers[0]);
    consumerapp.Start(Seconds(0));
    consumerapp.Stop(Seconds(SCENARIOTIME));

    ns3::ndn::AppHelper consumerHelper3("ns3::ndn::FdashClient");
    consumerHelper3.SetPrefix("/Dst3");
    consumerHelper3.SetAttribute("ConsumerID" , StringValue("1"));
    ApplicationContainer consumerapp3 = consumerHelper3.Install(consumers[2]);
    consumerapp3.Start(Seconds(100));
    consumerapp3.Stop(Seconds(SCENARIOTIME));

    // cross traffic generator
    ns3::ndn::AppHelper consumerHelper2("ns3::ndn::ConsumerCbr");
    consumerHelper2.SetPrefix("/Dst2");
    // consumerHelper2.SetAttribute("Randomize" , StringValue("exponential"));
    consumerHelper2.SetAttribute("Randomize" , StringValue("uniform"));
    consumerHelper2.SetAttribute("Frequency", StringValue("15.625")); // 0.5Mbps cbr cross traffic 0.5*2/(0.008*8)=7.8125
    // consumerHelper2.SetAttribute("Frequency", StringValue("5")); // 0.5Mbps cbr cross traffic 0.5/(0.008*8)=7.8125

    ApplicationContainer consumerapp2 = consumerHelper2.Install(consumers[1]);
    consumerapp2.Start(Seconds(0));
    consumerapp2.Stop(Seconds(SCENARIOTIME));
    // consumerapp2.Start(Seconds(0));
    // consumerapp2.Stop(Seconds(SCENARIOTIME));


    GlobalRoutingHelper::CalculateRoutes();
    Simulator::Stop(Seconds(SCENARIOTIME));

    L3RateTracer::Install(Names::Find<Node>("Rtr1"), "data/L3Rate.txt", Seconds(1.0));
    Simulator::Run();
    Simulator::Destroy();


    return 0;
}
