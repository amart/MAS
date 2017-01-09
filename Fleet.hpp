/* 
 * File:   Fleet.hpp
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

#ifndef MAS_FLEET_HPP
#define MAS_FLEET_HPP

#include "Common.hpp"
#include "Selectivity.hpp"


namespace mas {

    template<typename REAL_T>
    struct Fleet : mas::ModelObject<REAL_T> {
        typedef typename VariableTrait<REAL_T>::variable variable;
        variable f;
        variable year_frac_peak_fishing;
        std::string name;

        //area, year X season
        //        std::map<int, std::vector<variable> > f;

        std::unordered_map<int, std::unordered_map<int, int> > season_area_selectivity_ids;
        std::unordered_map<int, std::unordered_map<int, std::shared_ptr<mas::SelectivityBase<REAL_T> > > > season_area_selectivity;

        std::unordered_map<int, std::unordered_map<int, int> > area_season_selectivity_ids;
        std::unordered_map<int, std::unordered_map<int, std::shared_ptr<mas::SelectivityBase<REAL_T> > > > area_season_selectivity;

        std::unordered_map<int, std::unordered_map<int, int> > area_season_fishing_mortality_ids;
        std::unordered_map<int, std::unordered_map<int, std::shared_ptr<mas::FishingMortality<REAL_T> > > > area_season_fishing_mortality;

        std::unordered_map<int, std::unordered_map<int, int> > season_area_fishing_mortality_ids;
        std::unordered_map<int, std::unordered_map<int, std::shared_ptr<mas::FishingMortality<REAL_T> > > > season_area_fishing_mortality;

        typedef typename std::unordered_map<int, std::unordered_map<int, int> >::iterator season_area_selectivity_ids_iterator;
        typedef typename std::unordered_map<int, std::shared_ptr<mas::SelectivityBase<REAL_T> > >::iterator area_sectivity_iterator;
        typedef typename std::unordered_map<int, std::unordered_map<int, int> >::iterator season_area_id_iterator;
        typedef typename std::unordered_map<int, int>::iterator area_id_iteraor;
        typedef typename std::unordered_map<int, int>::iterator season_id_iteraor;
        typedef typename std::unordered_map<int, std::unordered_map<int, std::shared_ptr<mas::SelectivityBase<REAL_T> > > >::iterator season_area_selectivity_iterator;
        typedef typename std::unordered_map<int, std::unordered_map<int, std::shared_ptr<mas::FishingMortality<REAL_T> > > >::iterator season_area_fishing_mortality_iterator;
        
    };


}


#endif /* MAS_FLEET_HPP */

