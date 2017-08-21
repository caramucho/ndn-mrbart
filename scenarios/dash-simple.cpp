//
// Created by zhaoliang on 17/07/26.
//


#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"
#include "ns3/point-to-point-module.h"

#include <iostream>
#include "model/dash-client.h"
// #include "model/ndn-net-device-face.hpp"
#include "dash-parameters.h"
#include "scenario-parameters.h"

using namespace ns3;
using namespace ns3::ndn;
using namespace std;



int
main(int argc, char* argv[])
{

    int delay;
    double mean;
    CommandLine cmd;
    cmd.AddValue ("delay", "delay of link", delay);
    cmd.AddValue ("mean", "mean interval parameter", mean);
    cmd.Parse (argc, argv);


    // setting default parameters for PointToPoint links and channels
    Config::SetDefault("ns3::PointToPointNetDevice::DataRate", StringValue("1Mbps"));
    Config::SetDefault("ns3::PointToPointChannel::Delay", StringValue(to_string(delay)+"ms"));
    Config::SetDefault("ns3::DropTailQueue::MaxPackets", StringValue("20"));

    // Creating nodes
    NodeContainer nodes;
    nodes.Create(3);

    // Connecting nodes using two links
    PointToPointHelper p2p;
    p2p.Install(nodes.Get(0), nodes.Get(1));
    p2p.Install(nodes.Get(1), nodes.Get(2));


    // Install NDN stack on all nodes
    StackHelper ndnHelper;
    ndnHelper.setCsSize(CACHE_SIZE);
    ndnHelper.setPolicy("nfd::cs::lru");
    // ndnHelper.SetOldContentStore("ns3::ndn::cs::Lru", "MaxSize",
    // cacheSize); // ! Attention ! If set to 0, then MaxSize is infinite
    ndnHelper.SetDefaultRoutes(true);

    ndnHelper.InstallAll();


    StrategyChoiceHelper::InstallAll("/", "/localhost/nfd/strategy/best-route");


    // Producer
    AppHelper producerHelper("ns3::ndn::DashServer");
    producerHelper.SetPrefix("/Prefix");
//    producerHelper.SetAttribute("DashServerPayloadSize", StringValue("8000"));
    producerHelper.Install(nodes.Get(2));


    //Consumer application
    AppHelper consumerHelper(DASH_CLIENT_TYPE);
    consumerHelper.SetPrefix("/Prefix");
    consumerHelper.SetAttribute("VideoId", StringValue("1"));
    consumerHelper.SetAttribute("NumberOfContents", StringValue(CONTENT_NUMBER_STR));
    consumerHelper.SetAttribute("MeanParameter", DoubleValue(mean));


//  consumerHelper.SetAttribute("Frequency", StringValue("10")); // 10 interests a second
    consumerHelper.Install(nodes.Get(0));                           // first node

    Simulator::Stop(Seconds(SCENARIOTIME));

    // L3RateTracer::InstallAll("/Users/zhaoliang/ndnSIM/my-simulations/results/rate-trace.txt", Seconds(0.5));
    // CsTracer::InstallAll("/home/zhaoliang/Documents/ndnSIM/ndnSIM-scenario-template/results/cs-trace.txt", Seconds(1));
    // L2RateTracer::InstallAll("drop-trace.txt", Seconds(0.5));
    // ndn::AppDelayTracer::InstallAll("app-delays-trace.txt");

    Simulator::Run();
    Simulator::Destroy();



    return 0;
}



// int
// main(int argc, char* argv[])
// {
//   return ns3::main(argc, argv);
// }

