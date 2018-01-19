#ifndef NDN_DASH_MRBART_H
#define NDN_DASH_MRBART_H

#include "ndn-consumer-mrbart.hpp"
#include "parameter.hpp"
#include "mpeg-player.h"

namespace ns3 {
namespace ndn {

  class MpegPlayer;

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
    void
    SendPacket();

    void
    CalculateNextBitrate();

    /**
     * \brief Constructs the Interest packet and sends it using a callback to the underlying NDN
     * protocol
     */

    void
    OnData(shared_ptr<const Data> contentObject);

    uint32_t m_bitRate;
    uint32_t m_nextSegmentSeqMax;
    uint32_t m_segment_id;
    uint32_t m_SegmentSeqMax;
    // uint32_t m_segmentDownloadedSize;
    Time m_SegmentFetchStart;
    Ptr<MpegPlayer> m_player;

};

} // namespace ndn
} // namespace ns3

#endif
