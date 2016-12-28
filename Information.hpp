/* 
 * File:   Information.hpp
 * 
 * Author: Matthew Supernaw
 * National Oceanic and Atmospheric Administration
 * National Marine Fisheries Service 
 * Sustainable Fisheries Division
 * St. Petersburg, FL, 33701
 * 
 * Created on September 16, 2016, 12:35 PM
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

#ifndef MAS_INFORMATION_HPP
#define MAS_INFORMATION_HPP
#include "Common.hpp"
#include "Area.hpp"
#include "Population.hpp"
#include "Season.hpp"
#include "Movement.hpp"
#include "Selectivity.hpp"
#include "third_party/rapidjson/document.h"
#include "Fleet.hpp"
#include <ctime>
#include "NetCDF.hpp"



namespace mas {

    template<typename REAL_T>
    class Information : public mas::ModelObject<REAL_T> {
        typedef typename VariableTrait<REAL_T>::variable variable;


        std::map<int, std::map<std::string, DataObject<REAL_T> > > data_dictionary;


        std::string analyst = "NA";
        std::string study_name = "NA";

        std::string data_path;

        int nyears;
        int nseasons = 1;
        int first_year;
        int last_year;
        std::vector<REAL_T> ages;
        bool natal_movement = false;
        bool valid_configuration = true;

        std::unordered_map<int, std::shared_ptr<mas::Area<REAL_T> > > areas;
        std::unordered_map<int, std::shared_ptr<mas::Season<REAL_T> > > seasons;
        typedef typename std::unordered_map<int, std::shared_ptr<mas::Season<REAL_T> > >::iterator seasons_iterator;
        typedef typename std::unordered_map<int, std::shared_ptr<mas::Area<REAL_T> > >::iterator area_iterator;

        //System sub model objects
        std::unordered_map<int, std::shared_ptr<mas::Population<REAL_T> > > populations;
        std::unordered_map<int, std::shared_ptr<mas::GrowthBase<REAL_T> > > growth_models;
        std::unordered_map<int, std::shared_ptr<mas::RecruitmentBase<REAL_T> > > recruitment_models;
        std::unordered_map<int, std::shared_ptr<mas::NaturalMortality<REAL_T> > > natural_mortality_models;
        std::unordered_map<int, std::shared_ptr<mas::FishingMortality<REAL_T> > > finshing_mortality_models;
        std::unordered_map<int, std::shared_ptr<mas::FecundityBase<REAL_T> > > fecundity_models;
        std::unordered_map<int, std::shared_ptr<mas::Movement<REAL_T> > > movement_models;
        std::unordered_map<int, std::shared_ptr<mas::SelectivityBase<REAL_T> > > selectivity_models;
        std::unordered_map<int, std::shared_ptr<mas::Fleet<REAL_T> > > fleets;

        //System sub model iterators
        typedef typename std::unordered_map<int, std::shared_ptr<mas::Population<REAL_T> > >::iterator population_iterator;
        typedef typename std::unordered_map<int, std::shared_ptr<mas::GrowthBase<REAL_T> > >::iterator growth_model_iterator;
        typedef typename std::unordered_map<int, std::shared_ptr<mas::RecruitmentBase<REAL_T> > >::iterator recruitment_model_iterator;
        typedef typename std::unordered_map<int, std::shared_ptr<mas::NaturalMortality<REAL_T> > >::iterator natural_mortality_model_iterator;
        typedef typename std::unordered_map<int, std::shared_ptr<mas::FishingMortality<REAL_T> > >::iterator fishing_mortality_model_iterator;
        typedef typename std::unordered_map<int, std::shared_ptr<mas::FecundityBase<REAL_T> > >::iterator fecundity_model_iterator;
        typedef typename std::unordered_map<int, std::shared_ptr<mas::Movement<REAL_T> > >::iterator movement_model_iterator;
        typedef typename std::unordered_map<int, std::shared_ptr<mas::SelectivityBase<REAL_T> > >::iterator selectivity_model_iterator;
        typedef typename std::unordered_map<int, std::shared_ptr<mas::Fleet<REAL_T> > >::iterator fleet_iterator;

    public:

        Information() {
        }

        Information(const Information<REAL_T>& orig) {
        }

        virtual ~Information() {
        }

        void ParseConfig(const std::string& path) {
            std::time_t result = std::time(nullptr);
            mas_log << "MAS Run: " << std::asctime(std::localtime(&result)) << "\n";

            std::stringstream ss;
            std::ifstream config;
            config.open(path.c_str());
            if (!config.good()) {
                std::cerr << "MAS Configuration file \"" << path << "\" not found.\n";
                mas::mas_log << "MAS Configuration file \"" << path << "\" not found.\n";
                this->valid_configuration = false;
            }

            while (config.good()) {
                std::string line;
                std::getline(config, line);
                ss << line << "\n";
            }

            rapidjson::Document document;
            document.Parse(ss.str().c_str());
            rapidjson::Document::MemberIterator mit;

            for (mit = document.MemberBegin(); mit != document.MemberEnd(); ++mit) {
                if (std::string((*mit).name.GetString()) == "analyst") {
                    this->analyst = std::string((*mit).value.GetString());
                }

                if (std::string((*mit).name.GetString()) == "study_name") {
                    this->study_name = std::string((*mit).value.GetString());
                }

                if (std::string((*mit).name.GetString()) == "movement_type") {
                    if (std::string((*mit).value.GetString()) == std::string("natal_homing")) {
                        this->natal_movement = true;
                    }
                }

                if (std::string((*mit).name.GetString()) == "data") {
                    this->data_path = std::string((*mit).value.GetString());
                }

                if (std::string((*mit).name.GetString()) == "years") {
                    this->nyears = (*mit).value.GetInt();
                }

                if (std::string((*mit).name.GetString()) == "first_year") {
                    this->first_year = (*mit).value.GetInt();
                }

                if (std::string((*mit).name.GetString()) == "last_year") {
                    this->last_year = (*mit).value.GetInt();
                }

                if (std::string((*mit).name.GetString()) == "ages") {

                    rapidjson::Value& ages = (*mit).value;
                    if (ages.IsArray()) {
                        for (int i = 0; i < ages.Size(); i++) {
                            this->ages.push_back(static_cast<REAL_T> (ages[i].GetDouble()));
                        }

                    } else {
                        std::cout << "Configuration Error: \"ages\" should be a vector.\n";
                        mas::mas_log << "Configuration Error: \"ages\" should be a vector.\n";
                    }

                }


                if (std::string((*mit).name.GetString()) == "seasons") {
                    //                    HandleSeason(mit);
                    this->nseasons = (*mit).value.GetInt();
                }

                if (std::string((*mit).name.GetString()) == "recruitment") {
                    HandleRecruitmentModel(mit);
                }
                if (std::string((*mit).name.GetString()) == "growth") {
                    HandleGrowthModel(mit);
                }

                if (std::string((*mit).name.GetString()) == "selectivity") {
                    HandleSelectivityModel(mit);
                }

                if (std::string((*mit).name.GetString()) == "area") {
                    HandleAreaModel(mit);
                }

                if (std::string((*mit).name.GetString()) == "population") {
                    HandlePopulationModel(mit);
                }

                if (std::string((*mit).name.GetString()) == "fleet") {
                    HandleFleetModel(mit);
                }

                if (std::string((*mit).name.GetString()) == "movement") {
                    HandleMovementModel(mit);
                }

                if (std::string((*mit).name.GetString()) == "natural_mortality") {
                    HandleNaturalMortalityModel(mit);
                }
                if (std::string((*mit).name.GetString()) == "fishing_mortality") {
                    HandleFishingMortalityModel(mit);
                }
            }


        }

        void HandleFishingMortalityModel(rapidjson::Document::MemberIterator& mortality_model) {
            std::shared_ptr<mas::FishingMortality<REAL_T> > model(new mas::FishingMortality<REAL_T>());
            bool estimated = false;
            int phase = 1;
            bool bounded = false;
            REAL_T min = std::numeric_limits<REAL_T>::min();
            REAL_T max = std::numeric_limits<REAL_T>::max();

            rapidjson::Document::MemberIterator rit = (*mortality_model).value.FindMember("id");
            int model_id = 0;
            if (rit == (*mortality_model).value.MemberEnd()) {
                std::cout << "Configuration Error: Mortality is required to have a unique identifier\n";
                mas::mas_log << "Configuration Error: Mortality is required to have a unique identifier\n";
                this->valid_configuration = false;

            } else {
                model_id = (*rit).value.GetInt();
            }

            model->id = model_id;

            rapidjson::Document::MemberIterator pit = (*mortality_model).value.FindMember("parameters");

            if (pit == (*mortality_model).value.MemberEnd()) {
                std::cout << "Configuration Error: Fishing Mortality model has no parameter definitions.\n";
                mas::mas_log << "Configuration Error: Fishing Mortality model has no parameter definitions.\n";
                this->valid_configuration = false;

            } else {

                rapidjson::Document::MemberIterator vit = (*pit).value.FindMember("estimated");
                if (vit != (*pit).value.MemberEnd()) {
                    std::string estring = std::string((*vit).value.GetString());

                    if (estring == "true") {
                        estimated = true;
                    }
                }

                vit = (*pit).value.FindMember("min");
                if (vit != (*pit).value.MemberEnd()) {
                    min = static_cast<REAL_T> ((*vit).value.GetDouble());
                    bounded = true;
                }

                vit = (*pit).value.FindMember("max");
                if (vit != (*pit).value.MemberEnd()) {
                    max = static_cast<REAL_T> ((*vit).value.GetDouble());
                    bounded = true;
                }

                vit = (*pit).value.FindMember("phase");
                if (vit != (*pit).value.MemberEnd()) {
                    std::string pstring = std::string((*vit).value.GetString());
                    phase = StringToNumber<uint32_t>(pstring);
                }

                vit = (*pit).value.FindMember("values");

                if (vit == (*pit).value.MemberEnd()) {
                    std::cout << "Configuration Error: Fishing Mortality model supplied no values\n";
                    mas::mas_log << "Configuration Error: Fishing Mortality model supplied no values\n";
                    this->valid_configuration = false;
                } else {

                    if ((*vit).value.IsArray()) {
                        rapidjson::Value & v = (*vit).value;

                        if (v.IsArray()) {

                            model->fishing_mortality.resize(v.Size());

                            for (int i = 0; i < v.Size(); i++) {
                                rapidjson::Value & vv = v[i];
                                model->fishing_mortality[i].resize(vv.Size());
                                for (int j = 0; j < vv.Size(); j++) {
                                    mas::VariableTrait<REAL_T>::SetValue(model->fishing_mortality[i][j], static_cast<REAL_T> (vv[j].GetDouble()));
                                    if (estimated) {
                                        if (bounded) {
                                            mas::VariableTrait<REAL_T>::SetMinBoundary(model->fishing_mortality[i][j], min);
                                            mas::VariableTrait<REAL_T>::SetMaxBoundary(model->fishing_mortality[i][j], max);
                                        }
                                        std::stringstream ss;
                                        ss << "fishing_mortality[" << i << "][" << j << "]_" << model_id;
                                        mas::VariableTrait<REAL_T>::SetName(model->fishing_mortality[i][j], ss.str());
                                        model->Register(model->fishing_mortality[i][j], phase);
                                    }
                                }
                            }


                        } else {
                            std::cout << "Configuration Error: Fishing Mortality is requires values in a matrix[year,season] not a vector\n";
                            std::cout << "Configuration Error: Fishing Mortality is requires values in a matrix[year,season] not a vector\n";
                            this->valid_configuration = false;
                        }


                    } else {
                        std::cout << "Configuration Error: Fishing Mortality is requires values in a matrix[year,season]\n";
                        std::cout << "Configuration Error: Fishing Mortality is requires values in a matrix[year,season]\n";
                        this->valid_configuration = false;
                    }


                }

            }

            fishing_mortality_model_iterator fit = this->finshing_mortality_models.find(model->id);


            if (fit == this->finshing_mortality_models.end()) {
                this->finshing_mortality_models[model->id] = model;


            } else {
                std::cout << "Configuration Error: More than one fishing mortality model with the same identifier defined. Fishing mortality models require a unique id.\n";
                mas_log << "Configuration Error: More than one fishing mortality model with the same identifier defined. Fishing mortality models require a unique id.\n";
                this->valid_configuration = false;

            }


        }

        void HandleNaturalMortalityModel(rapidjson::Document::MemberIterator& mortality_model) {
            std::shared_ptr<mas::NaturalMortality<REAL_T> > model(new mas::NaturalMortality<REAL_T>());
            bool estimated = false;
            int phase = 1;

            rapidjson::Document::MemberIterator rit = (*mortality_model).value.FindMember("id");
            int model_id = 0;
            if (rit == (*mortality_model).value.MemberEnd()) {
                std::cout << "Configuration Error: Mortality is required to have a unique identifier\n";
                mas::mas_log << "Configuration Error: Mortality is required to have a unique identifier\n";
                this->valid_configuration = false;

            } else {
                model_id = (*rit).value.GetInt();
            }

            model->id = model_id;

            rapidjson::Document::MemberIterator pit = (*mortality_model).value.FindMember("parameters");
            rapidjson::Document::MemberIterator mvit = (*pit).value.FindMember("values");
            if ((*mvit).value.IsArray()) {
                //                        model->male_mortality.resize((*mvit).value.Size());
                rapidjson::Value& v = (*mvit).value;
                model->mortality_vector.resize((*mvit).value.Size());
                for (int i = 0; i < (*mvit).value.Size(); i++) {
                    std::stringstream ss;
                    ss << "male_mortality[" << i << "]_" << model_id;
                    mas::VariableTrait<REAL_T>::SetName(model->mortality_vector[i], ss.str());
                    mas::VariableTrait<REAL_T>::SetValue(model->mortality_vector[i], static_cast<REAL_T> (v[i].GetDouble()));
                }

            } else {
                std::cout << "Configuration Error: Mortality model values for males must be a vector.\n";
                mas::mas_log << "Configuration Error: Mortality model values for males must be a vector.\n";
                this->valid_configuration = false;
                return;
            }

            mvit = (*pit).value.FindMember("min");
            if (mvit != (*pit).value.MemberEnd()) {
                if ((*mvit).value.IsArray()) {

                    rapidjson::Value& v = (*mvit).value;
                    for (int i = 0; i < (*mvit).value.Size(); i++) {

                        mas::VariableTrait<REAL_T>::SetMinBoundary(model->mortality_vector[i], static_cast<REAL_T> (v[i].GetDouble()));
                    }

                } else {
                    std::cout << "Configuration Error: Mortality model min values for males must be a vector.\n";
                    mas::mas_log << "Configuration Error: Mortality model min values for males must be a vector.\n";
                    this->valid_configuration = false;

                }
            }

            mvit = (*pit).value.FindMember("max");
            if (mvit != (*pit).value.MemberEnd()) {
                if ((*mvit).value.IsArray()) {

                    rapidjson::Value& v = (*mvit).value;
                    for (int i = 0; i < (*mvit).value.Size(); i++) {

                        mas::VariableTrait<REAL_T>::SetMaxBoundary(model->mortality_vector[i], static_cast<REAL_T> (v[i].GetDouble()));
                    }

                } else {
                    std::cout << "Configuration Error: Mortality model min values for males must be a vector.\n";
                    mas::mas_log << "Configuration Error: Mortality model min values for males must be a vector.\n";
                    this->valid_configuration = false;

                }
            }


            rit = (*pit).value.FindMember("estimated");
            if (rit != (*pit).value.MemberEnd()) {
                std::string e = std::string((*rit).value.GetString());
                if (e == "true") {
                    estimated = true;
                }
            }

            if (estimated) {
                phase = 1;
                rit = (*pit).value.FindMember("phase");
                if (rit != (*pit).value.MemberEnd()) {
                    phase = (*rit).value.GetInt();
                }

                //register male mortality parameters
                for (int i = 0; i < model->mortality_vector.size(); i++) {
                    model->Register(model->mortality_vector[i], phase);
                }

            }

            natural_mortality_model_iterator it = this->natural_mortality_models.find(model->id);
            if (it != this->natural_mortality_models.end()) {
                std::cout << "Configuration Error: More than one mortality model with the same identifier defined. Mortality models require a unique id.\n";
                mas::mas_log << "Configuration Error: More than one mortality model with the same identifier defined. Mortality models require a unique id.\n";

                this->valid_configuration = false;

            } else {
                this->natural_mortality_models[model->id] = model;
            }

        }

        void HandleMovementModel(rapidjson::Document::MemberIterator& movement_model) {

            std::shared_ptr<mas::Movement<REAL_T> > model(new mas::Movement<REAL_T>());
            bool estimated = false;
            int phase = 1;

            rapidjson::Document::MemberIterator rit = (*movement_model).value.FindMember("id");
            int model_id = 0;
            if (rit == (*movement_model).value.MemberEnd()) {
                std::cout << "Configuration Error: Movement is required to have a unique identifier\n";
                mas::mas_log << "Configuration Error: Movement is required to have a unique identifier\n";
                this->valid_configuration = false;
                ;
            } else {
                model_id = (*rit).value.GetInt();
            }
            model->id = model_id;

            //            rit = (*movement_model).value.FindMember("season");
            //            if (rit == (*movement_model).value.MemberEnd()) {
            //                std::cout << "Configuration Error: Movement is required to have a season defined\n";
            //                mas::mas_log << "Configuration Error: Movement is required to have a season defined\n";
            //                this->valid_configuration = false;
            //                ;
            //            } else {
            //                model->season = (*rit).value.GetInt();
            //            }


            rit = (*movement_model).value.FindMember("recruits");
            if ((*rit).value.IsArray()) {
                model->recruit_connectivity.resize((*rit).value.Size());
                for (int i = 0; i < (*rit).value.Size(); i++) {//season
                    rapidjson::Value& m = (*rit).value[i];
                    if (m.IsArray()) {

                        model->recruit_connectivity[i].resize(m.Size());
                        for (int j = 0; j < m.Size(); j++) {//area
                            rapidjson::Value& n = m[j];
                            if (n.IsArray()) {
                                model->recruit_connectivity[i][j].resize(n.Size());

                                for (int k = 0; k < n.Size(); k++) {//area
                                    mas::VariableTrait<REAL_T>::SetValue(model->recruit_connectivity[i][j][k], static_cast<REAL_T> (n[k].GetDouble()));
                                }
                            }
                        }

                    } else {
                        std::cout << "Configuration Error: Movement connectivity rows must be a vector\n";
                        mas::mas_log << "Configuration Error: Movement connectivity rows must be a vector\n";
                        this->valid_configuration = false;

                    }
                }


            } else {
                std::cout << "Configuration Error: Movement connectivity must be a matrix\n";
                mas::mas_log << "Configuration Error: Movement connectivity must be a matrix\n";
                this->valid_configuration = false;

            }


            rit = (*movement_model).value.FindMember("male");
            if ((*rit).value.IsArray()) {
                model->male_connectivity.resize((*rit).value.Size());
                for (int i = 0; i < (*rit).value.Size(); i++) {//season
                    rapidjson::Value& m = (*rit).value[i];
                    if (m.IsArray()) {

                        model->male_connectivity[i].resize(m.Size());
                        for (int j = 0; j < m.Size(); j++) {//area
                            rapidjson::Value& n = m[j];
                            if (n.IsArray()) {
                                model->male_connectivity[i][j].resize(n.Size());

                                for (int k = 0; k < n.Size(); k++) {//area
                                    mas::VariableTrait<REAL_T>::SetValue(model->male_connectivity[i][j][k], static_cast<REAL_T> (n[k].GetDouble()));
                                }
                            }
                        }

                    } else {
                        std::cout << "Configuration Error: Movement connectivity rows must be a vector\n";
                        mas::mas_log << "Configuration Error: Movement connectivity rows must be a vector\n";
                        this->valid_configuration = false;

                    }
                }


            } else {
                std::cout << "Configuration Error: Movement connectivity must be a matrix\n";
                mas::mas_log << "Configuration Error: Movement connectivity must be a matrix\n";
                this->valid_configuration = false;

            }

            rit = (*movement_model).value.FindMember("female");
            if ((*rit).value.IsArray()) {
                model->female_connectivity.resize((*rit).value.Size());
                for (int i = 0; i < (*rit).value.Size(); i++) {
                    rapidjson::Value& m = (*rit).value[i];
                    if (m.IsArray()) {

                        model->female_connectivity[i].resize(m.Size());
                        for (int j = 0; j < m.Size(); j++) {
                            rapidjson::Value& n = m[j];
                            if (n.IsArray()) {
                                model->female_connectivity[i][j].resize(n.Size());

                                for (int k = 0; k < n.Size(); k++) {
                                    mas::VariableTrait<REAL_T>::SetValue(model->female_connectivity[i][j][k], static_cast<REAL_T> (n[k].GetDouble()));
                                }
                            }
                        }

                    } else {
                        std::cout << "Configuration Error: Movement connectivity rows must be a vector\n";
                        mas::mas_log << "Configuration Error: Movement connectivity rows must be a vector\n";
                        this->valid_configuration = false;

                    }
                }


            } else {
                std::cout << "Configuration Error: Movement connectivity must be a matrix\n";
                mas::mas_log << "Configuration Error: Movement connectivity must be a matrix\n";
                this->valid_configuration = false;
                ;
            }


            rit = (*movement_model).value.FindMember("estimated");
            if (rit != (*movement_model).value.MemberEnd()) {
                std::string e = std::string((*rit).value.GetString());
                if (e == "true") {
                    estimated = true;
                }
            }

            if (estimated) {
                phase = 1;
                rit = (*movement_model).value.FindMember("phase");
                if (rit != (*movement_model).value.MemberEnd()) {
                    phase = (*rit).value.GetInt();
                }
                //register movement coefficients
                for (int i = 0; i < model->male_connectivity.size(); i++) {
                    for (int j = 0; j < model->male_connectivity[i].size(); j++) {
                        for (int k = 0; k < model->male_connectivity[i][0].size(); k++) {
                            std::stringstream ss;
                            ss << "male_connectivity[" << i << "][" << j << "][" << k << "]_" << model->id;
                            //                            model->Register(model->male_connectivity[i][j][k], phase);
                            //                            mas::VariableTrait<REAL_T>::SetName(model->male_connectivity[i][j][k], ss.str());
                        }
                    }
                }

                //register movement coefficients
                for (int i = 0; i < model->female_connectivity.size(); i++) {
                    for (int j = 0; j < model->female_connectivity[i].size(); j++) {
                        for (int k = 0; k < model->female_connectivity[i][0].size(); k++) {
                            std::stringstream ss;
                            ss << "female_connectivity[" << i << "][" << j << "][" << k << "]_" << model->id;
                            //                            model->Register(model->male_connectivity[i][j][k], phase);
                            //                            mas::VariableTrait<REAL_T>::SetName(model->male_connectivity[i][j][k], ss.str());
                        }
                    }
                }
            }

            movement_model_iterator it = this->movement_models.find(model->id);
            if (it != this->movement_models.end()) {
                std::cout << "Configuration Error: More than one movement model with the same identifier defined. Movement models require a unique id.\n";
                mas::mas_log << "Configuration Error: More than one movement model with the same identifier defined. Movement models require a unique id.\n";
                this->valid_configuration = false;
                ;
            } else {
                this->movement_models[model->id] = model;
            }

        }

        void HandleFleetModel(rapidjson::Document::MemberIterator& fleet_model) {
            std::shared_ptr<mas::Fleet<REAL_T> > model(new mas::Fleet<REAL_T>());


            rapidjson::Document::MemberIterator rit = (*fleet_model).value.FindMember("id");
            int model_id = 0;
            if (rit == (*fleet_model).value.MemberEnd()) {
                std::cout << "Configuration Error: Fleets are required to have a unique identifier\n";
                mas::mas_log << "Configuration Error: Fleets are required to have a unique identifier\n";
                this->valid_configuration = false;
                ;
            } else {
                model_id = (*rit).value.GetInt();
            }
            model->id = model_id;

            rit = (*fleet_model).value.FindMember("name");
            if (rit != (*fleet_model).value.MemberEnd()) {
                model->name = std::string((*rit).value.GetString());
            } else {
                model->name = "NA";
            }


            rit = (*fleet_model).value.FindMember("selectivity");

            if (rit != (*fleet_model).value.MemberEnd()) {
                if (!(*rit).value.IsArray()) {
                    std::cout << "Configuration Error: Fleets are required to have selectivity definitions in a vector\n";
                    std::cout << "Configuration Error: Fleets are required to have selectivity definitions in a vector\n";
                    this->valid_configuration = false;

                } else {
                    for (int i = 0; i < (*rit).value.Size(); i++) {
                        rapidjson::Value& m = (*rit).value[i];

                        int sid = 0;
                        int sarea = 0;
                        int sseason = 0;
                        rapidjson::Document::MemberIterator mit = m.FindMember("id");
                        if (mit == m.MemberEnd()) {
                            std::cout << "Configuration Error: Fleets selectivity has no model identifier\n";
                            std::cout << "Configuration Error: Fleets selectivity has no model identifier\n";
                            this->valid_configuration = false;

                        } else {
                            sid = (*mit).value.GetInt();
                        }

                        mit = m.FindMember("season");
                        if (mit == m.MemberEnd()) {
                            std::cout << "Configuration Error: Fleets selectivity season has no identifier\n";
                            std::cout << "Configuration Error: Fleets selectivity season has no identifier\n";
                            this->valid_configuration = false;

                        } else {
                            sseason = (*mit).value.GetInt();
                        }

                        mit = m.FindMember("area");
                        if (mit == m.MemberEnd()) {
                            std::cout << "Configuration Error: Fleets selectivity area has no identifier\n";
                            std::cout << "Configuration Error: Fleets selectivity area has no identifier\n";
                            this->valid_configuration = false;

                        } else {
                            sarea = (*mit).value.GetInt();
                        }
                        model->season_area_selectivity_ids[sseason][sarea] = sid;
                        model->area_season_selectivity_ids[sarea][sseason] = sid;
                    }
                }
            }


            rit = (*fleet_model).value.FindMember("fishing_mortality");
            if (rit != (*fleet_model).value.MemberEnd()) {
                if (!(*rit).value.IsArray()) {
                    std::cout << "Configuration Error: Fleets are required to have fishing mortality definitions in a vector\n";
                    std::cout << "Configuration Error: Fleets are required to have fishing mortality definitions in a vector\n";
                    this->valid_configuration = false;

                } else {
                    for (int i = 0; i < (*rit).value.Size(); i++) {
                        rapidjson::Value& m = (*rit).value[i];

                        int sid = 0;
                        int sarea = 0;
                        int sseason = 0;
                        rapidjson::Document::MemberIterator mit = m.FindMember("id");
                        if (mit == m.MemberEnd()) {
                            std::cout << "Configuration Error: Fleets fishing mortality has no model identifier\n";
                            std::cout << "Configuration Error: Fleets fishing mortality has no model identifier\n";
                            this->valid_configuration = false;

                        } else {
                            sid = (*mit).value.GetInt();
                        }

                        mit = m.FindMember("season");
                        if (mit == m.MemberEnd()) {
                            std::cout << "Configuration Error: Fleets fishing mortality season has no identifier\n";
                            std::cout << "Configuration Error: Fleets fishing mortality season has no identifier\n";
                            this->valid_configuration = false;

                        } else {
                            sseason = (*mit).value.GetInt();
                        }

                        mit = m.FindMember("area");
                        if (mit == m.MemberEnd()) {
                            std::cout << "Configuration Error: Fleets fishing mortality area has no identifier\n";
                            std::cout << "Configuration Error: Fleets fishing mortality area has no identifier\n";
                            this->valid_configuration = false;

                        } else {
                            sarea = (*mit).value.GetInt();
                        }
                        model->season_area_fishing_mortality_ids[sseason][sarea] = sid;
                        model->area_season_fishing_mortality_ids[sarea][sseason] = sid;
                    }
                }
            }


            fleet_iterator fit;
            fit = this->fleets.find(model->id);
            if (fit == this->fleets.end()) {
                this->fleets[model->id] = model;
            } else {
                std::cout << "Configuration Error: More than one fleet with the same identifier defined. Fleets require a unique id.\n";
                mas::mas_log << "Configuration Error: More than one fleet with the same identifier defined. Fleets require a unique id.\n";
            }

        }

        void HandlePopulationModel(rapidjson::Document::MemberIterator & population_model) {
            std::shared_ptr<mas::Population<REAL_T> > model(new mas::Population<REAL_T>());


            rapidjson::Document::MemberIterator rit = (*population_model).value.FindMember("id");
            int model_id = 0;
            if (rit == (*population_model).value.MemberEnd()) {
                std::cout << "Configuration Error: Populations are required to have a unique identifier\n";
                mas::mas_log << "Configuration Error: Populations are required to have a unique identifier\n";
                this->valid_configuration = false;
                ;
            } else {
                model_id = (*rit).value.GetInt();
            }
            model->id = model_id;

            rit = (*population_model).value.FindMember("name");
            if (rit != (*population_model).value.MemberEnd()) {
                model->name = std::string((*rit).value.GetString());
            } else {
                model->name = "NA";
            }

            rit = (*population_model).value.FindMember("natal_area");
            if (rit != (*population_model).value.MemberEnd()) {
                model->natal_area_id = (*rit).value.GetInt();
            } else {
                std::cout << "Configuration Error: Population is required to have a natal area identifier\n";
                mas::mas_log << "Configuration Error: Population is required to have a natal area identifier\n";
                this->valid_configuration = false;

            }



            rit = (*population_model).value.FindMember("maturity");
            if (rit != (*population_model).value.MemberEnd()) {
                //                model->movement_model_id = (*rit).value.GetInt();

                if ((*rit).value.IsArray()) {

                    for (int i = 0; i < (*rit).value.Size(); i++) {
                        rapidjson::Value& v = (*rit).value[i];
                        int sex = 0;
                        int area = -9999;
                        std::vector<REAL_T> values;
                        rapidjson::Document::MemberIterator mit = v.FindMember("sex");
                        if (std::string((*mit).value.GetString()) == std::string("female")) {
                            sex = 1;
                        }

                        mit = v.FindMember("area");

                        if (mit == v.MemberEnd()) {
                            std::cout << "Configuration Error: Population maturity models must define a area.\n";
                            mas::mas_log << "Configuration Error: Population maturity models must define a area.\n";
                            this->valid_configuration = false;
                        } else {
                            area = (*mit).value.GetInt();
                        }

                        mit = v.FindMember("values");
                        if (mit == v.MemberEnd()) {
                            std::cout << "Configuration Error: Population maturity models must define a value vector.\n";
                            mas::mas_log << "Configuration Error: Population maturity models must define a value vector.\n";
                            this->valid_configuration = false;
                        } else {

                            if ((*mit).value.IsArray()) {

                                for (int j = 0; j < (*mit).value.Size(); j++) {
                                    rapidjson::Value& vv = (*mit).value[j];
                                    values.push_back(static_cast<REAL_T> (vv.GetDouble()));
                                }


                            } else {
                                std::cout << "Configuration Error: Population maturity models must define a value vector.\n";
                                mas::mas_log << "Configuration Error: Population maturity models must define a value vector.\n";
                            }



                        }


                        model->maturity_models[area][sex] = values;


                    }


                } else {
                    std::cout << "Configuration Error: Population maturity models must be defined in a vector.\n";
                    mas::mas_log << "Configuration Error: Population maturity models must be defined in a vector.\n";
                    this->valid_configuration = false;
                }

            } else {
                std::cout << "Configuration Error: Population is required to have maturity model defined\n";
                mas::mas_log << "Configuration Error: Population is required to have maturity model defined\n";
                this->valid_configuration = false;

            }



            rit = (*population_model).value.FindMember("movement");
            if (rit != (*population_model).value.MemberEnd()) {
                //                model->movement_model_id = (*rit).value.GetInt();

                if ((*rit).value.IsArray()) {

                    for (int i = 0; i < (*rit).value.Size(); i++) {
                        rapidjson::Value& v = (*rit).value[i];
                        int s = 0;
                        int m = 0;

                        rapidjson::Document::MemberIterator mit = v.FindMember("year");
                        if ((mit) != v.MemberEnd()) {
                            s = (*mit).value.GetInt();
                        } else {
                            std::cout << "Configuration Error: Movement year not defined in population.\n";
                            mas::mas_log << "Configuration Error: Movement year not defined in population.\n";
                            this->valid_configuration = false;
                        }

                        mit = v.FindMember("id");
                        if ((mit) != v.MemberEnd()) {
                            m = (*mit).value.GetInt();
                        } else {
                            std::cout << "Configuration Error: Movement model not defined in population.\n";
                            mas::mas_log << "Configuration Error: Movement model not defined in population.\n";
                            this->valid_configuration = false;
                        }

                        model->movement_models_ids[s] = m;
                    }


                } else {
                    std::cout << "Configuration Error: Population movement models must be defined in a vector.\n";
                    mas::mas_log << "Configuration Error: Population movement models must be defined in a vector.\n";
                    this->valid_configuration = false;
                }

            } else {
                std::cout << "Configuration Error: Population is required to have movement model defined\n";
                mas::mas_log << "Configuration Error: Population is required to have movement model defined\n";
                this->valid_configuration = false;

            }

            rit = (*population_model).value.FindMember("recruitment_type");
            if (rit != (*population_model).value.MemberEnd()) {
                std::string type = std::string((*rit).value.GetString());
                if (type == std::string("natal")) {
                    model->natal_recruitment = true;
                } else if (type == std::string("metapopulation")) {
                    model->natal_recruitment = false;
                } else {
                    std::cout << "Configuration Error: Unknown recruitment type value of \"" << type << "\" for Population with id " << model->id << "\n";
                    mas_log << "Configuration Error: Unknown recruitment type value of \"" << type << "\" for Population with id " << model->id << "\n";
                }


            } else {
                std::cout << "Configuration Error: Population is required to define recruitment type, \"natal\"or \"metapopulation\"\n";
                mas::mas_log << "Configuration Error: Population is required to define recruitment type, \"natal\"or \"metapopulation\"\n";
                this->valid_configuration = false;

            }


            rit = (*population_model).value.FindMember("parameters");

            if (rit == (*population_model).value.MemberEnd()) {
                std::cout << "Configuration Error: Population is required to define parameters.";
                mas_log << "Configuration Error: Population is required to define parameters.";
                this->valid_configuration = false;
                return;

            }

            rapidjson::Document::MemberIterator in_it = (*rit).value.FindMember("initial_numbers");

            if ((*in_it).value.IsArray()) {

                rapidjson::Value& numbers = ((*in_it).value);
                for (int i = 0; i < numbers.Size(); i++) {

                    InitialNumbers<REAL_T> initial_n;

                    rapidjson::Value& entry = numbers[i];
                    rapidjson::Document::MemberIterator nit;
                    std::string sex;
                    int area;

                    nit = entry.FindMember("sex");
                    if (nit != entry.MemberEnd()) {
                        sex = std::string((*nit).value.GetString());
                        if (sex == "male") {
                            initial_n.type = MALE;
                        } else if (sex == "female") {
                            initial_n.type = FEMALE;
                        }

                    } else {
                        std::cout << "Configuration Error: Population initial numbers is required to define a sex";
                        mas::mas_log << "Configuration Error: Population initial numbers is required to define a sex";

                    }

                    nit = entry.FindMember("area");
                    if (nit != entry.MemberEnd()) {
                        area = (*nit).value.GetInt();
                        initial_n.area_id = area;
                    } else {
                        std::cout << "Configuration Error: Population initial numbers is required to define a area";
                        mas::mas_log << "Configuration Error: Population initial numbers is required to define a area";

                    }

                    nit = entry.FindMember("values");
                    if (nit != entry.MemberEnd()) {
                        rapidjson::Value& values = (*nit).value;
                        if (values.IsArray()) {
                            initial_n.values.resize(values.Size());
                            for (int i = 0; i < values.Size(); i++) {

                                VariableTrait<REAL_T>::SetValue(initial_n.values[i], static_cast<REAL_T> (values[i].GetDouble()));
                            }

                        }

                    } else {
                        std::cout << "Configuration Error: Population initial numbers is required to define values";
                        mas::mas_log << "Configuration Error: Population initial numbers is required to define values";

                    }

                    model->initial_numbers.push_back(initial_n);
                }


            } else {
                std::cout << "Configuration Error: initial_numbers for population \"" << model->id << "\" must be a vector.\n";
                mas_log << "Configuration Error: initial_numbers for population \"" << model->id << "\" must be a vector.\n";
            }

            in_it = (*rit).value.FindMember("growth");
            if (in_it != (*rit).value.MemberEnd()) {
                model->growth_id = (*in_it).value.GetInt();
            } else {
                std::cout << "Configuration Warning: Growth model not defined in population.\n";
                mas::mas_log << "Configuration Warning: Growth model not defined in population.\n";
            }

            in_it = (*rit).value.FindMember("natural_mortality");
            if (in_it != (*rit).value.MemberEnd()) {

                if ((*in_it).value.IsArray()) {
                    int a = 0;
                    int m = 0;
                    std::string sex = "NA";

                    for (int i = 0; i < (*in_it).value.Size(); i++) {
                        rapidjson::Value& v = (*in_it).value[i];
                        m = 0;
                        sex = "NA";

                        rapidjson::Document::MemberIterator mit = v.FindMember("area");
                        if ((mit) != v.MemberEnd()) {
                            a = (*mit).value.GetInt();
                        } else {
                            std::cout << "Configuration Error: Natural mortality area not defined in population.\n";
                            mas::mas_log << "Configuration Error: Natural mortality area not defined in population.\n";
                            this->valid_configuration = false;
                        }


                        mit = v.FindMember("id");
                        if ((mit) != v.MemberEnd()) {
                            m = (*mit).value.GetInt();
                        } else {
                            std::cout << "Configuration Error: Natural mortality model not defined in population.\n";
                            mas::mas_log << "Configuration Error: Natural mortality model not defined in population.\n";
                            this->valid_configuration = false;
                        }

                        mit = v.FindMember("sex");
                        if ((mit) != v.MemberEnd()) {
                            sex = std::string((*mit).value.GetString());
                        } else {
                            std::cout << "Configuration Warning: Natural mortality sex not defined in population.\n";
                            mas::mas_log << "Configuration Warning: Natural mortality sex not defined in population.\n";
                            //                            this->valid_configuration = false;
                        }




                        if (sex == "female") {
                            model->female_natural_mortality_ids[a] = m;
                        } else {
                            model->male_natural_mortality_ids[a] = m;
                        }

                    }
                }

            }

            in_it = (*rit).value.FindMember("recruitment");
            if (in_it != (*rit).value.MemberEnd()) {

                if ((*in_it).value.IsArray()) {
                    rapidjson::Value& v = (*in_it).value;

                    for (int i = 0; i < v.Size(); i++) {
                        int id = 0;
                        int area = 0;
                        int season = 0;

                        rapidjson::Value& elem = v[i];

                        rapidjson::Document::MemberIterator mit = elem.FindMember("id");

                        if (mit != elem.MemberEnd()) {
                            id = (*mit).value.GetInt();
                        } else {
                            std::cout << "Configuration Error: Population \"" << model->id << "\" has a recruitment/area/season definition that does not provide a recruitment model id\n";
                            mas_log << "Configuration Error: Population \"" << model->id << "\" has a recruitment/area/season definition that does not provide a recruitment model id\n";
                            this->valid_configuration = false;
                        }

                        mit = elem.FindMember("area");

                        if (mit != elem.MemberEnd()) {
                            area = (*mit).value.GetInt();
                        } else {
                            std::cout << "Configuration Error: Population \"" << model->id << "\" has a recruitment/area/season definition that does not provide an area id\n";
                            mas_log << "Configuration Error: Population \"" << model->id << "\" has a recruitment/area/season definition that does not provide an area id\n";
                            this->valid_configuration = false;
                        }

                        mit = elem.FindMember("season");

                        if (mit != elem.MemberEnd()) {
                            season = (*mit).value.GetInt();
                        } else {
                            std::cout << "Configuration Error: Population \"" << model->id << "\" has a recruitment/area/season definition that does not provide an season id\n";
                            mas_log << "Configuration Error: Population \"" << model->id << "\" has a recruitment/area/season definition that does not provide an season id\n";
                            this->valid_configuration = false;
                        }

                        model->recruitment_ids[area][season] = id;

                    }

                }
            } else {
                std::cout << "Configuration Error: Population \"" << model->id << "\" has no recruitment definitions.\n";
                mas_log << "Configuration Error: Population \"" << model->id << "\" has no recruitment definitions.\n";

            }

            population_iterator it = this->populations.find(model->id);
            if (it != this->populations.end()) {
                std::cout << "Configuration Error: More than one population with the same identifier defined. Populations require a unique id.\n";
                mas::mas_log << "Configuration Error: More than one population with the same identifier defined. Populations require a unique id.\n";

                this->valid_configuration = false;
                ;
            } else {
                this->populations[model->id] = model;
            }

        }

        void HandleAreaModel(rapidjson::Document::MemberIterator & area_model) {

            std::shared_ptr<mas::Area<REAL_T> > model(new mas::Area<REAL_T>());


            rapidjson::Document::MemberIterator rit = (*area_model).value.FindMember("id");
            int model_id = 0;
            if (rit == (*area_model).value.MemberEnd()) {
                std::cout << "Configuration Error: Area is required to have a unique identifier\n";
                mas::mas_log << "Configuration Error: Area is required to have a unique identifier\n";
                this->valid_configuration = false;
                ;
            } else {
                model_id = (*rit).value.GetInt();
            }

            rit = (*area_model).value.FindMember("name");
            if (rit != (*area_model).value.MemberEnd()) {
                model->name = std::string((*rit).value.GetString());
            } else {
                model->name = "NA";
            }

            rit = (*area_model).value.FindMember("growth_model");
            if (rit != (*area_model).value.MemberEnd()) {
                model->growth_model_id = (*rit).value.GetInt();
            } else {
                std::cout << "Configuration Error: Area " << model->id << " is required to have a growth model defined\n";
                mas::mas_log << "Configuration Error: Area " << model->id << " is required to have a growth model defined\n";
                this->valid_configuration = false;
                ;
            }

            rit = (*area_model).value.FindMember("recruitment_model");
            if (rit != (*area_model).value.MemberEnd()) {
                model->recruitment_model_id = (*rit).value.GetInt();
            } else {
                std::cout << "Configuration Error: Area " << model->id << " is required to have a recruitment model defined\n";
                mas::mas_log << "Configuration Error: Area " << model->id << " is required to have a recruitment model defined\n";
                this->valid_configuration = false;
                ;
            }

            rit = (*area_model).value.FindMember("fecundity_model");
            if (rit != (*area_model).value.MemberEnd()) {
                model->fecundity_model_id = (*rit).value.GetInt();
            } else {
                std::cout << "Configuration Error: Area " << model->id << " is required to have a fecundity model defined\n";
                mas::mas_log << "Configuration Error: Area " << model->id << " is required to have a fecundity model defined\n";
                this->valid_configuration = false;

            }

            rit = (*area_model).value.FindMember("mortality_model");
            if (rit != (*area_model).value.MemberEnd()) {
                model->mortality_model_id = (*rit).value.GetInt();
            } else {
                std::cout << "Configuration Error: Area " << model->id << " is required to have a mortality model defined\n";
                mas::mas_log << "Configuration Error: Area " << model->id << " is required to have a mortality model defined\n";
                this->valid_configuration = false;

            }

            model->id = model_id;
            area_iterator it = this->areas.find(model->id);
            if (it != this->areas.end()) {
                std::cout << "Configuration Error: More than one area with the same identifier defined. Areas require a unique id.\n";
                mas::mas_log << "Configuration Error: More than one area with the same identifier defined. Areas require a unique id.\n";

                this->valid_configuration = false;
                ;
            } else {
                this->areas[model->id] = model;
            }



        }

        void HandleSelectivityModel(rapidjson::Document::MemberIterator & selectivity_model) {
            rapidjson::Document::MemberIterator rit;
            rit = (*selectivity_model).value.FindMember("model");

            if (rit == (*selectivity_model).value.MemberEnd()) {
                std::cout << "Configuration Error: Selectivity is required to have a model specified.\n";
                mas::mas_log << "Configuration Error: Selectivity is required to have a model specified.\n";
                this->valid_configuration = false;
                ;
            }


            std::shared_ptr<mas::SelectivityBase<REAL_T> > model(NULL);
            std::string smodel = std::string((*rit).value.GetString());

            rit = (*selectivity_model).value.FindMember("id");
            int model_id = 0;
            if (rit == (*selectivity_model).value.MemberEnd()) {
                std::cout << "Configuration Error: Selectivity is required to have a unique identifier\n";
                mas::mas_log << "Configuration Error: Selectivity is required to have a unique identifier\n";
                this->valid_configuration = false;
                ;
            } else {
                model_id = (*rit).value.GetInt();
            }

            rapidjson::Document::MemberIterator pit = (*selectivity_model).value.FindMember("parameters");

            if (smodel == std::string("logistic")) {
                model = std::make_shared<mas::Logistic<REAL_T> >();
                mas::Logistic<REAL_T>* l = (mas::Logistic<REAL_T>*)model.get();

                std::stringstream ss;
                ss << "logistic_a50_" << model_id;
                VariableTrait<REAL_T>::SetName(l->a50, ss.str());
                ss.str("");
                ss << "logistic_s_" << model_id;
                VariableTrait<REAL_T>::SetName(l->s, ss.str());

                if (pit == (*selectivity_model).value.MemberEnd()) {
                    std::cout << "Configuration Error: Selectivity model \"Logistic\" has no parameter definitions.\n";
                    mas::mas_log << "Configuration Error: Recruitment model \"Logistic\" has no parameter definitions.\n";
                    this->valid_configuration = false;
                    ;
                } else {

                    rapidjson::Document::MemberIterator ppit = (*pit).value.FindMember("a50");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Selectivity model \"Logistic\" " <<
                                "does not define \"a50\". Model will use the default value of 0 and \"a50\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Selectivity model \"Logistic\" " <<
                                "does not define \"a50\". Model will use the default value of 0 and \"a50\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Selectivity model \"Logistic\" does not provide a initial value for \"a50\".\n";
                            mas::mas_log << "Configuration Warning: Selectivity model \"Logistic\" does not provide a initial value for \"a50\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(l->a50, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(l->a50, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(l->a50, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register alpha
                            l->Register(l->a50, phase);

                        }

                    }

                    ppit = (*pit).value.FindMember("s");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Selectivity model \"Logistic\" " <<
                                "does not define \"s\". Model will use the default value of 0 and \"s\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Selectivity model \"Logistic\" " <<
                                "does not define \"s\". Model will use the default value of 0 and \"s\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Selectivity model \"Logistic\" does not provide a initial value for \"s\".\n";
                            mas::mas_log << "Configuration Warning: Selectivity model \"Logistic\" does not provide a initial value for \"s\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(l->s, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(l->s, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(l->s, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register alpha
                            l->Register(l->s, phase);

                        }

                    }



                }


            } else if (smodel == std::string("double_logistic")) {
                model = std::make_shared<mas::DoubleLogistic<REAL_T> >();
                mas::DoubleLogistic<REAL_T>* l = (mas::DoubleLogistic<REAL_T>*)model.get();

                std::stringstream ss;
                ss << "logistic_alpha_asc_" << model_id;
                VariableTrait<REAL_T>::SetName(l->alpha_asc, ss.str());

                ss.str("");
                ss << "logistic_beta_asc_" << model_id;
                VariableTrait<REAL_T>::SetName(l->beta_asc, ss.str());

                ss.str("");
                ss << "logistic_alpha_desc_" << model_id;
                VariableTrait<REAL_T>::SetName(l->beta_desc, ss.str());

                ss.str("");
                ss << "logistic_beta_desc_" << model_id;
                VariableTrait<REAL_T>::SetName(l->beta_desc, ss.str());

                if (pit == (*selectivity_model).value.MemberEnd()) {
                    std::cout << "Configuration Error: Selectivity model \"Logistic\" has no parameter definitions.\n";
                    mas::mas_log << "Configuration Error: Recruitment model \"Logistic\" has no parameter definitions.\n";
                    this->valid_configuration = false;
                    ;
                } else {

                    rapidjson::Document::MemberIterator ppit = (*pit).value.FindMember("alpha_asc");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Selectivity model \"Logistic\" " <<
                                "does not define \"alpha_asc\". Model will use the default value of 0 and \"alpha_asc\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Selectivity model \"Logistic\" " <<
                                "does not define \"alpha_asc\". Model will use the default value of 0 and \"alpha_asc\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Selectivity model \"Logistic\" does not provide a initial value for \"alpha_asc\".\n";
                            mas::mas_log << "Configuration Warning: Selectivity model \"Logistic\" does not provide a initial value for \"alpha_asc\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(l->alpha_asc, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(l->alpha_asc, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(l->alpha_asc, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register alpha
                            l->Register(l->alpha_asc, phase);

                        }

                    }

                    ppit = (*pit).value.FindMember("beta_asc");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Selectivity model \"Logistic\" " <<
                                "does not define \"beta_asc\". Model will use the default value of 0 and \"beta_asc\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Selectivity model \"Logistic\" " <<
                                "does not define \"beta_asc\". Model will use the default value of 0 and \"beta_asc\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Selectivity model \"Logistic\" does not provide a initial value for \"beta_asc\".\n";
                            mas::mas_log << "Configuration Warning: Selectivity model \"Logistic\" does not provide a initial value for \"beta_asc\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(l->beta_asc, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(l->beta_asc, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(l->beta_asc, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register alpha
                            l->Register(l->beta_asc, phase);

                        }

                    }

                    ppit = (*pit).value.FindMember("alpha_desc");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Selectivity model \"Logistic\" " <<
                                "does not define \"alpha_desc\". Model will use the default value of 0 and \"alpha_desc\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Selectivity model \"Logistic\" " <<
                                "does not define \"alpha_desc\". Model will use the default value of 0 and \"alpha_desc\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Selectivity model \"Logistic\" does not provide a initial value for \"alpha_desc\".\n";
                            mas::mas_log << "Configuration Warning: Selectivity model \"Logistic\" does not provide a initial value for \"alpha_desc\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(l->alpha_desc, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(l->alpha_desc, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(l->alpha_desc, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register alpha
                            l->Register(l->alpha_desc, phase);

                        }

                    }

                    ppit = (*pit).value.FindMember("beta_desc");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Selectivity model \"Logistic\" " <<
                                "does not define \"beta_desc\". Model will use the default value of 0 and \"beta_desc\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Selectivity model \"Logistic\" " <<
                                "does not define \"beta_desc\". Model will use the default value of 0 and \"beta_desc\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Selectivity model \"Logistic\" does not provide a initial value for \"beta_desc\".\n";
                            mas::mas_log << "Configuration Warning: Selectivity model \"Logistic\" does not provide a initial value for \"beta_desc\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(l->beta_desc, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(l->beta_desc, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(l->beta_desc, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register alpha
                            l->Register(l->beta_desc, phase);

                        }

                    }

                }

            }

            model->id = model_id;
            selectivity_model_iterator it = this->selectivity_models.find(model->id);
            if (it != this->selectivity_models.end()) {
                std::cout << "Configuration Error: More than one selectivity model with the same identifier defined. Selectivity models require a unique id.\n";
                mas::mas_log << "Configuration Error: More than one selectivity model with the same identifier defined. Selectivity models require a unique id.\n";
                this->valid_configuration = false;
                ;
            } else {
                this->selectivity_models[model->id] = model;
            }


        }

        void HandleGrowthModel(rapidjson::Document::MemberIterator & growth_model) {
            rapidjson::Document::MemberIterator rit;
            rit = (*growth_model).value.FindMember("model");

            if (rit == (*growth_model).value.MemberEnd()) {
                std::cout << "Configuration Error: Growth is required to have a model specified.\n";
                mas::mas_log << "Configuration Error: Growth is required to have a model specified.\n";
                this->valid_configuration = false;
                ;
            }


            std::shared_ptr<mas::GrowthBase<REAL_T> > model(NULL);
            std::string smodel = std::string((*rit).value.GetString());

            rit = (*growth_model).value.FindMember("id");
            int model_id = 0;
            if (rit == (*growth_model).value.MemberEnd()) {
                std::cout << "Configuration Error: Growth is required to have a unique identifier\n";
                mas::mas_log << "Configuration Error: Growth is required to have a unique identifier\n";
                this->valid_configuration = false;
                ;
            } else {
                model_id = (*rit).value.GetInt();
            }

            rapidjson::Document::MemberIterator pit = (*growth_model).value.FindMember("parameters");

            if (smodel == std::string("von_bertalanffy")) {
                model = std::make_shared<mas::VonBertalanffy<REAL_T> >();
                mas::VonBertalanffy<REAL_T>* vb = (mas::VonBertalanffy<REAL_T>*)model.get();

                std::stringstream ss;
                ss << "von_bertalanffy_k_" << model_id;
                VariableTrait<REAL_T>::SetName(vb->k, ss.str());
                ss.str("");
                ss << "von_bertalanffy_inf_" << model_id;
                VariableTrait<REAL_T>::SetName(vb->l_inf, ss.str());

                ss.str("");
                ss << "von_bertalanffy_amax_" << model_id;
                VariableTrait<REAL_T>::SetName(vb->a_max, ss.str());

                ss.str("");
                ss << "von_bertalanffy_amin_" << model_id;
                VariableTrait<REAL_T>::SetName(vb->a_min, ss.str());

                if (pit == (*growth_model).value.MemberEnd()) {
                    std::cout << "Configuration Error: Recruitment model \"Beverton-Holt\" has no parameter definitions.\n";
                    mas::mas_log << "Configuration Error: Recruitment model \"Beverton-Holt\" has no parameter definitions.\n";
                    this->valid_configuration = false;

                } else {
                    rapidjson::Document::MemberIterator ppit = (*pit).value.FindMember("k");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Growth model \"Von Bertalannfy\" " <<
                                "does not define \"k\". Model will use the default value of 0 and \"k\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Recruitment model \"Von Bertalannfy\" " <<
                                "does not define \"k\". Model will use the default value of 0 and \"k\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Von Bertalannfy\" does not provide a initial value for \"k\".\n";
                            mas::mas_log << "Configuration Warning: Growth model \"Von Bertalannfy\" does not provide a initial value for \"k\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(vb->k, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(vb->k, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(vb->k, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register alpha
                            vb->Register(vb->k, phase);

                        }

                    }


                    ppit = (*pit).value.FindMember("linf");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Growth model \"Von Bertalannfy\" " <<
                                "does not define \"l_inf\". Model will use the default value of 0 and \"l_inf\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Recruitment model \"Von Bertalannfy\" " <<
                                "does not define \"l_inf\". Model will use the default value of 0 and \"l_inf\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Von Bertalannfy\" does not provide a initial value for \"l_inf\".\n";
                            mas::mas_log << "Configuration Warning: Growth model \"Von Bertalannfy\" does not provide a initial value for \"l_inf\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(vb->l_inf, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(vb->l_inf, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(vb->l_inf, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register alpha
                            vb->Register(vb->l_inf, phase);

                        }

                    }



                }

            } else if (smodel == std::string("von_bertalanffy_modified")) {
                model = std::make_shared<mas::VonBertalanffyModified<REAL_T> >();
                mas::VonBertalanffyModified<REAL_T>* vb = (mas::VonBertalanffyModified<REAL_T>*)model.get();

                std::stringstream ss;
                ss << "von_bertalanffy_c_" << model_id;
                VariableTrait<REAL_T>::SetName(vb->c, ss.str());
                ss.str("");
                ss << "von_bertalanffy_inf_" << model_id;
                VariableTrait<REAL_T>::SetName(vb->l_inf, ss.str());

                ss.str("");
                ss << "von_bertalanffy_amax_" << model_id;
                VariableTrait<REAL_T>::SetName(vb->a_max, ss.str());

                ss.str("");
                ss << "von_bertalanffy_amin_" << model_id;
                VariableTrait<REAL_T>::SetName(vb->a_min, ss.str());

                if (pit == (*growth_model).value.MemberEnd()) {
                    std::cout << "Configuration Error: Recruitment model \"Beverton-Holt\" has no parameter definitions.\n";
                    mas::mas_log << "Configuration Error: Recruitment model \"Beverton-Holt\" has no parameter definitions.\n";
                    this->valid_configuration = false;

                } else {
                    rapidjson::Document::MemberIterator ppit = (*pit).value.FindMember("c");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Growth model \"Von Bertalannfy\" " <<
                                "does not define \"c\". Model will use the default value of 0 and \"c\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Recruitment model \"Von Bertalannfy\" " <<
                                "does not define \"c\". Model will use the default value of 0 and \"c\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Von Bertalannfy Modified\" does not provide a initial value for \"c\".\n";
                            mas::mas_log << "Configuration Warning: Growth model \"Von Bertalannfy Modified\" does not provide a initial value for \"c\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(vb->c, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(vb->c, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(vb->c, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register alpha
                            vb->Register(vb->c, phase);

                        }

                    }

                    ppit = (*pit).value.FindMember("lmin");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Growth model \"Von Bertalannfy Modified\" " <<
                                "does not define \"lmin\". Model will use the default value of 0 and \"lmin\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Growth model \"Von Bertalannfy Modified\" " <<
                                "does not define \"lmin\". Model will use the default value of 0 and \"lmin\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Von Bertalannfy Modified\" does not provide a initial value for \"lmin\".\n";
                            mas::mas_log << "Configuration Warning: Growth model \"Von Bertalannfy Modified\" does not provide a initial value for \"lmin\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(vb->lmin, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(vb->lmin, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(vb->lmin, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register alpha
                            vb->Register(vb->lmin, phase);

                        }

                    }

                    ppit = (*pit).value.FindMember("lmax");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Growth model \"Von Bertalannfy Modified\" " <<
                                "does not define \"lmax\". Model will use the default value of 0 and \"lmax\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Growth model \"Von Bertalannfy Modified\" " <<
                                "does not define \"lmax\". Model will use the default value of 0 and \"lmax\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Von Bertalannfy Modified\" does not provide a initial value for \"lmax\".\n";
                            mas::mas_log << "Configuration Warning: Growth model \"Von Bertalannfy Modified\" does not provide a initial value for \"lmax\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(vb->lmax, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(vb->lmax, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(vb->lmax, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register alpha
                            vb->Register(vb->lmax, phase);

                        }

                    }



                    ppit = (*pit).value.FindMember("linf");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Growth model \"Von Bertalannfy\" " <<
                                "does not define \"l_inf\". Model will use the default value of 0 and \"l_inf\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Recruitment model \"Von Bertalannfy\" " <<
                                "does not define \"l_inf\". Model will use the default value of 0 and \"l_inf\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Von Bertalannfy\" does not provide a initial value for \"l_inf\".\n";
                            mas::mas_log << "Configuration Warning: Growth model \"Von Bertalannfy\" does not provide a initial value for \"l_inf\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(vb->l_inf, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(vb->l_inf, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(vb->l_inf, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register alpha
                            vb->Register(vb->l_inf, phase);

                        }

                    }



                }

            } else if (smodel == std::string("schnute_case_I")) {

                model = std::make_shared<mas::SchnuteCaseI<REAL_T> >();
                mas::SchnuteCaseI<REAL_T>* s = (mas::SchnuteCaseI<REAL_T>*)model.get();

                std::stringstream ss;
                ss << "schnute_case_I_alpha_" << model_id;
                VariableTrait<REAL_T>::SetName(s->alpha, ss.str());

                ss.str("");
                ss << "schnute_case_I_beta_" << model_id;
                VariableTrait<REAL_T>::SetName(s->beta, ss.str());

                ss.str("");
                ss << "schnute_case_I_lmin_" << model_id;
                VariableTrait<REAL_T>::SetName(s->lmin, ss.str());

                ss.str("");
                ss << "schnute_case_I_lmax_" << model_id;
                VariableTrait<REAL_T>::SetName(s->lmax, ss.str());

                if (pit == (*growth_model).value.MemberEnd()) {
                    std::cout << "Configuration Error: Recruitment model \"Schnute Case I\" has no parameter definitions.\n";
                    mas::mas_log << "Configuration Error: Recruitment model \"Schnute Case I\" has no parameter definitions.\n";
                    this->valid_configuration = false;
                    ;
                } else {
                    rapidjson::Document::MemberIterator ppit = (*pit).value.FindMember("alpha");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Growth model \"Schnute Case I\" " <<
                                "does not define \"alpha\". Model will use the default value of 0 and \"alpha\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Recruitment model \"Schnute Case I\" " <<
                                "does not define \"alpha\". Model will use the default value of 0 and \"alpha\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Schnute Case I\" does not provide a initial value for \"alpha\".\n";
                            mas::mas_log << "Configuration Warning: Growth model \"Schnute Case I\" does not provide a initial value for \"alpha\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(s->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(s->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(s->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register alpha
                            s->Register(s->alpha, phase);

                        }

                        ppit = (*pit).value.FindMember("beta");
                        if (ppit == (*pit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Schnute Case I\" " <<
                                    "does not define \"beta\". Model will use the default value of 0 and \"beta\" will not be estimated.\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Schnute Case I\" " <<
                                    "does not define \"beta\". Model will use the default value of 0 and \"beta\" will not be estimated.\n";
                        } else {

                            bool estimated = false;
                            int phase = 1;
                            //1. Get initial value if there is one.
                            rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                            if (pm == (*ppit).value.MemberEnd()) {
                                std::cout << "Configuration Warning: Growth model \"Schnute Case I\" does not provide a initial value for \"beta\".\n";
                                mas::mas_log << "Configuration Warning: Growth model \"Schnute Case I\" does not provide a initial value for \"beta\".\n";
                            } else {
                                VariableTrait<REAL_T>::SetValue(s->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //2. Get min boundary if there is one.
                            pm = (*ppit).value.FindMember("min");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMinBoundary(s->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //3. Get max boundary if there is one.
                            pm = (*ppit).value.FindMember("max");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMaxBoundary(s->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            pm = (*ppit).value.FindMember("estimated");
                            if (pm != (*ppit).value.MemberEnd()) {
                                std::string e = std::string((*pm).value.GetString());
                                if (e == "true") {
                                    estimated = true;
                                }
                            }

                            if (estimated) {
                                phase = 1;
                                pm = (*ppit).value.FindMember("phase");
                                if (pm != (*ppit).value.MemberEnd()) {
                                    phase = (*pm).value.GetInt();
                                }
                                //register alpha
                                s->Register(s->beta, phase);

                            }

                        }

                        ppit = (*pit).value.FindMember("lmin");
                        if (ppit == (*pit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Schnute Case I\" " <<
                                    "does not define \"lmin\". Model will use the default value of 0 and \"lmin\" will not be estimated.\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Schnute Case I\" " <<
                                    "does not define \"lmin\". Model will use the default value of 0 and \"lmin\" will not be estimated.\n";
                        } else {

                            bool estimated = false;
                            int phase = 1;
                            //1. Get initial value if there is one.
                            rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                            if (pm == (*ppit).value.MemberEnd()) {
                                std::cout << "Configuration Warning: Growth model \"Schnute Case I\" does not provide a initial value for \"lmin\".\n";
                                mas::mas_log << "Configuration Warning: Growth model \"Schnute Case I\" does not provide a initial value for \"lmin\".\n";
                            } else {
                                VariableTrait<REAL_T>::SetValue(s->lmin, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //2. Get min boundary if there is one.
                            pm = (*ppit).value.FindMember("min");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMinBoundary(s->lmin, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //3. Get max boundary if there is one.
                            pm = (*ppit).value.FindMember("max");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMaxBoundary(s->lmin, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            pm = (*ppit).value.FindMember("estimated");
                            if (pm != (*ppit).value.MemberEnd()) {
                                std::string e = std::string((*pm).value.GetString());
                                if (e == "true") {
                                    estimated = true;
                                }
                            }

                            if (estimated) {
                                phase = 1;
                                pm = (*ppit).value.FindMember("phase");
                                if (pm != (*ppit).value.MemberEnd()) {
                                    phase = (*pm).value.GetInt();
                                }
                                //register alpha
                                s->Register(s->lmin, phase);

                            }

                        }

                        ppit = (*pit).value.FindMember("lmax");
                        if (ppit == (*pit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Schnute Case I\" " <<
                                    "does not define \"lmax\". Model will use the default value of 0 and \"lmax\" will not be estimated.\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Schnute Case I\" " <<
                                    "does not define \"lmax\". Model will use the default value of 0 and \"lmax\" will not be estimated.\n";
                        } else {

                            bool estimated = false;
                            int phase = 1;
                            //1. Get initial value if there is one.
                            rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                            if (pm == (*ppit).value.MemberEnd()) {
                                std::cout << "Configuration Warning: Growth model \"Schnute Case I\" does not provide a initial value for \"lmax\".\n";
                                mas::mas_log << "Configuration Warning: Growth model \"Schnute Case I\" does not provide a initial value for \"lmax\".\n";
                            } else {
                                VariableTrait<REAL_T>::SetValue(s->lmax, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //2. Get min boundary if there is one.
                            pm = (*ppit).value.FindMember("min");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMinBoundary(s->lmax, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //3. Get max boundary if there is one.
                            pm = (*ppit).value.FindMember("max");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMaxBoundary(s->lmax, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            pm = (*ppit).value.FindMember("estimated");
                            if (pm != (*ppit).value.MemberEnd()) {
                                std::string e = std::string((*pm).value.GetString());
                                if (e == "true") {
                                    estimated = true;
                                }
                            }

                            if (estimated) {
                                phase = 1;
                                pm = (*ppit).value.FindMember("phase");
                                if (pm != (*ppit).value.MemberEnd()) {
                                    phase = (*pm).value.GetInt();
                                }
                                //register alpha
                                s->Register(s->lmax, phase);

                            }

                        }
                    }
                }

            } else if (smodel == std::string("schnute_case_II")) {
                model = std::make_shared<mas::SchnuteCaseII<REAL_T> >();
                mas::SchnuteCaseII<REAL_T>* s = (mas::SchnuteCaseII<REAL_T>*)model.get();

                std::stringstream ss;
                ss << "schnute_case_II_alpha_" << model_id;
                VariableTrait<REAL_T>::SetName(s->alpha, ss.str());

                ss.str("");
                ss << "schnute_case_II_lmin_" << model_id;
                VariableTrait<REAL_T>::SetName(s->lmin, ss.str());

                ss.str("");
                ss << "schnute_case_II_lmax_" << model_id;
                VariableTrait<REAL_T>::SetName(s->lmax, ss.str());

                if (pit == (*growth_model).value.MemberEnd()) {
                    std::cout << "Configuration Error: Recruitment model \"Schnute Case II\" has no parameter definitions.\n";
                    mas::mas_log << "Configuration Error: Recruitment model \"Schnute Case II\" has no parameter definitions.\n";
                    this->valid_configuration = false;
                    ;
                } else {
                    rapidjson::Document::MemberIterator ppit = (*pit).value.FindMember("alpha");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Growth model \"Schnute Case II\" " <<
                                "does not define \"alpha\". Model will use the default value of 0 and \"alpha\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Recruitment model \"Schnute Case II\" " <<
                                "does not define \"alpha\". Model will use the default value of 0 and \"alpha\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Schnute Case II\" does not provide a initial value for \"alpha\".\n";
                            mas::mas_log << "Configuration Warning: Growth model \"Schnute Case II\" does not provide a initial value for \"alpha\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(s->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(s->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(s->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register alpha
                            s->Register(s->alpha, phase);

                        }


                        ppit = (*pit).value.FindMember("lmin");
                        if (ppit == (*pit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Schnute Case II\" " <<
                                    "does not define \"lmin\". Model will use the default value of 0 and \"lmin\" will not be estimated.\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Schnute Case II\" " <<
                                    "does not define \"lmin\". Model will use the default value of 0 and \"lmin\" will not be estimated.\n";
                        } else {

                            bool estimated = false;
                            int phase = 1;
                            //1. Get initial value if there is one.
                            rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                            if (pm == (*ppit).value.MemberEnd()) {
                                std::cout << "Configuration Warning: Growth model \"Schnute Case II\" does not provide a initial value for \"lmin\".\n";
                                mas::mas_log << "Configuration Warning: Growth model \"Schnute Case II\" does not provide a initial value for \"lmin\".\n";
                            } else {
                                VariableTrait<REAL_T>::SetValue(s->lmin, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //2. Get min boundary if there is one.
                            pm = (*ppit).value.FindMember("min");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMinBoundary(s->lmin, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //3. Get max boundary if there is one.
                            pm = (*ppit).value.FindMember("max");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMaxBoundary(s->lmin, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            pm = (*ppit).value.FindMember("estimated");
                            if (pm != (*ppit).value.MemberEnd()) {
                                std::string e = std::string((*pm).value.GetString());
                                if (e == "true") {
                                    estimated = true;
                                }
                            }

                            if (estimated) {
                                phase = 1;
                                pm = (*ppit).value.FindMember("phase");
                                if (pm != (*ppit).value.MemberEnd()) {
                                    phase = (*pm).value.GetInt();
                                }
                                //register alpha
                                s->Register(s->lmin, phase);

                            }

                        }

                        ppit = (*pit).value.FindMember("lmax");
                        if (ppit == (*pit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Schnute Case II\" " <<
                                    "does not define \"lmax\". Model will use the default value of 0 and \"lmax\" will not be estimated.\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Schnute Case II\" " <<
                                    "does not define \"lmax\". Model will use the default value of 0 and \"lmax\" will not be estimated.\n";
                        } else {

                            bool estimated = false;
                            int phase = 1;
                            //1. Get initial value if there is one.
                            rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                            if (pm == (*ppit).value.MemberEnd()) {
                                std::cout << "Configuration Warning: Growth model \"Schnute Case II\" does not provide a initial value for \"lmax\".\n";
                                mas::mas_log << "Configuration Warning: Growth model \"Schnute Case II\" does not provide a initial value for \"lmax\".\n";
                            } else {
                                VariableTrait<REAL_T>::SetValue(s->lmax, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //2. Get min boundary if there is one.
                            pm = (*ppit).value.FindMember("min");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMinBoundary(s->lmax, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //3. Get max boundary if there is one.
                            pm = (*ppit).value.FindMember("max");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMaxBoundary(s->lmax, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            pm = (*ppit).value.FindMember("estimated");
                            if (pm != (*ppit).value.MemberEnd()) {
                                std::string e = std::string((*pm).value.GetString());
                                if (e == "true") {
                                    estimated = true;
                                }
                            }

                            if (estimated) {
                                phase = 1;
                                pm = (*ppit).value.FindMember("phase");
                                if (pm != (*ppit).value.MemberEnd()) {
                                    phase = (*pm).value.GetInt();
                                }
                                //register alpha
                                s->Register(s->lmax, phase);

                            }

                        }
                    }
                }

            } else if (smodel == std::string("schnute_case_III")) {
                model = std::make_shared<mas::SchnuteCaseIII<REAL_T> >();
                mas::SchnuteCaseIII<REAL_T>* s = (mas::SchnuteCaseIII<REAL_T>*)model.get();

                std::stringstream ss;
                ss << "schnute_case_III_alpha_" << model_id;
                VariableTrait<REAL_T>::SetName(s->alpha, ss.str());

                ss.str("");
                ss << "schnute_case_III_beta_" << model_id;
                VariableTrait<REAL_T>::SetName(s->beta, ss.str());

                ss.str("");
                ss << "schnute_case_III_lmin_" << model_id;
                VariableTrait<REAL_T>::SetName(s->lmin, ss.str());

                ss.str("");
                ss << "schnute_case_III_lmax_" << model_id;
                VariableTrait<REAL_T>::SetName(s->lmax, ss.str());

                if (pit == (*growth_model).value.MemberEnd()) {
                    std::cout << "Configuration Error: Recruitment model \"Schnute Case III\" has no parameter definitions.\n";
                    mas::mas_log << "Configuration Error: Recruitment model \"Schnute Case III\" has no parameter definitions.\n";
                    this->valid_configuration = false;
                    ;
                } else {
                    rapidjson::Document::MemberIterator ppit = (*pit).value.FindMember("alpha");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Growth model \"Schnute Case III\" " <<
                                "does not define \"alpha\". Model will use the default value of 0 and \"alpha\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Recruitment model \"Schnute Case III\" " <<
                                "does not define \"alpha\". Model will use the default value of 0 and \"alpha\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Schnute Case III\" does not provide a initial value for \"alpha\".\n";
                            mas::mas_log << "Configuration Warning: Growth model \"Schnute Case III\" does not provide a initial value for \"alpha\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(s->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(s->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(s->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register alpha
                            s->Register(s->alpha, phase);

                        }

                        ppit = (*pit).value.FindMember("beta");
                        if (ppit == (*pit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Schnute Case III\" " <<
                                    "does not define \"beta\". Model will use the default value of 0 and \"beta\" will not be estimated.\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Schnute Case III\" " <<
                                    "does not define \"beta\". Model will use the default value of 0 and \"beta\" will not be estimated.\n";
                        } else {

                            bool estimated = false;
                            int phase = 1;
                            //1. Get initial value if there is one.
                            rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                            if (pm == (*ppit).value.MemberEnd()) {
                                std::cout << "Configuration Warning: Growth model \"Schnute Case III\" does not provide a initial value for \"beta\".\n";
                                mas::mas_log << "Configuration Warning: Growth model \"Schnute Case III\" does not provide a initial value for \"beta\".\n";
                            } else {
                                VariableTrait<REAL_T>::SetValue(s->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //2. Get min boundary if there is one.
                            pm = (*ppit).value.FindMember("min");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMinBoundary(s->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //3. Get max boundary if there is one.
                            pm = (*ppit).value.FindMember("max");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMaxBoundary(s->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            pm = (*ppit).value.FindMember("estimated");
                            if (pm != (*ppit).value.MemberEnd()) {
                                std::string e = std::string((*pm).value.GetString());
                                if (e == "true") {
                                    estimated = true;
                                }
                            }

                            if (estimated) {
                                phase = 1;
                                pm = (*ppit).value.FindMember("phase");
                                if (pm != (*ppit).value.MemberEnd()) {
                                    phase = (*pm).value.GetInt();
                                }
                                //register alpha
                                s->Register(s->beta, phase);

                            }

                        }

                        ppit = (*pit).value.FindMember("lmin");
                        if (ppit == (*pit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Schnute Case III\" " <<
                                    "does not define \"lmin\". Model will use the default value of 0 and \"lmin\" will not be estimated.\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Schnute Case III\" " <<
                                    "does not define \"lmin\". Model will use the default value of 0 and \"lmin\" will not be estimated.\n";
                        } else {

                            bool estimated = false;
                            int phase = 1;
                            //1. Get initial value if there is one.
                            rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                            if (pm == (*ppit).value.MemberEnd()) {
                                std::cout << "Configuration Warning: Growth model \"Schnute Case III\" does not provide a initial value for \"lmin\".\n";
                                mas::mas_log << "Configuration Warning: Growth model \"Schnute Case III\" does not provide a initial value for \"lmin\".\n";
                            } else {
                                VariableTrait<REAL_T>::SetValue(s->lmin, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //2. Get min boundary if there is one.
                            pm = (*ppit).value.FindMember("min");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMinBoundary(s->lmin, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //3. Get max boundary if there is one.
                            pm = (*ppit).value.FindMember("max");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMaxBoundary(s->lmin, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            pm = (*ppit).value.FindMember("estimated");
                            if (pm != (*ppit).value.MemberEnd()) {
                                std::string e = std::string((*pm).value.GetString());
                                if (e == "true") {
                                    estimated = true;
                                }
                            }

                            if (estimated) {
                                phase = 1;
                                pm = (*ppit).value.FindMember("phase");
                                if (pm != (*ppit).value.MemberEnd()) {
                                    phase = (*pm).value.GetInt();
                                }
                                //register alpha
                                s->Register(s->lmin, phase);

                            }

                        }

                        ppit = (*pit).value.FindMember("lmax");
                        if (ppit == (*pit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Schnute Case III\" " <<
                                    "does not define \"lmax\". Model will use the default value of 0 and \"lmax\" will not be estimated.\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Schnute Case III\" " <<
                                    "does not define \"lmax\". Model will use the default value of 0 and \"lmax\" will not be estimated.\n";
                        } else {

                            bool estimated = false;
                            int phase = 1;
                            //1. Get initial value if there is one.
                            rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                            if (pm == (*ppit).value.MemberEnd()) {
                                std::cout << "Configuration Warning: Growth model \"Schnute Case III\" does not provide a initial value for \"lmax\".\n";
                                mas::mas_log << "Configuration Warning: Growth model \"Schnute Case III\" does not provide a initial value for \"lmax\".\n";
                            } else {
                                VariableTrait<REAL_T>::SetValue(s->lmax, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //2. Get min boundary if there is one.
                            pm = (*ppit).value.FindMember("min");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMinBoundary(s->lmax, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //3. Get max boundary if there is one.
                            pm = (*ppit).value.FindMember("max");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMaxBoundary(s->lmax, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            pm = (*ppit).value.FindMember("estimated");
                            if (pm != (*ppit).value.MemberEnd()) {
                                std::string e = std::string((*pm).value.GetString());
                                if (e == "true") {
                                    estimated = true;
                                }
                            }

                            if (estimated) {
                                phase = 1;
                                pm = (*ppit).value.FindMember("phase");
                                if (pm != (*ppit).value.MemberEnd()) {
                                    phase = (*pm).value.GetInt();
                                }
                                //register alpha
                                s->Register(s->lmax, phase);

                            }

                        }
                    }
                }


            } else if (smodel == std::string("schnute_case_IV")) {
                model = std::make_shared<mas::SchnuteCaseIV<REAL_T> >();
                mas::SchnuteCaseIV<REAL_T>* s = (mas::SchnuteCaseIV<REAL_T>*)model.get();

                std::stringstream ss;
                ss << "schnute_case_IV_alpha_" << model_id;
                VariableTrait<REAL_T>::SetName(s->alpha, ss.str());

                ss.str("");
                ss << "schnute_case_IV_beta_" << model_id;
                VariableTrait<REAL_T>::SetName(s->beta, ss.str());

                ss.str("");
                ss << "schnute_case_IV_lmin_" << model_id;
                VariableTrait<REAL_T>::SetName(s->lmin, ss.str());

                ss.str("");
                ss << "schnute_case_IV_lmax_" << model_id;
                VariableTrait<REAL_T>::SetName(s->lmax, ss.str());

                if (pit == (*growth_model).value.MemberEnd()) {
                    std::cout << "Configuration Error: Recruitment model \"Schnute Case IV\" has no parameter definitions.\n";
                    mas::mas_log << "Configuration Error: Recruitment model \"Schnute Case IV\" has no parameter definitions.\n";
                    this->valid_configuration = false;
                    ;
                } else {
                    rapidjson::Document::MemberIterator ppit = (*pit).value.FindMember("alpha");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Growth model \"Schnute Case IV\" " <<
                                "does not define \"alpha\". Model will use the default value of 0 and \"alpha\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Recruitment model \"Schnute Case IV\" " <<
                                "does not define \"alpha\". Model will use the default value of 0 and \"alpha\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Schnute Case IV\" does not provide a initial value for \"alpha\".\n";
                            mas::mas_log << "Configuration Warning: Growth model \"Schnute Case IV\" does not provide a initial value for \"alpha\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(s->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(s->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(s->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register alpha
                            s->Register(s->alpha, phase);

                        }

                        ppit = (*pit).value.FindMember("beta");
                        if (ppit == (*pit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Schnute Case IV\" " <<
                                    "does not define \"beta\". Model will use the default value of 0 and \"beta\" will not be estimated.\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Schnute Case IV\" " <<
                                    "does not define \"beta\". Model will use the default value of 0 and \"beta\" will not be estimated.\n";
                        } else {

                            bool estimated = false;
                            int phase = 1;
                            //1. Get initial value if there is one.
                            rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                            if (pm == (*ppit).value.MemberEnd()) {
                                std::cout << "Configuration Warning: Growth model \"Schnute Case IV\" does not provide a initial value for \"beta\".\n";
                                mas::mas_log << "Configuration Warning: Growth model \"Schnute Case IV\" does not provide a initial value for \"beta\".\n";
                            } else {
                                VariableTrait<REAL_T>::SetValue(s->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //2. Get min boundary if there is one.
                            pm = (*ppit).value.FindMember("min");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMinBoundary(s->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //3. Get max boundary if there is one.
                            pm = (*ppit).value.FindMember("max");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMaxBoundary(s->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            pm = (*ppit).value.FindMember("estimated");
                            if (pm != (*ppit).value.MemberEnd()) {
                                std::string e = std::string((*pm).value.GetString());
                                if (e == "true") {
                                    estimated = true;
                                }
                            }

                            if (estimated) {
                                phase = 1;
                                pm = (*ppit).value.FindMember("phase");
                                if (pm != (*ppit).value.MemberEnd()) {
                                    phase = (*pm).value.GetInt();
                                }
                                //register alpha
                                s->Register(s->beta, phase);

                            }

                        }

                        ppit = (*pit).value.FindMember("lmin");
                        if (ppit == (*pit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Schnute Case IV\" " <<
                                    "does not define \"lmin\". Model will use the default value of 0 and \"lmin\" will not be estimated.\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Schnute Case IV\" " <<
                                    "does not define \"lmin\". Model will use the default value of 0 and \"lmin\" will not be estimated.\n";
                        } else {

                            bool estimated = false;
                            int phase = 1;
                            //1. Get initial value if there is one.
                            rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                            if (pm == (*ppit).value.MemberEnd()) {
                                std::cout << "Configuration Warning: Growth model \"Schnute Case IV\" does not provide a initial value for \"lmin\".\n";
                                mas::mas_log << "Configuration Warning: Growth model \"Schnute Case IV\" does not provide a initial value for \"lmin\".\n";
                            } else {
                                VariableTrait<REAL_T>::SetValue(s->lmin, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //2. Get min boundary if there is one.
                            pm = (*ppit).value.FindMember("min");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMinBoundary(s->lmin, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //3. Get max boundary if there is one.
                            pm = (*ppit).value.FindMember("max");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMaxBoundary(s->lmin, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            pm = (*ppit).value.FindMember("estimated");
                            if (pm != (*ppit).value.MemberEnd()) {
                                std::string e = std::string((*pm).value.GetString());
                                if (e == "true") {
                                    estimated = true;
                                }
                            }

                            if (estimated) {
                                phase = 1;
                                pm = (*ppit).value.FindMember("phase");
                                if (pm != (*ppit).value.MemberEnd()) {
                                    phase = (*pm).value.GetInt();
                                }
                                //register alpha
                                s->Register(s->lmin, phase);

                            }

                        }

                        ppit = (*pit).value.FindMember("lmax");
                        if (ppit == (*pit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Growth model \"Schnute Case IV\" " <<
                                    "does not define \"lmax\". Model will use the default value of 0 and \"lmax\" will not be estimated.\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Schnute Case IV\" " <<
                                    "does not define \"lmax\". Model will use the default value of 0 and \"lmax\" will not be estimated.\n";
                        } else {

                            bool estimated = false;
                            int phase = 1;
                            //1. Get initial value if there is one.
                            rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                            if (pm == (*ppit).value.MemberEnd()) {
                                std::cout << "Configuration Warning: Growth model \"Schnute Case IV\" does not provide a initial value for \"lmax\".\n";
                                mas::mas_log << "Configuration Warning: Growth model \"Schnute Case IV\" does not provide a initial value for \"lmax\".\n";
                            } else {
                                VariableTrait<REAL_T>::SetValue(s->lmax, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //2. Get min boundary if there is one.
                            pm = (*ppit).value.FindMember("min");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMinBoundary(s->lmax, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            //3. Get max boundary if there is one.
                            pm = (*ppit).value.FindMember("max");
                            if (pm != (*ppit).value.MemberEnd()) {
                                VariableTrait<REAL_T>::SetMaxBoundary(s->lmax, static_cast<REAL_T> ((*pm).value.GetDouble()));
                            }

                            pm = (*ppit).value.FindMember("estimated");
                            if (pm != (*ppit).value.MemberEnd()) {
                                std::string e = std::string((*pm).value.GetString());
                                if (e == "true") {
                                    estimated = true;
                                }
                            }

                            if (estimated) {
                                phase = 1;
                                pm = (*ppit).value.FindMember("phase");
                                if (pm != (*ppit).value.MemberEnd()) {
                                    phase = (*pm).value.GetInt();
                                }
                                //register alpha
                                s->Register(s->lmax, phase);

                            }

                        }
                    }
                }

            }

            rapidjson::Document::MemberIterator amin_it = (*pit).value.FindMember("amin");

            REAL_T amin = static_cast<REAL_T> (0.0);
            REAL_T amax = static_cast<REAL_T> (0.0);

            if (amin_it == (*pit).value.MemberEnd()) {
                std::cout << "Configuration Error: Growth is required to have a amin specified.\n";
                mas::mas_log << "Configuration Error: Growth is required to have a amin specified.\n";
                this->valid_configuration = false;
                ;
            } else {
                bool estimated = false;
                int phase = 1;
                rapidjson::Document::MemberIterator pm = (*amin_it).value.FindMember("value");

                if (pm == (*amin_it).value.MemberEnd()) {
                    std::cout << "Configuration Warning: Growth model does not provide a initial value for \"amin\".\n";
                    mas::mas_log << "Configuration Warning: Growth model  does not provide a initial value for \"amin\".\n";
                } else {
                    VariableTrait<REAL_T>::SetValue(model->a_min, static_cast<REAL_T> ((*pm).value.GetDouble()));
                }

                //2. Get min boundary if there is one.
                pm = (*amin_it).value.FindMember("min");
                if (pm != (*amin_it).value.MemberEnd()) {
                    VariableTrait<REAL_T>::SetMinBoundary(model->a_min, static_cast<REAL_T> ((*pm).value.GetDouble()));
                }

                //3. Get max boundary if there is one.
                pm = (*amin_it).value.FindMember("max");
                if (pm != (*amin_it).value.MemberEnd()) {
                    VariableTrait<REAL_T>::SetMaxBoundary(model->a_min, static_cast<REAL_T> ((*pm).value.GetDouble()));
                }

                pm = (*amin_it).value.FindMember("estimated");
                if (pm != (*amin_it).value.MemberEnd()) {
                    std::string e = std::string((*pm).value.GetString());
                    if (e == "true") {
                        estimated = true;
                    }
                }

                if (estimated) {
                    phase = 1;
                    pm = (*amin_it).value.FindMember("phase");
                    if (pm != (*amin_it).value.MemberEnd()) {
                        phase = (*pm).value.GetInt();
                    }
                    //register alpha
                    model->Register(model->a_min, phase);

                }
            }

            rapidjson::Document::MemberIterator amax_it = (*pit).value.FindMember("amax");

            if (amax_it == (*pit).value.MemberEnd()) {
                std::cout << "Configuration Error: Growth is required to have a amax specified.\n";
                mas::mas_log << "Configuration Error: Growth is required to have a amax specified.\n";
                this->valid_configuration = false;
                ;
            } else {
                bool estimated = false;
                int phase = 1;
                rapidjson::Document::MemberIterator pm = (*amax_it).value.FindMember("value");

                if (pm == (*amax_it).value.MemberEnd()) {
                    std::cout << "Configuration Warning: Growth model  does not provide a initial value for \"amax\".\n";
                    mas::mas_log << "Configuration Warning: Growth model  does not provide a initial value for \"amax\".\n";
                } else {
                    VariableTrait<REAL_T>::SetValue(model->a_max, static_cast<REAL_T> ((*pm).value.GetDouble()));
                }

                //2. Get min boundary if there is one.
                pm = (*amax_it).value.FindMember("min");
                if (pm != (*amax_it).value.MemberEnd()) {
                    VariableTrait<REAL_T>::SetMinBoundary(model->a_max, static_cast<REAL_T> ((*pm).value.GetDouble()));
                }

                //3. Get max boundary if there is one.
                pm = (*amax_it).value.FindMember("max");
                if (pm != (*amax_it).value.MemberEnd()) {
                    VariableTrait<REAL_T>::SetMaxBoundary(model->a_max, static_cast<REAL_T> ((*pm).value.GetDouble()));
                }

                pm = (*amax_it).value.FindMember("estimated");
                if (pm != (*amax_it).value.MemberEnd()) {
                    std::string e = std::string((*pm).value.GetString());
                    if (e == "true") {
                        estimated = true;
                    }
                }

                if (estimated) {
                    phase = 1;
                    pm = (*amax_it).value.FindMember("phase");
                    if (pm != (*amax_it).value.MemberEnd()) {
                        phase = (*pm).value.GetInt();
                    }
                    //register alpha
                    model->Register(model->a_max, phase);

                }


            }


            model->id = model_id;
            growth_model_iterator it = this->growth_models.find(model->id);
            if (it != this->growth_models.end()) {
                std::cout << "Configuration Error: More than one growth model with the same identifier defined. Growth models require a unique id.\n";
                mas::mas_log << "Configuration Error: More than one growth model with the same identifier defined. Growth models require a unique id.\n";

                this->valid_configuration = false;

            } else {
                this->growth_models[model->id] = model;
            }


        }

        void HandleRecruitmentModel(rapidjson::Document::MemberIterator & recruitment_model) {

            rapidjson::Document::MemberIterator rit;
            rit = (*recruitment_model).value.FindMember("model");

            if (rit == (*recruitment_model).value.MemberEnd()) {
                std::cout << "Configuration Error: Recruitment is required to have a model specified.\n";
                mas::mas_log << "Configuration Error: Recruitment is required to have a model specified.\n";
                this->valid_configuration = false;

            }

            std::shared_ptr<mas::RecruitmentBase<REAL_T> > model(NULL);
            std::string smodel = std::string((*rit).value.GetString());

            rit = (*recruitment_model).value.FindMember("id");
            int model_id = 0;
            if (rit == (*recruitment_model).value.MemberEnd()) {
                std::cout << "Configuration Error: Recruitment is required to have a unique identifier\n";
                mas::mas_log << "Configuration Error: Recruitment is required to have a unique identifier\n";
                this->valid_configuration = false;
            } else {
                model_id = (*rit).value.GetInt();
            }

            rapidjson::Document::MemberIterator pit = (*recruitment_model).value.FindMember("parameters");


            if (smodel == std::string("beverton_holt")) {
                model = std::make_shared<mas::BevertonHolt<REAL_T> >();
                mas::BevertonHolt<REAL_T>* bh = (mas::BevertonHolt<REAL_T>*)model.get();
                std::stringstream ss;
                ss << "beverton_holt_alpha_" << model_id;
                VariableTrait<REAL_T>::SetName(bh->alpha, ss.str());
                ss.str("");
                ss << "beverton_holt_beta_" << model_id;
                VariableTrait<REAL_T>::SetName(bh->beta, ss.str());



                if (pit == (*recruitment_model).value.MemberEnd()) {
                    std::cout << "Configuration Error: Recruitment model \"Beverton-Holt\" has no parameter definitions.\n";
                    mas::mas_log << "Configuration Error: Recruitment model \"Beverton-Holt\" has no parameter definitions.\n";
                    this->valid_configuration = false;

                } else {
                    rapidjson::Document::MemberIterator ppit = (*pit).value.FindMember("alpha");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Recruitment model \"Beverton-Holt\" " <<
                                "does not define \"alpha\". Model will use the default value of 0 and \"alpha\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Recruitment model \"Beverton-Holt\" " <<
                                "does not define \"alpha\". Model will use the default value of 0 and \"alpha\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Recruitment model \"Beverton-Holt\" does not provide a initial value for \"alpha\".\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Beverton-Holt\" does not provide a initial value for \"alpha\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(bh->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(bh->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(bh->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register alpha
                            bh->Register(bh->alpha, phase);

                        }

                    }

                    ppit = (*pit).value.FindMember("beta");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Recruitment model \"Beverton-Holt\" " <<
                                "does not define \"alpha\". Model will use the default value of 0 and \"alpha\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Recruitment model \"Beverton-Holt\" " <<
                                "does not define \"alpha\". Model will use the default value of 0 and \"alpha\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Recruitment model \"Beverton-Holt\" does not provide a initial value for \"alpha\".\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Beverton-Holt\" does not provide a initial value for \"alpha\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(bh->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(bh->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(bh->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register beta
                            bh->Register(bh->beta, phase);

                        }

                    }
                }

            } else if (smodel == std::string("beverton_holt_alt")) {
                model = std::make_shared<mas::BevertonHoltAlt<REAL_T> >();
                mas::BevertonHoltAlt<REAL_T>* bh = (mas::BevertonHoltAlt<REAL_T>*)model.get();
                std::stringstream ss;
                ss << "beverton_holt_alt_R0_" << model_id;
                VariableTrait<REAL_T>::SetName(bh->R0, ss.str());
                ss.str("");
                ss << "beverton_holt_alt_phi0_" << model_id;
                VariableTrait<REAL_T>::SetName(bh->phi0, ss.str());

                ss.str("");
                ss << "beverton_holt_alt_h_" << model_id;
                VariableTrait<REAL_T>::SetName(bh->h, ss.str());


                if (pit == (*recruitment_model).value.MemberEnd()) {
                    std::cout << "Configuration Error: Recruitment model \"Beverton-Holt\" has no parameter definitions.\n";
                    mas::mas_log << "Configuration Error: Recruitment model \"Beverton-Holt\" has no parameter definitions.\n";
                    this->valid_configuration = false;
                    ;
                } else {
                    rapidjson::Document::MemberIterator ppit = (*pit).value.FindMember("R0");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Recruitment model \"Beverton-Holt Alt\" " <<
                                "does not define \"Ro\". Model will use the default value of 0 and \"Ro\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Recruitment model \"Beverton-Holt\" " <<
                                "does not define \"Ro\". Model will use the default value of 0 and \"Ro\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Recruitment model \"Beverton-Holt Alt\" does not provide a initial value for \"Ro\".\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Beverton-Holt Alt\" does not provide a initial value for \"Ro\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(bh->R0, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(bh->R0, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(bh->R0, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register R0
                            bh->Register(bh->R0, phase);

                        }

                    }

                    ppit = (*pit).value.FindMember("phi0");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Recruitment model \"Beverton-Holt Alt\" " <<
                                "does not define \"phi0\". Model will use the default value of 0 and \"phi0\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Recruitment model \"Beverton-Holt\" " <<
                                "does not define \"phi0\". Model will use the default value of 0 and \"phi0\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Recruitment model \"Beverton-Holt Alt\" does not provide a initial value for \"phi0\".\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Beverton-Holt Alt\" does not provide a initial value for \"phi0\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(bh->phi0, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(bh->phi0, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(bh->phi0, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register phi0
                            bh->Register(bh->phi0, phase);

                        }

                    }

                    ppit = (*pit).value.FindMember("h");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Recruitment model \"Beverton-Holt Alt\" " <<
                                "does not define \"h\". Model will use the default value of 0 and \"h\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Recruitment model \"Beverton-Holt\" " <<
                                "does not define \"h\". Model will use the default value of 0 and \"h\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Recruitment model \"Beverton-Holt Alt\" does not provide a initial value for \"h\".\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Beverton-Holt Alt\" does not provide a initial value for \"h\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(bh->h, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(bh->h, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(bh->h, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register h
                            bh->Register(bh->h, phase);

                        }

                    }
                }
            } else if (smodel == std::string("beverton_holt_dep")) {
                model = std::make_shared<mas::BevertonHoltDep<REAL_T> >();
                mas::BevertonHoltDep<REAL_T>* bh = (mas::BevertonHoltDep<REAL_T>*)model.get();
                std::stringstream ss;
                ss << "beverton_holt_dep_alpha_" << model_id;
                VariableTrait<REAL_T>::SetName(bh->alpha, ss.str());
                ss.str("");
                ss << "beverton_holt_dep_beta_" << model_id;
                VariableTrait<REAL_T>::SetName(bh->beta, ss.str());
                ss.str("");
                ss << "beverton_holt_dep_c_" << model_id;
                VariableTrait<REAL_T>::SetName(bh->c, ss.str());


                if (pit == (*recruitment_model).value.MemberEnd()) {
                    std::cout << "Configuration Error: Recruitment model \"Beverton-Holt Dep\" has no parameter definitions.\n";
                    mas::mas_log << "Configuration Error: Recruitment model \"Beverton-Holt Dep\" has no parameter definitions.\n";
                    this->valid_configuration = false;
                    ;
                } else {
                    rapidjson::Document::MemberIterator ppit = (*pit).value.FindMember("alpha");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Recruitment model \"Beverton-Holt Dep\" " <<
                                "does not define \"alpha\". Model will use the default value of 0 and \"alpha\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Recruitment model \"Beverton-Holt Dep\" " <<
                                "does not define \"alpha\". Model will use the default value of 0 and \"alpha\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Recruitment model \"Beverton-Holt\" does not provide a initial value for \"alpha\".\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Beverton-Holt\" does not provide a initial value for \"alpha\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(bh->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(bh->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(bh->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register alpha
                            bh->Register(bh->alpha, phase);

                        }

                    }

                    ppit = (*pit).value.FindMember("beta");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Recruitment model \"Beverton-Holt Dep\" " <<
                                "does not define \"alpha\". Model will use the default value of 0 and \"alpha\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Recruitment model \"Beverton-Holt Dep\" " <<
                                "does not define \"alpha\". Model will use the default value of 0 and \"alpha\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Recruitment model \"Beverton-Holt Dep\" does not provide a initial value for \"alpha\".\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Beverton-Holt Dep\" does not provide a initial value for \"alpha\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(bh->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(bh->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(bh->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register beta
                            bh->Register(bh->beta, phase);

                        }

                    }

                    ppit = (*pit).value.FindMember("c");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Recruitment model \"Beverton-Holt Dep\" " <<
                                "does not define \"c\". Model will use the default value of 0 and \"c\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Recruitment model \"Beverton-Holt Dep\" " <<
                                "does not define \"c\". Model will use the default value of 0 and \"c\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Recruitment model \"Beverton-Holt Dep\" does not provide a initial value for \"c\".\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Beverton-Holt Dep\" does not provide a initial value for \"c\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(bh->c, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(bh->c, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(bh->c, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register c
                            bh->Register(bh->c, phase);

                        }

                    }
                }

            } else if (smodel == std::string("ricker")) {
                model = std::make_shared<mas::Ricker<REAL_T> >();
                mas::Ricker<REAL_T>* r = (mas::Ricker<REAL_T>*)model.get();
                std::stringstream ss;
                ss << "ricker_alpha_" << model_id;
                VariableTrait<REAL_T>::SetName(r->alpha, ss.str());
                ss.str("");
                ss << "ricker_beta_" << model_id;
                VariableTrait<REAL_T>::SetName(r->beta, ss.str());


                if (pit == (*recruitment_model).value.MemberEnd()) {
                    std::cout << "Configuration Error: Recruitment model \"Ricker\" has no parameter definitions.\n";
                    mas::mas_log << "Configuration Error: Recruitment model \"Ricker\" has no parameter definitions.\n";
                    this->valid_configuration = false;
                    ;
                } else {
                    rapidjson::Document::MemberIterator ppit = (*pit).value.FindMember("alpha");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Recruitment model \"Ricker\" " <<
                                "does not define \"alpha\". Model will use the default value of 0 and \"alpha\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Recruitment model \"Ricker\" " <<
                                "does not define \"alpha\". Model will use the default value of 0 and \"alpha\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Recruitment model \"Ricker\" does not provide a initial value for \"alpha\".\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Ricker\" does not provide a initial value for \"alpha\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(r->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(r->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(r->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register alpha
                            r->Register(r->alpha, phase);

                        }

                    }

                    ppit = (*pit).value.FindMember("beta");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Recruitment model \"Ricker\" " <<
                                "does not define \"beta\". Model will use the default value of 0 and \"beta\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Recruitment model \"Ricker\" " <<
                                "does not define \"beta\". Model will use the default value of 0 and \"beta\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Recruitment model \"Ricker\" does not provide a initial value for \"beta\".\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Ricker\" does not provide a initial value for \"beta\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(r->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(r->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(r->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register beta
                            r->Register(r->beta, phase);

                        }

                    }
                }
            } else if (smodel == std::string("ricker_alt")) {
                model = std::make_shared<mas::RickerAlt<REAL_T> >();
                mas::RickerAlt<REAL_T>* ra = (mas::RickerAlt<REAL_T>*)model.get();
                std::stringstream ss;
                ss << "ricker_alt_R0_" << model_id;
                VariableTrait<REAL_T>::SetName(ra->R0, ss.str());
                ss.str("");
                ss << "ricker_alt_phi0_" << model_id;
                VariableTrait<REAL_T>::SetName(ra->phi0, ss.str());

                ss.str("");
                ss << "ricker_alt_A_" << model_id;
                VariableTrait<REAL_T>::SetName(ra->A, ss.str());


                if (pit == (*recruitment_model).value.MemberEnd()) {
                    std::cout << "Configuration Error: Recruitment model \"Ricker Alt\" has no parameter definitions.\n";
                    mas::mas_log << "Configuration Error: Recruitment model \"Ricker Alt\" has no parameter definitions.\n";
                    this->valid_configuration = false;
                    ;
                } else {
                    rapidjson::Document::MemberIterator ppit = (*pit).value.FindMember("R0");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Recruitment model \"Ricker Alt\" " <<
                                "does not define \"Ro\". Model will use the default value of 0 and \"Ro\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Recruitment model \"Ricker Alt\" " <<
                                "does not define \"Ro\". Model will use the default value of 0 and \"Ro\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Recruitment model \"Ricker Alt\" does not provide a initial value for \"Ro\".\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Ricker Alt\" does not provide a initial value for \"Ro\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(ra->R0, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(ra->R0, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(ra->R0, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register R0
                            ra->Register(ra->R0, phase);

                        }

                    }

                    ppit = (*pit).value.FindMember("phi0");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Recruitment model \"Ricker Alt\" " <<
                                "does not define \"phi0\". Model will use the default value of 0 and \"phi0\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Recruitment model \"Ricker Alt\" " <<
                                "does not define \"phi0\". Model will use the default value of 0 and \"phi0\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Recruitment model \"Ricker Alt\" does not provide a initial value for \"phi0\".\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Ricker Alt\" does not provide a initial value for \"phi0\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(ra->phi0, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(ra->phi0, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(ra->phi0, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register phi0
                            ra->Register(ra->phi0, phase);

                        }

                    }

                    ppit = (*pit).value.FindMember("A");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Recruitment model \"Ricker Alt\" " <<
                                "does not define \"A\". Model will use the default value of 0 and \"A\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Recruitment model \"Ricker Alt\" " <<
                                "does not define \"A\". Model will use the default value of 0 and \"A\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Recruitment model \"Ricker Alt\" does not provide a initial value for \"A\".\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Ricker Alt\" does not provide a initial value for \"A\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(ra->A, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(ra->A, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(ra->A, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register A
                            ra->Register(ra->A, phase);

                        }
                    }
                }
            } else if (smodel == std::string("shepard")) {
                model = std::make_shared<mas::Shepherd<REAL_T> >();
                mas::Shepherd<REAL_T>* s = (mas::Shepherd<REAL_T>*)model.get();
                std::stringstream ss;
                ss << "shepard_alpha_" << model_id;
                VariableTrait<REAL_T>::SetName(s->alpha, ss.str());
                ss.str("");
                ss << "shepard_beta_" << model_id;
                VariableTrait<REAL_T>::SetName(s->beta, ss.str());
                ss.str("");
                ss << "shepard_c_" << model_id;
                VariableTrait<REAL_T>::SetName(s->c, ss.str());


                if (pit == (*recruitment_model).value.MemberEnd()) {
                    std::cout << "Configuration Error: Recruitment model \"Shepherd\" has no parameter definitions.\n";
                    mas::mas_log << "Configuration Error: Recruitment model \"Shepherd\" has no parameter definitions.\n";
                    this->valid_configuration = false;
                    ;
                } else {
                    rapidjson::Document::MemberIterator ppit = (*pit).value.FindMember("alpha");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Recruitment model \"Shepherd\" " <<
                                "does not define \"alpha\". Model will use the default value of 0 and \"alpha\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Recruitment model \"Shepherd\" " <<
                                "does not define \"alpha\". Model will use the default value of 0 and \"alpha\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Recruitment model \"Shepherd\" does not provide a initial value for \"alpha\".\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Shepherd\" does not provide a initial value for \"alpha\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(s->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(s->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(s->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register alpha
                            s->Register(s->alpha, phase);

                        }

                    }

                    ppit = (*pit).value.FindMember("beta");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Recruitment model \"Shepherd\" " <<
                                "does not define \"alpha\". Model will use the default value of 0 and \"alpha\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Recruitment model \"Shepherd\" " <<
                                "does not define \"alpha\". Model will use the default value of 0 and \"alpha\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Recruitment model \"Shepherd\" does not provide a initial value for \"alpha\".\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Shepherd\" does not provide a initial value for \"alpha\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(s->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(s->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(s->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register beta
                            s->Register(s->beta, phase);

                        }

                    }

                    ppit = (*pit).value.FindMember("c");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Recruitment model \"Shepherd\" " <<
                                "does not define \"c\". Model will use the default value of 0 and \"c\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Recruitment model \"Shepherd\" " <<
                                "does not define \"c\". Model will use the default value of 0 and \"c\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Recruitment model \"Shepherd\" does not provide a initial value for \"c\".\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Shepherd\" does not provide a initial value for \"c\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(s->c, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(s->c, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(s->c, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register c
                            s->Register(s->c, phase);

                        }

                    }
                }
            } else if (smodel == std::string("deriso")) {
                model = std::make_shared<mas::Deriso<REAL_T> >();
                mas::Deriso<REAL_T>* d = (mas::Deriso<REAL_T>*)model.get();
                std::stringstream ss;
                ss << "deriso_alpha_" << model_id;
                VariableTrait<REAL_T>::SetName(d->alpha, ss.str());
                ss.str("");
                ss << "deriso_beta_" << model_id;
                VariableTrait<REAL_T>::SetName(d->beta, ss.str());
                ss.str("");
                ss << "deriso_c_" << model_id;
                VariableTrait<REAL_T>::SetName(d->c, ss.str());


                if (pit == (*recruitment_model).value.MemberEnd()) {
                    std::cout << "Configuration Error: Recruitment model \"Deriso\" has no parameter definitions.\n";
                    mas::mas_log << "Configuration Error: Recruitment model \"Deriso\" has no parameter definitions.\n";
                    this->valid_configuration = false;
                    ;
                } else {
                    rapidjson::Document::MemberIterator ppit = (*pit).value.FindMember("alpha");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Recruitment model \"Deriso\" " <<
                                "does not define \"alpha\". Model will use the default value of 0 and \"alpha\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Recruitment model \"Deriso\" " <<
                                "does not define \"alpha\". Model will use the default value of 0 and \"alpha\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Recruitment model \"Deriso\" does not provide a initial value for \"alpha\".\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Deriso\" does not provide a initial value for \"alpha\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(d->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(d->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(d->alpha, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register alpha
                            d->Register(d->alpha, phase);

                        }

                    }

                    ppit = (*pit).value.FindMember("beta");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Recruitment model \"Deriso\" " <<
                                "does not define \"alpha\". Model will use the default value of 0 and \"alpha\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Recruitment model \"Deriso\" " <<
                                "does not define \"alpha\". Model will use the default value of 0 and \"alpha\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Recruitment model \"Deriso\" does not provide a initial value for \"alpha\".\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Deriso\" does not provide a initial value for \"alpha\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(d->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(d->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(d->beta, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register beta
                            d->Register(d->beta, phase);

                        }

                    }

                    ppit = (*pit).value.FindMember("c");
                    if (ppit == (*pit).value.MemberEnd()) {
                        std::cout << "Configuration Warning: Recruitment model \"Deriso\" " <<
                                "does not define \"c\". Model will use the default value of 0 and \"c\" will not be estimated.\n";
                        mas::mas_log << "Configuration Warning: Recruitment model \"Deriso\" " <<
                                "does not define \"c\". Model will use the default value of 0 and \"c\" will not be estimated.\n";
                    } else {

                        bool estimated = false;
                        int phase = 1;
                        //1. Get initial value if there is one.
                        rapidjson::Document::MemberIterator pm = (*ppit).value.FindMember("value");

                        if (pm == (*ppit).value.MemberEnd()) {
                            std::cout << "Configuration Warning: Recruitment model \"Deriso\" does not provide a initial value for \"c\".\n";
                            mas::mas_log << "Configuration Warning: Recruitment model \"Deriso\" does not provide a initial value for \"c\".\n";
                        } else {
                            VariableTrait<REAL_T>::SetValue(d->c, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //2. Get min boundary if there is one.
                        pm = (*ppit).value.FindMember("min");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMinBoundary(d->c, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        //3. Get max boundary if there is one.
                        pm = (*ppit).value.FindMember("max");
                        if (pm != (*ppit).value.MemberEnd()) {
                            VariableTrait<REAL_T>::SetMaxBoundary(d->c, static_cast<REAL_T> ((*pm).value.GetDouble()));
                        }

                        pm = (*ppit).value.FindMember("estimated");
                        if (pm != (*ppit).value.MemberEnd()) {
                            std::string e = std::string((*pm).value.GetString());
                            if (e == "true") {
                                estimated = true;
                            }
                        }

                        if (estimated) {
                            phase = 1;
                            pm = (*ppit).value.FindMember("phase");
                            if (pm != (*ppit).value.MemberEnd()) {
                                phase = (*pm).value.GetInt();
                            }
                            //register c
                            d->Register(d->c, phase);

                        }

                    }
                }
            }


            if (pit == (*recruitment_model).value.MemberEnd()) {
                std::cout << "Configuration Error: Recruitment model \"" << model_id << "\" has no parameter definitions.\n";
                mas_log << "Configuration Error: Recruitment model \"" << model_id << "\" has no parameter definitions.\n";
                this->valid_configuration = false;
                return;
            } else {


                rapidjson::Document::MemberIterator sr_it = (*pit).value.FindMember("sigma_r");
                if (sr_it != (*pit).value.MemberEnd()) {

                    rapidjson::Document::MemberIterator srv_it = (*sr_it).value.FindMember("value");
                    if (srv_it != (*sr_it).value.MemberEnd()) {
                        mas::VariableTrait<REAL_T>::SetValue(model->sigma_r, static_cast<REAL_T> ((*srv_it).value.GetDouble()));
                    }
                    std::string estimated = "false";
                    rapidjson::Document::MemberIterator sre_it = (*sr_it).value.FindMember("estimated");
                    if (sre_it != (*sr_it).value.MemberEnd()) {
                        estimated = std::string((*sre_it).value.GetString());
                        if (estimated == "true") {
                            REAL_T min = std::numeric_limits<REAL_T>::min();
                            REAL_T max = std::numeric_limits<REAL_T>::max();
                            int phase = 1;

                            rapidjson::Document::MemberIterator phase_it = (*sr_it).value.FindMember("phase");
                            if (phase_it != (*sr_it).value.MemberEnd()) {
                                phase = (*phase_it).value.GetInt();
                            }

                            rapidjson::Document::MemberIterator min_it = (*sr_it).value.FindMember("min");
                            if (min_it != (*sr_it).value.MemberEnd()) {
                                min = static_cast<REAL_T> ((*min_it).value.GetDouble());
                                mas::VariableTrait<REAL_T>::SetMinBoundary(model->sigma_r, static_cast<REAL_T> ((*min_it).value.GetDouble()));
                            }

                            rapidjson::Document::MemberIterator max_it = (*sr_it).value.FindMember("max");
                            if (max_it != (*sr_it).value.MemberEnd()) {
                                max = static_cast<REAL_T> ((*min_it).value.GetDouble());
                                mas::VariableTrait<REAL_T>::SetMaxBoundary(model->sigma_r, static_cast<REAL_T> ((*max_it).value.GetDouble()));
                            }

                            std::stringstream ss;
                            ss << "sigma_r_" << model->id;
                            mas::VariableTrait<REAL_T>::SetName(model->sigma_r, ss.str());

                            model->Register(model->sigma_r, phase);
                        }
                    }


                } else {
                    std::cout << "Configuration Error: Recruitment model \"" << model_id << "\" has no sigma_r definition.\n";
                    mas_log << "Configuration Error: Recruitment model \"" << model_id << "\" has no sigma_r definition.\n";
                    this->valid_configuration = false;
                }


                rapidjson::Document::MemberIterator rho_it = (*pit).value.FindMember("rho");
                if (rho_it != (*pit).value.MemberEnd()) {

                    rapidjson::Document::MemberIterator rhov_it = (*rho_it).value.FindMember("value");
                    if (rhov_it != (*rho_it).value.MemberEnd()) {
                        mas::VariableTrait<REAL_T>::SetValue(model->rho, static_cast<REAL_T> ((*rhov_it).value.GetDouble()));
                    }
                    std::string estimated = "false";
                    rapidjson::Document::MemberIterator sre_it = (*rho_it).value.FindMember("estimated");
                    if (sre_it != (*rho_it).value.MemberEnd()) {
                        estimated = std::string((*sre_it).value.GetString());
                        if (estimated == "true") {
                            REAL_T min = std::numeric_limits<REAL_T>::min();
                            REAL_T max = std::numeric_limits<REAL_T>::max();
                            int phase = 1;

                            rapidjson::Document::MemberIterator phase_it = (*rho_it).value.FindMember("phase");
                            if (phase_it != (*rho_it).value.MemberEnd()) {
                                phase = (*phase_it).value.GetInt();
                            }

                            rapidjson::Document::MemberIterator min_it = (*rho_it).value.FindMember("min");
                            if (min_it != (*rho_it).value.MemberEnd()) {
                                min = static_cast<REAL_T> ((*min_it).value.GetDouble());
                                mas::VariableTrait<REAL_T>::SetMinBoundary(model->rho, static_cast<REAL_T> ((*min_it).value.GetDouble()));
                            }

                            rapidjson::Document::MemberIterator max_it = (*rho_it).value.FindMember("max");
                            if (max_it != (*rho_it).value.MemberEnd()) {
                                max = static_cast<REAL_T> ((*min_it).value.GetDouble());
                                mas::VariableTrait<REAL_T>::SetMaxBoundary(model->rho, static_cast<REAL_T> ((*max_it).value.GetDouble()));
                            }

                            std::stringstream ss;
                            ss << "rho_" << model->id;
                            mas::VariableTrait<REAL_T>::SetName(model->rho, ss.str());

                            model->Register(model->rho, phase);
                        }
                    }


                }




                rapidjson::Document::MemberIterator rdevs_it = (*pit).value.FindMember("recruitment_deviations");

                if (rdevs_it == (*pit).value.MemberEnd()) {
                    std::cout << "Configuration Error: Recruitment model \"" << model_id << "\" has no deviation definition.\n";
                    mas_log << "Configuration Error: Recruitment model \"" << model_id << "\" has no deviation definition.\n";
                    this->valid_configuration = false;
                } else {
                    rapidjson::Document::MemberIterator vit = (*rdevs_it).value.FindMember("values");

                    if (!(*vit).value.IsArray()) {
                        std::cout << "Configuration Error: Recruitment model \"" << model_id << "\" deviations must be a vector.\n";
                        mas_log << "Configuration Error: Recruitment model \"" << model_id << "\" deviations must be a vector.\n";
                        this->valid_configuration = false;
                    } else {
                        rapidjson::Value& v = (*vit).value;
                        model->recruitment_deviations.resize(v.Size());
                        for (int i = 0; i < v.Size(); i++) {
                            mas::VariableTrait<REAL_T>::SetValue(model->recruitment_deviations[i], static_cast<REAL_T> (v[i].GetDouble()));
                        }

                    }

                    rapidjson::Document::MemberIterator e_it = (*rdevs_it).value.FindMember("estimated");

                    if (e_it != (*rdevs_it).value.MemberEnd()) {


                        if (std::string((*e_it).value.GetString()) == "true") {
                            int phase = 1;
                            rapidjson::Document::MemberIterator p_it = (*rdevs_it).value.FindMember("phase");

                            if (p_it != (*rdevs_it).value.MemberEnd()) {
                                phase = (*p_it).value.GetInt();
                            }
                            bool bounded = false;
                            REAL_T min = std::numeric_limits<REAL_T>::min();
                            rapidjson::Document::MemberIterator min_it = (*rdevs_it).value.FindMember("min");

                            if (min_it != (*rdevs_it).value.MemberEnd()) {
                                bounded = true;
                                min = static_cast<REAL_T> ((*min_it).value.GetDouble());
                            }

                            REAL_T max = std::numeric_limits<REAL_T>::max();
                            rapidjson::Document::MemberIterator max_it = (*rdevs_it).value.FindMember("max");

                            if (max_it != (*rdevs_it).value.MemberEnd()) {
                                bounded = true;
                                max = static_cast<REAL_T> ((*max_it).value.GetDouble());
                            }

                            if (bounded) {
                                for (int i = 0; i < model->recruitment_deviations.size(); i++) {
                                    mas::VariableTrait<REAL_T>::SetMinBoundary(model->recruitment_deviations[i], min);
                                    mas::VariableTrait<REAL_T>::SetMaxBoundary(model->recruitment_deviations[i], max);
                                    std::stringstream ss;
                                    ss << "recruitment_deviations[" << i << "]_" << model->id;
                                    mas::VariableTrait<REAL_T>::SetName(model->recruitment_deviations[i], ss.str());
                                    model->Register(model->recruitment_deviations[i], phase);
                                }
                            } else {
                                for (int i = 0; i < model->recruitment_deviations.size(); i++) {
                                    std::stringstream ss;
                                    ss << "recruitment_deviations[" << i << "]_" << model->id;
                                    mas::VariableTrait<REAL_T>::SetName(model->recruitment_deviations[i], ss.str());
                                    model->Register(model->recruitment_deviations[i], phase);
                                }
                            }



                        }
                    }

                }
            }





            model->id = model_id;
            recruitment_model_iterator it = this->recruitment_models.find(model->id);
            if (it != this->recruitment_models.end()) {
                std::cout << "Configuration Error: More than one recruitment model with the same identifier defined. Recruitment models require a unique id.\n";
                mas::mas_log << "Configuration Error: More than one recruitment model with the same identifier defined. Recruitment models require a unique id.\n";

                this->valid_configuration = false;
                ;
            } else {
                this->recruitment_models[model->id] = model;
            }



        }
        //
        //        void HandleSeason(rapidjson::Document::MemberIterator & season) {
        //
        //            rapidjson::Document::MemberIterator sit;
        //            std::shared_ptr<mas::Season<REAL_T> > s(new mas::Season<REAL_T>());
        //
        //            sit = (*season).value.FindMember("id");
        //            if (sit != (*season).value.MemberEnd()) {
        //                s->id = (*sit).value.GetInt();
        //            } else {
        //                std::cout << "Configuration Error: Seasons are required to have a unique identifier.\n";
        //                mas::mas_log << "Configuration Error: Seasons are required to have a unique identifier.\n";
        //                this->valid_configuration = false;
        //                ;
        //            }
        //
        //            sit = (*season).value.FindMember("name");
        //            if (sit != (*season).value.MemberEnd()) {
        //                s->name = std::string((*sit).value.GetString());
        //            } else {
        //                std::cout << "Configuration Warning: Season has no name specified.\n";
        //                mas::mas_log << "Configuration Warning: Season has no name specified.\n";
        //            }
        //
        //            sit = (*season).value.FindMember("months");
        //            if (sit != (*season).value.MemberEnd()) {
        //                s->months = (*sit).value.GetInt();
        //            } else {
        //                std::cout << "Configuration Warning: Season month duration specified.\n";
        //                mas::mas_log << "Configuration Warning: Season month duration specified.\n";
        //            }
        //
        //
        //            seasons_iterator it = this->seasons.find(s->id);
        //            if (it != this->seasons.end()) {
        //                std::cout << "Configuration Error: More than one season with the same identifier defined. Seasons require unique id.\n";
        //                mas::mas_log << "Configuration Error: More than one season with the same identifier defined. Seasons require unique id.\n";
        //                this->valid_configuration = false;
        //                ;
        //            } else {
        //                this->seasons[s->id] = s;
        //            }
        //
        //        }

        void ParseData(const std::string & path) {

            netcdf_input in(path);

            std::multimap<std::string, NcVar> variables = in.get_variables();
            std::multimap<std::string, NcVar>::iterator it;

            for (it = variables.begin(); it != variables.end(); ++it) {
                DataObject<REAL_T> data;
                data.dimensions = (*it).second.getDimCount();

                std::string type;
                (*it).second.getAtt("data_object_type").getValues(type);

                std::string area;
                (*it).second.getAtt("area").getValues(area);
                data.area_id = StringToNumber<uint32_t>(area);
                data.type = DataObject<REAL_T>::GetType(type);
                std::vector<NcDim> dims = (*it).second.getDims();
                int i, j, k, l, m, n;
                switch (data.dimensions) {
                    case 1:
                        data.imax = dims[0].getSize();
                        data.data.resize(data.imax);
                        for (i = 0; i < data.imax; i++) {
                            data.get(i) = in.read_float((*it).first, i);
                        }
                        break;
                    case 2:
                        data.imax = dims[0].getSize();
                        data.jmax = dims[1].getSize();

                        data.data.resize(data.imax * data.jmax);
                        for (i = 0; i < data.imax; i++) {
                            for (j = 0; j < data.jmax; j++) {
                                data.get(i, j) = in.read_float((*it).first, i, j);
                            }
                        }
                        break;
                    case 3:
                        data.imax = dims[0].getSize();
                        data.jmax = dims[1].getSize();
                        data.kmax = dims[2].getSize();
                        data.data.resize(data.imax * data.jmax * data.kmax);
                        for (i = 0; i < data.imax; i++) {
                            for (j = 0; j < data.jmax; j++) {
                                for (k = 0; k < data.kmax; k++) {
                                    data.get(i, j, k) = in.read_float((*it).first, i, j, k);
                                }
                            }
                        }
                        break;
                    case 4:
                        data.imax = dims[0].getSize();
                        data.jmax = dims[1].getSize();
                        data.kmax = dims[2].getSize();
                        data.lmax = dims[3].getSize();
                        data.data.resize(data.imax * data.jmax * data.kmax * data.lmax);
                        for (i = 0; i < data.imax; i++) {
                            for (j = 0; j < data.jmax; j++) {
                                for (k = 0; k < data.kmax; k++) {
                                    for (l = 0; l < data.lmax; l++) {
                                        data.get(i, j, k, l) = in.read_float((*it).first, i, j, k, l);
                                    }
                                }
                            }
                        }
                        break;
                    case 5:
                        data.imax = dims[0].getSize();
                        data.jmax = dims[1].getSize();
                        data.kmax = dims[2].getSize();
                        data.lmax = dims[3].getSize();
                        data.lmax = dims[4].getSize();
                        data.data.resize(data.imax * data.jmax * data.kmax * data.lmax * data.mmax);
                        for (i = 0; i < data.imax; i++) {
                            for (j = 0; j < data.jmax; j++) {
                                for (k = 0; k < data.kmax; k++) {
                                    for (l = 0; l < data.lmax; l++) {
                                        for (m = 0; m < data.mmax; m++) {
                                            data.get(i, j, k, l, m) = in.read_float((*it).first, i, j, k, l, m);
                                        }
                                    }
                                }
                            }
                        }
                        break;
                    case 6:
                        data.imax = dims[0].getSize();
                        data.jmax = dims[1].getSize();
                        data.kmax = dims[2].getSize();
                        data.lmax = dims[3].getSize();
                        data.mmax = dims[4].getSize();
                        data.nmax = dims[5].getSize();
                        data.data.resize(data.imax * data.jmax * data.kmax * data.lmax * data.mmax * data.nmax);
                        for (i = 0; i < data.imax; i++) {
                            for (j = 0; j < data.jmax; j++) {
                                for (k = 0; k < data.kmax; k++) {
                                    for (l = 0; l < data.lmax; l++) {
                                        for (m = 0; m < data.mmax; m++) {
                                            for (n = 0; n < data.nmax; n++) {
                                                data.get(i, j, k, l, m, n) = in.read_float((*it).first, i, j, k, l, m, n);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        break;
                }

                data_dictionary[data.area_id][data.name] = data;
                this->areas[data.area_id]->data.push_back(data);
            }
        }

        void CreateModel() {

            area_iterator it;
            for (it = this->areas.begin(); it != this->areas.end(); ++it) {

                mas::Area<REAL_T>* area = (*it).second.get();
                growth_model_iterator git = this->growth_models.find(area->growth_model_id);
                if (git == this->growth_models.end()) {
                    std::cout << "Configuration Error: Growth model " << area->growth_model_id << " has not been defined.\n";
                    mas_log << "Configuration Error: Growth model " << area->growth_model_id << " has not been defined.\n";
                    this->valid_configuration = false;
                } else {
                    area->growth_model = (*git).second;
                }

                recruitment_model_iterator rit = this->recruitment_models.find(area->recruitment_model_id);
                if (rit == this->recruitment_models.end()) {
                    std::cout << "Configuration Error: Recruitment model " << area->recruitment_model_id << " has not been defined.\n";
                    mas_log << "Configuration Error: Recruitment model " << area->recruitment_model_id << " has not been defined.\n";
                    this->valid_configuration = false;

                } else {
                    area->recruitment_model = (*rit).second;
                }

                natural_mortality_model_iterator mit = this->natural_mortality_models.find(area->mortality_model_id);
                if (mit == this->natural_mortality_models.end()) {
                    std::cout << "Configuration Error: Mortality model " << area->mortality_model_id << " has not been defined.\n";
                    mas_log << "Configuration Error: Mortality model " << area->mortality_model_id << " has not been defined.\n";
                    this->valid_configuration = false;
                } else {
                    area->mortality_model = (*mit).second;

                }
            }


            population_iterator pit;

            for (pit = this->populations.begin(); pit != this->populations.end(); ++pit) {



                mas::Population<REAL_T>* population = (*pit).second.get();
                population->ages = this->ages.size();
                population->years = this->nyears;
                population->seasons = this->nseasons;

                population->initial_population_males.resize(this->ages.size());
                population->initial_population_females.resize(this->ages.size());
                //                if (population->initial_population_males.size() < this->ages.size()) {
                //                    std::cout << "Configuration Error: Initial population vector size of "<<population->initial_population_males.size()<<" for population " << population->natal_area_id << " not equal to declared ages of size " << this->ages.size() << ".\n";
                //                    mas_log << "Configuration Error: Initial population vector size for population " << population->natal_area_id << " not equal to declared ages of size " << this->ages.size() << ".\n";
                //                    this->valid_configuration = false;
                //                }

                area_iterator ait = this->areas.find(population->natal_area_id);

                if (ait != this->areas.end()) {
                    population->natal_area = (*ait).second;
                } else {
                    std::cout << "Configuration Error: Natal area " << population->natal_area_id << " has not been defined.\n";
                    mas_log << "Configuration Error: Natal area " << population->natal_area_id << " has not been defined.\n";
                    this->valid_configuration = false;
                }

                for (ait = this->areas.begin(); ait != this->areas.end(); ++ait) {
                    population->areas_list.push_back((*ait).second);
                    mas::PopulationInfo<REAL_T>& male_pop_info = population->male_cohorts[(*ait).second->id];
                    male_pop_info.natal_area = population->natal_area;
                    male_pop_info.natal_population = population;
                    male_pop_info.area = (*ait).second;

                    male_pop_info.maturity_vector = population->maturity_models[male_pop_info.area->id][0];
                    if (male_pop_info.maturity_vector.size() == 0 || male_pop_info.maturity_vector.size() < this->ages.size()) {
                        std::cout << "Configuration Error: Maturity vector for population " << population->id << " has wrong size.\n";
                        std::cout << "Configuration Error: Maturity vector for population " << population->id << " has wrong size.\n";
                        this->valid_configuration = false;
                    }
                    growth_model_iterator git = this->growth_models.find(population->growth_id);
                    if (git != this->growth_models.end()) {
                        male_pop_info.growth_model = this->growth_models[population->growth_id];
                    } else {
                        std::cout << "Configuration Error: Growth for population " << population->id << " has not been defined.\n";
                        mas_log << "Configuration Error: Growth for population " << population->id << " has not been defined.\n";
                        this->valid_configuration = false;
                    }


                    typename mas::Population<REAL_T>::male_natural_mortality_ids_iterator mit;
                    mit = population->male_natural_mortality_ids.find(male_pop_info.area->id);

                    if (mit != population->male_natural_mortality_ids.end()) {
                        male_pop_info.natural_mortality_model = this->natural_mortality_models[(*mit).second];
                    } else {
                        std::cout << "Configuration Error: Male natural mortality for population " << population->id << " in area " << male_pop_info.area->id << " has not been defined.\n";
                        mas_log << "Configuration Error: Male natural mortality for population " << population->id << " in area " << male_pop_info.area->id << " has not been defined.\n";
                        this->valid_configuration = false;
                    }
                    typename mas::Population<REAL_T>::recruitment_ids_iterator rit;
                    rit = population->recruitment_ids.find(male_pop_info.area->id);

                    if (rit != population->recruitment_ids.end()) {
                        std::map<int, int>& seasonal_recruitment = (*rit).second; //this->recruitment_models[(*rit).second];
                        for (int s = 0; s< this->nseasons; s++) {
                            typename std::map<int, int>::iterator sit = seasonal_recruitment.find((s + 1));
                            if (sit != seasonal_recruitment.end()) {
                                male_pop_info.recruitment_model[(*sit).first] = this->recruitment_models[(*sit).second];
                            }else {
                                std::cout << "Recruitment model not defined for " << population->id << " season " << (s + 1) << ", area " << male_pop_info.area->id << "\n";
                                mas_log << "Recruitment model not defined for " << population->id << " season " << (s + 1) << ", area " << male_pop_info.area->id << "\n";
                                this->valid_configuration = false;
                            }

                        }
                        //                        male_pop_info.recruitment_model = ;
                    } else {
                        std::cout << "Configuration Warning: Recruitment for population " << population->id << " in area " << male_pop_info.area->id << " has not been defined.\n";
                        std::cout << "Configuration Warning: Recruitment for population " << population->id << " in area " << male_pop_info.area->id << " has not been defined.\n";
                        //                        this->valid_configuration = false;
                    }



                    male_pop_info.years = this->nyears;
                    male_pop_info.seasons = this->nseasons;
                    male_pop_info.ages = this->ages;
                    male_pop_info.male_chohorts = true;
                    male_pop_info.Initialize();

                    mas::PopulationInfo<REAL_T>& female_pop_info = population->female_cohorts[(*ait).second->id];
                    female_pop_info.natal_area = population->natal_area;
                    female_pop_info.natal_population = population;
                    female_pop_info.area = (*ait).second;
                    female_pop_info.maturity_vector = population->maturity_models[female_pop_info.area->id][1];
                    if (female_pop_info.maturity_vector.size() == 0 || female_pop_info.maturity_vector.size() < this->ages.size()) {
                        std::cout << "Configuration Error: Maturity vector for population " << population->id << " has wrong size.\n";
                        std::cout << "Configuration Error: Maturity vector for population " << population->id << " has wrong size.\n";
                        this->valid_configuration = false;
                    }
                    if (git != this->growth_models.end()) {
                        female_pop_info.growth_model = this->growth_models[population->growth_id];
                    } else {
                        std::cout << "Configuration Error: Growth for population " << population->id << " has not been defined.\n";
                        mas_log << "Configuration Error: Growth for population " << population->id << " has not been defined.\n";
                        this->valid_configuration = false;
                    }

                    typename mas::Population<REAL_T>::female_natural_mortality_ids_iterator fit;
                    fit = population->female_natural_mortality_ids.find(female_pop_info.area->id);

                    if (fit != population->female_natural_mortality_ids.end()) {
                        female_pop_info.natural_mortality_model = this->natural_mortality_models[(*fit).second];
                    } else {
                        std::cout << "Configuration Error: Female natural mortality for population " << population->id << " in area " << male_pop_info.area->id << " has not been defined.\n";
                        mas_log << "Configuration Error: Female natural mortality for population " << population->id << " in area " << male_pop_info.area->id << " has not been defined.\n";
                        this->valid_configuration = false;
                    }


                    //                    typename mas::Population<REAL_T>::recruitment_ids_iterator rit;
                    rit = population->recruitment_ids.find(female_pop_info.area->id);

                    if (rit != population->recruitment_ids.end()) {
                        std::map<int, int>& seasonal_recruitment = (*rit).second; //this->recruitment_models[(*rit).second];
                        for (int s = 0; s< this->nseasons; s++) {
                            typename std::map<int, int>::iterator sit = seasonal_recruitment.find((s + 1));
                            if (sit != seasonal_recruitment.end()) {
                                female_pop_info.recruitment_model[(*sit).first ] = this->recruitment_models[(*sit).second];
                            } else {
                                std::cout << "Recruitment model not defined for " << population->id << " season " << (s + 1) << ", area " << female_pop_info.area->id << "\n";
                                mas_log << "Recruitment model not defined for " << population->id << " season " << (s + 1) << ", area " << female_pop_info.area->id << "\n";
                                this->valid_configuration = false;
                            }

                        }
                    }

                    female_pop_info.years = this->nyears;
                    female_pop_info.seasons = this->nseasons;
                    female_pop_info.ages = this->ages;
                    female_pop_info.male_chohorts = false;
                    female_pop_info.Initialize();
                }



                //                movement_model_iterator move_it = this->movement_models.find(population->movement_model_id);
                //                if (move_it != this->movement_models.end()) {
                //                    population->movement_model = (*move_it).second;
                //                } else {
                //                    std::cout << "Configuration Error:  Movement model " << population->movement_model_id << " has not been defined.\n";
                //                    mas_log << "Configuration Error:  Movement model " << population->movement_model_id << " has not been defined.\n";
                //                }

                typename mas::Population<REAL_T>::movement_model_id_iterator mmit;
                for (mmit = population->movement_models_ids.begin(); mmit != population->movement_models_ids.end(); ++mmit) {

                    int year = (*mmit).first;
                    int model = (*mmit).second;

                    movement_model_iterator mm = this->movement_models.find(model);
                    if (mm != this->movement_models.end()) {
                        population->movement_models[year] = (*mm).second;
                    } else {
                        std::cout << "Configuration Error:  Movement model " << model << " has not been defined.\n";
                        mas_log << "Configuration Error:  Movement model " << model << " has not been defined.\n";
                        this->valid_configuration = false;
                    }

                }

            }

            //            this->nseasons = this->seasons.size();

            mas_log << "Estimated Parameters\t\t\t\t\t\tPhase\n";
            mas_log << "\nRecruitment:\n";
            recruitment_model_iterator rit;
            for (rit = this->recruitment_models.begin(); rit != this->recruitment_models.end(); ++rit) {
                typename mas::ModelObject<REAL_T>::estimable_parameter_iterator it;
                for (it = (*rit).second->estimated_parameters_map.begin(); it != (*rit).second->estimated_parameters_map.end(); ++it) {
                    mas_log << (*it).first->GetName() << "\t\t\t\t\t\t" << (*it).second << "\n";
                    this->Register(*(*it).first, (*it).second);
                }

            }
            mas_log << "\nGrowth:\n";
            growth_model_iterator git;
            for (git = this->growth_models.begin(); git != this->growth_models.end(); ++git) {
                typename mas::ModelObject<REAL_T>::estimable_parameter_iterator it;
                for (it = (*git).second->estimated_parameters_map.begin(); it != (*git).second->estimated_parameters_map.end(); ++it) {
                    mas_log << (*it).first->GetName() << "\t\t\t\t\t\t" << (*it).second << "\n";
                    this->Register(*(*it).first, (*it).second);
                }

            }

            mas_log << "\nSelectivity:\n";
            selectivity_model_iterator sit;
            for (sit = this->selectivity_models.begin(); sit != this->selectivity_models.end(); ++sit) {
                typename mas::ModelObject<REAL_T>::estimable_parameter_iterator it;
                for (it = (*sit).second->estimated_parameters_map.begin(); it != (*sit).second->estimated_parameters_map.end(); ++it) {
                    mas_log << (*it).first->GetName() << "\t\t\t\t\t\t" << (*it).second << "\n";
                    this->Register(*(*it).first, (*it).second);

                }

            }

            mas_log << "\nMovement:\n";
            movement_model_iterator mit;
            for (mit = this->movement_models.begin(); mit != this->movement_models.end(); ++mit) {
                typename mas::ModelObject<REAL_T>::estimable_parameter_iterator it;
                for (it = (*mit).second->estimated_parameters_map.begin(); it != (*mit).second->estimated_parameters_map.end(); ++it) {
                    mas_log << (*it).first->GetName() << "\t\t\t\t\t\t" << (*it).second << "\n";
                    this->Register(*(*it).first, (*it).second);
                }

            }
            mas_log << "\nNatural Mortality:\n";
            natural_mortality_model_iterator mmit;
            for (mmit = this->natural_mortality_models.begin(); mmit != this->natural_mortality_models.end(); ++mmit) {
                if ((*mmit).second->mortality_vector.size() != this->ages.size()) {
                    this->valid_configuration = false;
                    std::cout << "Configuration Error: Natural mortality vector for model \"" << (*mmit).second->id << "\" not eqaul to " << ages.size() << "\n";
                    mas_log << "Configuration Error: Natural mortality vector for model \"" << (*mmit).second->id << "\" not eqaul to " << ages.size() << "\n";

                }
                typename mas::ModelObject<REAL_T>::estimable_parameter_iterator it;
                for (it = (*mmit).second->estimated_parameters_map.begin(); it != (*mmit).second->estimated_parameters_map.end(); ++it) {
                    mas_log << (*it).first->GetName() << "\t\t\t\t\t\t" << (*it).second << "\n";
                    this->Register(*(*it).first, (*it).second);
                }

                //            for (int a = 0; a <this->ages.size(); a++) {
                //                (*mmit).second->male_mortality[this->ages[a]] = (*mmit).second->male_mortality_vector[a];
                //                (*mmit).second->female_mortality[this->ages[a]] = (*mmit).second->female_mortality_vector[a];
                //
                //            }

            }





            fleet_iterator fit;
            for (fit = this->fleets.begin(); fit != this->fleets.end(); ++fit) {
                typename Fleet<REAL_T>::season_area_id_iterator sit;
                for (sit = (*fit).second->season_area_selectivity_ids.begin(); sit != (*fit).second->season_area_selectivity_ids.end(); ++sit) {
                    typename Fleet<REAL_T>::area_id_iteraor ait;
                    int season = (*sit).first;
                    for (ait = (*sit).second.begin(); ait != (*sit).second.end(); ++ait) {
                        int area = (*ait).first;
                        int id = (*ait).second;
                        (*fit).second->area_season_selectivity[area][season] = this->selectivity_models[id];
                        (*fit).second->season_area_selectivity[season][area] = this->selectivity_models[id];
                    }
                }

                for (sit = (*fit).second->season_area_fishing_mortality_ids.begin(); sit != (*fit).second->season_area_fishing_mortality_ids.end(); ++sit) {
                    typename Fleet<REAL_T>::area_id_iteraor ait;
                    int season = (*sit).first;
                    for (ait = (*sit).second.begin(); ait != (*sit).second.end(); ++ait) {
                        int area = (*ait).first;
                        int id = (*ait).second;
                        (*fit).second->area_season_fishing_mortality[area][season] = this->finshing_mortality_models[id];
                        (*fit).second->season_area_fishing_mortality[season][area] = this->finshing_mortality_models[id];
                        this->areas[area]->seasonal_fleet_operations[season].push_back((*fit).second);

                    }
                }

            }
            // exit(0);

            //            for (fit = this->fleets.begin(); fit != this->fleets.end(); ++fit) {
            //                typename Fleet<REAL_T>::season_area_selectivity_iterator sait;
            //                for (sait = (*fit).second->season_area_selectivity.begin(); sait != (*fit).second->season_area_selectivity.end(); ++sait) {
            //                    int season = (*sait).first;
            //                    typename Fleet<REAL_T>::area_sectivity_iterator ait;
            //                    for (ait = (*sait).second.begin(); ait != (*sait).second.begin(); ++ait) {
            //                        int area = (*ait).first;
            //                        this->areas[area]->seasonal_fleet_operations[season].push_back((*fit).second);
            //                    }
            //                }
            //                
            //                typename Fleet<REAL_T>::season_area_fishing_mortality_iterator sait;
            //                for (sait = (*fit).second->season_area_selectivity.begin(); sait != (*fit).second->season_area_selectivity.end(); ++sait) {
            //                    int season = (*sait).first;
            //                    typename Fleet<REAL_T>::area_sectivity_iterator ait;
            //                    for (ait = (*sait).second.begin(); ait != (*sait).second.begin(); ++ait) {
            //                        int area = (*ait).first;
            //                        this->areas[area]->seasonal_fleet_operations[season].push_back((*fit).second);
            //                    }
            //                }
            //            }


            //            std::cout << "Estimated parameters:\n";
            //            typename mas::ModelObject<REAL_T>::estimable_parameter_iterator eit;
            //            for (eit = this->estimated_parameters_map.begin(); eit != this->estimated_parameters_map.end(); ++eit) {
            //                std::cout << (*eit).first->GetName() << " " << (*eit).second << "\n";
            //            }



            if (!this->valid_configuration) {
                std::cout << "Configuration Error:  Invalid model configuration. See mas.log for errors.\n";
                mas_log << "Configuration Error:  Invalid model configuration. See mas.log for errors.\n";
                exit(0);
            }

        }

        std::unordered_map<int, std::shared_ptr<mas::Population<REAL_T> > >& GetPopulations() {
            return populations;
        }







    private:

    };


}


#endif /* MAS_INFORMATION_HPP */

