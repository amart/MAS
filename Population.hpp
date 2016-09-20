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
#include <unordered_map>
#include "Area.hpp"

#include "../AutoDiff_Standalone/AutoDiff/AutoDiff.hpp"

namespace mas {



    template<class REAL_T>
    class Population;

    /**
     *Runtime calculated information for a population in a specific area.
     */
    template<class REAL_T>
    struct PopulationInfo {
        typedef atl::Variable<REAL_T> variable;
        Population<REAL_T>* natal_population;

        bool natal_homing = false;
        std::shared_ptr<Area<REAL_T> > area;
        std::shared_ptr<Area<REAL_T> > natal_area;

        int years;
        int seasons;

        std::vector<variable> F;
        std::vector<variable> Z;
        std::vector<variable> S;
        std::vector<variable> N;
        std::vector<variable> C;
        std::vector<variable> predicted_N;

        /**
         * Evaluates spawn and recruitment for all ages in a year and season.
         * 
         * @param year
         * @param season
         */
        inline void Recruitment(int year, int season) {

            if (natal_homing) {
                if (natal_area->id == area->id) {
                    //do spawn and recruitment for natal area
                }
            } else {
                //do spawn and recruitment for current area
            }
        }

        /**
         * Evaluates growth for all ages in a year and season.
         * 
         * @param year
         * @param season
         */
        inline void Growth(int year, int season) {
            if (natal_homing) {
                //use natal area parameters
            } else {
                // use  area parameters
            }
        }

        /**
         * Evaluates mortality for all ages in a year and season.
         * @param year
         * @param season
         */
        inline void Mortality(int year, int season) {
            if (natal_homing) {
                //use natal area parameters
            } else {
                // use  area parameters
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
        inline void NumbersAtAge() {

        }

        inline void Reset() {

        }

    };

    template<class REAL_T>
    class Population {
    public:
        typedef atl::Variable<REAL_T> variable;
        int id;
        std::string name;
        int natal_area_id;
        int movement_model_id;
        bool natal_homing = false;
        bool natal_recruitment = false;
        bool move_fish_before_lh = false;
        int years;
        int seasons;
        int areas;
        std::shared_ptr<Area<REAL_T> > natal_area; //birth area
        std::vector<std::shared_ptr<Area<REAL_T> > > areas_list; //all areas

        //Movement Tracking
        typedef typename std::unordered_map<int, PopulationInfo<REAL_T> >::iterator cohort_iterator;
        std::unordered_map<int, PopulationInfo<REAL_T> > male_cohorts;
        std::unordered_map<int, PopulationInfo<REAL_T> > female_cohorts;

        //Estimable
        std::vector<std::vector<variable> > movement_coefficients;
        std::vector<variable> initial_population_males;
        std::vector<variable> initial_population_females;

        std::vector<variable*> estimated_parameters;
        std::vector<int> estimated_phase;


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

            }



        }

        void Register(variable& var, int phase = 1) {
            this->estimated_parameters.push_back(&var);
            this->estimated_phase.push_back(phase);
        }

        void Prepare() {


            for (int a = 0; a < areas_list.size(); a++) {

                male_cohorts[a].Reset();
                female_cohorts[a].Reset();
            }

        }

        inline void MoveFish() {


        }

        void InitializePopulationinAreas() {

        }

        void Evaluate() {

            InitializePopulationinAreas();

            if (this->move_fish_before_lh) {
                for (int y = 0; y < this->years; y++) {
                    for (int s = 0; s < this->seasons; s++) {

                        this->MoveFish();

                        for (int a = 0; a < areas_list.size(); a++) {
                            male_cohorts[areas_list[a]->id].Mortality(y, s);
                            female_cohorts[areas_list[a]->id].Mortality(y, s);

                            male_cohorts[areas_list[a]->id].Fecundity(y, s);
                            female_cohorts[areas_list[a]->id].Fecundity(y, s);

                            male_cohorts[areas_list[a]->id].Recruitment(y, s);
                            female_cohorts[areas_list[a]->id].Recruitment(y, s);

                            male_cohorts[areas_list[a]->id].Growth(y, s);
                            female_cohorts[areas_list[a]->id].Growth(y, s);
                        }


                        for (int a = 0; a < areas_list.size(); a++) {
                            male_cohorts[areas_list[a]->id].NumbersAtAge(y, s);
                            female_cohorts[areas_list[a]->id].NumbersAtAge(y, s);
                        }

                    }
                }
            } else {
                for (int y = 0; y < this->years; y++) {
                    for (int s = 0; s < this->seasons; s++) {

                        for (int a = 0; a < areas_list.size(); a++) {
                            male_cohorts[areas_list[a]->id].Mortality(y, s);
                            female_cohorts[areas_list[a]->id].Mortality(y, s);

                            male_cohorts[areas_list[a]->id].Fecundity(y, s);
                            female_cohorts[areas_list[a]->id].Fecundity(y, s);

                            male_cohorts[areas_list[a]->id].Recruitment(y, s);
                            female_cohorts[areas_list[a]->id].Recruitment(y, s);

                            male_cohorts[areas_list[a]->id].Growth(y, s);
                            female_cohorts[areas_list[a]->id].Growth(y, s);
                        }



                        this->MoveFish();

                        for (int a = 0; a < areas_list.size(); a++) {
                            male_cohorts[areas_list[a]->id].NumbersAtAge(y, s);
                            female_cohorts[areas_list[a]->id].NumbersAtAge(y, s);
                        }

                    }
                }
            }


            for (int a = 0; a < areas_list.size(); a++) {
                male_cohorts[areas_list[a]->id].NumbersAtAge();
            }
        }

    };

}

#endif /* POPULATION_HPP */

