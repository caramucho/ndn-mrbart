#ifndef PHASES_H
#define PHASES_H

#include "ns3/sequence-number.h"
#include "ns3/nstime.h"
#include "ns3/object.h"


namespace ns3 {
namespace ndn {

  class Phases : public Object
  {
  public:
    enum phases {
	     INITAL_PHASE,	/* ramp up sending rate rapidly to fill pipe */
       MAIN_PHASE,	/* drain any queue created during startup */
       PROBE_PHASE,	/* discover, share bw: pace around estimated bw */
    };
    static TypeId
    GetTypeId(void);

    Phases();

    virtual TypeId
    GetInstanceTypeId(void) const;

    void
    PhaseSwitching(double ips, double U);

    double
    CalculateFreq();

  private:
    int currentPhase;
  };


} // namespace ndn
} // namespace ns3

#endif
