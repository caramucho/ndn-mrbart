#include "dash-name.h"
#include "ns3/core-module.h"
#include "ns3/ndnSIM-module.h"

NS_LOG_COMPONENT_DEFINE("ndn.DashName");

namespace ns3 {
  namespace ndn {

    // NS_OBJECT_ENSURE_REGISTERED(NamingSchemeModel);

    DashName::DashName() :
    m_producerDomain(""),
    m_videoId(-1),
    m_periodId(-1),
    m_adaptationSetId(-1),
    m_representation(-1),
    m_segmentId(-1)
    {
    }

    DashName::~DashName()
    {
    }

    Name
    DashName::GetInterestName()
    {
      string namestr = "/" +
      m_producerDomain +
      "/DashOverICN" + "/" +
      to_string(m_videoId) + "/" +
      to_string(m_periodId) + "/" +
      to_string(m_adaptationSetId) + "/" +
      to_string(m_representation) + "/" +
      to_string(m_segmentId);
      // std::cout <<  namestr  << std::endl;

      return Name(namestr);
    }

    DashName&
    DashName::parseName(const Name& name)
    {
      cout << "DashName::parseName initilizing" << endl;
      m_producerDomain = name.getSubName(0,1).toUri().substr(1);
      // std::cout <<  name.getSubName(0,1).toUri().substr(1)  << std::endl;

      m_videoId = stoi(name.getSubName(2,1).toUri().substr(1));
      // std::cout <<  name.getSubName(2,1).toUri().substr(1)  << std::endl;

      m_periodId = stoi(name.getSubName(3,1).toUri().substr(1));
      // std::cout <<  name.getSubName(3,1).toUri().substr(1)  << std::endl;

      m_adaptationSetId = stoi(name.getSubName(4,1).toUri().substr(1));
      // std::cout <<  name.getSubName(4,1).toUri().substr(1)  << std::endl;

      m_representation = stoi(name.getSubName(5,1).toUri().substr(1));
      // std::cout <<  name.getSubName(5,1).toUri().substr(1)  << std::endl;

      m_segmentId = stoi(name.getSubName(6,1).toUri().substr(1));
      // std::cout <<  name.getSubName(6,1).toUri().substr(1)  << std::endl;

      // Update();
      return *this;
    }

    // void
    // DashName::Update()
    // {
    //   *this = DashName(createInterestName().c_str());
    // }
    void
    DashName::SetProducerDomain(string producerDomain)
    {
      m_producerDomain = producerDomain;
    }

    void
    DashName::SetVideoId(uint32_t videoId)
    {
      m_videoId = videoId;
    }

    void
    DashName::SetPeriodId(uint32_t periodId)
    {
      m_periodId = periodId;
    }

    void
    DashName::SetAdaptationSetId(uint32_t adaptationSetId)
    {
      m_adaptationSetId = adaptationSetId;
    }

    void
    DashName::SetRepresentation(uint32_t representation)
    {
      m_representation = representation;
    }

    void
    DashName::SetSegmentId(uint32_t segmentId)
    {
      m_segmentId = segmentId;
    }

    uint32_t
    DashName::GetVideoId(){
      return m_videoId;
    }
    uint32_t
    DashName::GetRepresentation(){
      return m_representation;
    }
    uint32_t
    DashName::GetSegmentId(){
      return m_segmentId;
    }
    string
    DashName::GetProducerDomain(){
      return m_producerDomain;
    }
    uint32_t
    DashName::GetAdaptationSetId(){
      return m_adaptationSetId;
    }
    uint32_t
    DashName::GetPeriodId(){
      return m_periodId;
    }




  }
}
