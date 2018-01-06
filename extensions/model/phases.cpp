#include "phases.hpp"
#include "ns3/simulator.h"
#include "ns3/double.h"
#include "ns3/integer.h"
#include "ns3/uinteger.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE("ndn.phases");

namespace ns3 {
namespace ndn {


NS_OBJECT_ENSURE_REGISTERED(Phases);

TypeId
Phases::GetTypeId(void)
{
  static TypeId tid =
    TypeId("ns3::ndn::Phases")
      .SetParent<Object>()
      .AddConstructor<Phases>();
  return tid;
}

Phases::Phases()
{
  NS_LOG_FUNCTION(this);
}

TypeId
Phases::GetInstanceTypeId(void) const
{
  return GetTypeId();
}



} // namespace ndn
} // namespace ns3
