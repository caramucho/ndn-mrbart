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

#include "ns3/log.h"
#include "ns3/nstime.h"
#include "ns3/simulator.h"
#include "http-header.h"
#include "mpeg-header.h"
#include "mpeg-player.h"
// #include "dash-client.h"
#include <cmath>

NS_LOG_COMPONENT_DEFINE("MpegPlayer");
namespace ns3
{
  namespace ndn
  {

  MpegPlayer::MpegPlayer() :
      m_state(MPEG_PLAYER_NOT_STARTED), m_interrruptions(0), m_totalRate(0), m_minRate(
          100000000), m_framesPlayed(0), m_bufferDelay("0s"), m_making_segment(false)
  {
    NS_LOG_FUNCTION(this);
  }

  MpegPlayer::~MpegPlayer()
  {
    NS_LOG_FUNCTION(this);
  }

  int
  MpegPlayer::GetQueueSize()
  {
    return m_queue.size();
  }

  Time
  MpegPlayer::GetRealPlayTime(Time playTime)
  {
//    NS_LOG_INFO(
//        " Start: " << m_start_time.GetSeconds() << " Inter: " << m_interruption_time.GetSeconds() << " playtime: " << playTime.GetSeconds() << " now: " << Simulator::Now().GetSeconds() << " actual: " << (m_start_time + m_interruption_time + playTime).GetSeconds());

    return m_start_time + m_interruption_time
        + (m_state == MPEG_PLAYER_PAUSED ?
            (Simulator::Now() - m_lastpaused) : Seconds(0)) + playTime
        - Simulator::Now();
  }

  void
  MpegPlayer::ReceiveFrame(Ptr<Packet> message)
  {
//    NS_LOG_FUNCTION(this << message);
//    NS_LOG_INFO("Received Frame " << m_state);

    Ptr<Packet> msg = message->Copy();
    Ptr<Packet> msg2 = message->Copy();
    MPEGHeader mpeg_header;
    message->RemoveHeader(mpeg_header);
    m_currDt = GetRealPlayTime(mpeg_header.GetPlaybackTime());

    m_queue.push(msg);
    if (m_state == MPEG_PLAYER_PAUSED)
      {
//        NS_LOG_INFO("Play resumed");
        m_state = MPEG_PLAYER_PLAYING;
        m_interruption_time += (Simulator::Now() - m_lastpaused);
        PlayFrame();
      }
    else if (m_state == MPEG_PLAYER_NOT_STARTED)
      {
//        NS_LOG_INFO("Play started");
        m_state = MPEG_PLAYER_PLAYING;
        m_start_time = Simulator::Now();
        Simulator::Schedule(Simulator::Now(), &MpegPlayer::PlayFrame, this);
      }
  }

  void
  MpegPlayer::Start(void)
  {
//    NS_LOG_FUNCTION(this);
    m_state = MPEG_PLAYER_PLAYING;
    m_interruption_time = Seconds(0);

  }

  void
  MpegPlayer::PlayFrame(void)
  {
//    NS_LOG_FUNCTION(this);
    if (m_state == MPEG_PLAYER_DONE)
      {
        return;
      }
    if (m_queue.empty())
      {
        NS_LOG_INFO(Simulator::Now().GetSeconds() << " No frames to play");
        m_state = MPEG_PLAYER_PAUSED;
        m_lastpaused = Simulator::Now();
        m_interrruptions++;
        return;
      }
    Ptr<Packet> message = m_queue.front();
    m_queue.pop();

    MPEGHeader mpeg_header;
    HTTPHeader http_header;

    message->RemoveHeader(mpeg_header);
    message->RemoveHeader(http_header);

    m_totalRate += http_header.GetResolution();
    if (http_header.GetSegmentId() > 0) // Discard the first segment for the minRate
      {                                 // calculation, as it is always the minimum rate
        m_minRate =
            http_header.GetResolution() < m_minRate ?
                http_header.GetResolution() : m_minRate;
      }
    m_framesPlayed++;

    /*std::cerr << "res= " << http_header.GetResolution() << " tot="
     << m_totalRate << " played=" << m_framesPlayed << std::endl;*/

    Time b_t = GetRealPlayTime(mpeg_header.GetPlaybackTime());

    if (m_bufferDelay > Time("0s") && b_t < m_bufferDelay && m_dashClient)
      {
        // m_dashClient->RequestSegment();
        m_bufferDelay = Seconds(0);
        m_dashClient = NULL;
      }

//    NS_LOG_INFO(
//        Simulator::Now().GetSeconds() << " PLAYING FRAME: " << " VidId: " << http_header.GetVideoId() << " SegId: " << http_header.GetSegmentId() << " Res: " << http_header.GetResolution() << " FrameId: " << mpeg_header.GetFrameId() << " PlayTime: " << mpeg_header.GetPlaybackTime().GetSeconds() << " Type: " << (char) mpeg_header.GetType() << " interTime: " << m_interruption_time.GetSeconds() << " queueLength: " << m_queue.size());

    // std::cout << " frId: " << mpeg_header.GetFrameId()
    //  << " playtime: " << mpeg_header.GetPlaybackTime()
    //  << " target: " << (m_start_time + m_interruption_time + mpeg_header.GetPlaybackTime()).GetSeconds()
    //  << " now: " << Simulator::Now().GetSeconds()
    //  << std::endl;

    Simulator::Schedule(MilliSeconds(20), &MpegPlayer::PlayFrame, this);

  }

  void
  MpegPlayer::ReceiveData(uint32_t resolution , uint32_t segment_id)
  {
    if(m_buffer.count(segment_id) != 0) {
      get<0>(m_buffer[segment_id]) += 8000;
    }else{
      m_buffer[segment_id] = make_pair(8000,0);
    }
    // std::cout << "buffer" << get<0>(m_buffer[segment_id])<< '\n';
    if(!m_making_segment){
      MakeSegment(resolution, segment_id);
    }
  }

  void
  MpegPlayer::MakeSegment(uint32_t resolution, uint32_t segment_id)
  {
    m_making_segment = true;
    int avg_packetsize = resolution / (50 * 8);
    HTTPHeader http_header_tmp;
    MPEGHeader mpeg_header_tmp;

    Ptr<UniformRandomVariable> frame_size_gen = CreateObject<UniformRandomVariable> ();

    frame_size_gen->SetAttribute ("Min", DoubleValue (0));
    frame_size_gen->SetAttribute ("Max", DoubleValue (
        std::max(
            std::min(2 * avg_packetsize, MPEG_MAX_MESSAGE)
                - (int) (mpeg_header_tmp.GetSerializedSize()
                    + http_header_tmp.GetSerializedSize()
                  ), 1)));
    uint32_t frame_size = (unsigned) frame_size_gen->GetValue() + (int) mpeg_header_tmp.GetSerializedSize();
    int& buffer = get<0>(m_buffer[segment_id]);
    uint32_t& fid = get<1>(m_buffer[segment_id]);

    while (((int)buffer - (int)frame_size) > 0)
    {
        if(fid >= MPEG_FRAMES_PER_SEGMENT){
          // fid = 0;
          // segment_id++;
          // m_buffer[segment_id] = 0;
          break;
        }
        // if (buffer - frame_size < 0){
        //   break;
        // }
        HTTPHeader http_header;
        http_header.SetMessageType(HTTP_RESPONSE);
        // http_header.SetVideoId(video_id);
        http_header.SetVideoId(1);
        http_header.SetResolution(resolution);
        http_header.SetSegmentId(segment_id);

        MPEGHeader mpeg_header;
        mpeg_header.SetFrameId(fid);
        mpeg_header.SetPlaybackTime(
            MilliSeconds(
                (fid + (segment_id * MPEG_FRAMES_PER_SEGMENT))
                    * MPEG_TIME_BETWEEN_FRAMES)); //50 fps
        mpeg_header.SetType('B');
        mpeg_header.SetSize(frame_size);

        Ptr<Packet> frame = Create<Packet>(frame_size);
        // frame->AddHeader(http_header);
        frame->AddHeader(mpeg_header);
        NS_LOG_INFO(
            "SENDING PACKET " << fid << " " << frame->GetSize() << " res=" << http_header.GetResolution() << " size=" << mpeg_header.GetSize() << " avg=" << avg_packetsize);
        ReceiveFrame(frame);
        fid++;
        buffer -= frame_size;
        frame_size = (unsigned) frame_size_gen->GetValue() + (int) mpeg_header_tmp.GetSerializedSize();

      }
      m_making_segment = false;
      // m_remained_buffer = buffer;
    // DataSend(socket, 0);
  }

  Time
  MpegPlayer::GetCurrDt(){
    return m_currDt;
  }
}
} // namespace ns3
