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

#ifndef NDN_PARSER_H_
#define NDN_PARSER_H_

#include <ns3/ptr.h>
#include "mpeg-header.h"
#include "http-header.h"
#include <ns3/ndnSIM-module.h>
#include "dash-name.h"

namespace ns3
{
  namespace ndn
  {

    // class Socket;
    class DashClient;

    class NdnParser
    {
    public:
      NdnParser();
      virtual
      ~NdnParser();
      void
      OnData(shared_ptr<const Data> data);
      void
      SetApp(DashClient *app);


    private:
      uint32_t
      getMessageSize();
      void
      makeHTTPheader(DashName name);
      void
      readContent(::ndn::Buffer::const_iterator begin,uint8_t* buffer,uint32_t bytes);
      void
      readAllFrames();
      void
      readFrame(uint32_t message_size);

      uint8_t m_buffer[MPEG_MAX_MESSAGE * 50];
      uint32_t m_bytes;
      DashClient *m_app;
      HTTPHeader http_header;
      MPEGHeader mpeg_header;
      Time m_lastmeasurement;

    };
  } // namespace ndn
} // namespace ns3

#endif /* HTTP_PARSER_H_ */
