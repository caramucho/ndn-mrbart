#ifndef PHASES_H
#define PHASES_H
#include "kalmanfilter.hpp"
#include "ns3/sequence-number.h"
#include "ns3/nstime.h"
#include "ns3/object.h"
#define IPSTHRESHOLD 0.2
#define FREQGAIN 1.2

namespace ns3 {
namespace ndn {

  class Phases : public Object
  {
  public:
    enum phases {
	     INITAL_PHASE,
       MAIN_PHASE,
       PROBE_PHASE
    };
    static TypeId
    GetTypeId(void);

    Phases();

    virtual TypeId
    GetInstanceTypeId(void) const;

    void
    Measurement(double ips, double U);

    void
    CalculateNextFreq();

    void
    PhaseSwitch();

    double
    freqToRate(double freq);

    double
    rateToFreq(double rate);

    double
    GetFreq();

    double
    GetEstimatedBandwidth();

  private:
    int m_currentPhase;
    double m_ips;
    double m_u;
    int m_ipsCounter;
    double m_frequency;
    Ptr<KalmanFilter> m_kf;
  };


} // namespace ndn
} // namespace ns3

#endif
