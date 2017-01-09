/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Population.hpp
 * Author: matthewsupernaw
 *
 * Created on September 14, 2016, 1:44 PM
 */

#ifndef POPULATION_HPP
#define POPULATION_HPP



#include <memory>
#include <vector>
#include <iomanip>
#include <unordered_map>
#include "Area.hpp"

#include "../ATL/AutoDiff/AutoDiff.hpp"
#include "Movement.hpp"
#include "Recruitment.hpp"

namespace mas {


    template<class REAL_T>
    class Population;

    /**
     *Runtime calculated information for a population in a specific area.
     */
    template<class REAL_T>
    struct AreaPopulation {
        bool male_cohorts = true;
        typedef typename mas::VariableTrait<REAL_T>::variable variable;
        Population<REAL_T>* natal_population;

        bool natal_homing = false;
        std::shared_ptr<Area<REAL_T> > area;    // Q:  what is the definition of area?
        std::shared_ptr<Area<REAL_T> > natal_area;

        std::shared_ptr<GrowthBase<REAL_T> > growth_model;
        std::shared_ptr<MovementBase<REAL_T> > movement_model;
        std::shared_ptr<mas::NaturalMortality<REAL_T> > natural_mortality_model; //area specific
        std::map<int, std::shared_ptr<mas::RecruitmentBase<REAL_T> > > recruitment_model; // area-season specific
        typedef typename std::map<int, std::shared_ptr<mas::RecruitmentBase<REAL_T> > >::iterator recruitment_model_iterator;

        // Q:  what is the definition of fecundity?  use new MaturityBase here instead?
        std::map<int, std::shared_ptr<mas::FecundityBase<REAL_T> > > maturity_model; // area-season specific

        // NOTE:  all population dynamics calculations done for both males and females
        // NOTE:  male_cohort will indicate whether user wants results by sex or aggregated (I guess)
        int sexes = 2;

        int pop_areas;      // in how many areas does the whole population operate?
        int years;
        int seasons;
        std::vector<REAL_T> ages;
        std::vector<REAL_T> season_duration;        // fraction of year duration for each season
        std::vector<REAL_T> peak_spawning_event;    // fraction of year duration from the beginning of each season

        std::vector<std::shared_ptr<Fleet<REAL_T> > > fleet_list; //all fleets in this area

        // Q:  are both of these needed?  immigrants from area 1 to area 2 == emigrants from area 1 to area 2
        std::vector<variable> growth;
        std::vector<variable> recruitment;
        std::vector<variable> abundance;
        std::vector<variable> spawning_biomass;
        std::vector<variable> initial_numbers;

        std::vector<variable> M;    // natural mortality-at-age, by area, year, season, sex, age (could omit year for now...)
        std::vector<variable> F;    // total fishing mortality-at-age, by fleet, area, year, season, sex, age
        std::vector<variable> Z;    // total mortality-at-age, by area, year, season, sex, age
        std::vector<variable> S;    // dunno
        std::vector<variable> N;    // numbers-at-age, by area, year, season, sex, age
        std::vector<variable> SN;   // spawning numbers-at-age by area, year, season, sex, age (could omit males in the future...)
        std::vector<variable> FN;   // fishery numbers-at-age, by fleet, area, year, season, sex, age
        std::vector<variable> fishery_biomass;  // fishery catch mass, by fleet, area, year, season
        std::vector<variable> IN;   // index numbers-at-age, by fleet, area, year, season, sex, age
        std::vector<variable> index_biomass;    // index catch mass, by fleet, area, year, season
        std::vector<variable> spawning_biomass; // male and female spawning mass, by area, year, season, sex
        std::vector<variable> emigrants;        // emigrating numbers-at-age, by to_area, year, season, sex, age

        AreaPopulation(const std::vector<REAL_T> ages,
                       int pop_areas,
                       int years,
                       int seasons,
                       const std::vector<REAL_T> season_duration,
                       const std::vector<REAL_T> peak_spawning_event,
                       const std::shared_ptr<Area<REAL_T> >& natal_area,
                       const std::shared_ptr<Area<REAL_T> >& area,
                       const std::vector<std::shared_ptr<Fleet<REAL_T> > >& fleet_list) :
        ages(ages),
        pop_areas(pop_areas),
        years(years),
        seasons(seasons),
        season_duration(season_duration),
        peak_spawning_event(peak_spawning_event),
        natal_area(natal_area),
        area(area),
        fleet_list(fleet_list) {

        }

        void Initialize() {

            recruitment.resize(years * seasons * sexes);     // age-0
            // Q:  what is the definition of abundance?
            abundance.resize(years * seasons);
            initial_numbers.resize(seasons * sexes * ages.size());

            // Q:  what is the definition of growth?  need the age-length array and/or length-at-age array
            growth.resize(years * seasons * sexes * ages.size());

            M.resize(years * seasons * sexes * ages.size());
            Z.resize(years * seasons * sexes * ages.size());
            N.resize(years * seasons * sexes * ages.size());
            SN.resize(years * seasons * sexes * ages.size());

            F.resize(fleet_list.size() * years * seasons * sexes * ages.size());
            FN.resize(fleet_list.size() * years * seasons * sexes * ages.size());
            fishery_biomass(fleet_list.size() * years * seasons);
            IN.resize(fleet_list.size() * years * seasons * sexes * ages.size());
            index_biomass(fleet_list.size() * years * seasons);

            spawning_biomass(years * seasons);

            emigrants.resize(pop_areas * years * seasons * sexes * ages.size());

            // Q:  what is S?  MRS said survival - definition?
            // S.resize(areas.size() * years * seasons * sexes * ages.size());
        }

        /**
         * Evaluates recruitment (age 0) in all areas
         *
         * @param year
         * @param season
         */
        inline void Recruitment(int year, int season) {
            //#warning add compiler hint here
            if (years == 0) {   // ?????
                for (int x = 0; x < this->sexes; ++x) {
                    recruitment[area->id][year][season][x] = this->N[area->id][year][season][x][0];
                }
            } else {
                recruitment_model_iterator rit = this->recruitment_model.find(area->id, season);

                if (rit != this->recruitment_model.end()) {
//                      std::cout << this->growth_model->Evaluate(this->ages[0]) << " ";
                    // calculate recruits, then
                    // split between males and females
                    // into this->N[area->id][year][season][x][0]
                }
            }
        }

        /**
         * Evaluates growth in all areas
         *
         * @param year
         * @param season
         */
        inline void Growth(int year, int season) {
            // Q:  what is the definition of growth?
            // NOTE:  need age-length transition array and/or length-at-age array
            growth[year * this->seasons * this->ages.size() + (season - 1) * this->ages.size()] = variable(.01);
            for (int a = 1; a< this->ages.size(); a++) {
                growth[year * this->seasons * this->ages.size() + (season - 1) * this->ages.size() + a] =
                        this->area->growth_model->getLength(this->ages[a]);
            }
        }

        /**
         * Evaluates total mortality in all areas
         *
         * @param year
         * @param season
         */
        inline void Mortality(int year, int season) {

            for (int x = 0; x < this->sexes; ++x) {
                for (int a = 0; a< this->ages.size(); a++) {

                    variable f_a = static_cast<REAL_T> (0.0);

                    std::vector< std::shared_ptr<Fleet<REAL_T> > >& fleets = this->area->seasonal_fishing_operations[area->id, year, season];

                    if (fleets) {
                        for (int f = 0; f < fleets.size(); f++) {
                            F[fleets[f]->id][area->id][year][season][x][a] =
                                fleets[f]->area_season_fishing_mortality[area->id][season]->Evaluate(year, (season - 1)) *
                                    fleets[f]->season_area_selectivity[area->id][season]->Evaluate(x, ages[a]);
                            f_a += F[fleets[f]->id][area->id][year][season][x][a];
                        }
                    }

                    // NOTE:  need Z by area, year, season, sex, and age - multiply by season duration when using
                    Z[area->id][year][season][x][a] = this->natural_mortality_model->Evaluate(x, a) + f_a;
                }
            }
        }

        /**
         * Evaluates fecundity in all areas
         *
         * @param year
         * @param season
         */
        inline void Fecundity(int year, int season) {
            // Q:  what is the definition of fecundity?
            if (natal_homing) {
                //use natal area parameters
            } else {
                // use  area parameters
            }
        }

        /**
         * calculates length-at-age by sex
         *
         * @param season
         * @param sex
         * @param age
         * @param additional_year_fraction
         * @returns length
         */
        inline REAL_T getLength(int season, int sex, REAL_T age, REAL_T additional_year_fraction)
        {
            REAL_T year_fraction = additional_year_fraction;

            if (season > 1)
            {
                for (int s = 1; s < season; ++s)
                {
                    year_fraction += season_duration[season - 1];
                }
            }

            return this->growth_model->getLength(sex, age + year_fraction);
        }

        /**
         * calculates weight-at-length by sex
         *
         * @param sex
         * @param length
         * @return weight
         */
        inline REAL_T getWeight(int sex, REAL_T length)
        {
            if (length < static_cast<REAL_T>(0.00001))
            {
                return static_cast<REAL_T>(0.0001);
            }

            return this->growth_model->getWeight(sex, length);
        }

        /**
         * Evaluates spawning biomass in all areas
         *
         * @param year
         * @param season
         */
        inline void SpawningBiomass(int year, int season) {
            if (natal_homing) {
                //use natal area parameters
                variable peak_spawning_event = this->peak_spawning_event[area->id][season];

                for (int x = 0; x < this->sexes; ++x)
                {
                    spawning_biomass[area->id][year][season][x] = static_cast<REAL_T> (0.0);

                    for (int a = 0; a < ages.size(); ++a)
                    {
                        // numbers at the time of peak spawning
                        SN[area->id][year][season][x][a] = this->N[area->id][year][season][x][a] * this->maturity_model[season]->Evaluate(x, a) *
                            atl::exp(static_cast<REAL_T>(-1.0) * peak_spawning_event * Z[area->id][year][season][x][a]);

                        spawning_biomass[area->id][year][season][x] +=
                            (SN[area->id][year][season][x][a] * getWeight(getLength(season, x, a, peak_spawning_event)));
                    }
                }
            } else {
                // use  area parameters
            }
        }

        /**
         * Evaluates numbers at age in all areas
         *
         * @param year
         * @param season
         */
        inline void NumbersAtAge(int year, int season) {

            if (year == 0 && season == 1) { // ?????
                for (int x = 0; x < this->sexes; ++x) {
                    for (int a = 0; a < ages.size(); a++) {
                        this->N[area->id][year][season][x][a] = this->initial_numbers[area->id][season][x][a];
                    }

                    // NOTE:  the above loop should be changed to this loop - to calculate equilibrium unfished numbers-at-age
                    // this->initial_numbers[area->id][season][x][0] = atl::exp(log_median_recruit);
                    // for (int a = 1; a < ages.size(); a++) {
                    //     this->initial_numbers[area->id][season][x][a] = this->initial_numbers[area->id][season][x][a-1] *
                    //         atl::exp(static_cast<REAL_T>(-1.0) * this->natural_mortality_model->Evaluate(x, (a-1)));
                    // }
                    // plus group
                    // int a = ages.size() - 1;
                    // this->initial_numbers[area->id][season][x][a] /=
                    //     (static_cast<REAL_T> (1.0) - atl::exp(static_cast<REAL_T>(-1.0) * this->natural_mortality_model->Evaluate(x, (a-1))));
                }
            } else {
                int y = year;
                int s = season;
                bool same_year = true;
                if (season == 1) {
                    s = this->seasons;
                    y--;
                    same_year = false;
                }
                else
                {
                    s--;
                }

                //calc new numbers at the beginning of the season

                for (int x = 0; x < this->sexes; ++x) {

                    // NOTE:  need to apply annual rec devs in first season
                    // how to access rec dev parameters?
                    if (season == 1)
                    {
                        // this->N[area->id][year][season][x][0] *= atl::exp(rec_dev[year]);
                    }

                    for (int a = 1; a < ages.size(); a++) {
                        int prev_age = same_year ? a : (a-1);
                        this->N[area->id][year][season][x][a] = this->N[area->id][y][s][x][prev_age] *
                            atl::exp(static_cast<REAL_T> (-1.0) * this->season_duration[s-1] * this->Z[area->id][y][s][x][prev_age]);
                    }
                    // plus group
                    int plus_age = (ages.size() - 1);
                    this->N[area->id][year][season][x][plus_age] += (this->N[area->id][y][s][x][plus_age] *
                        atl::exp(static_cast<REAL_T> (-1.0) * this->season_duration[s-1] * this->Z[area->id][y][s][x][plus_age]));
                }
            }

            // NOTE: NEED TO REORG THESE TWO BLOCKS
            // calculate emigrants at the beginning of the season
            for (int from_area_id; from_area_id < areas.size(); ++from_area_id)
            {
                for (int x = 0; x < this->sexes; ++x)
                {
                    for (int a = 0; a < ages.size(); ++a)
                    {
                        for (int to_area_id = 0; to_area_id < areas.size(); ++to_area_id)
                        {
                            if (to_area_id != from_area_id)
                            {
                                this->emigrants[from_area_id][to_area_id][year][season][x][a] =
                                    (this->N[from_area_id][year][season][x][a] *
                                     this->movement_model->Evaluate(from_area_id, to_area_id, x, a));
                            }
                        }
                    }
                }
            }

            //move fish
            for (int from_area_id; from_area_id < areas.size(); ++from_area_id)
            {
                for (int x = 0; x < this->sexes; ++x)
                {
                    for (int a = 0; a < ages.size(); ++a)
                    {
                        for (int to_area_id = 0; to_area_id < areas.size(); ++to_area_id)
                        {
                            if (to_area_id != from_area_id)
                            {
                                this->N[from_area_id][year][season][x][a] +=
                                    // arrivals
                                    (this->emigrants[to_area_id][from_area_id][year][season][x][a] -
                                    // departures
                                     this->emigrants[from_area_id][to_area_id][year][season][x][a]);
                            }
                        }
                    }
                }
            }

            // calculate spawning biomass
            this->SpawningBiomass(year, season);

            // calculate recruits for next season or first season of next year
            int s = season + 1;
            int y = year;
            if (season == this->seasons)
            {
                s = 1;
                y++;
            }
            this->Recruitment(y, s);
        }

        /**
         * Evaluates catch biomass in all areas
         *
         * @param year
         * @param season
         */
        inline void CatchBiomass(int year, int season) {

            if (natal_homing) {
                //use natal area parameters
                std::vector< std::shared_ptr<Fleet<REAL_T> > >& fleets = this->area->seasonal_fishing_operations[area->id, year, season];

                if (fleets)
                {
                    for (int f = 0; f < fleets.size(); ++f)
                    {
                        fishery_biomass[fleets[f]->id][area->id][year][season] = static_cast<REAL_T> (0.0);

                        for (int x = 0; x < this->sexes; ++x)
                        {
                            for (int a = 0; a < ages.size(); ++a)
                            {
                                // this calculation assumes constant fishing throughout the season
                                variable tot_fmort = F[fleets[f]->id][area->id][year][season][x][a];
                                variable tot_mort  = this->natural_mortality_model->Evaluate(x, a) + tot_fmort;
                                FN[fleets[f]->id][area->id][year][season][x][a] = this->N[area->id][year][season][x][a] * (tot_fmort / tot_mort) *
                                    (static_cast<REAL_T>(1.0) - atl::exp(static_cast<REAL_T>(-1.0) * this->season_duration[season] * tot_mort));

                                fishery_biomass[fleets[f]->id][area->id][year][season] +=
                                    (FN[fleets[f]->id][year][season][x][a] * getWeight(getLength(season, x, a, fleets[f]->peak_fleet_event[area->id][season])));
                            }
                        }
                    }
                }
            } else {
                // use  area parameters
            }
        }

        /**
         * Evaluates index biomass in all areas
         *
         * @param year
         * @param season
         */
        inline void SurveyBiomass(int year, int season) {

            if (natal_homing) {
                //use natal area parameters
                std::vector< std::shared_ptr<Fleet<REAL_T> > >& fleets = this->area->seasonal_index_operations[area->id, year, season];

                if (fleets)
                {
                    for (int f = 0; f < fleets.size(); ++f)
                    {
                        index_biomass[fleets[f]->id][area][year][season] = static_cast<REAL_T> (0.0);
                        variable peak_index_event = fleets[f]->peak_fleet_event[area->id][season];

                        for (int x = 0; x < this->sexes; ++x)
                        {
                            for (int a = 0; a < ages.size(); ++a)
                            {
                                // numbers at the time of peak index
                                IN[fleets[f]->id][area->id][year][season][x][a] = this->N[area->id][year][season][x][a] *
                                    fleets[f]->season_area_selectivity[area->id][season]->Evaluate(x, ages[a]) *
                                    atl::exp(static_cast<REAL_T>(-1.0) * peak_index_event * Z[area->id][year][season][x][a]);

                                index_biomass[fleets[f]->id][area->id][year][season] +=
                                    (IN[fleets[f]->id][year][season][x][a] * getWeight(getLength(season, x, a, peak_index_event)));
                            }
                        }
                    }
                }
            } else {
                // use  area parameters
            }
        }

        inline void Reset() {

        }

    };

    template<typename REAL_T>
    std::ostream& operator<<(std::ostream& out, mas::AreaPopulation<REAL_T>& pi) {
        out << std::fixed;
        out << std::setprecision(2);
        out << "Population " << pi.natal_population->id << "\n";
        out << "Area " << pi.area->id << "\n";
        out << "Total Mortality at Age (Z)\n";
        if (pi.male_cohorts) {
            out << "Males\n";
        } else {
            out << "Females\n";
        }
        for (int a = 0; a < pi.ages.size(); a++) {
            for (int y = 0; y < pi.years; y++) {
                for (int s = 0; s < pi.seasons; s++) {
                    out << pi.Z[y * pi.seasons * pi.ages.size() + s * pi.ages.size() + a] << " ";
                }
            }
            out << "\n";
        }
        out << "\n\n";

        out << "Population " << pi.natal_population->id << "\n";
        out << "Area " << pi.area->id << "\n";
        out << "Numbers at Age \n";
        if (pi.male_cohorts) {
            out << "Males\n";
        } else {
            out << "Females\n";
        }
        for (int a = 0; a < pi.ages.size(); a++) {
            for (int y = 0; y < pi.years; y++) {
                for (int s = 0; s < pi.seasons; s++) {
                    out << pi.N[y * pi.seasons * pi.ages.size() + s * pi.ages.size() + a] << " ";
                }
            }
            out << "\n";
        }
        out << "\n\n";

        out << "Population " << pi.natal_population->id << "\n";
        out << "Area " << pi.area->id << "\n";
        out << "Total Emigrants \n";
        if (pi.male_cohorts) {
            out << "Males\n";
        } else {
            out << "Females\n";
        }
        for (int a = 0; a < pi.ages.size(); a++) {
            for (int y = 0; y < pi.years; y++) {
                for (int s = 0; s < pi.seasons; s++) {
                    out << pi.emigrants[y * pi.seasons * pi.ages.size() + s * pi.ages.size() + a] << " ";
                }
            }
            out << "\n";
        }
        out << "\n\n";

        out << "Population " << pi.natal_population->id << "\n";
        out << "Area " << pi.area->id << "\n";
        out << "Growth Pattern \n";
        if (pi.male_cohorts) {
            out << "Males\n";
        } else {
            out << "Females\n";
        }
        for (int a = 0; a < pi.ages.size(); a++) {
            for (int y = 0; y < pi.years; y++) {
                for (int s = 0; s < pi.seasons; s++) {
                    out << pi.growth[y * pi.seasons * pi.ages.size() + s * pi.ages.size() + a] << " ";
                }
            }
            out << "\n";
        }
        out << "\n\n";

        return out;
    }

    template<typename REAL_T>
    struct InitialNumbers {
        typedef typename mas::VariableTrait<REAL_T>::variable variable;
        FishSexType type;
        int area_id;
        std::vector<variable> values;
    };

    template<typename REAL_T>
    class Population : public mas::ModelObject<REAL_T> {
    public:
        /*********************************************
         * Area specific natural mortality           *
         *********************************************/
        std::map<int, int> male_natural_mortality_ids; //area, natural mortality model
        std::map<int, int> female_natural_mortality_ids; //area, natural mortality model
        typedef typename std::map<int, int>::iterator male_natural_mortality_ids_iterator;
        typedef typename std::map<int, int>::iterator female_natural_mortality_ids_iterator;

        /*********************************************
         * Area specific recruitment                 *
         *********************************************/
        std::map<int, std::map<int, int> > recruitment_ids; //area, recruitment model
        typedef typename std::map<int, std::map<int, int> >::iterator recruitment_ids_iterator;
        typedef typename std::map<int, int>::iterator recruitment_season_ids_iterator;


        std::vector<InitialNumbers<REAL_T> > initial_numbers;
        typedef typename mas::VariableTrait<REAL_T>::variable variable;
        std::string name;
        int natal_area_id;
        int movement_model_id;
        bool natal_homing = false;
        bool natal_recruitment = false;
        bool move_fish_before_lh = false;
        int years;
        int seasons;
        int areas;
        int fleets;
        int ages;
        int growth_id;

        std::vector<REAL_T> season_duration[seasons];
        std::shared_ptr<Area<REAL_T> > natal_area; //birth area
        std::vector<std::shared_ptr<Area<REAL_T> > > area_list; //all areas
        std::vector<std::shared_ptr<Fleet<REAL_T> > > fleet_list; //all fleets

        //Movement Tracking
        typedef typename std::unordered_map<int, AreaPopulation<REAL_T> >::iterator cohort_iterator;
        std::unordered_map<int, AreaPopulation<REAL_T> > ap_map;
        std::unordered_map<int, int > movement_models_ids; //season keyed
        typedef typename std::unordered_map<int, int >::iterator movement_model_id_iterator;

        std::shared_ptr<mas::MovementBase<REAL_T> > movement_model;

        std::unordered_map<int, std::shared_ptr<mas::MovementBase<REAL_T> > > movement_models; //year keyed
        typedef typename std::unordered_map<int, std::shared_ptr<mas::MovementBase<REAL_T> > >::iterator movement_model_iterator;
        //Estimable
        std::vector<std::vector<variable> > movement_coefficients;
        std::vector<variable> initial_population;



        //    typedef typename std::unordered_map<std::vector<std::vector<variable> > >::iterator movement_coefficient_iterator;

        Population() {
        }

        Population(int years,
                int seasons,
                const std::vector<REAL_T> season_duration,
                int areas,
                int fleets,
                const std::shared_ptr<Area<REAL_T> >& natal_area,
                const std::vector<std::shared_ptr<Area<REAL_T> > >& area_list,
                const std::vector<std::shared_ptr<Fleet<REAL_T> > >& fleet_list) :
        years(years),
        seasons(seasons),
        season_duration(season_duration),
        areas(areas),
        fleets(fleets),
        natal_area(natal_area),
        area_list(area_list),
        fleet_list(fleet_list) {

            for (int a = 0; a < area_list.size(); a++) {

                ap_map[area_list[a]->id].natal_homing = this->natal_homing;
                ap_map[area_list[a]->id].area = area_list[a];
                ap_map[area_list[a]->id].natal_area = this->natal_area;

            }

        }

        void Prepare() {


            for (int a = 0; a < area_list.size(); a++) {

                ap_map[a].Reset();
            }

            for (int i = 0; i < this->initial_numbers.size(); i++) {

                for (int x = 0; x < this->sexes; ++x) {
                        std::cout << "Setting initial numbers for males in area " << this->initial_numbers[i].area_id << "\n";
                        this->ap_map[this->initial_numbers[i].area_id].initial_numbers = this->initial_numbers[i].values;
  
                }
            }

        }

        inline void MoveFish(int year, int season) {


            movement_model_iterator it = this->movement_models.find(year + 1);
            if (it != this->movement_models.end()) {

                int s = season - 1;
                std::vector<std::vector<variable> >& male_probabilities = (*it).second->male_connectivity[s];
                std::vector<std::vector<variable> >& female_probabilities = (*it).second->female_connectivity[s];

                //should be square
                for (int i = 0; i < male_probabilities.size(); i++) {
                    AreaPopulation<REAL_T>& male_info_from = this->male_cohorts[(i + 1)];
                    AreaPopulation<REAL_T>& female_info_from = this->female_cohorts[(i + 1)];
                    for (int j = 0; j < male_probabilities.size(); j++) {
                        if (i != j) {
                            AreaPopulation<REAL_T>& male_info_to = this->male_cohorts[(j + 1)];
                            AreaPopulation<REAL_T>& female_info_to = this->female_cohorts[(j + 1)];

                            for (int a = 0; a < this->ages; a++) {
                                male_info_from.emigrants[year * this->seasons * this->ages + (season - 1) * this->ages + a] +=
                                        male_probabilities[i][j] * male_info_from.N[year * this->seasons * this->ages + (season - 1) * this->ages + a];

                                male_info_to.immigrants[year * this->seasons * this->ages + (season - 1) * this->ages + a] +=
                                        male_probabilities[i][j] * male_info_from.N[year * this->seasons * this->ages + (season - 1) * this->ages + a];

                                female_info_from.emigrants[year * this->seasons * this->ages + (season - 1) * this->ages + a] +=
                                        female_probabilities[i][j] * female_info_from.N[year * this->seasons * this->ages + (season - 1) * this->ages + a];

                                female_info_to.immigrants[year * this->seasons * this->ages + (season - 1) * this->ages + a] +=
                                        female_probabilities[i][j] * female_info_from.N[year * this->seasons * this->ages + (season - 1) * this->ages + a];
                            }

                        }

                    }
                }


            } else {
                std::cout << "Configuration Error: Population " << this->id << " has no movement model defined for year " << (year + 1) << "\n";
                mas_log << "Configuration Error: Population " << this->id << " has no movement model defined for year " << (year + 1) << "\n";

            }

        }

        void InitializePopulationinAreas() {

        }

        void Show() {
            for (int a = 0; a < area_list.size(); a++) {
                std::cout << male_cohorts[area_list[a]->id];
                std::cout << female_cohorts[area_list[a]->id];
            }
        }

        void Evaluate() {

            InitializePopulationinAreas();


            if (this->move_fish_before_lh) {
                for (int y = 0; y < this->years; y++) {
                    for (int s = 1; s <= this->seasons; s++) {


                        for (int a = 0; a < area_list.size(); a++) {
                            ap_map[area_list[a]->id].Mortality(y, s);

                            ap_map[area_list[a]->id].NumbersAtAge(y, s);

                            this->MoveFish(y, s);

                        }




                        //                        for (int a = 0; a < area_list.size(); a++) {
                        //                            male_cohorts[area_list[a]->id].NumbersAtAge(y, s);
                        //                            female_cohorts[area_list[a]->id].NumbersAtAge(y, s);
                        //                        }

                        for (int a = 0; a < area_list.size(); a++) {
                            ap_map[area_list[a]->id].CatchAtAge(y, s);
                        }

                    }
                }
            } else {
                for (int y = 0; y < this->years; y++) {
                    for (int s = 1; s <= this->seasons; s++) {

                        for (int a = 0; a < area_list.size(); a++) {
                            ap_map[area_list[a]->id].Mortality(y, s);

                            ap_map[area_list[a]->id].Growth(y, s);

                            ap_map[area_list[a]->id].NumbersAtAge(y, s);

                            this->MoveFish(y, s);

                            //                            male_cohorts[area_list[a]->id].Fecundity(y, s);
                            //                            female_cohorts[area_list[a]->id].Fecundity(y, s);
                            //
                            //                            male_cohorts[area_list[a]->id].Recruitment(y, s);
                            //                            female_cohorts[area_list[a]->id].Recruitment(y, s);
                            //
                            //                            male_cohorts[area_list[a]->id].Growth(y, s);
                            //                            female_cohorts[area_list[a]->id].Growth(y, s);
                        }



                        //                        this->MoveFish();

                        //                        for (int a = 0; a < area_list.size(); a++) {
                        //                            male_cohorts[area_list[a]->id].NumbersAtAge(y, s);
                        //                            female_cohorts[area_list[a]->id].NumbersAtAge(y, s);
                        //                        }

                    }
                }
            }


            for (int a = 0; a < area_list.size(); a++) {
                //                male_cohorts[area_list[a]->id].NumbersAtAge();
            }
        }

    };

    template<typename REAL_T>
    std::ostream& operator<<(std::ostream& out, const mas::Population<REAL_T>& pop) {
        out << "Population:\n";
        out << "Name: " << pop.name << "\n";
        out << "Id: " << pop.id << "\n";
        out << "Natal Area: " << pop.natal_area->id << "\n";
        out << "Movement Areas: ";
        for (int i = 0; i < pop.area_list.size(); i++) {
            out << pop.area_list[i]->id << " ";
        }
        out << "\n";

        return out;
    }

}

#endif /* POPULATION_HPP */

