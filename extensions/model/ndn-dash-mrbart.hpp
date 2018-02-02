#ifndef NDN_DASH_MRBART_H
#define NDN_DASH_MRBART_H

#include "ndn-consumer-mrbart.hpp"
#include "parameter.hpp"
#include "mpeg-player.h"
#include "mpeg-header.h"
#include <fstream>

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

    void
    RequestSegment();

  protected:
    void
    SendPacket();

    void
    CalculateNextBitrate();

    virtual void
    CalcNextSegment(uint32_t currRate, uint32_t & nextRate, Time & delay);

    void
    LogBufferLevel(Time t);

    double
    GetBufferDifferential();

    double
    GetBufferEstimate();

    double
    GetSegmentFetchTime();




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
    Time m_segmentFetchTime;
    Ptr<MpegPlayer> m_player;

    std::map<Time, Time> m_bufferState;
    Time m_window; //The window for measuring the average throughput (Time)
    double m_bitrateEstimate;
    Time m_target_dt;
    ofstream m_fout;


};

} // namespace ndn
} // namespace ns3

#endif
