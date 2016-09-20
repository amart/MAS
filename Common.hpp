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


namespace mas {

    std::ofstream mas_log("mas.log");

    template<typename REAL_T>
    struct VariableTrait {
        typedef atl::Variable<REAL_T> variable;

        static void SetName(variable& var, const std::string& value) {
            std::cout<<value<<"\n";
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
}


#endif /* COMMON_HPP */

