//
// Created by zhaoliang on 17/09/22.
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
    topologyReader.SetFileName("topo/topo-6-node.txt");
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


    Ptr<Node> consumer1 = Names::Find<Node>("Src1");
    Ptr<Node> consumer2 = Names::Find<Node>("Src2");

//    Ptr<Node> producer1 = Names::Find<Node>("Dst1");
//    Ptr<Node> producer2 = Names::Find<Node>("Dst2");


    // Producer
//    AppHelper producerHelper("ns3::ndn::DashServer");
//    producerHelper.SetPrefix("/Producer1");
//    producerHelper.SetAttribute("DashServerPayloadSize", StringValue("8000"));
//    producerHelper.Install(producer1);
//    producerHelper.SetPrefix("/Producer2");
//    producerHelper.Install(producer2);

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
    consumerHelper.SetAttribute("ConsumerId", StringValue("1"));



    ApplicationContainer  consumerapp1 = consumerHelper.Install(consumer1);
    consumerapp1.Start(Seconds(0));
    consumerapp1.Stop(Seconds(SCENARIOTIME));

    consumerHelper.SetAttribute("ConsumerId", StringValue("2"));

    ApplicationContainer  consumerapp2 = consumerHelper.Install(consumer2);
    consumerapp2.Start(Seconds(0));
    consumerapp2.Stop(Seconds(SCENARIOTIME));

////    Consumer application
//    AppHelper consumerHelper2(DASH_CLIENT_TYPE);
//
////    consumerHelper.SetPrefix("/Prefix");
//    consumerHelper2.SetAttribute("VideoId", StringValue("1"));
//    consumerHelper2.SetAttribute("NumberOfContents", StringValue(CONTENT_NUMBER_STR));
//    consumerHelper2.SetAttribute("MeanParameter", DoubleValue(mean));
//
//    ApplicationContainer  consumerapp2 = consumerHelper2.Install(consumer2);
//    consumerapp2.Start(Seconds(0));
//    consumerapp2.Stop(Seconds(SCENARIOTIME));
//
//  consumerHelper.SetAttribute("Frequency", StringValue("10")); // 10 interests a second
////    consumerHelper.Install(consumer1);                           // first node
//    consumerHelper.Install(consumer2);                           // first node
//
//
    GlobalRoutingHelper::CalculateRoutes();
    Simulator::Stop(Seconds(SCENARIOTIME));

    // L3RateTracer::InstallAll("/Users/zhaoliang/ndnSIM/my-simulations/results/rate-trace.txt", Seconds(0.5));
    // CsTracer::InstallAll("/home/zhaoliang/Documents/ndnSIM/ndnSIM-scenario-template/results/cs-trace.txt", Seconds(1));
    // L2RateTracer::InstallAll("drop-trace.txt", Seconds(0.5));
    // ndn::AppDelayTracer::InstallAll("app-delays-trace.txt");

    Simulator::Run();
    Simulator::Destroy();



    return 0;
}
