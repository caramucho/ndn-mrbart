
#include "dash-client-zipf.h"

#include <math.h>
#include "../dash-parameters.h"
#include "dash-content.h"


NS_LOG_COMPONENT_DEFINE("ndn.DashClientZipf");

namespace ns3 {
namespace ndn {

NS_OBJECT_ENSURE_REGISTERED(DashClientZipf);

TypeId
DashClientZipf::GetTypeId(void)
{
  static TypeId tid =
    TypeId("ns3::ndn::DashClientZipf")
      .SetGroupName("Ndn")
      .SetParent<DashClient>()
      .AddConstructor<DashClientZipf>()

      .AddAttribute("NumberOfContents", "Number of the Contents in total", StringValue("100"),
                    MakeUintegerAccessor(&DashClientZipf::SetNumberOfContents,
                                         &DashClientZipf::GetNumberOfContents),
                    MakeUintegerChecker<uint32_t>())

      .AddAttribute("q", "parameter of improve rank", StringValue("0.7"),
                    MakeDoubleAccessor(&DashClientZipf::SetQ,
                                       &DashClientZipf::GetQ),
                    MakeDoubleChecker<double>())

      .AddAttribute("s", "parameter of power", StringValue("0.7"),
                    MakeDoubleAccessor(&DashClientZipf::SetS,
                                       &DashClientZipf::GetS),
                    MakeDoubleChecker<double>());

  return tid;
}


DashClientZipf::DashClientZipf()
  : m_N(100) // needed here to make sure when SetQ/SetS are called, there is a valid value of N
  , m_q(0.5)
  , m_s(1.2)
  , m_seqRng(CreateObject<UniformRandomVariable>())
  , m_contentCounts(CONTENT_NUMBER,0)
{
  // SetNumberOfContents is called by NS-3 object system during the initialization
  NS_LOG_FUNCTION(this);
}

DashClientZipf::~DashClientZipf()
{
  NS_LOG_FUNCTION(this);
}
void
DashClientZipf::RequestSegment()
{
  NS_LOG_FUNCTION(this);
  // std::cout << "DashClientZipf::RequestSegment() initialization" << '\n';
  if (m_segmentId == m_segmentIdMax || m_segmentIdMax == 0) {

    m_videoId = GetNextContentId();
    m_producerDomain = DashContent::GetProducerDomain(m_videoId);

    m_contentCounts[m_videoId-1]++;

    m_segmentId = 0;
    m_bitRate = INIT_BITRATE;
     Ptr<UniformRandomVariable> segmentIdRng = CreateObject<UniformRandomVariable> ();
    segmentIdRng->SetAttribute ("Min", DoubleValue (MIN_SEGMENT_ID));
    segmentIdRng->SetAttribute ("Max", DoubleValue (MAX_SEGMENT_ID));
    m_segmentIdMax = (unsigned)segmentIdRng->GetValue();

    NS_LOG_INFO("Next Content:" << m_producerDomain << " VideoId:" << m_videoId << "Max Segment: " << m_segmentIdMax );
  }
  DashClient::RequestSegment();
}

void
DashClientZipf::GetContentPopularity(){
  for (auto itr = m_contentCounts.begin(); itr != m_contentCounts.end(); itr++) {
    std::cout << *itr << " ";
  }
}

void
DashClientZipf::SetNumberOfContents(uint32_t numOfContents)
{
  m_N = numOfContents;

  NS_LOG_DEBUG(m_q << " and " << m_s << " and " << m_N);

  m_Pcum = std::vector<double>(m_N + 1);

  m_Pcum[0] = 0.0;
  for (uint32_t i = 1; i <= m_N; i++) {
    m_Pcum[i] = m_Pcum[i - 1] + 1.0 / std::pow(i + m_q, m_s);
  }

  for (uint32_t i = 1; i <= m_N; i++) {
    m_Pcum[i] = m_Pcum[i] / m_Pcum[m_N];
    NS_LOG_LOGIC("Cumulative probability [" << i << "]=" << m_Pcum[i]);
  }
}


uint32_t
DashClientZipf::GetNumberOfContents() const
{
  return m_N;
}

void
DashClientZipf::SetQ(double q)
{
  m_q = q;
  SetNumberOfContents(m_N);
}

double
DashClientZipf::GetQ() const
{
  return m_q;
}

void
DashClientZipf::SetS(double s)
{
  m_s = s;
  SetNumberOfContents(m_N);
}

double
DashClientZipf::GetS() const
{
  return m_s;
}


uint32_t
DashClientZipf::GetNextContentId()
{
  uint32_t content_index = 1; //[1, m_N]
  double p_sum = 0;

  double p_random = m_seqRng->GetValue();
  while (p_random == 0) {
    p_random = m_seqRng->GetValue();
  }
  // if (p_random == 0)
  NS_LOG_LOGIC("p_random=" << p_random);
  for (uint32_t i = 1; i <= m_N; i++) {
    p_sum = m_Pcum[i]; // m_Pcum[i] = m_Pcum[i-1] + p[i], p[0] = 0;   e.g.: p_cum[1] = p[1],
                       // p_cum[2] = p[1] + p[2]
    if (p_random <= p_sum) {
      content_index = i;
      break;
    } // if
  }   // for
  // content_index = 1;
  NS_LOG_DEBUG("RandomNumber=" << content_index);
  return content_index;
}

} // namespace ndn
} // namespace ns3
