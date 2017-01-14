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

#include "ndn-parser.h"
#include "mpeg-header.h"
#include "dash-client.h"
#include "dash-content.h"

NS_LOG_COMPONENT_DEFINE("NdnParser");

namespace ns3
{
  namespace ndn
  {

    NdnParser::NdnParser() :
    m_bytes(0), m_app(NULL), m_lastmeasurement("0s")
    {
      NS_LOG_FUNCTION(this);
    }

    NdnParser::~NdnParser()
    {
      NS_LOG_FUNCTION(this);
    }

    void
    NdnParser::SetApp(DashClient *app)
    {
      NS_LOG_FUNCTION(this << app);
      m_app = app;
    }



    void
    NdnParser::makeHTTPheader(DashName name){
      http_header.SetSeq(1);
      http_header.SetMessageType(HTTP_REQUEST);
      http_header.SetVideoId(name.GetVideoId());
      http_header.SetResolution(name.GetRepresentation());
      http_header.SetSegmentId(name.GetSegmentId());
    }
    void
    NdnParser::makeMPEGheader(uint32_t frame_size){
      if (m_firstframe) {
        m_frame_id = 0;
        m_firstframe = false;
      }
      mpeg_header.SetFrameId(m_frame_id);
      mpeg_header.SetPlaybackTime(
          MilliSeconds(
              (m_frame_id + (m_segmentId * MPEG_FRAMES_PER_SEGMENT))
                  * MPEG_TIME_BETWEEN_FRAMES)); //50 fps
      mpeg_header.SetType('B');
      mpeg_header.SetSize(frame_size);
      m_frame_id++;
    }




    void
    NdnParser::readAllFrames(uint32_t frame_size){
      uint32_t message_size = frame_size + mpeg_header.GetSerializedSize();

      while (m_bytes >= message_size) {
        if (m_frame_id == MPEG_FRAMES_PER_SEGMENT) {
          break;
        }
        readFrame(frame_size);
        m_bytes -= message_size;
        m_app->m_segment_bytes += frame_size;
        m_app->m_totBytes += frame_size;
      }
    }

    void
    NdnParser::readFrame(uint32_t frame_size){
      Ptr<Packet> frame = Create<Packet>(frame_size);
      m_frame_id++;

      makeMPEGheader(frame_size);

      frame->AddHeader(http_header);
      frame->AddHeader(mpeg_header);
      m_app->m_player.ReceiveFrame(frame);
      switch (m_app->m_player.m_state)
      {
      case MPEG_PLAYER_PLAYING:
        m_app->m_sumDt += m_app->m_player.GetRealPlayTime(mpeg_header.GetPlaybackTime());
        break;
      case MPEG_PLAYER_PAUSED:
        break;
      case MPEG_PLAYER_DONE:
        return;
      default:
        NS_FATAL_ERROR("WRONG STATE");
      }
      // Time currDt = m_player.GetRealPlayTime(mpegHeader.GetPlaybackTime());
      // And tell the player to monitor the buffer level
      // LogBufferLevel(currDt);
    }

    void
    NdnParser::OnData(shared_ptr<const Data> data)
    {
      uint32_t bytes = data->getContent().value_size();
      // cout << "Bytes: "<< bytes << " received"<< endl;
      if (bytes > 0)
      {
        m_bytes += bytes;
      }
      uint32_t seq = data->getName().at(-1).toSequenceNumber();

      if (seq == 0) {
        m_firstframe = true;
      }



      DashName dashname;
      dashname.parseName(data->getName());
      m_segmentId = dashname.GetSegmentId();

      makeHTTPheader(dashname);
      DashContent dashcontent;
      uint32_t frame_size = dashcontent.GetFrameSize(dashname.GetRepresentation());
      readAllFrames(frame_size);

    }
  } // namespace ndn
} // namespace ns3
