#ifndef FSWF_H
#define FSWF_H

#include <QObject>
#include "nlopt.h"
#include "nlopt.hpp"

class FSWF : public QObject
{
    Q_OBJECT
public:
    explicit FSWF(QObject *parent = nullptr);
    double *getVelocity(const double &length, const double &sigama2, const double &elevationAngle, double *azimuthAngle, double *losVelocity);
signals:

public slots:
};

#endif // FSWF_H
