//
//
//

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"
#include "ns3/point-to-point-module.h"

#include <iostream>
// #include "model/ndn-net-device-face.hpp"
#include "model/dash-client.h"
#include "dash-parameters.h"
#include "scenario-parameters.h"


using namespace ns3;
using namespace ns3::ndn;
using namespace std;



int
main(int argc, char* argv[])
{

    int delay = 10;
    double mean = 1.0;

    CommandLine cmd;
//    cmd.AddValue ("mean", "mean interval parameter", mean);
    cmd.Parse (argc, argv);

    AnnotatedTopologyReader topologyReader("", 25);
    topologyReader.SetFileName("topo/topo-10-consumers.txt");
    topologyReader.Read();
//    topologyReader.ApplyOspfMetric();


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
    Ptr<Node> consumers[10];
    for(int i=0;i<10;i++){
        consumers[i] = Names::Find<Node>("Src" + to_string(i+1));
    }

//    DashClient::RegisterProducerDomain(producerList);
    const std::vector<string> producerList = {"Dst1","Dst2"};

    for (auto itr = producerList.begin();  itr != producerList.end(); itr++) {
        Ptr<Node> producerNode = Names::Find<Node>(*itr);
        AppHelper producerHelper("ns3::ndn::DashServer");
        // producerHelper.SetAttribute("DashServerPayloadSize", StringValue("8000"));
        string prefix = "/" + *itr;
//         std::cout << prefix << '\n';

        ndnGlobalRoutingHelper.AddOrigins(prefix, producerNode);
        producerHelper.SetPrefix(prefix);
        ApplicationContainer producer = producerHelper.Install(producerNode);
        producer.Start(Seconds(0));
        producer.Stop(Seconds(SCENARIOTIME));
    }

    //Consumer application
    AppHelper consumerHelper(DASH_CLIENT_TYPE);

//    consumerHelper.SetPrefix("/Prefix");
    consumerHelper.SetAttribute("VideoId", StringValue("1"));
    consumerHelper.SetAttribute("NumberOfContents", StringValue(CONTENT_NUMBER_STR));
    consumerHelper.SetAttribute("MeanParameter", DoubleValue(mean));

    ApplicationContainer consumerapps[10];
    for(int i=0;i<10;i++) {
        consumerHelper.SetAttribute("ConsumerId", StringValue(to_string(i+1)));
        ApplicationContainer consumerapp = consumerHelper.Install(consumers[i]);
        consumerapps[i] = consumerapp;
        consumerapp.Start(Seconds(0));
        consumerapp.Stop(Seconds(SCENARIOTIME));
    }


    GlobalRoutingHelper::CalculateRoutes();
    Simulator::Stop(Seconds(SCENARIOTIME));

    Simulator::Run();
    Simulator::Destroy();
//    for(int i=0;i<10;i++) {
//        Ptr<DashClient> app = DynamicCast<DashClient>(consumerapps[i].Get(0));
//    // std::cout << protocols[k % protoNum] << "-Node: " << k;
//        app->GetStats();
//    }


    return 0;
}
