//
// Created by balint.galgoczi on 2021.11.21..
//

#ifndef MATPLOTLIB_CPP_SPECTRUM_MODEL_H
#define MATPLOTLIB_CPP_SPECTRUM_MODEL_H

#include "probability_distribution.h"
#include <map>
#include <list>

enum PeakType{
    Gauss,
    Lorentz
};

class SpectrumModel {
private:
    std::vector<PeakType> peaks;
    int npix;

public:
    std::vector<NormalDistribution> InitialGuess;
    Eigen::ArrayXX<double> GenerateInitialPopulation(int nsamples, int nparams, pcg32 & rng, bool simulated);
    Eigen::ArrayX<double> Calculate(Eigen::ArrayX<double> parameters);
    double ErrorCalculation(Eigen::ArrayX<double> diffSpectrum);
    void SetPeakList(std::vector<PeakType>& peaks);
    double PriorLikelihood(Eigen::ArrayX<double> posterior);
    Eigen::ArrayX<double> energy;
    Eigen::ArrayX<double> intensity;


    SpectrumModel(const Eigen::ArrayX<double>& energy, const Eigen::ArrayX<double>& intensity){
        this->npix = energy.size();
        this->energy = energy;
        this->intensity = intensity;
    }
};


#endif //MATPLOTLIB_CPP_SPECTRUM_MODEL_H
