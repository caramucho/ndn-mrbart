#include "kalmanfilter.hpp"
#include "ns3/log.h"
#define MSIZE 2


NS_LOG_COMPONENT_DEFINE("ndn.kalmanFilter");

namespace ns3 {
namespace ndn {

KalmanFilter::KalmanFilter()
  // : m_frequency(1.0)
  // , m_firstTime(true)
  // , m_counter(0)
{
  NS_LOG_FUNCTION_NOARGS();
  // m_a << 10.0,-1;
  m_P << 10.0, 0.0,
         0.0, 0.5;
  m_u << -m_a(0,1)/m_a(0,0),
          1;
  m_H << 0.0001, 0,
        0, 0.0001;
  m_Q << 0.0001, 0,
        0, 0.0001;
}


void
KalmanFilter::Init_KalmanInfo(double C)
{
  NS_LOG_FUNCTION_NOARGS();
  m_a << 1/C, -1;
  // std::cout << "initial rate :" << C << '\n';
  // info->A = 1;  //标量卡尔曼
  // info->H = 1;  //
  // info->P = 10;  //后验状态估计值误差的方差的初始值（不要为0问题不大）
  // info->Q = Q;    //预测（过程）噪声方差 影响收敛速率，可以根据实际需求给出
  // info->R = R;    //测量（观测）噪声方差 可以通过实验手段获得
  // info->filterValue = 0;// 测量的初始值
}

void
KalmanFilter::Measurement(double u, double ips)
{
  NS_LOG_FUNCTION_NOARGS();
  if (u == 0 || ips == -1 || ips == 0){
    return;
  }
  m_history.push_back(std::make_tuple(u,ips));
  if(m_history.size() >= MSIZE){
    m_u << u , 1;
    // std::cout <<"u:" << u << "\t" <<"ips:" << ips << std::endl;
    double v = ips - m_u.transpose() * m_a;
    // std::cout << "v:" << "\t" << v << std::endl;
    double f = m_u.transpose() * m_P * m_u + m_H;
    m_kalmanGain = m_P * m_u / f;
    Vector2d apost = m_a + m_kalmanGain * v;
    Matrix2d ppost = m_P - (m_kalmanGain * f) * m_kalmanGain.transpose() ;
    m_a = apost;
    m_P = ppost + m_Q;
  }

  // std::cout <<"B= " << -m_a(1)/m_a(0) << " u=" << u  << " 1/c=" << 1/m_a(0)<<std::endl;
}

double
KalmanFilter::GetEstimatedBandwidth()
{
  NS_LOG_FUNCTION_NOARGS();
  return -m_a(1)/m_a(0);
}
}
}
