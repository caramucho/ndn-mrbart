#ifndef KALMANFILTER_H
#define KALMANFILTER_H

#include <ns3/core-module.h>
#include <Eigen/Dense>
using namespace Eigen;
namespace ns3 {
namespace ndn {

  class KalmanFilter : public Object{
  public:

    KalmanFilter();

    /**
    * @brief Init_KalmanInfo   初始化滤波器的初始值
    * @param info  滤波器指针
    * @param Q 预测噪声方差 由系统外部测定给定
    * @param R 测量噪声方差 由系统外部测定给定
    */
    void
    Init_KalmanInfo();

    void
    Measurement(double u, double ips);

    double
    GetEstimatedBandwidth();

  private:
    // Vector2d m_filterValue;  //k-1时刻的滤波值，即是k-1时刻的值
    Vector2d m_kalmanGain;   //   Kalman增益
    Vector2d m_a;   // z(n)=H*x(n)+w(n),w(n)~N(0,R)
    Matrix2d m_P;   //估计误差协方差
    Vector2d m_u;   // x(n)=A*x(n-1)+u(n),u(n)~N(0,Q)
    Matrix2d m_Q;   //预测过程噪声偏差的方差
    double H;   //测量噪声偏差，(系统搭建好以后，通过测量统计实验获得)
};
}
}
#endif
