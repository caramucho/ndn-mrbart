/*
 * raahs-client.h
 *
 *  Created on: Jun 16, 2014
 *      Author: dimitriv
 */

#ifndef RAAHS_CLIENT_H_
#define RAAHS_CLIENT_H_
#include "../ndn-dash-mrbart.hpp"
namespace ns3
{
  namespace ndn{
  class RaahsClient : public DashMrbart
  {
  public:
    static TypeId
    GetTypeId(void);

    RaahsClient();

    virtual
    ~RaahsClient();

    virtual void
    CalcNextSegment(uint32_t currRate, uint32_t & nextRate, Time & delay);

  private:

  };
}
} /* namespace ns3 */

#endif /* RAAHS_CLIENT_H_ */
