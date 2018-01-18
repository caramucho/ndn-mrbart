#ifndef DASH_CONTENT_H
#define DASH_CONTENT_H

#include <ns3/core-module.h>
#include <ns3/ndnSIM-module.h>
#include "parameter.hpp"
// #include "ns3/ndnSIM/apps/ndn-producer.hpp"
// #include <map>
// #include <queue>
// #include "dash-content.h"
// #include "dash-name.h"
// #include "mpeg-player.h"
// #include "ndn-parser.h"
// #include "http-header.h"
#include "mpeg-header.h"

namespace ns3 {
  namespace ndn {

    /**
    * \ingroup dash
    *
    * \breif This application was written to complement DashClient. It received
    * requests for MPEG Segments from clients, and responds by transmitting back
    * the MPEG frames that are contained in the frame
    *
    * The constructor specifies the Address (IP address and port) and the
    * transport protocol to use.   A virtual Receive () method is installed
    * as a callback on the receiving socket.  By default, when logging is
    * enabled, it prints out the size of packets and their address, but
    * we intend to also add a tracing source to Receive() at a later date.
    */
    class DashContent
    {
    public:
      static TypeId
      GetTypeId(void);
      DashContent();

      virtual
      ~DashContent();

      void
      MakeSegment(uint32_t video_id, uint32_t resolution,
          uint32_t segment_id)

    private:

      // uint32_t m_totalRx;      // Total bytes received
      // TypeId m_tid;          // Protocol TypeId
      // // A structure that contains the generated MPEG frames, for each client.
      // std::map<Ptr<Socket>, std::queue<Packet> > m_queues;
      std::queue<Packet> m_queues;
      // Name m_prefix;
      // Name m_postfix;
      // uint32_t m_virtualPayloadSize;
      uint32_t m_payloadSize;
      Time m_freshness;
    };
  }
} // namespace ns3

#endif /* DASH_SERVER_H */
