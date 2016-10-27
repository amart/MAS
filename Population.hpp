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

#include "../AutoDiff_Standalone/AutoDiff/AutoDiff.hpp"
#include "Movement.hpp"
#include "Recruitment.hpp"

namespace mas {



    template<class REAL_T>
    class Population;

    /**
     *Runtime calculated information for a population in a specific area.
     */
    template<class REAL_T>
    struct PopulationInfo {
        bool male_chohorts = true;
        typedef typename mas::VariableTrait<REAL_T>::variable variable;
        Population<REAL_T>* natal_population;

        bool natal_homing = false;
        std::shared_ptr<Area<REAL_T> > area;
        std::shared_ptr<Area<REAL_T> > natal_area;
        std::shared_ptr<GrowthBase<REAL_T> > growth_model;
        std::shared_ptr<mas::NaturalMortality<REAL_T> > natural_mortality_model; //area specific   
        std::map<int, std::shared_ptr<mas::RecruitmentBase<REAL_T> > > recruitment_model; //season area specific  
        typedef typename std::map<int, std::shared_ptr<mas::RecruitmentBase<REAL_T> > >::iterator recruitment_model_iterator;
        //
        int years;
        int seasons;
        std::vector<REAL_T> ages;

        std::vector<variable> imigrants;
        std::vector<variable> emigrants;
        std::vector<variable> growth;
        std::vector<variable> recruitment;
        std::vector<variable> abundance;
        std::vector<variable> spawning_biomass;
        std::vector<variable> M;
        std::vector<variable> initial_numbers;
        std::vector<variable> F;
        std::vector<variable> Z;
        std::vector<variable> S;
        std::vector<variable> N;
        std::vector<variable> C;
        std::vector<variable> predicted_N;

        void Initialize() {
            recruitment.resize(years * seasons);
            abundance.resize(years * seasons);
            initial_numbers.resize(years * seasons);
            F.resize(years * seasons * ages.size());
            emigrants.resize(years * seasons * ages.size());
            imigrants.resize(years * seasons * ages.size());
            growth.resize(years * seasons * ages.size());
            Z.resize(years * seasons * ages.size());
            S.resize(years * seasons * ages.size());
            N.resize(years * seasons * ages.size());
            predicted_N.resize(years * seasons * ages.size());

        }

        /**
         * Evaluates spawn and recruitment for all ages in a year and season.
         * 
         * @param year
         * @param season
         */
        inline void Recruitment(int year, int season) {
            //#warning add compiler hint here
            if (years == 0 && season == 1) {
                for (int a = 0; a< this->ages.size(); a++) {
                    recruitment[0] = this->N[a];
                }
            } else {
                recruitment_model_iterator rit = this->recruitment_model.find(season);
                if (rit != this->recruitment_model.end()) {
//                    std::cout << this->growth_model->Evaluate(this->ages[0]) << " ";


                }
            }
        }

        /**
         * Evaluates growth for all ages in a year and season.
         * 
         * @param year
         * @param season
         */
        inline void Growth(int year, int season) {
            growth[year * this->seasons * this->ages.size() + (season - 1) * this->ages.size()] = variable(.01);
            for (int a = 1; a< this->ages.size(); a++) {
                growth[year * this->seasons * this->ages.size() + (season - 1) * this->ages.size() + a] =
                        this->area->growth_model->Evaluate(this->ages[a]);
            }
        }

        /**
         * Evaluates mortality for all ages in a year and season.
         * @param year
         * @param season
         */
        inline void Mortality(int year, int season) {
            std::vector< std::shared_ptr<Fleet<REAL_T> > >& fleets = this->area->seasonal_fleet_operations[season];
            for (int a = 0; a< this->ages.size(); a++) {


                variable f_a = static_cast<REAL_T> (0.0);
                for (int f = 0; f < fleets.size(); f++) {
                    f_a += fleets[f]->area_season_fishing_mortality[this->area->id][season]->Evaluate(year, (season - 1)) *
                            fleets[f]->season_area_selectivity[season][this->area->id]->Evaluate(ages[a]);
                }

                Z[year * this->seasons * this->ages.size() + (season - 1) * this->ages.size() + a]
                        = this->natural_mortality_model->Evaluate(a) + f_a;
            }
        }

        /**
         * Evaluates fecundity for all ages in a year and season.
         * 
         * @param year
         * @param season
         */
        inline void Fecundity(int year, int season) {
            if (natal_homing) {
                //use natal area parameters
            } else {
                // use  area parameters
            }
        }

        /**
         * Evaluates numbers at age for all ages in a year and season.
         * 
         * @param year
         * @param season
         */
        inline void NumbersAtAge(int year, int season) {


            if (year == 0 && season == 1) {
                for (int a = 0; a < ages.size(); a++) {
                    this->N[a] = this->initial_numbers[a];
                }
            } else {
                int y = year;
                int s = season;
                if (season == 1) {
                    s = this->seasons;
                    y--;
                }

                //                this->N[year * this->seasons * this->ages.size() + (season - 1) * this->ages.size()]= recruit;
                for (int a = 1; a < ages.size(); a++) {
                    //                    this->N[year * this->seasons * this->ages.size() + (season - 1) * this->ages.size() + a] =
                    //                            year * this->seasons * this->ages.size() + (season - 1) * this->ages.size() + a
                }

                //move fish
                //calc new numbers
            }



        }

        /**
         * Evaluates catch at age for all ages and season
         * @param year
         * @param season
         */
        inline void CatchAtAge(int year, int season) {
            if (natal_homing) {
                //use natal area parameters

            } else {
                // use  area parameters
            }
        }

        /**
         * Evaluates survey numbers at age for all ages and season
         * @param year
         * @param season
         */
        inline void SurveyNumbersAtAge(int year, int season) {
            if (natal_homing) {
                //use natal area parameters

            } else {
                // use  area parameters
            }
        }

        /**
         * Evaluates catch biomass at age for all ages and season
         * @param year
         * @param season
         */
        inline void CatchBiomassAtAge(int year, int season) {
            if (natal_homing) {
                //use natal area parameters

            } else {
                // use  area parameters
            }
        }

        /**
         * Evaluates catch biomass at age for all ages and season
         * @param year
         * @param season
         */
        inline void SurveyBiomassAtAge(int year, int season) {
            if (natal_homing) {
                //use natal area parameters

            } else {
                // use  area parameters
            }
        }

        inline void Reset() {

        }

    };

    template<typename REAL_T>
    std::ostream& operator<<(std::ostream& out, mas::PopulationInfo<REAL_T>& pi) {
        out << std::fixed;
        out << std::setprecision(2);
        out << "Population " << pi.natal_population->id << "\n";
        out << "Area " << pi.area->id << "\n";
        out << "Total Mortality at Age (Z)\n";
        if (pi.male_chohorts) {
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
        if (pi.male_chohorts) {
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
        if (pi.male_chohorts) {
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
        if (pi.male_chohorts) {
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
        int ages;
        int growth_id;
        
        std::shared_ptr<Area<REAL_T> > natal_area; //birth area
        std::vector<std::shared_ptr<Area<REAL_T> > > areas_list; //all areas

        //Movement Tracking
        typedef typename std::unordered_map<int, PopulationInfo<REAL_T> >::iterator cohort_iterator;
        std::unordered_map<int, PopulationInfo<REAL_T> > male_cohorts;
        std::unordered_map<int, PopulationInfo<REAL_T> > female_cohorts;
        std::unordered_map<int, int > movement_models_ids; //season keyed
        typedef typename std::unordered_map<int, int >::iterator movement_model_id_iterator;

        std::shared_ptr<mas::Movement<REAL_T> > movement_model;

        std::unordered_map<int, std::shared_ptr<mas::Movement<REAL_T> > > movement_models; //year keyed
        typedef typename std::unordered_map<int, std::shared_ptr<mas::Movement<REAL_T> > >::iterator movement_model_iterator;
        //Estimable
        std::vector<std::vector<variable> > movement_coefficients;
        std::vector<variable> initial_population_males;
        std::vector<variable> initial_population_females;



        //    typedef typename std::unordered_map<std::vector<std::vector<variable> > >::iterator movement_coefficient_iterator;

        Population() {
        }

        Population(int years,
                int seasons,
                int areas,
                const std::shared_ptr<Area<REAL_T> >& natal_area,
                const std::vector<std::shared_ptr<Area<REAL_T> > >& areas_list) :
        years(years),
        seasons(seasons),
        areas(areas),
        natal_area(natal_area),
        areas_list(areas) {

            for (int a = 0; a < areas_list.size(); a++) {

                male_cohorts[areas_list[a]->id].natal_homing = this->natal_homing;
                male_cohorts[areas_list[a]->id].area = areas_list[a];
                male_cohorts[areas_list[a]->id].natal_area = this->natal_area;

                female_cohorts[areas_list[a]->id].natal_homing = this->natal_homing;
                female_cohorts[areas_list[a]->id].area = areas_list[a];
                female_cohorts[areas_list[a]->id].natal_area = this->natal_area;
                female_cohorts[areas_list[a]->id].male_chohorts = false;

            }



        }

        void Prepare() {


            for (int a = 0; a < areas_list.size(); a++) {

                male_cohorts[a].Reset();
                female_cohorts[a].Reset();
            }

            for (int i = 0; i < this->initial_numbers.size(); i++) {

                switch (this->initial_numbers[i].type) {
                    case MALE:
                        std::cout << "Setting initial numbers for males in area " << this->initial_numbers[i].area_id << "\n";
                        this->male_cohorts[this->initial_numbers[i].area_id].initial_numbers = this->initial_numbers[i].values;
                        break;

                    case FEMALE:
                        std::cout << "Setting initial numbers for females in area " << this->initial_numbers[i].area_id << "\n";
                        this->female_cohorts[this->initial_numbers[i].area_id].initial_numbers = this->initial_numbers[i].values;

                        break;

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
                    PopulationInfo<REAL_T>& male_info_from = this->male_cohorts[(i + 1)];
                    PopulationInfo<REAL_T>& female_info_from = this->female_cohorts[(i + 1)];
                    for (int j = 0; j < male_probabilities.size(); j++) {
                        if (i != j) {
                            PopulationInfo<REAL_T>& male_info_to = this->male_cohorts[(j + 1)];
                            PopulationInfo<REAL_T>& female_info_to = this->female_cohorts[(j + 1)];

                            for (int a = 0; a < this->ages; a++) {
                                male_info_from.emigrants[year * this->seasons * this->ages + (season - 1) * this->ages + a] +=
                                        male_probabilities[i][j] * male_info_from.N[year * this->seasons * this->ages + (season - 1) * this->ages + a];

                                male_info_to.imigrants[year * this->seasons * this->ages + (season - 1) * this->ages + a] +=
                                        male_probabilities[i][j] * male_info_from.N[year * this->seasons * this->ages + (season - 1) * this->ages + a];

                                female_info_from.emigrants[year * this->seasons * this->ages + (season - 1) * this->ages + a] +=
                                        female_probabilities[i][j] * female_info_from.N[year * this->seasons * this->ages + (season - 1) * this->ages + a];

                                female_info_to.imigrants[year * this->seasons * this->ages + (season - 1) * this->ages + a] +=
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
            for (int a = 0; a < areas_list.size(); a++) {
                std::cout << male_cohorts[areas_list[a]->id];
                std::cout << female_cohorts[areas_list[a]->id];
            }
        }

        void Evaluate() {

            InitializePopulationinAreas();


            if (this->move_fish_before_lh) {
                for (int y = 0; y < this->years; y++) {
                    for (int s = 1; s <= this->seasons; s++) {


                        for (int a = 0; a < areas_list.size(); a++) {
                            male_cohorts[areas_list[a]->id].Mortality(y, s);
                            female_cohorts[areas_list[a]->id].Mortality(y, s);



                            male_cohorts[areas_list[a]->id].NumbersAtAge(y, s);
                            female_cohorts[areas_list[a]->id].NumbersAtAge(y, s);

                            this->MoveFish(y, s);

                        }




                        //                        for (int a = 0; a < areas_list.size(); a++) {
                        //                            male_cohorts[areas_list[a]->id].NumbersAtAge(y, s);
                        //                            female_cohorts[areas_list[a]->id].NumbersAtAge(y, s);
                        //                        }

                        for (int a = 0; a < areas_list.size(); a++) {
                            male_cohorts[areas_list[a]->id].CatchAtAge(y, s);
                            female_cohorts[areas_list[a]->id].CatchAtAge(y, s);
                        }

                    }
                }
            } else {
                for (int y = 0; y < this->years; y++) {
                    for (int s = 1; s <= this->seasons; s++) {

                        for (int a = 0; a < areas_list.size(); a++) {
                            male_cohorts[areas_list[a]->id].Mortality(y, s);
                            female_cohorts[areas_list[a]->id].Mortality(y, s);

                            male_cohorts[areas_list[a]->id].Growth(y, s);
                            female_cohorts[areas_list[a]->id].Growth(y, s);


                            male_cohorts[areas_list[a]->id].NumbersAtAge(y, s);
                            female_cohorts[areas_list[a]->id].NumbersAtAge(y, s);

                            this->MoveFish(y, s);

                            //                            male_cohorts[areas_list[a]->id].Fecundity(y, s);
                            //                            female_cohorts[areas_list[a]->id].Fecundity(y, s);
                            //
                            //                            male_cohorts[areas_list[a]->id].Recruitment(y, s);
                            //                            female_cohorts[areas_list[a]->id].Recruitment(y, s);
                            //
                            //                            male_cohorts[areas_list[a]->id].Growth(y, s);
                            //                            female_cohorts[areas_list[a]->id].Growth(y, s);
                        }



                        //                        this->MoveFish();

                        //                        for (int a = 0; a < areas_list.size(); a++) {
                        //                            male_cohorts[areas_list[a]->id].NumbersAtAge(y, s);
                        //                            female_cohorts[areas_list[a]->id].NumbersAtAge(y, s);
                        //                        }

                    }
                }
            }


            for (int a = 0; a < areas_list.size(); a++) {
                //                male_cohorts[areas_list[a]->id].NumbersAtAge();
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
        for (int i = 0; i < pop.areas_list.size(); i++) {
            out << pop.areas_list[i]->id << " ";
        }
        out << "\n";

        return out;
    }

}

#endif /* POPULATION_HPP */

