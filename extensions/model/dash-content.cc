
#include "dash-content.h"


NS_LOG_COMPONENT_DEFINE("DashContent");

namespace ns3{
  namespace ndn{
    DashContent::DashContent()
    {

    }
    DashContent::~DashContent()
    {

    }
    Data
    DashContent::getDataPacket(DashName dashname, uint32_t seq)
    {
      uint32_t representation = dashname.GetRepresentation();

      auto itr = m_map.find(representation);
      if( itr != m_map.end() ){
        return itr->second[seq];
      }else{
        MakeDataPacket(representation);
        return getDataPacket(dashname, seq);
      }

    }

    void
    DashContent::MakeDataPacket(uint32_t representation){

    }


  }
}
