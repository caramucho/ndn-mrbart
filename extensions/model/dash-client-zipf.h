#ifndef DASH_CLIENT_ZIPF_H
#define DASH_CLIENT_ZIPF_H

#include "dash-client.h"

namespace ns3
{

  namespace ndn
  {
      class DashClientZipf : public DashClient {
      public:
        static TypeId
        GetTypeId();

        DashClientZipf();
        virtual ~DashClientZipf();

        virtual void
        RequestSegment();


        uint32_t
        GetNextContentId();
        void
        SetNumberOfContents(uint32_t numOfContents);
        uint32_t
        GetNumberOfContents() const;

        void
        SetQ(double q);

        double
        GetQ() const;

        void
        SetS(double s);

        double
        GetS() const;

        void
        GetContentPopularity();

      private:
        uint32_t m_N;               // number of the contents
        double m_q;                 // q in (k+q)^s
        double m_s;                 // s in (k+q)^s
        std::vector<double> m_Pcum; // cumulative probability

        Ptr<UniformRandomVariable> m_seqRng; // RNG

        uint32_t m_segmentIdMax;
        std::vector<int> m_contentCounts;


      };

  }
}

#endif
