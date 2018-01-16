#ifndef NDN_DASH_MRBART_H
#define NDN_DASH_MRBART_H

#include "ndn-consumer-mrbart.hpp"

namespace ns3 {
namespace ndn {

  class DashMrbart : public ConsumerMrbart {
  public:
    static TypeId
    GetTypeId();

    /**
     * \brief Default constructor
     * Sets up randomizer function and packet sequence number
     */
    DashMrbart();
    virtual ~DashMrbart();

  protected:
    /**
     * \brief Constructs the Interest packet and sends it using a callback to the underlying NDN
     * protocol
     */
    // virtual void
    // ScheduleNextPacket();

    void
    OnData(shared_ptr<const Data> contentObject);
};

} // namespace ndn
} // namespace ns3

#endif
