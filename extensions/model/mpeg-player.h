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

#ifndef MPEG_PLAYER_H_
#define MPEG_PLAYER_H_

#include <queue>
#include <map>
#include "ndn-dash-mrbart.hpp"
#include <ns3/core-module.h>
#include <ns3/ndnSIM-module.h>

namespace ns3
{
  namespace ndn
  {
  enum
  {
    MPEG_PLAYER_PAUSED, MPEG_PLAYER_PLAYING, MPEG_PLAYER_NOT_STARTED, MPEG_PLAYER_DONE
  };

  class DashMrbart;

  class MpegPlayer : public Object
  {
  public:
    MpegPlayer();

    virtual
    ~MpegPlayer();

    void
    ReceiveFrame(Ptr<Packet> message);

    int
    GetQueueSize();

    void
    Start();
    // get the time since video play started
    Time
    GetRealPlayTime(Time playTime);

    void inline
    SchduleBufferWakeup(const Time t, DashMrbart * client)
    {
      m_bufferDelay = t;
      m_dashClient = client;
    }

    int m_state;
    Time m_interruption_time;
    int m_interrruptions;

    Time m_start_time;
    uint64_t m_totalRate;
    uint32_t m_minRate;
    uint32_t m_framesPlayed;

    uint32_t m_f_id;
    // uint32_t m_segment_id;

    void
    ReceiveData(uint32_t resolution, uint32_t segment_id);

    void
    MakeSegment(uint32_t resolution, uint32_t segment_id);

  private:
    void
    PlayFrame();

    Time m_lastpaused;
    std::queue<Ptr<Packet>> m_queue;
    Time m_bufferDelay;
    DashMrbart * m_dashClient;
    std::map<uint32_t, std::pair<int, uint32_t>> m_buffer;
    bool m_making_segment;

  };
}
} // namespace ns3

#endif /* MPEG_PLAYER_H_ */
