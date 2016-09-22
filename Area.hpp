/* 
 * File:   Area.hpp
 * 
 * Author: Matthew Supernaw
 * National Oceanic and Atmospheric Administration
 * National Marine Fisheries Service 
 * Sustainable Fisheries Division
 * St. Petersburg, FL, 33701
 * 
 * Created on September 16, 2016, 12:36 PM
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

#ifndef MAS_AREA_HPP
#define MAS_AREA_HPP
#include "Recruitment.hpp"
#include "Mortality.hpp"
#include "Growth.hpp"
#include "Fecundity.hpp"
namespace mas {

    template<typename REAL_T>
    struct Area {
        std::string name;
        int id;
        int growth_model_id;
        int recruitment_model_id;
        int mortality_model_id;
        int fecundity_model_id;
        
        
        std::shared_ptr<GrowthBase<REAL_T> > growth_model;
        std::shared_ptr<RecruitmentBase<REAL_T> > recruitment_model;
        std::shared_ptr<Mortality<REAL_T> > mortality_model;
        std::shared_ptr<FecundityBase<REAL_T> > fecundity_model;

    };
    
    template<typename REAL_T>
    std::ostream& operator << (std::ostream& out, const mas::Area<REAL_T>& area){
     
        out<<"Area:\n";
        out<<"Name: "<<area.name<<"\n";
        out<<"Id: "<<area.id<<"\n";
        out<<"Growth Model: "<<area.growth_model->id<<"\n";
        out<<"Recruitment Model: "<<area.recruitment_model->id<<"\n";
        out<<"Mortality Model: "<<area.mortality_model->id<<"\n\n";
        
        return out;
    }


}


#endif /* MAS_AREA_HPP */

