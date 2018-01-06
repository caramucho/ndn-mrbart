#include "phases.hpp"
#include "ns3/simulator.h"
#include "ns3/double.h"
#include "ns3/integer.h"
#include "ns3/uinteger.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE("ndn.phases");

namespace ns3 {
namespace ndn {


NS_OBJECT_ENSURE_REGISTERED(Phases);

TypeId
Phases::GetTypeId(void)
{
  static TypeId tid =
    TypeId("ns3::ndn::Phases")
      .SetParent<Object>()
      .AddConstructor<Phases>();
  return tid;
}

Phases::Phases()
  : m_currentPhase(INITAL_PHASE)
  , m_ipsCounter(0)
  , m_frequency(1.0)

{
  NS_LOG_FUNCTION(this);
  m_kf = CreateObject<KalmanFilter>();
}

TypeId
Phases::GetInstanceTypeId(void) const
{
  return GetTypeId();
}
void
Phases::Measurement(double ips, double U){
  m_ips = ips;
  m_u = U;
}

void
Phases::PhaseSwitch(){
  // std::cout << m_currentPhase << '\n';
  switch(m_currentPhase)
  {
    case INITAL_PHASE:
   {
     if(m_ips > IPSTHRESHOLD){
       m_currentPhase = MAIN_PHASE;
       m_kf->Init_KalmanInfo(freqToRate(m_frequency));
     }
     break;
   }
   case MAIN_PHASE:
   {
     if(m_ips == 0){
       m_ipsCounter += 1;
     }
     if(m_ipsCounter > 4){
       m_currentPhase = PROBE_PHASE;
       m_ipsCounter = 0;
     }
     break;
   }
   case PROBE_PHASE:
   {
     if(m_ips > IPSTHRESHOLD){
       m_currentPhase = MAIN_PHASE;
       m_kf->Init_KalmanInfo(freqToRate(m_frequency));
     }
     break;
   }
 }
}

void
Phases::CalculateNextFreq(){
  switch(m_currentPhase)
  {
    case INITAL_PHASE:
    {
      m_frequency *= FREQGAIN;
      break;
    }
    case MAIN_PHASE:
    {
      m_kf->Measurement(m_u,m_ips);
      if(m_ips < IPSTHRESHOLD){
        m_frequency =  FREQGAIN * rateToFreq(m_kf->GetEstimatedBandwidth());
      }else{
        m_frequency =  rateToFreq(m_kf->GetEstimatedBandwidth());
      }
      break;
    }
    case PROBE_PHASE:
    {
      m_frequency *= FREQGAIN;
      break;
    }
  }
}

double
Phases::GetFreq(){
  return m_frequency;
}

double
Phases::GetEstimatedBandwidth(){
  double bw;
  switch(m_currentPhase)
  {
    case INITAL_PHASE:
    {
      bw = freqToRate(m_frequency);
      break;

    }
    case MAIN_PHASE:
    {
      bw = m_kf->GetEstimatedBandwidth();
      break;
    }
    case PROBE_PHASE:
    {
      bw = freqToRate(m_frequency);
      break;

    }
  }
  return bw;
}

double
Phases::freqToRate(double freq){
  return freq * 8 * 0.008;
}

double
Phases::rateToFreq(double rate){
  return rate / (8*0.008);
}

} // namespace ndn
} // namespace ns3
