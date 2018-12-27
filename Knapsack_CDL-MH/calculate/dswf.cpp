#include "dswf.h"
#include <QtMath>

DSWF::DSWF(double elevationAngle, VectorXd azimuthAngle, int losNum,uint heightNum, MatrixXd losVelocity)
{
    this->heightNum = heightNum;
    qDebug() << losVelocity.rows();
    qDebug() << losVelocity.cols();
    vectorVelocity = MatrixXd::Zero(heightNum,3);

    Eigen::Matrix3d SiSum;
    SiSum << 0,0,0,0,0,0,0,0,0;
    Eigen::Vector3d Si;
    MatrixXd SiSi = MatrixXd::Zero(3,losNum);
    for(int i=0;i<losNum;i++){
        Si(0) = qSin(qDegreesToRadians(elevationAngle));
        Si(1) = qCos(qDegreesToRadians(elevationAngle))*qCos(qDegreesToRadians(azimuthAngle(i)));
        Si(2) = qCos(qDegreesToRadians(elevationAngle))*qSin(qDegreesToRadians(azimuthAngle(i)));
        SiSi(0,i) = Si(0);
        SiSi(1,i) = Si(1);
        SiSi(2,i) = Si(2);
        SiSum = SiSum - Si*Si.transpose();
    }

//    std::cout << "SiSum = " << SiSum << "\n";
    for(int m=0;m<heightNum;m++) {
        Eigen::Vector3d SiVri(0,0,0);
        Eigen::Vector3d temp(0,0,0);
        for(int n=0;n<losNum;n++) {
            SiVri = SiVri + losVelocity(m,n)*SiSi.col(n);
        }
//        std::cout << "SiVri = " << SiVri << "\n";
        temp = SiSum.inverse()*SiVri;
        vectorVelocity.row(m) = temp;
    }
}
double *DSWF::getHVelocity()
{
    if (HVelocity != nullptr) {
        delete [] HVelocity;
    }
    HVelocity = new double[heightNum];
    for (uint i = 0; i < heightNum; i++) {
        HVelocity[i] = qSqrt(vectorVelocity(i,1)*vectorVelocity(i,1) +
                             vectorVelocity(i,2)*vectorVelocity(i,2));
    }
    return HVelocity;
}

double *DSWF::getHAngle()
{
    if (HAngle != nullptr) {
        delete [] HAngle;
    }
    HAngle = new double[heightNum];
    for (uint i = 0; i < heightNum; i++) {
        HAngle[i] = qRadiansToDegrees(qAtan2(vectorVelocity(i,2), vectorVelocity(i,1)))+180.0;
    }
    return HAngle;
}

double *DSWF::getVVelocity()
{
    if (VVelocity != nullptr) {
        delete [] VVelocity;
    }
    VVelocity = new double[heightNum];
    for (int i = 0; i < heightNum; i++) {
        VVelocity[i] = vectorVelocity(i,0);
    }
    return VVelocity;
}
