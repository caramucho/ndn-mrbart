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
      .AddConstructor<DashMrbart>()
      .AddAttribute("Issue", "issue name",
                    StringValue("Issue27"),
                    MakeStringAccessor(&DashMrbart::m_issue), MakeStringChecker())
      .AddAttribute("Simutag", "simulation tag",
                    StringValue("simu1"),
                    MakeStringAccessor(&DashMrbart::m_simutag), MakeStringChecker())

      .AddAttribute("TargetDt", "The target buffering time",
                    TimeValue(Time("35s")),
                    MakeTimeAccessor(&DashMrbart::m_target_dt), MakeTimeChecker())

      ;

  return tid;
}

DashMrbart::DashMrbart()
  : m_bitRate(45000)
  , m_nextSegmentSeqMax(0)
  // , m_segmentDownloadedSize(0)
  , m_segment_id(0)
  , m_SegmentSeqMax(0)
  , m_SegmentFetchStart(Seconds(0))
  , m_segmentFetchTime(Seconds(0))
  , m_window(Seconds(10.0))
  , m_target_dt(Seconds(35.0))
  , m_rateChanges(0)
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


    Time currDt = m_player->GetCurrDt();
    // And tell the player to monitor the buffer level
    LogBufferLevel(currDt);
    // uint32_t old = m_bitRate;
    // double diff = m_lastDt >= 0 ? (currDt - m_lastDt).GetSeconds() : 0;
    Time bufferDelay;
    // m_player.CalcNextSegment(m_bitRate, m_player.GetBufferEstimate(), diff,
    //m_bitRate, bufferDelay);
    uint32_t prevBitrate = m_bitRate;
    m_bitrateEstimate = m_phase->GetEstimatedBandwidth() * 1000000;
    m_segmentBitrates.push_back(m_bitRate/1000000.0);
    CalcNextSegment(prevBitrate, m_bitRate, bufferDelay);

    if (prevBitrate != m_bitRate)
      {
        m_rateChanges++;
      }
    // CalcNextSegment(prevBitrate, m_bitRate, bufferDelay);
    // std::cout <<Simulator::Now().GetSeconds()<< "\t" << m_bitRate / (1000000.0) << '\n';
    // std::cout << Simulator::Now().GetSeconds() << "\t" <<currDt.GetSeconds()<< '\n';
    //
    // m_fout.open(std::string("data/") + m_issue + "/" + m_simutag + "/" + std::string("bitrate.txt"), ios::app);
    // m_fout <<Simulator::Now().GetSeconds()<< "\t" << m_bitRate / (1000000.0) << '\n';
    // m_fout.close();
    // m_fout.open(std::string("data/") + m_issue + "/" + m_simutag + "/" + std::string("buffer.txt"), ios::app);
    // m_fout << Simulator::Now().GetSeconds() << "\t" <<currDt.GetSeconds()<< '\n';
    // m_fout.close();
    // // m_fout.open(std::string("data/") + m_issue + "/" + m_simutag + "/" + std::string("interruption.txt"), ios::app);
    // // m_fout <<Simulator::Now().GetSeconds()<< "\t" << m_player->m_interruption_time.GetSeconds() << '\n';
    // // m_fout.close();
    // m_fout.open(std::string("data/") + m_issue + "/" + m_simutag + "/" + std::string("bandwidth-estimation.txt"), ios::app);
    // m_fout << Simulator::Now().GetSeconds() << "\t" <<  m_phase->GetEstimatedBandwidth() << endl;
    // m_fout.close();
    // m_fout.open(std::string("data/") + m_issue + "/" + m_simutag + "/" + std::string("bitrate-change.txt"), ios::app);
    // m_fout << Simulator::Now().GetSeconds() << "\t" <<  m_player->m_interruption_time << endl;
    // m_fout.close();

    // std::cout <<Simulator::Now().GetSeconds()<< "\t" << GetSegmentFetchTime() << '\n';
    // std::cout <<Simulator::Now().GetSeconds()<< "\t" << m_player->m_interruption_time.GetSeconds() << '\n';
    // cout << Simulator::Now().GetSeconds() << "\t" <<  m_phase->GetEstimatedBandwidth() << endl;




    if (bufferDelay == Seconds(0)){
      NS_LOG_INFO( "Request next segment" );
      RequestSegment();
      // ConsumerMrbart::SendPacket();
    }
    else
    {
      std::cout << "Delay " << bufferDelay.GetSeconds() << '\n';
      m_player->SchduleBufferWakeup(bufferDelay, this);
    }
  }else{
    ConsumerMrbart::SendPacket();
  }

}

void
DashMrbart::RequestSegment() {
  m_segment_id += 1;
  Name prefix= m_interestName.getPrefix(1);
  m_interestName.clear();
  m_interestName.append(prefix);
  m_interestName.appendNumber(m_bitRate);
  m_interestName.appendNumber(m_segment_id);

  m_SegmentSeqMax = m_nextSegmentSeqMax;
  m_nextSegmentSeqMax = m_seq + (m_bitRate * SEGMENT_LENGTH / (NDN_PAYLOAD_SIZE * 8));
  // std::cout << "next seg seqmax" <<m_nextSegmentSeqMax<< '\n';
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
    m_segmentFetchTime = Simulator::Now() - m_SegmentFetchStart;
    // m_segment_id += 1;
    // CalculateNextBitrate();
    // std::cout << "SegmentFetchTime " <<SegmentFetchTime.GetSeconds()<< '\n';
  }


  uint32_t bitrate = data->getName().at(1).toNumber();
  uint32_t segment_id = data->getName().at(2).toNumber();
  // std::cout << "bitrate " << bitrate  <<" segid "<< segment_id <<'\n';
  // std::cout << data->getName().toUri() << '\n';
  m_player->ReceiveData(bitrate, segment_id);

  // m_avgRate = (1.0 * m_player->m_totalRate) / m_player->m_framesRecieved;
  // AddBitRate(Simulator::Now(), 8 * m_segment_bytes / m_segmentFetchTime.GetSeconds());
}

void
DashMrbart::CalcNextSegment(uint32_t currRate, uint32_t & nextRate, Time & delay){
  CalculateNextBitrate();
  delay = Seconds(0);
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

  uint8_t rates_size = sizeof(rates) / sizeof(rates[0]);
  m_SegmentFetchStart = Simulator::Now();

  // double previousRate = m_segmentDownloadedSize * 8 / SegmentFetchTime.GetSeconds();
  // m_segmentDownloadedSize = 0;
  // m_bitRate = (uint32_t)(previousRate);
  // uint32_t previousRate = m_bitRate;
  if(Phases::freqToRate(m_frequency) < rates[0]){
    m_bitRate = rates[0];
  }

  for (uint8_t i = 0;i < rates_size;i++){
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

  // std::cout << Simulator::Now().GetSeconds() << "\t" << m_bitRate / 1000000.0 << '\n';
  // std::cout << "size " << (m_bitRate * SEGMENT_LENGTH / (PAYLOADSIZE * 8)) << "segment max"<< m_SegmentSeqMax << " next seqMax" << m_nextSegmentSeqMax << '\n';
  // std::cout << "bitrate "<< m_bitRate << '\n';
}

void
DashMrbart::LogBufferLevel(Time t)
{
  m_bufferState[Simulator::Now()] = t;
  for (std::map<Time, Time>::iterator it = m_bufferState.begin();
  it != m_bufferState.end(); ++it)
  {
    if (it->first < (Simulator::Now() - m_window))
    {
      m_bufferState.erase(it->first);
    }
  }
}

double
DashMrbart::GetBufferEstimate()
{
  double sum = 0;
  int count = 0;
  for (std::map<Time, Time>::iterator it = m_bufferState.begin();
  it != m_bufferState.end(); ++it)
  {
    sum += it->second.GetSeconds();
    count++;
  }
  return sum / count;
}

double
DashMrbart::GetBufferDifferential()
{
  std::map<Time, Time>::iterator it = m_bufferState.end();

  if (it == m_bufferState.begin())
  {
    // Empty buffer
    return 0;
  }
  it--;
  Time last = it->second;

  if (it == m_bufferState.begin())
  {
    // Only one element
    return 0;
  }
  it--;
  Time prev = it->second;
  return (last - prev).GetSeconds();
}
double
DashMrbart::GetSegmentFetchTime(){
  return m_segmentFetchTime.GetSeconds();
}
void
DashMrbart::GetStats() {
  // cout << " avgRate: "
  // // << (1.0 * m_player->m_totalRate) / m_player->m_framesPlayed << " changes: "
  // << m_avgRate << " changes: "
  // << m_rateChanges << std::endl;
  const auto ave = std::accumulate(std::begin(m_segmentBitrates), std::end(m_segmentBitrates), 0.0) / m_segmentBitrates.size();
  const auto var = std::accumulate(std::begin(m_segmentBitrates), std::end(m_segmentBitrates), 0.0, [ave](double sum, const uint32_t& e){
      const auto temp = e - ave;
      return sum + temp * temp;
  }) / m_segmentBitrates.size();

  for (auto i = m_segmentBitrates.begin(); i < m_segmentBitrates.end(); i++) {
    std::cout << *i << '\n';
  }
  // m_segmentBitrates

  std::cout << " InterruptionTime: "
        << m_player->m_interruption_time.GetSeconds() << " interruptions: "
        << m_player->m_interrruptions << " avgRate: "
        << (1.0 * m_player->m_totalRate) / (m_player->m_framesPlayed * 1000000.0)
        // << " minRate: " << m_player->m_minRate
        // << " AvgDt: "
        // << m_sumDt.GetSeconds() / m_player->m_framesPlayed
        << " changes: "
        << m_rateChanges
        << " avg: "
        << ave
        << " var : "
        <<  var  << std::endl;


  m_fout.open(std::string("data/") + m_issue + "/" + m_simutag + "/" + std::string("stats.txt"), ios::out);
  m_fout << "Issue\t"<< "Simutag\t" << "targetDt\t" << "InterruptionTime" << "\t" << "interruptions" << "\t" << "avgRate" << "\t" << "changes\t" << "SegmentsAvgRate\t" << "SegmentsVar" << std::endl;
  m_fout << m_issue << "\t" <<
            m_simutag << "\t" <<
            m_target_dt.GetSeconds() << "\t" <<
            m_player->m_interruption_time.GetSeconds() << "\t" <<
            m_player->m_interrruptions << "\t" <<
            (1.0 * m_player->m_totalRate) / (m_player->m_framesPlayed * 1000000.0)  << "\t" <<
            m_rateChanges  << "\t" <<
            ave  << "\t" <<
            var <<  '\n';
  m_fout.close();
}


}
}
