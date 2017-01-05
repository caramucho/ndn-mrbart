#ifndef DASH_NAME_H
#define DASH_NAME_H

#include <ns3/core-module.h>
#include <ns3/ndnSIM-module.h>


namespace ns3 {
  namespace ndn {

    class DashName : public Name{
    public:
      DashName(string producerDomain, int contentId, int periodId, int representationId,  int adaptationSetId);
      DashName(const char* uri);
      DashName();
      // DashName(const std::string& uri);
      virtual ~DashName();

      // DataRate
      // getBitrateForRepresentationId(int repId);

      // DataRate
      // getRate();

      // void setRepresentation(int repId);

      std::string
      createInterestName();
      void parseName(DashName);
      void Update();

      //setters
      void SetVideoId(uint32_t videoId);
      void SetRepresentation(uint32_t bitrate);
      void SetSegmentId(uint32_t segmentId);
      void SetProducerDomain(string producerDomain);
      void SetAdaptationSet(uint32_t adaptationSetId);

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
