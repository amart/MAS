/* 
 * File:   Movement.hpp
 * 
 * Author: Matthew Supernaw
 * National Oceanic and Atmospheric Administration
 * National Marine Fisheries Service 
 * Sustainable Fisheries Division
 * St. Petersburg, FL, 33701
 * 
 * Created on September 16, 2016, 12:34 PM
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

#ifndef MAS_MOVEMENT_HPP
#define MAS_MOVEMENT_HPP

#include "Common.hpp"

namespace mas {

    /**
     *Movement probabilities for a box-transfer model for a given year.
     * 
     * Probability information is defined by season and areas.
     */
    template<typename REAL_T>
    struct MovementBase : mas::ModelObject<REAL_T> {
        typedef typename VariableTrait<REAL_T>::variable variable;
        std::vector<std::vector<std::vector<variable> > >  male_connectivity; 
        std::vector<std::vector<std::vector<variable> > >  female_connectivity; 

        virtual const variable Evaluate(const int& from_area_id, const int& to_area_id, const int& sex, const variable& age) = 0;

        virtual const std::string Name() {
            return "MovementBase";
        }

    };

    template<typename REAL_T>
    struct BoxTransfer : MovementBase<REAL_T> {
        typedef typename VariableTrait<REAL_T>::variable variable;

        /**
         * Age based movement
         *
         * @param from_area_id
         * @param to_area_id
         * @param sex
         * @param age
         * @return fraction_to_move
         */
        virtual const variable Evaluate(const int& from_area_id, const int& to_area_id, const int& sex, const variable& age) {
            // Q:  how to access the population-specific movement array in here?
            // FIX!!!!!
            return static_cast<REAL_T> (1.0);
        }

        virtual const std::string Name() {
            return "Box Transfer";
        }

    };

}


#endif /* MAS_MOVEMENT_HPP */

