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
{
  NS_LOG_FUNCTION_NOARGS();
}

DashMrbart::~DashMrbart()
{
}

void
DashMrbart::OnData(shared_ptr<const Data> data)
{
  ConsumerMrbart::OnData(data);

  // if == m_nextSegmentSeqMax
}

}
}
