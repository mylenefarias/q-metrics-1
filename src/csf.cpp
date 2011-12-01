#include "csf.h"

#define CSF_SIGMA 2
double CSFFreqResponse(double u, double v)
{
    double f,w,Sw,sita,Ow;
    f = sqrt(u*u + v*v);
    w = (2*M_PI*f)/60;

    Sw = 1.5 * exp(-((CSF_SIGMA)*(CSF_SIGMA)*w*w)/2) -
               exp(-((CSF_SIGMA)*(CSF_SIGMA)*w*w));

    sita = atan(v/(u+EPSILON));
    Ow = pow(cos(2*sita),4);

    return Sw * Ow;
}
