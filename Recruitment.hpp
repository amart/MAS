/* 
 * File:   Recruitment.hpp
 * 
 * Author: Matthew Supernaw
 * National Oceanic and Atmospheric Administration
 * National Marine Fisheries Service 
 * Sustainable Fisheries Division
 * St. Petersburg, FL, 33701
 * 
 * Created on September 16, 2016, 12:32 PM
 * 
 * This File is part of the NOAA, National Marine Fisheries Service 
 * Metapopulation Assessment System project.
 * 
 * This software is a "United States Government Work" under the terms of the
 * United States Copyright Act.  It was written as part of the author's official
 * duties as a United States Government employee and thus cannot be copyrighted.
 * This software is freely available to the public for use. The National Oceanic 
 * And Atmospheric Administration and the U.S. Government have not placed any 
 * restriction on its use or reproduction.  Although all reasonable efforts have 
 * been taken to ensure the accuracy and reliability of the software and data, 
 * the National Oceanic And Atmospheric Administration and the U.S. Government 
 * do not and cannot warrant the performance or results that may be obtained by 
 * using this  software or data. The National Oceanic And Atmospheric
 * Administration and the U.S. Government disclaim all warranties, express or 
 * implied, including warranties of performance, merchantability or fitness 
 * for any particular purpose.
 *
 * Please cite the author(s) in any work or product based on this material.
 *
 */

#ifndef RECRUITMENT_HPP
#define RECRUITMENT_HPP
#include "Common.hpp"
namespace mas {

    template<typename REAL_T>
    struct RecruitmentBase : mas::ModelObject<REAL_T> {
        typedef typename mas::VariableTrait<REAL_T>::variable variable;
        std::vector<variable> recruitment_deviations;
        variable sigma_r;
        variable rho;

        virtual const variable Evaluate(const variable& s) = 0;

        virtual const std::string Name() {
            return "RecruitmentBase";
        }
    };

    template<typename REAL_T>
    struct Ricker : RecruitmentBase<REAL_T> {
        typedef typename VariableTrait<REAL_T>::variable variable;
        variable alpha;
        variable beta;

        const variable Evaluate(const variable& s) {
            return alpha * s * std::exp(static_cast<REAL_T> (-1.0) * beta * s);
        }

        virtual const std::string Name() {
            return "Ricker";
        }
    };

    template<typename REAL_T>
    struct RickerAlt : RecruitmentBase<REAL_T> {
        typedef typename VariableTrait<REAL_T>::variable variable;
        variable R0; //unfished equilibrium recruitment
        variable phi0; //unfished spawning biomass or abundance per recruit
        variable A; //adjustment factor

        /**
         * Alternative Ricker Spawn-Recruit relationship
         * @param s - spawning biomass or abundance
         * @return 
         */
        const variable Evaluate(const variable& s) {
            return (s / phi0) * std::exp(A * (static_cast<REAL_T> (1.0)
                    - (s / (phi0 * R0))));
        }

        virtual const std::string Name() {
            return "Ricker Alt";
        }
    };

    template<typename REAL_T>
    struct BevertonHolt : RecruitmentBase<REAL_T> {
        typedef typename VariableTrait<REAL_T>::variable variable;
        variable alpha; //maximum recruitment
        variable beta; //the stock level needed to produce the half of maximum recruitment

        /**
         * Beverton-Holt Spawn-Recruit relationship
         * 
         * @param s - spawning biomass or abundance
         * @return 
         */
        const variable Evaluate(const variable& s) {
            return (alpha * s) / (beta + s);
        }

        virtual const std::string Name() {
            return "Beverton-Holt";
        }
    };

    template<typename REAL_T>
    struct BevertonHoltAlt : RecruitmentBase<REAL_T> {
        typedef typename VariableTrait<REAL_T>::variable variable;
        variable R0; //unfished equilibrium recruitment
        variable phi0; //unfished spawning biomass or abundance per recruit
        variable h; //steepness

        /**
         * Alternative Beverton-Holt S-R relationship
         * 
         * @param s - spawning biomass or abundance
         * @return 
         */
        const variable Evaluate(const variable& s) {
            return (static_cast<REAL_T> (4.0) * R0 * h * s) /
                    ((phi0 * R0 * (static_cast<REAL_T> (1.0) - h)) +
                    (s * (static_cast<REAL_T> (5.0) * h
                    - static_cast<REAL_T> (1.0))));
        }

        virtual const std::string Name() {
            return "Beverton-Holt Alt";
        }
    };

    template<typename REAL_T>
    struct BevertonHoltDep : RecruitmentBase<REAL_T> {
        typedef typename VariableTrait<REAL_T>::variable variable;
        variable alpha; // maximum recruitment
        variable beta; // the stock level needed to produce the half of maximum recruitment
        variable c; //exponent

        /**
         * Depensatory Beverton-Holt S-R relationship
         * 
         * @param s - spawning biomass or abundance
         * @return 
         */
        const variable Evaluate(const variable& s) {
            variable s_c = std::pow(s, c);
            return (alpha * s_c) / (beta + s_c);
        }

        virtual const std::string Name() {
            return "Beverton-Holt Dep";
        }
    };

    template<typename REAL_T>
    struct Shepherd : RecruitmentBase<REAL_T> {
        typedef typename VariableTrait<REAL_T>::variable variable;
        variable alpha;
        variable beta;
        variable c;

        const variable Evaluate(const variable& s) {
            return (alpha * s) / (static_cast<REAL_T> (1.0) + std::pow((s / beta), c));
        }

        virtual const std::string Name() {
            return "Shepard";
        }
    };

    template<typename REAL_T>
    struct Deriso : RecruitmentBase<REAL_T> {
        typedef typename VariableTrait<REAL_T>::variable variable;
        variable alpha;
        variable beta;
        variable c;

        const variable Evaluate(const variable& s) {
            return (alpha * s) * std::pow((static_cast<REAL_T> (1.0) - beta * c * s), static_cast<REAL_T> (1.0) / c);
        }

        virtual const std::string Name() {
            return "Deriso";
        }
    };



}


#endif /* RECRUITMENT_HPP */

