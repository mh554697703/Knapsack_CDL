#include "fswf.h"
#include <stdlib.h>
#include <iostream>
#include <io.h>
#include <QDebug>
#include <math.h>
#include <QtMath>

typedef struct {double *azimuthAngle, *losVelocity ,length ,sigama2 ,elevationAngle;} transfer_data;

FSWF::FSWF(QObject *parent) : QObject(parent)
{

}

double myfunc(unsigned n, const double *x, double *grad, void *my_func_data)              //目标函数
{
    transfer_data *InputData = ( transfer_data *) my_func_data ;
    double result,gradX0,gradX1,gradX2;
    result=gradX0=gradX1=gradX2=0;

    for(int i=0;i<InputData->length;i++)   //目标函数及其各方向梯度
    {
        result += exp(-pow((x[0]*qSin(qDegreesToRadians(InputData->elevationAngle))+x[1]*qCos(qDegreesToRadians(InputData->elevationAngle))*qCos(qDegreesToRadians(*(InputData->azimuthAngle+i)))+x[2]*qCos(qDegreesToRadians(InputData->elevationAngle))*qSin(qDegreesToRadians(*(InputData->azimuthAngle+i)))-*(InputData->losVelocity+i)),2)/2/(InputData->sigama2));
        gradX0 += -exp(-pow((x[0]*qSin(qDegreesToRadians(InputData->elevationAngle))+x[1]*qCos(qDegreesToRadians(InputData->elevationAngle))*qCos(qDegreesToRadians(*(InputData->azimuthAngle+i)))+x[2]*qCos(qDegreesToRadians(InputData->elevationAngle))*qSin(qDegreesToRadians(*(InputData->azimuthAngle+i)))-*(InputData->losVelocity+i)),2)/2/(InputData->sigama2))*qSin(qDegreesToRadians(InputData->elevationAngle))*((x[0]*qSin(qDegreesToRadians(InputData->elevationAngle))+x[1]*qCos(qDegreesToRadians(InputData->elevationAngle))*qCos(qDegreesToRadians(*(InputData->azimuthAngle+i)))+x[2]*qCos(qDegreesToRadians(InputData->elevationAngle))*qSin(qDegreesToRadians(*(InputData->azimuthAngle+i)))-*(InputData->losVelocity+i)))/(InputData->sigama2);
         gradX1 +=-exp(-pow((x[0]*qSin(qDegreesToRadians(InputData->elevationAngle))+x[1]*qCos(qDegreesToRadians(InputData->elevationAngle))*qCos(qDegreesToRadians(*(InputData->azimuthAngle+i)))+x[2]*qCos(qDegreesToRadians(InputData->elevationAngle))*qSin(qDegreesToRadians(*(InputData->azimuthAngle+i)))-*(InputData->losVelocity+i)),2)/2/(InputData->sigama2))*qCos(qDegreesToRadians(InputData->elevationAngle))*qCos(qDegreesToRadians(*(InputData->azimuthAngle+i)))*((x[0]*qSin(qDegreesToRadians(InputData->elevationAngle))+x[1]*qCos(qDegreesToRadians(InputData->elevationAngle))*qCos(qDegreesToRadians(*(InputData->azimuthAngle+i)))+x[2]*qCos(qDegreesToRadians(InputData->elevationAngle))*qSin(qDegreesToRadians(*(InputData->azimuthAngle+i)))-*(InputData->losVelocity+i)))/(InputData->sigama2);
          gradX2 +=-exp(-pow((x[0]*qSin(qDegreesToRadians(InputData->elevationAngle))+x[1]*qCos(qDegreesToRadians(InputData->elevationAngle))*qCos(qDegreesToRadians(*(InputData->azimuthAngle+i)))+x[2]*qCos(qDegreesToRadians(InputData->elevationAngle))*qSin(qDegreesToRadians(*(InputData->azimuthAngle+i)))-*(InputData->losVelocity+i)),2)/2/(InputData->sigama2))*qCos(qDegreesToRadians(InputData->elevationAngle))*qSin(qDegreesToRadians(*(InputData->azimuthAngle+i)))*((x[0]*qSin(qDegreesToRadians(InputData->elevationAngle))+x[1]*qCos(qDegreesToRadians(InputData->elevationAngle))*qCos(qDegreesToRadians(*(InputData->azimuthAngle+i)))+x[2]*qCos(qDegreesToRadians(InputData->elevationAngle))*qSin(qDegreesToRadians(*(InputData->azimuthAngle+i)))-*(InputData->losVelocity+i)))/(InputData->sigama2);
    }

    if (grad) {
        grad[0] = gradX0;
        grad[1] = gradX1;
        grad[2] = gradX2;
    }

    return result;
}

double *FSWF::getVelocity(const double &length, const double &sigama2, const double &elevationAngle, double *azimuthAngle, double *losVelocity)
{
    double *vectorVelocity;
    transfer_data *myData;
    myData=new transfer_data();

    //通过指针myData向myfunc传入径向方向数，能正常反演的方差，俯仰角，方向角，各方向径向风速
    myData->length=length;
    myData->sigama2=sigama2;
    myData->elevationAngle=elevationAngle;
    myData->azimuthAngle=azimuthAngle;
    myData->losVelocity=losVelocity;

    double lb[3] = {-20,-50,-50};  //给定风速上下界
    double ub[3] = {20,50,50};
    nlopt_opt opt;
    opt = nlopt_create(NLOPT_LD_LBFGS, 3); //采用拟牛顿法LBFGS
    nlopt_set_lower_bounds(opt,lb);  //设置目标函数上下界限
    nlopt_set_upper_bounds(opt,ub);

    nlopt_set_max_objective(opt, myfunc, myData);  //设置目标函数求取最大值
    double x [3] = {0,0,0}; /* some initial guess */
    double minf ;
    if ( nlopt_optimize (opt, x, & minf) < 0) {
//        printf(" nlopt failed !\n");
    }
    else {
//        printf("found minimum at f(%g ,%g ,%g) = %0.10g\n", x[0] , x[1] , x[2] ,minf);
        qDebug()<<"found max at"<<x[0]<<","<<x[1]<<","<<x[2]<<"="<<minf;
    }
    vectorVelocity=x;
    nlopt_destroy ( opt );
    delete myData;
    return vectorVelocity;   //返回[Vz,Vx,Vy]指针

}









