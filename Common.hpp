/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Common.hpp
 * Author: matthewsupernaw
 *
 * Created on September 16, 2016, 12:42 PM
 */

#ifndef COMMON_HPP
#define COMMON_HPP

#include "../AutoDiff_Standalone/AutoDiff/AutoDiff.hpp"

#include <vector>
#include <map>

namespace mas {

    std::ofstream mas_log("mas.log");

    template<typename REAL_T>
    struct VariableTrait {
        typedef atl::Variable<REAL_T> variable;

        static void SetName(variable& var, const std::string& value) {
            var.SetName(value);
        }

        static void SetValue(variable& var, const REAL_T& value) {
            var.SetValue(value);
        }

        static void SetMinBoundary(variable& var, const REAL_T& value) {
            var.SetMinBoundary(value);
        }

        static void SetMaxBoundary(variable& var, const REAL_T& value) {
            var.SetMaxBoundary(value);
        }
    };

    template<typename REAL_T>
    struct ModelObject {
        typedef typename VariableTrait<REAL_T>::variable variable;
        int id;
        std::map<variable*, int> estimated_parameters_map;
        typedef typename std::map<variable*, int>::iterator estimable_parameter_iterator;
        std::vector<variable*> estimated_parameters;
        std::vector<int> estimated_phase;

        void Register(variable& var, int phase = 1) {
            estimated_parameters_map[&var] = phase;
            this->estimated_parameters.push_back(&var);
            this->estimated_phase.push_back(phase);
        }
    };

    enum DataUnits {
        MT = 0,
        KG,
        LBS,
        IT,
        NUMBERS,
        NA
    };

    enum DataObjectType {
        CATCH_BIOMASS = 0,
        CATCH_PROPORTION_AT_AGE,
        CATCH_PROPORTION_AT_LENGTH,
        CATCH_MEAN_SIZE_AT_AGE,
        SURVEY_BIOMASS = 0,
        SURVEY_PROPORTION_AT_AGE,
        SURVEY_PROPORTION_AT_LENGTH,
        SURVEY_MEAN_SIZE_AT_AGE,
        UNKNOWN

    };

    enum FishSexType {
        MALE = 0,
        FEMALE,
        UNDIFFERENTIATED

    };

    template<typename REAL_T>
    struct DataObject {
        std::vector<REAL_T> data;
        DataObjectType type;
        FishSexType sex_type;
        DataUnits units;
        std::string name;

        uint32_t area_id;
        uint32_t population_id;
        uint32_t dimensions;
        size_t imax;
        size_t jmax;
        size_t kmax;
        size_t lmax;
        size_t mmax;
        size_t nmax;

        inline REAL_T& get(int i) {
            return data[i];
        }

        inline REAL_T& get(int i, int j) {
            return data[i * jmax + j];
        }

        inline REAL_T& get(int i, int j, int k) {
            return data[i * jmax * kmax + j * kmax + k];
        }

        inline REAL_T& get(int i, int j, int k, int l) {
            return data[i * jmax * kmax * lmax + j * kmax * lmax + k * lmax + l];
        }

        inline REAL_T& get(int i, int j, int k, int l, int m) {
            return data[i * jmax * kmax * lmax * mmax + j * kmax * lmax * mmax + k * lmax * mmax + l * mmax + m];
        }

        inline REAL_T& get(int i, int j, int k, int l, int m, int n) {
            return data[i * jmax * kmax * lmax * mmax * nmax + j * kmax * lmax * mmax * nmax + k * lmax * mmax * nmax + l * mmax * nmax + m * nmax + m];
        }

        static DataObjectType GetType(const std::string& str) {
            if (str == "catch_biomass") {
                return CATCH_BIOMASS;
            } else if (str == "catch_proportion_at_age") {
                return CATCH_PROPORTION_AT_AGE;
            } else if (str == "catch_proportion_at_length") {
                return CATCH_PROPORTION_AT_LENGTH;
            } else if (str == "catch_mean_size_at_age") {
                return CATCH_MEAN_SIZE_AT_AGE;
            } else if (str == "survey_biomass") {
                return CATCH_BIOMASS;
            } else if (str == "survey_proportion_at_age") {
                return CATCH_PROPORTION_AT_AGE;
            } else if (str == "survey_proportion_at_length") {
                return CATCH_PROPORTION_AT_LENGTH;
            } else if (str == "survey_mean_size_at_age") {
                return CATCH_MEAN_SIZE_AT_AGE;
            } else {
                std::cout << "Data Error: unknown data_object_type \"" << str << "\"";
            }

            return UNKNOWN;

        }



    };

    template <typename T>
    T StringToNumber(const std::string &Text) {
        std::istringstream ss(Text);
        T result;
        return (ss >> result) ? result : 0;
    }

}


#endif /* COMMON_HPP */

