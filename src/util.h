//
// Created by balint.galgoczi on 2021.08.10..
//

#ifndef ABC_SMC_ALGORITHM_UTIL_H
#define ABC_SMC_ALGORITHM_UTIL_H

#include <iostream>
#include <vector>
#include <random>
#include <map>
#include "../include/third-party-library/Eigen/Core"
#include "../include/third-party-library/pcg-cpp/pcg_random.hpp"
#include "models/spectrum_model.h"

Eigen::Array<int, Eigen::Dynamic, 1> randomWeightedIndices(int draws, const Eigen::ArrayX<double>& weights, pcg32 & rng);

Eigen::ArrayX<double> resampling(const Eigen::ArrayX<double>& vector, const Eigen::Array<int, Eigen::Dynamic, 1>& indices); //        (!)

Eigen::ArrayXX<double> resampling_rows(const Eigen::ArrayXX<double>& vector, const Eigen::Array<int, Eigen::Dynamic, 1>& indices); // (!)

void populationStatistics(const Eigen::MatrixXd& population);

Eigen::ArrayX<double> getSimulatedSpectrum(const Eigen::ArrayX<double>& parameters, std::vector<PeakType> peaks, int npix, bool withNoise);

/*
Eigen::ArrayX<double> getDistribution(double x_mu, double x_sigma, size_t numberOfValues);

Eigen::ArrayX<double> staticPeakModel(const Eigen::ArrayX<double>& x, const Eigen::ArrayX<double>& params);

std::vector<std::vector<double>> transpose(const std::vector<std::vector<double>> &m);

double arithmetic_mean(const std::vector<double> &vector);

int searchVector(const std::vector<std::pair<int, double>> &vec, double &item);

std::vector<std::pair<int, double>> sortByAscending(std::map<int, double>& M);

void histogram(std::vector<double> data, int nbins = 20 );

std::vector<double> resampling(std::vector<double> vector, std::vector<int> indices);

double getUniformRandomNumber();

void populationStatistics(std::vector<std::vector<double>> population);
*/


//template<class T> void reorder(std::vector<T> &v, std::vector<size_t> const &order ){
//    for ( int s = 1, d; s < order.capacity(); ++ s ) {
//        for ( d = order[s]; d < s; d = order[d] ) ;
//        if ( d == s ) while ( d = order[d], d != s ) swap( v[s], v[d] );
//    }
//}

//template< typename order_iterator, typename value_iterator >
//void reorder( order_iterator order_begin, order_iterator order_end, value_iterator v )  {
//    typedef typename std::iterator_traits<value_iterator>::value_type value_t;
//    typedef typename std::iterator_traits<order_iterator>::value_type index_t;
//    typedef typename std::iterator_traits<order_iterator>::difference_type diff_t;
//
//    diff_t remaining = order_end - 1 - order_begin;
//    for ( index_t s = index_t(), d; remaining > 0; ++ s ) {
//        for ( d = order_begin[s]; d > s; d = order_begin[d] ) ;
//        if ( d == s ) {
//            -- remaining;
//            value_t temp = v[s];
//            while ( d = order_begin[d], d != s ) {
//                swap( temp, v[d] );
//                -- remaining;
//            }
//            v[s] = temp;
//        }
//    }
//}

#endif //ABC_SMC_ALGORITHM_UTIL_H
