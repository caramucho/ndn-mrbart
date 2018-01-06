#ifndef PHASES_H
#define PHASES_H

#include "ns3/sequence-number.h"
#include "ns3/nstime.h"
#include "ns3/object.h"

namespace ns3 {
namespace ndn {


  class Phases : public Object
  {
  public:
    static TypeId
    GetTypeId(void);

    Phases();
    // InterpacketStrainEstimator(const InterpacketStrainEstimator&);

    virtual TypeId
    GetInstanceTypeId(void) const;



  private:

  };


} // namespace ndn
} // namespace ns3

#endif
