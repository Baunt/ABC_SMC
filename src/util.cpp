//
// Created by balint.galgoczi on 2021.08.10..
//
#define _USE_MATH_DEFINES

#include "util.h"
#include <iomanip>
#include <random>
#include <cstdlib>


Eigen::ArrayX<int> randomWeightedIndices(int draws, const Eigen::ArrayX<double>& weights, pcg32 & rng)
{
    int nweights = weights.size();
    Eigen::ArrayX<int> returnedIndices(draws);
    Eigen::ArrayX<double> cumulativeWeights(nweights);
    std::uniform_real_distribution<double> uniformDistribution(0.0, 1.0);

    // Calculate the cumulative weights
    double s = weights(0);
    cumulativeWeights(0) = s;
    for (int i = 1; i < nweights; ++i) {
        s += weights(i);
        cumulativeWeights(i) = s;
    }

    // for each random draw
    for (int i = 0; i < draws; ++i) {
        double r = uniformDistribution(rng);
        //TODO this is not optimal, use binary search instead
        for (int k = 0; k < nweights; ++k) {
            if (r < cumulativeWeights(k)) {
                returnedIndices(i) = k;
                break;
            }
        }
    }
    return returnedIndices;
}

Eigen::ArrayX<double> resampling(const Eigen::ArrayX<double>& vector, const Eigen::Array<int, Eigen::Dynamic, 1>& indices){
    Eigen::ArrayX<double> resampledResult(vector.size());  
    for (int i = 0; i < vector.size(); ++i) {
        resampledResult(i)= vector(indices(i));
    }

    return resampledResult;
}

Eigen::ArrayXX<double> resampling_rows(const Eigen::ArrayXX<double>& array, const Eigen::Array<int, Eigen::Dynamic, 1>& indices){
    Eigen::ArrayXX<double> resampledResult(array.rows(), array.cols());    
    for (int i = 0; i < array.rows(); ++i) {
        resampledResult.row(i)= array.row(indices(i));
    }

    return resampledResult;
}


Eigen::ArrayX<double> getSimulatedSpectrum(const Eigen::ArrayX<double>& parameters, std::vector<PeakType> peaks, int npix, bool withNoise){
    Eigen::ArrayX<double> energy = Eigen::VectorXd::LinSpaced(npix, 0, 1);
    pcg32 rng(4444);
    std::normal_distribution<double> norm_dist(0.0, 1.0);
    auto rand_fn = [&](){return norm_dist(rng);};
    Eigen::ArrayX<double> noise = Eigen::ArrayX<double>::NullaryExpr(npix, rand_fn);

    Eigen::ArrayX<double> internalSpectrum(npix);
    internalSpectrum.setZero();

    for (int i = 0; i < peaks.size(); ++i) {
        PeakType actualPeak = peaks[i];
        switch (actualPeak) {
            case Gauss:
            {
                Eigen::ArrayX<double> gaussParametersSegment = parameters.segment(i * 3, 3);
                Eigen::ArrayX<double> spectrum(npix);
                double sigma = std::abs(gaussParametersSegment[1]) / 2.35482;
                double c0 = gaussParametersSegment[2] / (sigma * 2.5066283);
                double c1 = 0.5 / (sigma * sigma);
                spectrum = c0 * exp(-c1 * (energy - gaussParametersSegment[0]) * (energy - gaussParametersSegment[0]));
                internalSpectrum += spectrum;
                break;
            }
            case Lorentz:
            {
                Eigen::ArrayX<double> lorentzParametersSegment = parameters.segment(i * 3, 3);
                Eigen::ArrayX<double> spectrum(npix);
                double gamma = std::abs(lorentzParametersSegment[1]) / 2;
                double inverseGamma = 1.0 / gamma;
                spectrum = lorentzParametersSegment[2] / (gamma * M_PI * (1.0 + ((energy - lorentzParametersSegment[0]) * inverseGamma) * ((energy - lorentzParametersSegment[0]) * inverseGamma)));
                internalSpectrum  += spectrum;
                break;
            }
            default:
                internalSpectrum.setZero();
                break;
        }
    }

    if (withNoise){
        internalSpectrum += noise;
    }

    return internalSpectrum;
}

void populationStatistics(const Eigen::MatrixXd& population)
{
    Eigen::VectorXd means = population.colwise().mean();
    Eigen::MatrixXd centered = population.rowwise() - means.adjoint();
    Eigen::MatrixXd cov = (centered.adjoint() * centered) / double(population.rows());
    int n = cov.rows();
    std::cout << "      ";
    for (int i = 0; i<n; ++i)
    {
        std::cout << std::setw(6) << std::fixed << std::setprecision(4) << means(i) << "(" << std::sqrt(cov(i,i)) << ")  ";
    }
    std::cout << "\n";
}


#pragma region UNUSED FUNCTIONS
/*

Eigen::ArrayX<double> getDistribution(double x_mu, double x_sigma, size_t numberOfValues){
    pcg32 rng = PcgRandomGenerator::GetInstance()->value();

    std::normal_distribution<double> nd(x_mu, x_sigma);
    std::vector<double> result;
    result.reserve(numberOfValues);
    while(numberOfValues-- > 0)
    {
        result.push_back(nd(rng));
    }

    Eigen::ArrayX<double> res(result.capacity());
    for (int i = 0; i < result.capacity(); ++i) {
        res(i) = result[i];
    }

    return res;
}

Eigen::ArrayX<double> staticPeakModel(const Eigen::ArrayX<double>& x, const Eigen::ArrayX<double>& params)
{
    Eigen::ArrayX<double> spectrum(x.size());

    double sigma = std::abs(params(1)) / 2.35482;
    double c0 = params(2) / (sigma * 2.5066283);
    double c1 = 0.5 / (sigma * sigma);

    double gamma = std::abs(params(4)) / 2;
    double inverseGamma = 1.0 / gamma;

    for (int i = 0; i < x.size(); ++i)
    {
        spectrum(i) = c0 * std::exp(-c1 * (x(i) - params(0)) * (x(i) - params(0)));
        spectrum(i) += params(5) / (gamma * M_PI * (1.0 + ((x(i) - params(3)) * inverseGamma) * ((x(i) - params(3)) * inverseGamma)));
    }
    return spectrum;
}


std::vector<std::vector<double>> transpose(const std::vector<std::vector<double>> &m)
{
    std::vector<std::vector<double>> result(m[0].size(), std::vector<double>(m.size()));

    for (std::vector<double>::size_type i(0); i < m[0].size(); ++i)
        for (std::vector<double>::size_type j(0); j < m.size(); ++j)
            result[i][j] = m[j][i];

    return result;
}

double arithmetic_mean(const std::vector<double> &vector){
    double sum = 0;
    for (int i = 0; i < vector.capacity(); i++)
    {   
        sum += vector[i];
    }
    return sum / vector.capacity();
}

int binarySearch(const std::vector<std::pair<int, double>> &vec, double &item, int s1, int s2) {
    if (s1 > s2)
        return -1;

    auto middle = (s1 + s2) / 2;

    if (item == vec.at(middle).second)
        return middle;

    if (item > vec.at(middle).second)
        return binarySearch(vec, item, middle + 1, s2);
    else
        return binarySearch(vec, item, s1, middle - 1);
}

int searchVector(const std::vector<std::pair<int, double>> &vec, double &item) {
    return binarySearch(vec, item, 0, vec.size() - 1);
}

bool sortByVal(const std::pair<int, double> &a,
               const std::pair<int, double > &b)
{
    return (a.second < b.second);
}

std::vector<std::pair<int, double>> sortByAscending(std::map<int, double>& M)
{
    std::vector<std::pair<int, double>> vec;

    // copy key-value pairs from the map to the vector
    std::map<int, double> :: iterator it2;
    for (it2=M.begin(); it2!=M.end(); it2++)
    {
        vec.push_back(std::make_pair(it2->first, it2->second));
    }

    // // sort the vector by increasing order of its pair's second value
    sort(vec.begin(), vec.end(), sortByVal);

    return vec;
}

double getUniformRandomNumber(){
    pcg_extras::seed_seq_from<std::random_device> seed_source;
    pcg32 rng(seed_source);
    std::uniform_real_distribution<double> uniformdistribution(0.0, 1.0);
    return uniformdistribution(rng);
}

void histogram(std::vector<double> data, int nbins ){
    int n = data.capacity();
    double histmax = 20.0;
    // auto [dmin, dmax] = std::minmax_element(begin(data), end(data));
    double dmin = 1.0e20;
    double dmax = -1.0e20;
    std::vector<double> bins(nbins, 0.0);
    std::vector<double> bounds(nbins+1, 0.0);
    
    // determine histogram bins
    for (int i = 0; i < n; ++i)
    {
        if (data[i] < dmin)
        {
            dmin = data[i];
        }
        if (data[i] > dmax)
        {
            dmax = data[i];
        }
    }
    double binsize = (dmax-dmin) / (float)nbins;
    for (int i = 0; i < (nbins + 1); ++i)
    {   
        bounds[i] = dmin + i*binsize;
    }    


    // histogram: this is inefficient but whatever:
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < nbins; ++j)
        {
            if (bounds[j] < data[i] && data[i] < bounds[j+1])
                bins[j] += 1.0;
        }
    }

    // normalize
    double maxcount = 0.0;
    for (int i = 0; i < nbins; ++i)
    {
        if (bins[i] > maxcount)
            maxcount = bins[i];
    }
    std::cout << maxcount << "\n";
    for (int i = 0; i < nbins; ++i)
    {
        bins[i] = bins[i] * (histmax/maxcount);
    }


    // plotting    
    for (int i = 0; i < 20 + (int)round(histmax); ++i)
    {
        std::cout << "_";
    }
    std::cout << "\n";
    for (int i = 0; i < nbins; ++i)
    {
        int counts = (int)round(bins[i]);
        std::cout << std::setw(12) << 0.5*(bounds[i]+bounds[i+1]) << " |";
        for (int j = 0; j < counts; ++j)
        {
            std::cout << "X";
        }
        std::cout << "\n";
    }

    std::cout << "samples: " << n << ", bins: " << nbins << '\n';
    std::cout << "min = " << dmin << ", max = " << dmax << ", bin size: " << binsize << '\n';
    return;
}

std::vector<double> resampling(std::vector<double> vector, std::vector<int> indices){
    std::vector<double> resampledResult(vector.capacity());
    for (int i = 0; i < vector.capacity(); ++i) {
        resampledResult[i]= vector[indices[i]];
    }
    return resampledResult;
}

void populationStatistics(std::vector<std::vector<double>> population)
{
    int draws = population.capacity();
    int nparams = population[0].capacity();
    for (int i = 0; i < nparams; ++i) {
        double mean = 0.0;
        double stdev = 0.0;
        for (int j = 0; j < draws; ++j) {
            mean += population[j][i];
        }
        mean /= double(draws);
        for (int j = 0; j < draws; ++j) {
            stdev += (population[j][i]-mean) * (population[j][i]-mean);
        }
        stdev = std::sqrt(stdev / double(draws));
        std::cout << std::setw(6) << std::fixed << std::setprecision(4) << mean << "(" << stdev << ")  ";
    }
    std::cout << "\n";
}

*/
#pragma endregion UNUSED FUNCTIONS