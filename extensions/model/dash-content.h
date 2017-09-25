#ifndef DASH_CONTENT_H
#define DASH_CONTENT_H
#include <ns3/core-module.h>
#include <ns3/ndnSIM-module.h>
#include "mpeg-header.h"
#include "dash-name.h"

namespace ns3{
  namespace ndn{


    class DashContent
    {
        public:
          DashContent();
          ~DashContent();
          Data getDataPacket(uint32_t representation, uint32_t seq);
          void
          setPayloadSize(uint32_t PayloadSize);
          static uint32_t
          GetFrameSize(uint32_t representation);
          static const string&
          GetProducerDomainByVideoID(uint32_t VideoId);
          static void RegisterProducerDomain(const std::vector<string> producerList);


        private:
          void MakeDataPacket(uint32_t representation);
          std::map<uint32_t, std::vector<Data>> m_map;
          uint32_t m_videoId;
          uint32_t m_segmentId;
          uint8_t m_buffer[MPEG_MAX_MESSAGE * 50];
          uint32_t m_payloadSize;
          static std::vector<string> m_producerlist;
    };
  }
}

#endif /* DASH_CLIENT_H */
