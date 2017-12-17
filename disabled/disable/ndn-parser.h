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

#include "mpeg-header.h"
#include "http-header.h"
#include "dash-name.h"
// #include "dash-client.h"
// #include "mpeg-player.h"
// #include "dash-content.h"
#include <ns3/core-module.h>
#include <ns3/ndnSIM-module.h>

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
      void
      makeHTTPheader(DashName name);
      void
      makeMPEGheader(uint32_t frame_size);
      void
      readAllFrames(uint32_t frame_size);
      void
      readFrame(uint32_t frame_size);

      uint32_t m_bytes;
      DashClient *m_app;
      HTTPHeader http_header;
      MPEGHeader mpeg_header;
      Time m_lastmeasurement;
      bool m_firstframe;
      uint32_t m_frame_id;
      uint32_t m_segmentId;

    };
  } // namespace ndn
} // namespace ns3

#endif /* HTTP_PARSER_H_ */
