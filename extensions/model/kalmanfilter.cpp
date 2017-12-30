#include "kalmanfilter.hpp"
#include "ns3/log.h"

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
  m_H = 0.1;
  m_Q << 0.0001, 0,
        0, 0.0001;
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
  NS_LOG_FUNCTION_NOARGS();
  if (u == 0 || ips == -1 || ips == 0){
    return;
  }
  m_u << u , 1;
  // std::cout <<"u:" << u << "\t" <<"ips:" << ips << std::endl;
  double v = ips - m_u.transpose() * m_a;
  std::cout << "v:" << "\t" << v << std::endl;
  double f = m_u.transpose() * m_P * m_u + m_H;
  m_kalmanGain = m_P * m_u / f;
  Vector2d apost = m_a + m_kalmanGain * v;
  Matrix2d ppost = m_P - (m_kalmanGain * f) * m_kalmanGain.transpose() ;
  m_a = apost;
  m_P = ppost + m_Q;
  // std::cout <<"B= " << -m_a(1)/m_a(0) << " u=" << u  << " 1/c=" << 1/m_a(0)<<std::endl;
  // //预测下一时刻的值
  // double predictValue = kalmanInfo->A* kalmanInfo->filterValue;   //x的先验估计由上一个时间点的后验估计值和输入信息给出，此处需要根据基站高度做一个修改
  //
  // //求协方差
  // kalmanInfo->P = kalmanInfo->A*kalmanInfo->A*kalmanInfo->P + kalmanInfo->Q;  //计算先验均方差 p(n|n-1)=A^2*p(n-1|n-1)+q
  // double preValue = kalmanInfo->filterValue;  //记录上次实际坐标的值
  //
  // //计算kalman增益
  // kalmanInfo->kalmanGain = kalmanInfo->P*kalmanInfo->H / (kalmanInfo->P*kalmanInfo->H*kalmanInfo->H + kalmanInfo->R);  //Kg(k)= P(k|k-1) H’ / (H P(k|k-1) H’ + R)
  // //修正结果，即计算滤波值
  // kalmanInfo->filterValue = predictValue + (lastMeasurement - predictValue)*kalmanInfo->kalmanGain;  //利用残余的信息改善对x(t)的估计，给出后验估计，这个值也就是输出  X(k|k)= X(k|k-1)+Kg(k) (Z(k)-H X(k|k-1))
  // //更新后验估计
  // kalmanInfo->P = (1 - kalmanInfo->kalmanGain*kalmanInfo->H)*kalmanInfo->P;//计算后验均方差  P[n|n]=(1-K[n]*H)*P[n|n-1]
  //
  // return  kalmanInfo->filterValue;
}

double
KalmanFilter::GetEstimatedBandwidth()
{
  NS_LOG_FUNCTION_NOARGS();
  return -m_a(1)/m_a(0);
}
}
}
