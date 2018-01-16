#include "ndn-dash-mrbart.hpp"
NS_LOG_COMPONENT_DEFINE("ndn.DashMrbart");
#define SEGMENT_LENGTH 2
#define PAYLOADSIZE 8000
namespace ns3 {
namespace ndn {

NS_OBJECT_ENSURE_REGISTERED(DashMrbart);

TypeId
DashMrbart::GetTypeId(void)
{
  static TypeId tid =
    TypeId("ns3::ndn::DashMrbart")
      .SetGroupName("Ndn")
      .SetParent<ConsumerMrbart>()
      .AddConstructor<DashMrbart>();

  return tid;
}

DashMrbart::DashMrbart()
  : m_nextSegmentSeqMax(0)
  // , m_segmentDownloadedSize(0)
  , m_SegmentFetchStart(Seconds(0))
  , m_segmentID(0)
  , m_SegmentSeqMax(0)
{
  NS_LOG_FUNCTION_NOARGS();
}

DashMrbart::~DashMrbart()
{
}


void
DashMrbart::SendPacket() {
  if (m_seq == m_nextSegmentSeqMax){
    CalculateNextBitrate();
  }
  ConsumerMrbart::SendPacket();


}

void
DashMrbart::OnData(shared_ptr<const Data> data)
{
  // m_segmentDownloadedSize += PAYLOADSIZE;
  ConsumerMrbart::OnData(data);
  // if(!m_phase->isInitialized()){
  //   return;
  // }
  uint32_t seq = data->getName().at(-1).toSequenceNumber();

  if (seq == m_SegmentSeqMax){
    Time SegmentFetchTime = Simulator::Now() - m_SegmentFetchStart;
    m_segmentID += 1;
    CalculateNextBitrate();

    std::cout << "SegmentFetchTime " <<SegmentFetchTime.GetSeconds()<< '\n';
  }
}


void
DashMrbart::CalculateNextBitrate()
{
  m_SegmentFetchStart = Simulator::Now();

  // double previousRate = m_segmentDownloadedSize * 8 / SegmentFetchTime.GetSeconds();
  // m_segmentDownloadedSize = 0;
  // m_bitRate = (uint32_t)(previousRate);
  m_bitRate = freqToRate(m_frequency) * 1000000;
  m_SegmentSeqMax = m_nextSegmentSeqMax;
  m_nextSegmentSeqMax = m_seq + (m_bitRate * SEGMENT_LENGTH / (PAYLOADSIZE * 8));
  // std::cout << "m_bitrate "<< m_bitRate << '\n';
  std::cout << "size " << (m_bitRate * SEGMENT_LENGTH / (PAYLOADSIZE * 8)) << "segment max"<< m_SegmentSeqMax << " next seqMax" << m_nextSegmentSeqMax << '\n';
}

}
}
