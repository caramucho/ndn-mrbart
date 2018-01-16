#ifndef NDN_CONSUMER_MRBART_H
#define NDN_CONSUMER_MRBART_H

#include "ns3/ndnSIM/model/ndn-common.hpp"
#include <ns3/core-module.h>
#include <ns3/ndnSIM-module.h>
#include "ns3/ndnSIM/apps/ndn-consumer.hpp"
#include "ndn-interpacket-strain-estimator.hpp"
#include "kalmanfilter.hpp"
#include "phases.hpp"

namespace ns3 {
namespace ndn {

/**
 * @ingroup ndn-apps
 * @brief Ndn application for sending out Interest packets at a "constant" rate (Poisson process)
 */
class ConsumerMrbart : public Consumer {
public:
  static TypeId
  GetTypeId();

  /**
   * \brief Default constructor
   * Sets up randomizer function and packet sequence number
   */
  ConsumerMrbart();
  virtual ~ConsumerMrbart();

protected:
  /**
   * \brief Constructs the Interest packet and sends it using a callback to the underlying NDN
   * protocol
   */
  virtual void
  ScheduleNextPacket();

  /**
   * @brief Set type of frequency randomization
   * @param value Either 'none', 'uniform', or 'exponential'
   */

  /**
   * @brief Get type of frequency randomization
   * @returns either 'none', 'uniform', or 'exponential'
   */
  std::string
  GetRandomize() const;

  virtual void
  SendPacket();

  virtual void
  OnData(shared_ptr<const Data> contentObject);

  void
  sendPacketTrain(uint32_t amount, double frequency);

  void
  StartApplication();

  void
  WillSendOutInterest(uint32_t sequenceNumber);

  double
  freqToRate(double freq);

  double
  rateToFreq(double rate);

protected:
  double m_frequency; // Frequency of interest packets (in hertz)
  bool m_firstTime;
  bool m_initial;
  int m_counter;
  int m_ips0counter;
  double m_inflight;
  Time m_minrtt;
  Ptr<InterpacketStrainEstimator> m_ips;
  Ptr<Phases> m_phase;

};

} // namespace ndn
} // namespace ns3

#endif
