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
:m_lastu(0.0) , m_window(Seconds(4))
{
  NS_LOG_FUNCTION(this);
  // m_previousAckSeq = CreateObject<IpsHistory>(SequenceNumber32(0),0,Seconds(0.0));
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

// void
// InterpacketStrainEstimator::SentSeq(SequenceNumber32 seq, uint32_t size)
// {
//   NS_LOG_FUNCTION(this << seq << size);
//   IpsHistory_t::iterator i;
//   for (i = m_history.begin(); i != m_history.end(); ++i) {
//     if (seq == i->seq) { // Found it
//       i->retx = true;
//       i->time = Simulator::Now();
//       break;
//     }
//   }
//
//   // Note that a particular sequence has been sent
//   if (i == m_history.end())
//     m_history.push_back(IpsHistory(seq, size, Simulator::Now()));
//   // NS_LOG_INFO ("Added IpsHistory  " <<  i->seq  << "   " << i->time.GetMilliSeconds());
// }

void
InterpacketStrainEstimator::SentSeq(SequenceNumber32 seq, uint32_t size){
  m_history[seq] = Simulator::Now();
}

double
InterpacketStrainEstimator::AckSeq(SequenceNumber32 ackSeq, uint32_t size)
{
  NS_LOG_FUNCTION(this << ackSeq);
  // An ack has been received, calculate rtt and log this measurement
  // Note we use a linear search (O(n)) for this since for the common
  // case the ack'ed packet will be at the head of the list
  NS_LOG_INFO("Acked seq " << ackSeq);

  m_arrival.push_back(IpsHistory(ackSeq, size, Simulator::Now()));
  IpsHistory_t::iterator i;
  for (i = m_arrival.begin(); i != m_arrival.end(); ++i) {
    if (i->time < Simulator::Now() - m_window) {
      m_arrival.erase(i);
    }
  }
}

double
InterpacketStrainEstimator::GetIpsEstimation()
{
  if (m_history.size() == 0)
  {
    NS_LOG_DEBUG("No pending history");
    return -1; // No pending history, just exit
  }
  if (m_arrival.size() < 2) {
    NS_LOG_DEBUG("No previous log");
    return -1;
  }

  // NS_LOG_DEBUG("Previous AckSeq is " << m_previousAckSeq->seq << " " << m_previousAckSeq->time.GetMilliSeconds());

  Time DeltaOut = m_arrival.back().time - m_arrival.front().time;
  NS_LOG_DEBUG("DeltaOut " << DeltaOut.GetSeconds());
  Time DeltaIn = Seconds(0.0);
  DeltaIn = m_history[m_arrival.back().seq] - m_history[m_arrival.front().seq];
  // std::cout << "size "  <<  m_arrival.size()  << '\n';
  m_lastu = Seconds(1.0).GetSeconds() * ( DATA_PACKET_SIZE / 1000000.0 * 8) * m_arrival.size() / DeltaIn.GetSeconds();

  return DeltaOut.GetSeconds() / DeltaIn.GetSeconds() - 1;
  // return 0;
}

double
InterpacketStrainEstimator::GetU()
{
  if (m_lastu != 0){
    return m_lastu; //u is the average of 8 interests
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
