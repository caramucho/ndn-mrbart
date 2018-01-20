#ifndef NDN_INTERPACKET_STRAIN_ESTIMATOR_H
#define NDN_INTERPACKET_STRAIN_ESTIMATOR_H

// #include "ns3/ndnSIM/utils/ndn-rtt-estimator.hpp"
#include <deque>
#include "ns3/sequence-number.h"
#include "ns3/nstime.h"
#include "ns3/object.h"
#include <memory>
#include "parameter.hpp"
#include <map>
namespace ns3 {
namespace ndn {

  class IpsHistory : public Object{
  public:
    IpsHistory(SequenceNumber32 s, uint32_t c, Time t);
    IpsHistory(const IpsHistory& h); // Copy constructor
  public:
    SequenceNumber32 seq; // First sequence number in packet sent
    uint32_t count;       // Number of bytes sent
    Time time;            // Time this one was sent
    bool retx;            // True if this has been retransmitted
  };

  typedef std::deque<IpsHistory> IpsHistory_t;

  class InterpacketStrainEstimator : public Object
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

    double
    AckSeq(SequenceNumber32 ackSeq, uint32_t size);

    double
    GetIpsEstimation();

    double
    GetU();

    // Ptr<RttEstimator>
    // Copy() const;
    void
    Reset();
    // void
    // Gain(double g);

  private:
    Time m_variance; // Current variance
    // IpsHistory_t m_history;
    std::map<SequenceNumber32, Time> m_history;
    IpsHistory_t m_arrival;
    Time m_window;
    // Ptr<IpsHistory> m_previousAckSeq;
    double m_lastu;
  };


} // namespace ndn
} // namespace ns3

#endif
