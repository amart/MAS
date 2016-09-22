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
        std::map<variable*,int> estimated_parameters_map;
        typedef typename std::map<variable*,int>::iterator estimable_parameter_iterator; 
        std::vector<variable*> estimated_parameters;
        std::vector<int> estimated_phase;

        void Register(variable& var, int phase = 1) {
            estimated_parameters_map[&var] = phase;
            this->estimated_parameters.push_back(&var);
            this->estimated_phase.push_back(phase);
        }
    };
}


#endif /* COMMON_HPP */

