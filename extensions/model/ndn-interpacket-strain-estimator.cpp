#include "ndn-interpacket-strain-estimator.hpp"
#include "ns3/simulator.h"
#include "ns3/double.h"
#include "ns3/integer.h"
#include "ns3/uinteger.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE("ndn.interpacketStrainEstimator");

namespace ns3 {
namespace ndn {

//---------------------------------------------------------------------------------
// A modified version of Mean-Deviation Estimator optimized for NDN packet delivery

NS_OBJECT_ENSURE_REGISTERED(InterpacketStrainEstimator);

TypeId
InterpacketStrainEstimator::GetTypeId(void)
{
  static TypeId tid =
    TypeId("ns3::ndn::InterpacketStrainEstimator")
      // .SetParent<RttEstimator>()
      .SetParent<Object>()
      .AddConstructor<InterpacketStrainEstimator>();
      // .AddAttribute("Gain", "Gain used in estimating the RTT (smoothed RTT), must be 0 < Gain < 1",
      //               DoubleValue(0.125), MakeDoubleAccessor(&InterpacketStrainEstimator::m_gain),
      //               MakeDoubleChecker<double>())
      // .AddAttribute("Gain2", "Gain2 used in estimating the RTT (variance), must be 0 < Gain2 < 1",
      //               DoubleValue(0.25), MakeDoubleAccessor(&InterpacketStrainEstimator::m_gain2),
      //               MakeDoubleChecker<double>());
  return tid;
}

InterpacketStrainEstimator::InterpacketStrainEstimator()
:m_lastu(0.0)
{
  NS_LOG_FUNCTION(this);
  m_previousAckSeq = CreateObject<IpsHistory>(SequenceNumber32(0),0,Seconds(0.0));
}

// InterpacketStrainEstimator::InterpacketStrainEstimator(const InterpacketStrainEstimator& c)
//   : RttEstimator(c)
//   , m_gain(c.m_gain)
//   , m_gain2(c.m_gain2)
//   , m_variance(c.m_variance)
// {
//   NS_LOG_FUNCTION(this);
// }

TypeId
InterpacketStrainEstimator::GetInstanceTypeId(void) const
{
  return GetTypeId();
}

void
InterpacketStrainEstimator::Measurement(Time m)
{
  NS_LOG_FUNCTION(this);
}


// Ptr<RttEstimator>
// InterpacketStrainEstimator::Copy() const
// {
//   NS_LOG_FUNCTION(this);
//   return CopyObject<InterpacketStrainEstimator>(this);
// }

void
InterpacketStrainEstimator::Reset()
{
  NS_LOG_FUNCTION(this);
  // Reset to initial state
  m_variance = Seconds(0);
  // RttEstimator::Reset();
}

void
InterpacketStrainEstimator::SentSeq(SequenceNumber32 seq, uint32_t size)
{
  NS_LOG_FUNCTION(this << seq << size);
  IpsHistory_t::iterator i;
  for (i = m_history.begin(); i != m_history.end(); ++i) {
    if (seq == i->seq) { // Found it
      i->retx = true;
      i->time = Simulator::Now();
      break;
    }
  }

  // Note that a particular sequence has been sent
  if (i == m_history.end())
    m_history.push_back(IpsHistory(seq, size, Simulator::Now()));
  // NS_LOG_INFO ("Added IpsHistory  " <<  i->seq  << "   " << i->time.GetMilliSeconds());
}

double
InterpacketStrainEstimator::AckSeq(SequenceNumber32 ackSeq)
{
  NS_LOG_FUNCTION(this << ackSeq);
  // An ack has been received, calculate rtt and log this measurement
  // Note we use a linear search (O(n)) for this since for the common
  // case the ack'ed packet will be at the head of the list
  NS_LOG_INFO("Acked seq " << ackSeq);

  Time m = Seconds(0.0);
  Time n = Seconds(0.0);
  if (m_history.size() == 0)
  {
    NS_LOG_DEBUG("No pending history");
    return -1; // No pending history, just exit
  }
  if (m_previousAckSeq->time == Seconds(0.0))
  {
    NS_LOG_DEBUG("Previous AckSeq is 0.0");
    m_previousAckSeq->seq = ackSeq;
    m_previousAckSeq->time = Simulator::Now();
    return -1;
  }
  NS_LOG_DEBUG("Previous AckSeq is " << m_previousAckSeq->seq << " " << m_previousAckSeq->time.GetMilliSeconds());
  Time DeltaOut = Simulator::Now() - m_previousAckSeq->time;
  NS_LOG_DEBUG("DeltaOut " << DeltaOut.GetMilliSeconds());
  Time DeltaIn = Seconds(0.0);
  for (IpsHistory_t::iterator i = m_history.begin(); i != m_history.end(); ++i) {
    if (ackSeq == i->seq) { // Found it
      m = i->time;
      NS_LOG_DEBUG("m " << m.GetMilliSeconds());
    }
    if (m_previousAckSeq->seq == i->seq){
      n = i->time;
      NS_LOG_DEBUG("n " << n.GetMilliSeconds());
      m_history.erase(i);// erase the previous seq
    }
    if (m!=Seconds(0.0)){
      DeltaIn = m - n;
      break;
    }
  }
  NS_LOG_DEBUG("DeltaIn " << DeltaIn.GetMilliSeconds());

  m_lastu = Seconds(1.0).GetSeconds() / DeltaIn.GetSeconds() * (0.008 * 8);

  // Update the previous seq
  m_previousAckSeq->seq = ackSeq;
  m_previousAckSeq->time = Simulator::Now();

  double retval = (DeltaOut.GetNanoSeconds() / (double)DeltaIn.GetNanoSeconds()) - 1.0;
  NS_LOG_DEBUG("ips " << retval);
  if (retval<=-1){
    return -1;
  }
  // std::cout << DeltaIn.GetMilliSeconds() << " " << retval << std::endl;
  return retval;
}

double
InterpacketStrainEstimator::GetU()
{
  if (m_lastu != 0){
    return m_lastu * IPSCYCLE; //u is the average of 8 interests
  }else{
    // std::cout << "m_lastu == 0" << '\n';
    return 0;
  }
}

// IpsHistory methods
IpsHistory::IpsHistory(SequenceNumber32 s, uint32_t c, Time t)
  : seq(s)
  , count(c)
  , time(t)
  , retx(false)
{
  NS_LOG_FUNCTION(this);
}

IpsHistory::IpsHistory(const IpsHistory& h)
  : seq(h.seq)
  , count(h.count)
  , time(h.time)
  , retx(h.retx)
{
  NS_LOG_FUNCTION(this);
}



} // namespace ndn
} // namespace ns3
