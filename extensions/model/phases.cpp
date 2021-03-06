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
  : m_currentPhase(INITIAL_PHASE)
  , m_ipsCounter(0)
  , m_probestep(0)
  , m_frequency(1.0)
  , m_first(true)
  , m_initialized(false)
  , m_ppSent(0)
  , m_pptime(Seconds(0))
  , m_previousSmallIps(false)
{
  NS_LOG_FUNCTION(this);
  m_kf = CreateObject<KalmanFilter>();
  SetRandomize();
}

TypeId
Phases::GetInstanceTypeId(void) const
{
  return GetTypeId();
}
void
Phases::Measurement(double ips, double U){
  if (m_currentPhase == INITIAL_PHASE){
    AckPP();
  }else{
    m_ips = ips;
    m_u = U;
  }
}

void
Phases::PhaseSwitch(){
  NS_LOG_FUNCTION(this);
  NS_LOG_INFO("Current phase " << m_currentPhase << " freq " << m_frequency);

  // std::cout << m_currentPhase << '\n';
  switch(m_currentPhase)
  {
    case INITIAL_PHASE:
   {
     if(m_initialized){
       m_currentPhase = MAIN_PHASE;
        m_kf->Init_KalmanInfo(freqToRate(m_frequency));

     }
     break;
   }
   case MAIN_PHASE:
   {
                        //  if(m_ips < IPSTHRESHOLD){
                        //    if(m_previousSmallIps){
                        //      m_ipsCounter += 1;
                        //    }else{
                        //      m_previousSmallIps = true;
                        //    }
                        //  }else{
                        //    m_ipsCounter = 0;
                        //    m_previousSmallIps = false;
                        //  }
                         //
                        //  if(m_ipsCounter > 4){
                        //    m_currentPhase = PROBE_PHASE;
                        //    m_ipsCounter = 0;
                        //    m_probeInitial = m_frequency;
                        //   //  m_currentPhase = INITIAL_PHASE;
                        //    Reset();
                        //  }
                        //  break;

  }
   case PROBE_PHASE:
   {
    //  if(m_ips > IPSTHRESHOLD){
    //    m_currentPhase = MAIN_PHASE;
    //   //  m_kf->Init_KalmanInfo(freqToRate(m_frequency));
    //  }
     m_probestep += 1;
     if(m_ips > IPSTHRESHOLD){
       m_ipsCounter += 1;
     }
     if(m_ipsCounter > 4){
       m_currentPhase = MAIN_PHASE;
       m_ipsCounter = 0;
       m_probestep = 0;
      //  m_kf->Init_KalmanInfo(freqToRate(m_frequency));
      //  m_currentPhase = INITIAL_PHASE;
       Reset();
       break;
     }
   }
 }
}

void
Phases::CalculateNextFreq(){
  switch(m_currentPhase)
  {
    case INITIAL_PHASE:{
      m_frequency = rateToFreq((double)(DATA_PACKET_SIZE * 8 / (1000 * 1000 * m_pptime.GetSeconds())));
      break;
    }
    case MAIN_PHASE:
    {
      m_kf->Measurement(m_u,m_ips);
      double gain = m_random->GetValue();
      // std::cout << "gain "<< gain << '\n';
      m_frequency =  gain * rateToFreq(m_kf->GetEstimatedBandwidth());

      // if(m_ips < IPSTHRESHOLD){
      //   // m_frequency =  FREQGAIN * rateToFreq(m_kf->GetEstimatedBandwidth());
      //   m_frequency =  FREQGAIN * rateToFreq(m_kf->GetEstimatedBandwidth());
      //
      // }else{
      //   m_frequency =  rateToFreq(m_kf->GetEstimatedBandwidth());
      // }
      break;
    }
    case PROBE_PHASE:
    {
      m_kf->Measurement(m_u,m_ips);
      // std::cout << "gain: " << m_frequency / m_probeInitial  <<'\n';
      m_frequency = m_probeInitial + (1 + (FREQGAIN-1) * (m_probestep/4.0));
      // m_frequency = std::max(m_frequency * (1 + (FREQGAIN-1) * (m_ipsCounter/4.0)) , rateToFreq(m_kf->GetEstimatedBandwidth()));
      // m_frequency *= FREQGAIN;

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
    case MAIN_PHASE:
    {
      bw = m_kf->GetEstimatedBandwidth();
      break;
    }
    case PROBE_PHASE:
    {
      bw = m_kf->GetEstimatedBandwidth();
      break;
    }
    default:
    {
      bw = freqToRate(m_frequency);
      break;
    }
  }
  return bw;
}

double
Phases::freqToRate(double freq){
  return freq * 8 * DATA_PACKET_SIZE / 1000000.0;
}

double
Phases::rateToFreq(double rate){
  return rate / (8 * DATA_PACKET_SIZE / 1000000.0);
}

int
Phases::GetCurrentPhase(){
  return m_currentPhase;
}

void
Phases::AckPP() {
  if(m_first){
    m_pptime = Simulator::Now();
    m_first = false;
  }else{
    if(m_pptime != Seconds(0.0)){
      m_pptime = Simulator::Now() - m_pptime;
      m_initialized = true;
    }
  }
}

bool
Phases::isInitialized(){
  return m_initialized;
}

int
Phases::ppSent(){
  return m_ppSent;
}
void
Phases::SendPP(){
  m_ppSent += 1;
}

void
Phases::Reset() {
  m_first = true;
  m_initialized = false;
  m_ppSent = 0;
  m_pptime = Seconds(0);
}

void
Phases::SetRandomize()
{
  RngSeedManager::SetSeed (2);
  m_random = CreateObject<UniformRandomVariable>();
  m_random->SetAttribute("Min", DoubleValue(1.2));
  m_random->SetAttribute("Max", DoubleValue(1.4));
}


} // namespace ndn
} // namespace ns3
