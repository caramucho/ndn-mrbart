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
#include <ns3/ndnSIM-module.h>


namespace ns3
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
    uint8_t m_buffer[MPEG_MAX_MESSAGE];
    uint32_t m_bytes;
    DashClient *m_app;

    Time m_lastmeasurement;

  };

} // namespace ns3

#endif /* HTTP_PARSER_H_ */
