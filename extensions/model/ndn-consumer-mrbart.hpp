#ifndef NDN_CONSUMER_MRBART_H
#define NDN_CONSUMER_MRBART_H

#include "ns3/ndnSIM/model/ndn-common.hpp"
#include <ns3/core-module.h>
#include <ns3/ndnSIM-module.h>
#include "ns3/ndnSIM/apps/ndn-consumer.hpp"

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
  void
  SetRandomize(const std::string& value);

  /**
   * @brief Get type of frequency randomization
   * @returns either 'none', 'uniform', or 'exponential'
   */
  std::string
  GetRandomize() const;

  void
  SendPacket();

  virtual void
  OnData(shared_ptr<const Data> contentObject);

  void
  sendPacketTrain(uint32_t amount, double frequency);


  void
  StartApplication();


protected:
  double m_frequency; // Frequency of interest packets (in hertz)
  bool m_firstTime;
  Ptr<RandomVariableStream> m_random;
  std::string m_randomType;
  bool m_initial;
};

} // namespace ndn
} // namespace ns3

#endif
