#include "dash-name.h"
#include "ns3/core-module.h"
#include "ns3/ndnSIM-module.h"

NS_LOG_COMPONENT_DEFINE("ndn.DashName");

namespace ns3 {
  namespace ndn {

    // NS_OBJECT_ENSURE_REGISTERED(NamingSchemeModel);


    DashName::DashName(string producerDomain, int contentId, int periodId, int representationId, int adaptationSetId) :
    m_producerDomain(producerDomain),
    m_contentId(contentId),
    m_periodId(periodId),
    m_adaptationSetId(adaptationSetId),
    m_representationId(representationId)
    {
      // std::cout <<  "DashName::DashName(int contentId,... is called"  << std::endl;
      Update();
    }

    DashName::DashName(const char* uri)
    : Name(uri)
    {
      // std::cout <<  "DashName::DashName(const char* uri) is called"  << std::endl;
    }

    DashName::DashName() :
    Name(),
    m_producerDomain(""),
    m_contentId(-1),
    m_periodId(-1),
    m_adaptationSetId(-1),
    m_representationId(-1)
    {
    }

    DashName::~DashName()
    {
    }

    std::string
    DashName::createInterestName()
    {
      string namestr = "/" + m_producerDomain + "/DashOverICN" + "/" + to_string(m_videoId) + "/" +  to_string(m_periodId) + "/" + to_string(m_adaptationSetId) + "/" + to_string(m_representation);
      // std::cout <<  namestr  << std::endl;

      return namestr;
    }

    void
    DashName::parseName(DashName name)
    {
      m_producerDomain = name.getSubName(0,1).toUri().substr(1);
      std::cout <<  name.getSubName(0,1).toUri().substr(1)  << std::endl;

      m_contentId = stoi(name.getSubName(2,1).toUri().substr(1));
      std::cout <<  name.getSubName(2,1).toUri().substr(1)  << std::endl;

      m_periodId = stoi(name.getSubName(3,1).toUri().substr(1));
      std::cout <<  name.getSubName(3,1).toUri().substr(1)  << std::endl;

      m_adaptationSetId = stoi(name.getSubName(4,1).toUri().substr(1));
      std::cout <<  name.getSubName(4,1).toUri().substr(1)  << std::endl;

      m_representationId = stoi(name.getSubName(5,1).toUri().substr(1));
      std::cout <<  name.getSubName(5,1).toUri().substr(1)  << std::endl;

      Update();
    }

    void
    DashName::Update()
    {
      *this = DashName(createInterestName().c_str());
    }
    void
    DashName::SetProducerDomain(string producerDomain)
    {
      m_producerDomain = producerDomain;
    }

    void
    Dashname::SetVideoId(uint32_t videoId)
    {
      m_videoId = videoId;
    }

    void
    DashName::SetAdaptationSet(uint32_t adaptationSetId)
    {
      m_adaptationSetId = adaptationSetId;
    }

    void
    DashName::SetRepresentation(uint32_t representation)
    {
      m_representation = representation;
    }

    DashName::SetSegmentId(uint32_t segmentId)
    {
      m_segmentId = segmentId;
    }



  }
}
