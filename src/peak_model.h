//
// Created by balint.galgoczi on 2021.07.28..
//

#ifndef ABC_SMC_ALGORITHM_PEAK_MODEL_H
#define ABC_SMC_ALGORITHM_PEAK_MODEL_H

#include <vector>
#include "../include/third-party-library/Eigen/Core"

class PeakModel {
private:
    Eigen::ArrayX<double> p_x;
    double p_x0;
    double p_fwhm;
    double p_intensity;
    int p_npix;
public:
    Eigen::ArrayX<double> Gaussian();

//    std::vector<double> GaussianNonOptimized();

    Eigen::ArrayX<double> Lorenzt();

//    std::vector<double> LorenztNonOptimized();

    PeakModel(Eigen::ArrayX<double> x, double x0, double fwhm, double intensity, int npix)
    {
        p_x = x;
        p_x0 = x0;
        p_fwhm = fwhm;
        p_intensity = intensity;
        p_npix = npix;
    }
};


#endif //ABC_SMC_ALGORITHM_PEAK_MODEL_H
