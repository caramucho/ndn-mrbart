#include "ndn-dash-mrbart.hpp"
NS_LOG_COMPONENT_DEFINE("ndn.DashMrbart");


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
  , m_segment_id(0)
  , m_SegmentSeqMax(0)
  , m_bitRate(45000)
{
  NS_LOG_FUNCTION_NOARGS();
  m_player = CreateObject<MpegPlayer>();
}

DashMrbart::~DashMrbart()
{
}


void
DashMrbart::SendPacket() {
  if (m_seq > m_nextSegmentSeqMax){
    m_segment_id += 1;
    CalculateNextBitrate();
    Name prefix= m_interestName.getPrefix(1);
    m_interestName.clear();
    m_interestName.append(prefix);
    m_interestName.appendNumber(m_bitRate);
    m_interestName.appendNumber(m_segment_id);
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

  // std::cout << "size" <<data->wireEncode().size()<< '\n';
  if (data->getName().size() < 4) {
    return;
  }
  uint32_t seq = data->getName().at(-1).toSequenceNumber();

  if (seq == m_SegmentSeqMax){
    Time SegmentFetchTime = Simulator::Now() - m_SegmentFetchStart;
    // m_segment_id += 1;
    // CalculateNextBitrate();
    // std::cout << "SegmentFetchTime " <<SegmentFetchTime.GetSeconds()<< '\n';
  }


  uint32_t bitrate = data->getName().at(1).toNumber();
  uint32_t segment_id = data->getName().at(2).toNumber();
  // std::cout << "bitrate " << bitrate  <<" segid "<< segment_id <<'\n';
  // std::cout << data->getName().toUri() << '\n';
  m_player->ReceiveData(bitrate, segment_id);
  // AddBitRate(Simulator::Now(), 8 * m_segment_bytes / m_segmentFetchTime.GetSeconds());
  Time currDt = m_player->GetCurrDt();
  std::cout << Simulator::Now().GetSeconds() << "\t" <<currDt.GetSeconds()<< '\n';
  // And tell the player to monitor the buffer level
  // LogBufferLevel(currDt);
  // uint32_t old = m_bitRate;
  // double diff = m_lastDt >= 0 ? (currDt - m_lastDt).GetSeconds() : 0;
  // Time bufferDelay;
  //m_player.CalcNextSegment(m_bitRate, m_player.GetBufferEstimate(), diff,
  //m_bitRate, bufferDelay);
  // uint32_t prevBitrate = m_bitRate;

  // CalcNextSegment(prevBitrate, m_bitRate, bufferDelay);

}


void
DashMrbart::CalculateNextBitrate()
{
  uint32_t rates[] =
/*  { 13281, 18593, 26030, 36443, 51020, 71428, 100000, 140000, 195999,
 274399, 384159, 537823 };*/
  { 45000, 89000, 131000, 178000, 221000, 263000, 334000, 396000, 522000,
      595000, 791000, 1033000, 1245000, 1547000, 2134000, 2484000, 3079000,
      3527000, 3840000, 4220000 };

  uint32_t rates_size = sizeof(rates) / sizeof(rates[0]);
  m_SegmentFetchStart = Simulator::Now();

  // double previousRate = m_segmentDownloadedSize * 8 / SegmentFetchTime.GetSeconds();
  // m_segmentDownloadedSize = 0;
  // m_bitRate = (uint32_t)(previousRate);
  uint32_t previousRate = m_bitRate;
  if(Phases::freqToRate(m_frequency) < rates[0]){
    m_bitRate = rates[0];
  }

  for (int i = 0;i < rates_size;i++){
    // if (rates[i+1] >= freqToRate(m_frequency) * 1000000){
    if (rates[i+1] >= m_phase->GetEstimatedBandwidth() * 1000000){
      m_bitRate = rates[i];
      break;
    }
    if(i == rates_size-1){
      m_bitRate = rates[rates_size-1];
      break;
    }
  }
  // if (previousRate != m_bitRate){
  //
  // }

  // m_bitRate = freqToRate(m_frequency) * 1000000;

  m_SegmentSeqMax = m_nextSegmentSeqMax;
  m_nextSegmentSeqMax = m_seq + (m_bitRate * SEGMENT_LENGTH / (NDN_PAYLOAD_SIZE * 8));
  // std::cout << Simulator::Now().GetSeconds() << "\t" << m_bitRate / 1000000.0 << '\n';
  // std::cout << "size " << (m_bitRate * SEGMENT_LENGTH / (PAYLOADSIZE * 8)) << "segment max"<< m_SegmentSeqMax << " next seqMax" << m_nextSegmentSeqMax << '\n';
  // std::cout << "bitrate "<< m_bitRate << '\n';
}

}
}
