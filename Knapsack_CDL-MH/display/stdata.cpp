#include "stdata.h"

STData::STData()
{
    updatedata = nullptr;
}

void STData::setSize(uint nLayers)
{
    n_layer = nLayers;
    updatedata = new double[10*n_layer]{0};
}

STData::~STData()
{
    delete updatedata;
}

void STData::setHeightsLabel(double minHeight, double maxHeight)
{
    setInterval( Qt::XAxis, QwtInterval( -10, 0, QwtInterval::ExcludeMaximum ) );
    setInterval( Qt::YAxis, QwtInterval( minHeight-44, maxHeight+44, QwtInterval::ExcludeMaximum ) );

}

void STData::setdata(const double *s)
{
//    qDebug() << "layer =" << n_layer;
//判断数据
//    for(int i=0;i<n_layer;i++) {
//        if(abs(*(s+i))>16) {
//            *(s+i) = 0;
//        }
//    }

    for(uint i=0;i<n_layer;i++) {
        updatedata[n_layer*9+i] = *(s+i);
//        qDebug() << " input data = ======" << updatedata[n_layer*9+i];
    }

    displaydata.clear();
    for(uint i=0;i<n_layer;i++) {
        for(uint j=0;j<10;j++) {
            displaydata += updatedata[i+j*n_layer];
        }
    }

    qDebug() << "displaydata = " << displaydata;

    setValueMatrix(displaydata, 10);                 //数据栅格化

    for(uint m=0;m<n_layer;m++) {
        for(uint n=0;n<9;n++) {
            updatedata[m+n*n_layer] = updatedata[m+(n+1)*n_layer];
        }
    }

    for(uint i=9*n_layer;i<10*n_layer;i++) {
        updatedata[i] = 0;
    }

    double max = updatedata[0];
    for(uint i=0;i<n_layer*10;i++) {
        if(updatedata[i]>max){
            max = updatedata[i];
        }
    }
    double x = ceil(max/10.0);
//    qDebug() << "x======" << x;
    max = x*10;
    qDebug() << "max = " << max;

    setInterval( Qt::ZAxis, QwtInterval( 0, max ) );

}

