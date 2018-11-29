#ifndef DSWF_H
#define DSWF_H
#include <eigen/Eigen>
#include <Eigen/Dense>
#include <math.h>
#include <iostream>
#include <QDebug>

const double PI = 3.1415926;
using namespace Eigen;
using namespace std;

class DSWF
{
public:
    DSWF(double elevationAngle, VectorXd azimuthAngle, int losNum,uint heightNum, MatrixXd losVelocity);
    double *getHVelocity();
    double *getHAngle();
    double *getVVelocity();
private:
    MatrixXd vectorVelocity;
    uint heightNum;
    double *HVelocity = NULL;
    double *HAngle = NULL;
    double *VVelocity = NULL;
};
#endif // DSWF_H
