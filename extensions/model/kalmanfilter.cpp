#include "kalmanfilter.hpp"
#include "ns3/log.h"
#include <deque>


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
  m_index = 0;
  m_P << 10.0, 0.0,
         0.0, 1.0;
  // m_u << -m_a(0,1)/m_a(0,0),
  //         1;
  m_H << 0.001, 0,
        0, 0.001;
  m_Q << 0.001, 0,
        0, 0.001;
}


void
KalmanFilter::Init_KalmanInfo(double C)
{
  NS_LOG_FUNCTION_NOARGS();
  m_a << 1/C, -1;
  std::cout << "initial rate :" << C << '\n';
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
  // std::cout <<"u "<< u << "\t" << " ips " <<ips<< '\n';

  NS_LOG_FUNCTION_NOARGS();
  if (u < 0.00001 || ips == -1 ){
    m_measures.clear();
    return;
  }
  m_measures.push_back(std::make_tuple(u,ips));

  // std::cout <<"u "<< u << "\t" << " ips " <<ips<< '\n';
  // std::cout << u << "\t" << ips<< '\n';

  Vector2d ipsvec;
  if (m_measures.size() == 2){
    // std::cout << "KalmanFilter Update" << '\n';
    for (auto i = m_measures.begin(); i != m_measures.end(); ++i){
      size_t index = std::distance(m_measures.begin(), i);
      m_u.row(index) << std::get<0>(*i), 1;
      ipsvec.row(index) << std::get<1>(*i);
    }
    m_measures.clear();
    // std::cout << "a  " << m_a << '\n';

    m_index = 0;
    Vector2d v = ipsvec - m_u * m_a;
    // std::cout << "v:" << "\t" << v << std::endl;
    Matrix2d f = m_u * m_P * m_u.transpose() + m_H;
    m_kalmanGain = m_P * m_u.transpose() * f.inverse();
    Vector2d apost = m_a + m_kalmanGain * v;
    Matrix2d ppost = m_P - (m_kalmanGain * f) * m_kalmanGain.transpose();
    m_a = apost;
    m_P = ppost + m_Q;
  }
}



double
KalmanFilter::GetEstimatedBandwidth()
{
  NS_LOG_FUNCTION_NOARGS();
  return -m_a(1)/m_a(0);
}
}
}
