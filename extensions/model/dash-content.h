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
          Data getDataPacket(DashName dashname, uint32_t seq);
          void MakeDataPacket(uint32_t representation);

        private:
          std::map<uint32_t, std::vector<Data>> m_map;
    };
  }
}

#endif /* DASH_CLIENT_H */
