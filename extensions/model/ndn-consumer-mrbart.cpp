/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2011-2015  Regents of the University of California.
 *
 * This file is part of ndnSIM. See AUTHORS for complete list of ndnSIM authors and
 * contributors.
 *
 * ndnSIM is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * ndnSIM is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ndnSIM, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 **/

#include "ndn-consumer-mrbart.hpp"
#include "ns3/ptr.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/packet.h"
#include "ns3/callback.h"
#include "ns3/string.h"
#include "ns3/boolean.h"
#include "ns3/uinteger.h"
#include "ns3/integer.h"
#include "ns3/double.h"


#include <ndn-cxx/lp/tags.hpp>

#ifndef NDN_PAYLOAD_SIZE
#define NDN_PAYLOAD_SIZE 8000
#endif

NS_LOG_COMPONENT_DEFINE("ndn.ConsumerMrbart");

namespace ns3 {
namespace ndn {

NS_OBJECT_ENSURE_REGISTERED(ConsumerMrbart);

TypeId
ConsumerMrbart::GetTypeId(void)
{
  static TypeId tid =
    TypeId("ns3::ndn::ConsumerMrbart")
      .SetGroupName("Ndn")
      .SetParent<Consumer>()
      .AddConstructor<ConsumerMrbart>()

      .AddAttribute("Frequency", "Frequency of interest packets", StringValue("1.0"),
                    MakeDoubleAccessor(&ConsumerMrbart::m_frequency), MakeDoubleChecker<double>())

      .AddAttribute("Randomize",
                    "Type of send time randomization: none (default), uniform, exponential",
                    StringValue("none"),
                    MakeStringAccessor(&ConsumerMrbart::SetRandomize, &ConsumerMrbart::GetRandomize),
                    MakeStringChecker())

      .AddAttribute("MaxSeq", "Maximum sequence number to request",
                    IntegerValue(std::numeric_limits<uint32_t>::max()),
                    MakeIntegerAccessor(&ConsumerMrbart::m_seqMax), MakeIntegerChecker<uint32_t>())

    ;

  return tid;
}

ConsumerMrbart::ConsumerMrbart()
  : m_frequency(1.0)
  , m_firstTime(true)
  , m_counter(0)
  , m_initial(true)
  , m_inflight(0.0)
  , m_minrtt(Seconds(999))
{
  NS_LOG_FUNCTION_NOARGS();
  m_seqMax = std::numeric_limits<uint32_t>::max();
  m_ips = CreateObject<InterpacketStrainEstimator>();
  m_kf = CreateObject<KalmanFilter>();
}

ConsumerMrbart::~ConsumerMrbart()
{
}

void
ConsumerMrbart::ScheduleNextPacket()
{
  NS_LOG_FUNCTION_NOARGS();

  // double mean = 8.0 * m_payloadSize / m_desiredRate.GetBitRate ();
  // std::cout << "next: " << Simulator::Now().ToDouble(Time::S) << "s\n";
  if (m_firstTime) {
    m_sendEvent = Simulator::Schedule(Seconds(0.0), &ConsumerMrbart::SendPacket, this);
    m_firstTime = false;
  }
  else if (!m_sendEvent.IsRunning())
    m_sendEvent = Simulator::Schedule((m_random == 0) ? Seconds(1.0 / m_frequency)
                                                      : Seconds(m_random->GetValue()),
                                      &ConsumerMrbart::SendPacket, this);
}

void
ConsumerMrbart::SetRandomize(const std::string& value)
{
  // if (value == "uniform") {
  //   m_random = CreateObject<UniformRandomVariable>();
  //   m_random->SetAttribute("Min", DoubleValue(0.0));
  //   m_random->SetAttribute("Max", DoubleValue(2 * 1.0 / m_frequency));
  // }
  // else if (value == "exponential") {
  //   m_random = CreateObject<ExponentialRandomVariable>();
  //   m_random->SetAttribute("Mean", DoubleValue(1.0 / m_frequency));
  //   m_random->SetAttribute("Bound", DoubleValue(50 * 1.0 / m_frequency));
  // }
  // else
    m_random = 0;
  // m_random = CreateObject<NormalRandomVariable>();
  // m_random->SetAttribute("Mean", DoubleValue(1.0 / m_frequency));
  // m_random->SetAttribute ("Variance", DoubleValue (2));
  // m_randomType = value;
}

std::string
ConsumerMrbart::GetRandomize() const
{
  return m_randomType;
}

void
ConsumerMrbart::SendPacket()
{
  NS_LOG_FUNCTION_NOARGS();

  if(!m_initial){
    // std::cout << "BDP=" << m_kf->GetEstimatedBandwidth() * m_minrtt.GetSeconds() <<"inflight= " <<m_inflight<<'\n';

    if(m_inflight >= m_kf->GetEstimatedBandwidth() * m_minrtt.GetSeconds()){
      // std::cout << "BDP=" << m_kf->GetEstimatedBandwidth() * m_minrtt.GetSeconds() <<"inflight= " <<m_inflight<<'\n';

      ScheduleNextPacket();
      return;
    }
  }
  m_inflight += 0.008;
  Consumer::SendPacket();

}

void
ConsumerMrbart::OnData(shared_ptr<const Data> data)
{
  NS_LOG_FUNCTION_NOARGS();

  Consumer::OnData(data);
  // float gain[8] = {1.25,0.75,1,1,1,1,1,1};
  m_inflight -= 0.008;
  if(m_rtt->GetCurrentEstimate() < m_minrtt){
    m_minrtt = m_rtt->GetCurrentEstimate();
  }
  int cycleindex = 0;

  uint32_t seq = data->getName().at(-1).toSequenceNumber();
  double ips = 0;
  double ebw;
  float freqGain = 1.3;
  float ipsThreshold = 0.2;
  int cyclesteps = 8;
  double ipsavg;
  double ipsstdev;
  if (ips == -1) {
    return;
  }
  if(m_counter < cyclesteps){
    // m_ipsvec.push_back(ips);
    m_counter += 1;
    return;
  }else{
    ipsavg = m_ips->AckSeq(SequenceNumber32(seq));
  //   ipsavg = std::accumulate(m_ipsvec.begin(),m_ipsvec.end(),0.0)/m_ipsvec.size(); // calculate the average of ips
  //   double sq_sum = std::inner_product(m_ipsvec.begin(), m_ipsvec.end(), m_ipsvec.begin(), 0.0);
  //   ipsstdev = sq_sum / m_ipsvec.size() - ipsavg * ipsavg; // calculate the stdev of ips
  //   m_ipsvec.clear();
    m_counter = 0;
  }

  cout << Simulator::Now ().GetSeconds() <<"\t" << m_ips->GetU() <<"\t"<<  ipsavg << endl;
  // cout << Simulator::Now ().GetSeconds() <<"\t" << m_ips->GetU()<< endl;;


  if (m_initial){
    if(ipsavg > ipsThreshold){
      m_initial = false;
      m_kf->Init_KalmanInfo(freqToRate(m_frequency));
      ebw = freqToRate(m_frequency);

      // std::cout << "estimated bw= "<< m_kf->GetEstimatedBandwidth() << '\n';
      // m_frequency = m_kf->GetEstimatedBandwidth() / (8.0 * 0.008);
      // std::cout << "frequency=  "<<  m_frequency << '\n';
    }else{
      m_kf->Measurement(m_ips->GetU(),ipsavg);
      m_frequency *= freqGain;
      ebw = freqToRate(m_frequency);
      NS_LOG_INFO("initial phrase: frequency increased " << m_frequency << " ipsavg=" << ipsavg);
    }
  }
  else{
    m_kf->Measurement(m_ips->GetU(),ipsavg);

    if(ipsavg < ipsThreshold){
      m_frequency =  freqGain * rateToFreq(m_kf->GetEstimatedBandwidth()) ;
    }else{
      // m_frequency = gain[cycleindex%8] * m_kf->GetEstimatedBandwidth() / (8.0 * 0.008);
      m_frequency =  rateToFreq(m_kf->GetEstimatedBandwidth()) ;
      // cycleindex +=1;
    }
    ebw = m_kf->GetEstimatedBandwidth();

    NS_LOG_INFO("main phrase: frequency " << m_frequency << " InterPacketStrain " << ipsavg << "estimated bw= "<< m_kf->GetEstimatedBandwidth());

  }
  // cout << Simulator::Now ().GetSeconds() << "\t" <<  ebw << endl;
}

// void
// ConsumerMrbart::sendPacketTrain(){
//   for (Trains::const_iterator i = m_trains.begin(); i != m_trains.end(); i++) {
//     Simulator::ScheduleWithContext(GetNode()->GetId(), std::get<0>(*i), &ConsumerMrbart::AddBatch,
//                                    this, std::get<1>(*i));
//   }
// }

void
ConsumerMrbart::sendPacketTrain(uint32_t amount, double frequency)
{
  // std::cout << Simulator::Now () << " adding batch of " << amount << "\n";
  m_frequency  = frequency;
  m_seqMax += amount;
  m_rtt->ClearSent(); // this is important, otherwise RTT estimation for the new batch will be
                      // affected by previous batch history
  m_initial = true;
  ScheduleNextPacket();
}


void
ConsumerMrbart::StartApplication()
{
  Consumer::StartApplication();

  // std::cout << "Batches: " << batches << "\n";
  // sendPacketTrain(100, 100);
}

void
ConsumerMrbart::WillSendOutInterest(uint32_t sequenceNumber)
{
  Consumer::WillSendOutInterest(sequenceNumber);
  NS_LOG_INFO(Simulator::Now().GetMilliSeconds() << " Interest for " << sequenceNumber);
  m_ips->SentSeq(SequenceNumber32(sequenceNumber), NDN_PAYLOAD_SIZE);
}

double
ConsumerMrbart::freqToRate(double freq){
  return freq * 8 * 0.008;
}

double
ConsumerMrbart::rateToFreq(double rate){
  return rate / (8*0.008);
}




} // namespace ndn
} // namespace ns3
