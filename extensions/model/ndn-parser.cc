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
#include "ns3/log.h"
#include "ns3/address.h"
#include "ns3/socket.h"
#include "ns3/packet.h"
#include "ns3/simulator.h"

#include "mpeg-header.h"
#include "dash-client.h"


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
    NdnParser::readContent(::ndn::Buffer::const_iterator begin,uint8_t* buffer,uint32_t bytes){
      for(uint32_t bytesReaded = 0; bytesReaded < bytes; bytesReaded++){
        uint8_t v;
        v = *begin;
        buffer[bytesReaded] = v;
        begin++;
      }
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
    NdnParser::readAllFrames(){
      MPEGHeader mpeg_header;
      uint32_t headersize = mpeg_header.GetSerializedSize();
      NS_LOG_INFO("### Buffer space: " << m_bytes << " Queue length " << m_app->GetPlayer().GetQueueSize());

      if (m_bytes < headersize)
      {
        return;
      }

      Packet headerPacket(m_buffer, headersize);
      headerPacket.RemoveHeader(mpeg_header);

      uint32_t message_size = headersize + mpeg_header.GetSize();

      // if (m_bytes < message_size)
      // {
      //   return;
      // }
      while (m_bytes >= message_size) {
        /* code */
        readFrame(message_size);
        m_app->m_segment_bytes += mpeg_header.GetSize();
        m_app->m_totBytes += mpeg_header.GetSize();
      }
    }

    void
    NdnParser::readFrame(uint32_t message_size){


      Packet message(m_buffer, message_size);
      // Add the httpHear again
      message.AddHeader(http_header);
      //recalculate headersize and message_size
      // headersize = mpeg_header.GetSerializedSize()
      //  + http_header.GetSerializedSize();
      // message_size = headersize + mpeg_header.GetSize();

      memmove(m_buffer, &m_buffer[message_size], m_bytes - message_size);
      m_bytes -= message_size;

      m_app->m_player.ReceiveFrame(&message);
    }

    void
    NdnParser::OnData(shared_ptr<const Data> data)
    {
      // NS_LOG_FUNCTION(this << socket);
      // Address from;

      // int bytes = socket->RecvFrom(&m_buffer[m_bytes], MPEG_MAX_MESSAGE - m_bytes, 0, from);
      uint32_t bytes = data->getContent().value_size();
      ::ndn::Buffer::const_iterator i = data->getContent().value_begin();
      readContent(i,&m_buffer[m_bytes],bytes);

      DashName dashname;
      dashname.parseName(data->getName());

      makeHTTPheader(dashname);
      // HTTPHeader http_header;
      // + http_header.GetSerializedSize();
      if (bytes > 0)
      {
        m_bytes += bytes;

        if (m_lastmeasurement > Time("0s"))
        {
          // NS_LOG_INFO(Simulator::Now().GetSeconds() << " bytes: " << bytes << " dt: " << (Simulator::Now() - m_lastmeasurement).GetSeconds() << " bitrate: " << (8 * (bytes + headersize)/ (Simulator::Now() - m_lastmeasurement).GetSeconds()));
        }
        m_lastmeasurement = Simulator::Now();
      }
      readAllFrames();

    }
  } // namespace ndn
} // namespace ns3
