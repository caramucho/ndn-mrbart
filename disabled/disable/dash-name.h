#ifndef DASH_NAME_H
#define DASH_NAME_H
#include <ns3/core-module.h>
#include <ns3/ndnSIM-module.h>

namespace ns3 {
  namespace ndn {

    class DashName {
    public:
      DashName();
      // DashName(const std::string& uri);
      virtual ~DashName();

      // DataRate
      // getBitrateForRepresentationId(int repId);

      // DataRate
      // getRate();

      // void setRepresentation(int repId);

      Name
      GetInterestName();

      DashName& parseName(const Name& name);
      void Update();

      //setters
      uint32_t GetVideoId();
      uint32_t GetRepresentation();
      uint32_t GetSegmentId();
      string GetProducerDomain();
      uint32_t GetAdaptationSetId();
      uint32_t GetPeriodId();

      void SetVideoId(uint32_t videoId);
      void SetRepresentation(uint32_t bitrate);
      void SetSegmentId(uint32_t segmentId);
      void SetProducerDomain(string producerDomain);
      void SetAdaptationSetId(uint32_t adaptationSetId);
      void SetPeriodId(uint32_t periodId);


      // SetRepresentation()

    protected:
      std::string m_producerDomain;
      uint32_t m_videoId;
      uint32_t m_periodId;
      uint32_t m_adaptationSetId;
      uint32_t m_representation;
      uint32_t m_segmentId;

      Time m_segmentLength; // length of each segment in Seconds
    };

  }
}
#endif
