/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
* Copyright (c) 2014 TEI of Western Macedonia, Greece
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 as
* published by the Free Software Foundation;
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* Author: Dimitrios J. Vergados <djvergad@gmail.com>
*/

// #include <ns3/log.h>
// #include <ns3/uinteger.h>
// #include <ns3/tcp-socket-factory.h>
// #include <ns3/simulator.h>
// #include <ns3/inet-socket-address.h>
// #include <ns3/inet6-socket-address.h>
// #include "http-header.h"

#include "dash-client.h"
#include "../dash-parameters.h"
#include <ndn-cxx/lp/tags.hpp>



NS_LOG_COMPONENT_DEFINE("ndn.DashClient");

namespace ns3{
  namespace ndn{


    NS_OBJECT_ENSURE_REGISTERED(DashClient);

    int DashClient::m_countObjs = 0;

    TypeId
    DashClient::GetTypeId(void)
    {
      static TypeId tid =
      TypeId("ns3::ndn::DashClient")
      .SetGroupName("Ndn")
      .SetParent<Consumer>()
      .AddConstructor<DashClient>()
      .AddAttribute("VideoId","The Id of the video that is played.", UintegerValue(0),MakeUintegerAccessor(&DashClient::m_videoId),MakeUintegerChecker<uint32_t>(1))
      .AddAttribute("TargetDt", "The target buffering time", TimeValue(Time("35s")),MakeTimeAccessor(&DashClient::m_target_dt), MakeTimeChecker())
      .AddAttribute("MeanParameter","The mean parameter", DoubleValue(1.0),MakeDoubleAccessor(&DashClient::m_mean_parameter),MakeDoubleChecker<double>())
      .AddAttribute("window", "The window for measuring the average throughput (Time)",TimeValue(Time("10s")), MakeTimeAccessor(&DashClient::m_window),MakeTimeChecker())
      .AddTraceSource("Tx", "A new packet is created and is sent",MakeTraceSourceAccessor(&DashClient::m_txTrace));
      return tid;
    }

    DashClient::DashClient() :
    m_rateChanges(0),
    m_target_dt("3.5s"),
    m_bitrateEstimate(0.0),
    m_segmentId(0),
    m_videoId(1),
    m_bitRate(INIT_BITRATE),
    m_producerDomain(ORIGIN_DOMAIN),
    m_totBytes(0),
    m_startedReceiving(Seconds(0)),
    m_sumDt(Seconds(0)),
    m_lastDt(Seconds(-1)),
    m_id(m_countObjs++),
    m_requestTime("0s"),
    m_segment_bytes(0),
    m_window(Seconds(10)),
    m_segmentFetchTime(Seconds(0)),
    m_segmentLength("2s"),
    m_firstTime(true),
    // m_payloadSize(::ndn::MAX_NDN_PACKET_SIZE),
    m_payloadSize(NDN_PAYLOAD_SIZE),
    m_seqMax(0),
    m_adaptationSetId(1),
    m_periodId(1)
    {
      // m_rtt = CreateObject<RttMeanDeviation>();
      NS_LOG_FUNCTION(this);
      m_parser.SetApp(this); // So the parser knows where to send the received messages
    }

    DashClient::~DashClient()
    {
      NS_LOG_FUNCTION(this);
    }

    // Application Methods
    void
    DashClient::StartApplication() // Called at time specified by Start
    {
      NS_LOG_FUNCTION_NOARGS();

      // do base stuff
      App::StartApplication();
      // request the first segment
      RequestSegment();
    }
    // Private helpers
    // Request Next Segment
    void
    DashClient::RequestSegment()
    {
      // if (m_seq > m_seqMax + 1) {
      //   return; // we are totally done
      // }

      m_seq = 0;
      CalcSeqMax();
      m_requestTime = Simulator::Now(); // the time to request the first packet of the segment
      m_segment_bytes = 0;
      m_segmentId++;
      SetInterestName();
      m_firstTime = true;
      m_segmentFetchTime = Seconds(0);
      ScheduleNextPacket();
    }

    void
    DashClient::GetContentPopularity(){
      std::cout << "GetContentPopularity" << '\n';
    }


    void
    DashClient::SetInterestName()
    {
      DashName dashname;
      dashname.SetProducerDomain(m_producerDomain);
      dashname.SetVideoId(m_videoId);
      dashname.SetRepresentation(m_bitRate);
      dashname.SetAdaptationSetId(m_adaptationSetId);
      dashname.SetPeriodId(m_periodId);
      dashname.SetSegmentId(m_segmentId);
      m_interestName = dashname.GetInterestName();
    }


    void
    DashClient::SendPacket()
    {
      // cout << "SendPacket initilizing" << endl;
      // if (!m_active)
      // return;
      uint32_t seq = std::numeric_limits<uint32_t>::max(); // invalid
//      NS_LOG_FUNCTION_NOARGS();

      while (m_retxSeqs.size()) {
        seq = *m_retxSeqs.begin();
        m_retxSeqs.erase(m_retxSeqs.begin());
        break;
      }
      if (seq == std::numeric_limits<uint32_t>::max()) // No retxs
      {
        if (m_seq > m_seqMax){
          return;
        }
      }
      seq = m_seq;
      m_seq++;

      shared_ptr<Name> nameWithSequence = make_shared<Name>(m_interestName);
      nameWithSequence->appendSequenceNumber(seq);


      shared_ptr<Interest> interest = make_shared<Interest>();
      interest->setNonce(m_rand->GetValue(0, std::numeric_limits<uint32_t>::max()));
      interest->setName(*nameWithSequence);
      time::milliseconds interestLifeTime(m_interestLifeTime.GetMilliSeconds());
      interest->setInterestLifetime(interestLifeTime);
//
//        NS_LOG_INFO ("Requesting Interest: \n" << *interest);
//        NS_LOG_INFO("> Interest for " << seq);
//        NS_LOG_INFO ( Simulator::Now().GetSeconds() <<" sending "<< m_interestName.toUri() << "/"<< seq );

      WillSendOutInterest(seq);

      m_transmittedInterests(interest, this, m_face);
      // send out interest
      m_appLink->onReceiveInterest(*interest);

      ScheduleNextPacket();
    }

      void
      DashClient::ScheduleNextPacket()
      {
          // cout << "ScheduleNextPacket initilizing" << endl;
//          cout << "mean parameter=" <<m_mean_parameter << endl;
          double mean =  m_mean_parameter * 8.0 * m_payloadSize / m_bitRate;
          // std::cout << "next: " << Simulator::Now().ToDouble(Time::S) + mean << "s\n";

          if (m_firstTime) {
              m_sendEvent = Simulator::Schedule(Seconds(0.0), &DashClient::SendPacket, this);
              m_firstTime = false;
          }
          else if (!m_sendEvent.IsRunning()){
              m_sendEvent = Simulator::Schedule(Seconds(mean), &DashClient::SendPacket, this);
          }else {
              // cout << "Sending event is busy" << endl;
          }
      }


    void
    DashClient::OnData(shared_ptr<const Data> data)
    {

      // if (!m_active)
      // return;
      // cout << "OnData initilizing" << endl;
      // Consumer::OnData(data);

      m_parser.OnData(data);
      m_segment_bytes += m_payloadSize;
      uint32_t seq = data->getName().at(-1).toSequenceNumber();


      int hopCount = 0;
      auto hopCountTag = data->getTag<lp::HopCountTag>();
      if (hopCountTag != nullptr) { // e.g., packet came from local node's cache
        hopCount = *hopCountTag;
      }
      // NS_LOG_DEBUG("Hop count: " << hopCount);

      SeqTimeoutsContainer::iterator entry = m_seqLastDelay.find(seq);
      if (entry != m_seqLastDelay.end()) {
        m_lastRetransmittedInterestDataDelay(this, seq, Simulator::Now() - entry->time, hopCount);
      }

      entry = m_seqFullDelay.find(seq);
      if (entry != m_seqFullDelay.end()) {
        m_firstInterestDataDelay(this, seq, Simulator::Now() - entry->time, m_seqRetxCounts[seq], hopCount);
        // std::cout << "seq: " << seq << " FullDelay:" <<  (Simulator::Now() - entry->time).GetSeconds() << '\n';
        m_segmentFetchTime += Simulator::Now() - entry->time;
      }

      m_seqRetxCounts.erase(seq);
      m_seqFullDelay.erase(seq);
      m_seqLastDelay.erase(seq);

      m_seqTimeouts.erase(seq);
      m_retxSeqs.erase(seq);

      m_rtt->AckSeq(SequenceNumber32(seq));

      // If we received the last packet of the segment
        if (seq == m_seqMax)
        {

            // m_segmentFetchTime = Simulator::Now() - m_requestTime;


//        NS_LOG_INFO(
//         Simulator::Now().GetSeconds() << " bytes: " << m_segment_bytes << " segmentTime: " << m_segmentFetchTime.GetSeconds() << " segmentAvgRate: " << 0.5 * 8 * m_segment_bytes / m_segmentFetchTime.GetSeconds());
            cout << Simulator::Now().GetSeconds() << "\t" << m_segmentFetchTime.GetSeconds() << "\t" <<  0.5 * 8 * m_segment_bytes / m_segmentFetchTime.GetSeconds() << "\t" << m_bitRate <<endl;
            // Feed the bitrate info to the player
            AddBitRate(Simulator::Now(),
                       8 * m_segment_bytes / m_segmentFetchTime.GetSeconds());

            // Time currDt = m_player.GetRealPlayTime(mpegHeader.GetPlaybackTime());
            // // And tell the player to monitor the buffer level
            // LogBufferLevel(currDt);

            // uint32_t old = m_bitRate;
            // double diff = m_lastDt >= 0 ? (currDt - m_lastDt).GetSeconds() : 0;

            Time bufferDelay;

            //m_player.CalcNextSegment(m_bitRate, m_player.GetBufferEstimate(), diff,
            //m_bitRate, bufferDelay);

            uint32_t prevBitrate = m_bitRate;

            CalcNextSegment(prevBitrate, m_bitRate, bufferDelay);
            // @Todo calculate next m_seqMax

            if (prevBitrate != m_bitRate)
            {
                NS_LOG_INFO( "Bitrate Adaptation:    " << prevBitrate <<"->"<<m_bitRate );
//             cout << Simulator::Now().GetSeconds() << "\t" << prevBitrate << endl;
//             cout << Simulator::Now().GetSeconds() << "\t" << m_bitRate << endl;

                m_rateChanges++;
            }

            if (bufferDelay == Seconds(0))
            {
                RequestSegment();
            }
            else
            {
                m_player.SchduleBufferWakeup(bufferDelay, this);
            }


        //  m_lastDt = currDt;

        // RequestSegment();
      }

    }


    void
    DashClient::CalcNextSegment(uint32_t currRate, uint32_t & nextRate,Time & delay)
    {
      nextRate = currRate;
      delay = Seconds(0);
    }

    void
    DashClient::GetStats()
    {
      std::cout << " InterruptionTime: "
          << m_player.m_interruption_time.GetSeconds() << " interruptions: "
          << m_player.m_interrruptions << " avgRate: "
          << (1.0 * m_player.m_totalRate) / m_player.m_framesPlayed
          << " minRate: " << m_player.m_minRate << " AvgDt: "
          << m_sumDt.GetSeconds() / m_player.m_framesPlayed << " changes: "
          << m_rateChanges << std::endl;

    }


    void
    DashClient::LogBufferLevel(Time t)
    {
      m_bufferState[Simulator::Now()] = t;
      for (std::map<Time, Time>::iterator it = m_bufferState.begin();
      it != m_bufferState.end(); ++it)
      {
        if (it->first < (Simulator::Now() - m_window))
        {
          m_bufferState.erase(it->first);
        }
      }
    }

    double
    DashClient::GetBufferEstimate()
    {
      double sum = 0;
      int count = 0;
      for (std::map<Time, Time>::iterator it = m_bufferState.begin();
      it != m_bufferState.end(); ++it)
      {
        sum += it->second.GetSeconds();
        count++;
      }
      return sum / count;
    }

    double
    DashClient::GetBufferDifferential()
    {
      std::map<Time, Time>::iterator it = m_bufferState.end();

      if (it == m_bufferState.begin())
      {
        // Empty buffer
        return 0;
      }
      it--;
      Time last = it->second;

      if (it == m_bufferState.begin())
      {
        // Only one element
        return 0;
      }
      it--;
      Time prev = it->second;
      return (last - prev).GetSeconds();
    }

    double
    DashClient::GetSegmentFetchTime()
    {
      return m_segmentFetchTime.GetSeconds();
    }

    void
    DashClient::AddBitRate(Time time, double bitrate)
    {
      m_bitrates[time] = bitrate;
      double sum = 0;
      int count = 0;
      for (std::map<Time, double>::iterator it = m_bitrates.begin();
      it != m_bitrates.end(); ++it)
      {
        if (it->first < (Simulator::Now() - m_window))
        {
          m_bitrates.erase(it->first);
        }
        else
        {
          sum += it->second;
          count++;
        }
      }
      m_bitrateEstimate = sum / count;
    }

    void
    DashClient::CalcSeqMax(){
      m_seqMax =  m_bitRate * m_segmentLength.GetSeconds()  / (m_payloadSize * 8);
      if (((uint32_t)(m_bitRate * m_segmentLength.GetSeconds()) % (m_payloadSize * 8)) == 0 ) {
        m_seqMax = m_seqMax - 1;
      }


    }

  } // Namespace ndn
} // Namespace ns3
