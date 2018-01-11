#ifndef PHASES_H
#define PHASES_H
#include "kalmanfilter.hpp"
#include "ns3/sequence-number.h"
#include "ns3/nstime.h"
#include "ns3/object.h"
#define IPSTHRESHOLD 0.05
#define FREQGAIN 1.1
#define NDN_PAYLOAD_SIZE 8000

namespace ns3 {
namespace ndn {

  enum phases {
     INITIAL_PHASE_1,
     INITIAL_PHASE_2,
     MAIN_PHASE,
     PROBE_PHASE
  };

  class Phases : public Object
  {
  public:

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

    int
    GetCurrentPhase();

    void
    AckPP();

    bool
    isInitialized();

    void
    SendPP();

    int
    ppSent();


  private:
    int m_currentPhase;
    double m_ips;
    double m_u;
    int m_ipsCounter;
    double m_frequency;
    Ptr<KalmanFilter> m_kf;
    bool m_first;
    bool m_initialized;
    int m_ppSent;
    Time m_pptime;
  };


} // namespace ndn
} // namespace ns3

#endif
