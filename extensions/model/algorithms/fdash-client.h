/*
 * fdash-client.h
 *
 *  Created on: Jun 16, 2014
 *      Author: dimitriv
 */

#ifndef FDASH_CLIENT_H_
#define FDASH_CLIENT_H_

#include "../ndn-dash-mrbart.hpp"
namespace ns3
{
  namespace ndn{

  class FdashClient : public DashMrbart
  {
  public:
    static TypeId
    GetTypeId(void);

    FdashClient();

    virtual
    ~FdashClient();

    virtual void
    CalcNextSegment(uint32_t currRate, uint32_t & nextRate, Time & delay);

  private:
    bool
    BufferInc();
    Time m_target_dt;

  };
}
} /* namespace ns3 */

#endif /* FDASH_CLIENT_H_ */
