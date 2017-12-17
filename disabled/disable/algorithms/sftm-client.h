/*
 * sftm-client.h
 *
 *  Created on: Jun 16, 2014
 *      Author: dimitriv
 */

#ifndef SFTM_CLIENT_H_
#define SFTM_CLIENT_H_

#include "../dash-client-zipf.h"
namespace ns3
{
  namespace ndn{


  class SftmClient : public DashClientZipf
  {
  public:
    static TypeId
    GetTypeId(void);

    SftmClient();

    virtual
    ~SftmClient();

    virtual void
    CalcNextSegment(uint32_t currRate, uint32_t & nextRate, Time & delay);

  private:
    bool m_rsft_exceeded;

  };
}
} /* namespace ns3 */

#endif /* SFTM_CLIENT_H_ */
