/* 
 * File:   Mortality.hpp
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

#ifndef MAS_MORTALITY_HPP
#define MAS_MORTALITY_HPP

#include "Common.hpp"


namespace mas {

    template<typename REAL_T>
    struct NaturalMortality : mas::ModelObject<REAL_T> {
        typedef typename VariableTrait<REAL_T>::variable variable;
        std::vector<variable> mortality_vector;

        std::unordered_map<REAL_T, variable> mortality;
        
        typedef typename std::unordered_map<REAL_T, variable>::iterator mortality_iterator;

        const variable Evaluate(const int& age_index) {
            return mortality_vector[age_index];

        }

        virtual const std::string Name() {
            return "Natural Mortality";
        }

    };
    
    
     template<typename REAL_T>
    struct FishingMortality : mas::ModelObject<REAL_T> {
        typedef typename VariableTrait<REAL_T>::variable variable;
        std::vector<std::vector<variable> > fishing_mortality;

        const variable Evaluate(const int& year, const int& season) {
//            std::cout<<__func__<<"\n size ==== "<<this->id<<"\n";
            return fishing_mortality[year][season];
        }

        virtual const std::string Name() {
            return "Natural Mortality";
        }

    };

    
    template<typename REAL_T>
    std::ostream& operator <<(std::ostream& out, const FishingMortality<REAL_T>& fm ){
        std::cout<<"Fishing Mortality{\n";
        std::cout<<"id "<<fm.id<<"\n";
        std::cout<<"matrix["<<fm.fishing_mortality.size()<<"\n}";
        return out;
    }

}


#endif /* MAS_MORTALITY_HPP */

