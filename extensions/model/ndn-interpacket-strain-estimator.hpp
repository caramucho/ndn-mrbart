#ifndef NDN_INTERPACKET_STRAIN_ESTIMATOR_H
#define NDN_INTERPACKET_STRAIN_ESTIMATOR_H

#include "ns3/ndnSIM/utils/ndn-rtt-estimator.hpp"
namespace ns3 {
namespace ndn {

  class InterpacketStrainEstimator : public RttEstimator
  {
  public:
    static TypeId
    GetTypeId(void);

    InterpacketStrainEstimator();
    // InterpacketStrainEstimator(const InterpacketStrainEstimator&);

    virtual TypeId
    GetInstanceTypeId(void) const;

    void
    SentSeq(SequenceNumber32 seq, uint32_t size);

    Time
    AckSeq(SequenceNumber32 ackSeq);

    void
    Measurement(Time measure);

    Ptr<RttEstimator>
    Copy() const;
    void
    Reset();
    // void
    // Gain(double g);

  private:
    Time m_variance; // Current variance
  }


} // namespace ndn
} // namespace ns3

#endif
