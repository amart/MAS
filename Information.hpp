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


namespace mas {

    template<typename REAL_T>
    class Information {
        typedef typename VariableTrait<REAL_T>::variable variable;



        std::string analyst = "NA";
        std::string study_name = "NA";

        std::string data_path;

        int nyears;
        int nseasons;
        int first_year;
        int last_year;
        bool natal_movement = false;

        std::unordered_map<int, std::shared_ptr<mas::Area<REAL_T> > > areas;
        std::unordered_map<int, std::shared_ptr<mas::Season<REAL_T> > > seasons;
        typedef typename std::unordered_map<int, std::shared_ptr<mas::Season<REAL_T> > >::iterator seasons_iterator;
        typedef typename std::unordered_map<int, std::shared_ptr<mas::Area<REAL_T> > >::iterator area_iterator;

        //System sub model objects
        std::unordered_map<int, std::shared_ptr<mas::Population<REAL_T> > > populations;
        std::unordered_map<int, std::shared_ptr<mas::GrowthBase<REAL_T> > > growth_models;
        std::unordered_map<int, std::shared_ptr<mas::RecruitmentBase<REAL_T> > > recruitment_models;
        std::unordered_map<int, std::shared_ptr<mas::MortalityBase<REAL_T> > > mortality_models;
        std::unordered_map<int, std::shared_ptr<mas::FecundityBase<REAL_T> > > fecundity_models;
        std::unordered_map<int, std::shared_ptr<mas::Movement<REAL_T> > > movement_models;
        std::unordered_map<int, std::shared_ptr<mas::SelectivityBase<REAL_T> > > selectivity_models;

        //System sub model iterators
        typedef typename std::unordered_map<int, std::shared_ptr<mas::Population<REAL_T> > >::iterator population_iterator;
        typedef typename std::unordered_map<int, std::shared_ptr<mas::GrowthBase<REAL_T> > >::iterator growth_model_iterator;
        typedef typename std::unordered_map<int, std::shared_ptr<mas::RecruitmentBase<REAL_T> > >::iterator recruitment_model_iterator;
        typedef typename std::unordered_map<int, std::shared_ptr<mas::MortalityBase<REAL_T> > >::iterator mortality_model_iterator;
        typedef typename std::unordered_map<int, std::shared_ptr<mas::FecundityBase<REAL_T> > >::iterator fecundity_model_iterator;
        typedef typename std::unordered_map<int, std::shared_ptr<mas::Movement<REAL_T> > >::iterator movement_model_iterator;
        typedef typename std::unordered_map<int, std::shared_ptr<mas::SelectivityBase<REAL_T> > >::iterator selectivity_model_iterator;

    public:

        Information() {
        }

        Information(const Information<REAL_T>& orig) {
        }

        virtual ~Information() {
        }

        void ParseConfig(const std::string& path) {

            std::stringstream ss;
            std::ifstream config;
            config.open(path.c_str());
            if (!config.good()) {
                std::cerr << "MAS Configuration file \"" << path << "\" not found.\n";
                mas::mas_log << "MAS Configuration file \"" << path << "\" not found.\n";
                exit(0);
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


                if (std::string((*mit).name.GetString()) == "season") {
                    HandleSeason(mit);
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
                exit(0);
            } else {
                model_id = (*rit).value.GetInt();
            }
            model->id = model_id;

            rit = (*movement_model).value.FindMember("season");
            if (rit == (*movement_model).value.MemberEnd()) {
                std::cout << "Configuration Error: Movement is required to have a season defined\n";
                mas::mas_log << "Configuration Error: Movement is required to have a season defined\n";
                exit(0);
            } else {
                model->season = (*rit).value.GetInt();
            }

            rit = (*movement_model).value.FindMember("male");
            if ((*rit).value.IsArray()) {
                model->male_connectivity.resize((*rit).value.Size());
                for (int i = 0; i < (*rit).value.Size(); i++) {
                    rapidjson::Value& m = (*rit).value[i];
                    if (m.IsArray()) {
                        model->male_connectivity[i].resize(m.Size());
                        for (int j = 0; j < m.Size(); j++) {
                            mas::VariableTrait<REAL_T>::SetValue(model->male_connectivity[i][j], static_cast<REAL_T> (m[j].GetDouble()));
                        }

                    } else {
                        std::cout << "Configuration Error: Movement connectivity rows must be a vector\n";
                        mas::mas_log << "Configuration Error: Movement connectivity rows must be a vector\n";
                        exit(0);
                    }
                }


            } else {
                std::cout << "Configuration Error: Movement connectivity must be a matrix\n";
                mas::mas_log << "Configuration Error: Movement connectivity must be a matrix\n";
                exit(0);
            }

            rit = (*movement_model).value.FindMember("female");
            if ((*rit).value.IsArray()) {
                model->female_connectivity.resize((*rit).value.Size());
                for (int i = 0; i < (*rit).value.Size(); i++) {
                    rapidjson::Value& m = (*rit).value[i];
                    if (m.IsArray()) {
                        model->female_connectivity[i].resize(m.Size());
                        for (int j = 0; j < m.Size(); j++) {
                            mas::VariableTrait<REAL_T>::SetValue(model->female_connectivity[i][j], static_cast<REAL_T> (m[j].GetDouble()));
                        }

                    } else {
                        std::cout << "Configuration Error: Movement connectivity rows must be a vector\n";
                        mas::mas_log << "Configuration Error: Movement connectivity rows must be a vector\n";
                        exit(0);
                    }
                }


            } else {
                std::cout << "Configuration Error: Movement connectivity must be a matrix\n";
                mas::mas_log << "Configuration Error: Movement connectivity must be a matrix\n";
                exit(0);
            }


            rit = (*movement_model).value.FindMember("estimated");
            if (rit != (*movement_model).value.MemberEnd()) {
                std::string e = std::string((*rit).value.GetString());
                if (e == "true") {
                    estimated = true;
                }
            }

            if (estimated) {
                rit = (*movement_model).value.FindMember("phase");
                if (rit != (*movement_model).value.MemberEnd()) {
                    phase = (*rit).value.GetInt();
                }
                //register movement coefficients
                for (int i = 0; i < model->male_connectivity.size(); i++) {
                    for (int j = 0; j < model->male_connectivity[i].size(); j++) {
                        model->Register(model->male_connectivity[i][j], phase);
                    }
                }

                for (int i = 0; i < model->female_connectivity.size(); i++) {
                    for (int j = 0; j < model->female_connectivity[i].size(); j++) {
                        model->Register(model->female_connectivity[i][j], phase);
                    }
                }
            }

            movement_model_iterator it = this->movement_models.find(model->id);
            if (it != this->movement_models.end()) {
                std::cout << "Configuration Error: More than one movement model with the same identifier defined. Movement models require a unique id.\n";
                mas::mas_log << "Configuration Error: More than one movement model with the same identifier defined. Movement models require a unique id.\n";
                exit(0);
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
                exit(0);
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
                    exit(0);
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
                            exit(0);
                        } else {
                            sid = (*mit).value.GetInt();
                        }

                        mit = m.FindMember("season");
                        if (mit == m.MemberEnd()) {
                            std::cout << "Configuration Error: Fleets selectivity season has no identifier\n";
                            std::cout << "Configuration Error: Fleets selectivity season has no identifier\n";
                            exit(0);
                        } else {
                            sseason = (*mit).value.GetInt();
                        }

                        mit = m.FindMember("area");
                        if (mit == m.MemberEnd()) {
                            std::cout << "Configuration Error: Fleets selectivity area has no identifier\n";
                            std::cout << "Configuration Error: Fleets selectivity area has no identifier\n";
                            exit(0);
                        } else {
                            sarea = (*mit).value.GetInt();
                        }
                        model->season_area_selectivity_ids[sseason][sarea] = sid;

                    }
                }
            }


        }

        void HandlePopulationModel(rapidjson::Document::MemberIterator& population_model) {
            std::shared_ptr<mas::Population<REAL_T> > model(new mas::Population<REAL_T>());


            rapidjson::Document::MemberIterator rit = (*population_model).value.FindMember("id");
            int model_id = 0;
            if (rit == (*population_model).value.MemberEnd()) {
                std::cout << "Configuration Error: Populations are required to have a unique identifier\n";
                mas::mas_log << "Configuration Error: Populations are required to have a unique identifier\n";
                exit(0);
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
                exit(0);
            }

            rit = (*population_model).value.FindMember("movement_model");
            if (rit != (*population_model).value.MemberEnd()) {
                model->movement_model_id = (*rit).value.GetInt();
            } else {
                std::cout << "Configuration Error: Population is required to have a natal area identifier\n";
                mas::mas_log << "Configuration Error: Population is required to have a natal area identifier\n";
                exit(0);
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
                exit(0);
            }


            rit = (*population_model).value.FindMember("parameters");

            rapidjson::Document::MemberIterator in_it = (*rit).value.FindMember("initial_numbers");

            rapidjson::Document::MemberIterator m_it = (*in_it).value.FindMember("male");
            if (m_it != (*in_it).value.MemberEnd()) {
                bool estimated = false;
                int phase = 1;
                std::vector<REAL_T> init_numbers;
                std::vector<REAL_T> init_numbers_min;
                std::vector<REAL_T> init_numbers_max;
                rapidjson::Document::MemberIterator v_it = (*m_it).value.FindMember("value");
                if (v_it != (*m_it).value.MemberEnd()) {
                    if ((*v_it).value.IsArray()) {
                        rapidjson::Value& m = (*v_it).value;

                        for (int i = 0; i < m.Size(); i++) {
                            init_numbers.push_back(static_cast<REAL_T> (m[i].GetDouble()));
                        }

                    } else {
                        std::cout << "Configuration Error: Initial population numbers must be a vector.\n";
                        mas::mas_log << "Configuration Error: Initial population numbers must be a vector.\n";
                        exit(0);
                    }
                } else {
                    std::cout << "Configuration Error: Initial population numbers not defined.\n";
                    mas::mas_log << "Configuration Error: Initial population numbers  not defined.\n";
                    exit(0);
                }

                v_it = (*m_it).value.FindMember("min");
                if (v_it != (*m_it).value.MemberEnd()) {
                    if ((*v_it).value.IsArray()) {
                        rapidjson::Value& m = (*v_it).value;

                        for (int i = 0; i < m.Size(); i++) {
                            init_numbers_min.push_back(static_cast<REAL_T> (m[i].GetDouble()));
                        }

                    } else {
                        std::cout << "Configuration Error: Initial population min numbers must be a vector.\n";
                        mas::mas_log << "Configuration Error: Initial population min numbers must be a vector.\n";
                        exit(0);
                    }
                }

                v_it = (*m_it).value.FindMember("max");
                if (v_it != (*m_it).value.MemberEnd()) {
                    if ((*v_it).value.IsArray()) {
                        rapidjson::Value& m = (*v_it).value;

                        for (int i = 0; i < m.Size(); i++) {
                            init_numbers_max.push_back(static_cast<REAL_T> (m[i].GetDouble()));
                        }

                    } else {
                        std::cout << "Configuration Error: Initial population max numbers must be a vector.\n";
                        mas::mas_log << "Configuration Error: Initial population max numbers must be a vector.\n";
                        exit(0);
                    }
                }


                model->initial_population_males.resize(init_numbers.size());
                for (int i = 0; i < init_numbers.size(); i++) {
                    std::stringstream ss;
                    ss<<"init_population_males"<<"["<<i<<"]_"<<model_id;
                    mas::VariableTrait<REAL_T>::SetValue(model->initial_population_males[i], init_numbers[i]);
                    mas::VariableTrait<REAL_T>::SetName(model->initial_population_males[i],ss.str());
                }

                if (init_numbers_min.size()) {
                    for (int i = 0; i < init_numbers_min.size(); i++) {
                        mas::VariableTrait<REAL_T>::SetMinBoundary(model->initial_population_males[i], init_numbers_min[i]);
                    }
                }

                if (init_numbers_max.size()) {
                    for (int i = 0; i < init_numbers_min.size(); i++) {
                        mas::VariableTrait<REAL_T>::SetMaxBoundary(model->initial_population_males[i], init_numbers_max[i]);
                    }
                }

                v_it = (*m_it).value.FindMember("estimated");
                if (v_it != (*m_it).value.MemberEnd()) {
                    std::string e = std::string((*v_it).value.GetString());
                    if (e == "true") {
                        estimated = true;
                    }
                }

                if (estimated) {
                    v_it = (*m_it).value.FindMember("phase");
                    if (v_it != (*m_it).value.MemberEnd()) {
                        phase = (*v_it).value.GetInt();
                    }
                    //register initial numbers
                    for (int i = 0; i < init_numbers.size(); i++) {
                        model->Register(model->initial_population_males[i], phase);
                    }


                }



            } else {
                std::cout << "Configuration Warning: No initial population numbers for males given for population " << model->id << ".\n";
                mas::mas_log << "Configuration Warning: No initial population numbers for males given for population " << model->id << ".\n";
            }

            m_it = (*in_it).value.FindMember("female");
            if (m_it != (*in_it).value.MemberEnd()) {
                bool estimated = false;
                int phase = 1;
                std::vector<REAL_T> init_numbers;
                std::vector<REAL_T> init_numbers_min;
                std::vector<REAL_T> init_numbers_max;
                rapidjson::Document::MemberIterator v_it = (*m_it).value.FindMember("value");
                if (v_it != (*m_it).value.MemberEnd()) {
                    if ((*v_it).value.IsArray()) {
                        rapidjson::Value& m = (*v_it).value;

                        for (int i = 0; i < m.Size(); i++) {
                            init_numbers.push_back(static_cast<REAL_T> (m[i].GetDouble()));
                        }

                    } else {
                        std::cout << "Configuration Error: Initial population numbers must be a vector.\n";
                        mas::mas_log << "Configuration Error: Initial population numbers must be a vector.\n";
                        exit(0);
                    }
                } else {
                    std::cout << "Configuration Error: Initial population numbers not defined.\n";
                    mas::mas_log << "Configuration Error: Initial population numbers  not defined.\n";
                    exit(0);
                }


                v_it = (*m_it).value.FindMember("min");
                if (v_it != (*m_it).value.MemberEnd()) {
                    if ((*v_it).value.IsArray()) {
                        rapidjson::Value& m = (*v_it).value;

                        for (int i = 0; i < m.Size(); i++) {
                            init_numbers_min.push_back(static_cast<REAL_T> (m[i].GetDouble()));
                        }

                    } else {
                        std::cout << "Configuration Error: Initial population min numbers must be a vector.\n";
                        mas::mas_log << "Configuration Error: Initial population min numbers must be a vector.\n";
                        exit(0);
                    }
                }

                v_it = (*m_it).value.FindMember("max");
                if (v_it != (*m_it).value.MemberEnd()) {
                    if ((*v_it).value.IsArray()) {
                        rapidjson::Value& m = (*v_it).value;

                        for (int i = 0; i < m.Size(); i++) {
                            init_numbers_max.push_back(static_cast<REAL_T> (m[i].GetDouble()));
                        }

                    } else {
                        std::cout << "Configuration Error: Initial population max numbers must be a vector.\n";
                        mas::mas_log << "Configuration Error: Initial population max numbers must be a vector.\n";
                        exit(0);
                    }
                }


                model->initial_population_females.resize(init_numbers.size());
                for (int i = 0; i < init_numbers.size(); i++) {
                    std::stringstream ss;
                    ss<<"init_population_females"<<"["<<i<<"]_"<<model_id;
                    mas::VariableTrait<REAL_T>::SetValue(model->initial_population_females[i], init_numbers[i]);
                    mas::VariableTrait<REAL_T>::SetName(model->initial_population_females[i],ss.str());
                }

                if (init_numbers_min.size()) {
                    for (int i = 0; i < init_numbers_min.size(); i++) {
                        mas::VariableTrait<REAL_T>::SetMinBoundary(model->initial_population_females[i], init_numbers_min[i]);
                    }
                }

                if (init_numbers_max.size()) {
                    for (int i = 0; i < init_numbers_min.size(); i++) {
                        mas::VariableTrait<REAL_T>::SetMaxBoundary(model->initial_population_females[i], init_numbers_max[i]);
                    }
                }

                v_it = (*m_it).value.FindMember("estimated");
                if (v_it != (*m_it).value.MemberEnd()) {
                    std::string e = std::string((*v_it).value.GetString());
                    if (e == "true") {
                        estimated = true;
                    }
                }

                if (estimated) {
                    v_it = (*m_it).value.FindMember("phase");
                    if (v_it != (*m_it).value.MemberEnd()) {
                        phase = (*v_it).value.GetInt();
                    }
                    //register initial numbers
                    for (int i = 0; i < init_numbers.size(); i++) {
                        model->Register(model->initial_population_females[i], phase);
                    }


                }



            } else {
                std::cout << "Configuration Warning: No initial population numbers for females given for population " << model->id << ".\n";
                mas::mas_log << "Configuration Warning: No initial population numbers for females given for population " << model->id << ".\n";
            }

            population_iterator it = this->populations.find(model->id);
            if (it != this->populations.end()) {
                std::cout << "Configuration Error: More than one population with the same identifier defined. Populations require a unique id.\n";
                mas::mas_log << "Configuration Error: More than one population with the same identifier defined. Populations require a unique id.\n";

                exit(0);
            } else {
                this->populations[model->id] = model;
            }

        }

        void HandleAreaModel(rapidjson::Document::MemberIterator& area_model) {

            std::shared_ptr<mas::Area<REAL_T> > model(new mas::Area<REAL_T>());


            rapidjson::Document::MemberIterator rit = (*area_model).value.FindMember("id");
            int model_id = 0;
            if (rit == (*area_model).value.MemberEnd()) {
                std::cout << "Configuration Error: Area is required to have a unique identifier\n";
                mas::mas_log << "Configuration Error: Area is required to have a unique identifier\n";
                exit(0);
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
                exit(0);
            }

            rit = (*area_model).value.FindMember("recruitment_model");
            if (rit != (*area_model).value.MemberEnd()) {
                model->recruitment_model_id = (*rit).value.GetInt();
            } else {
                std::cout << "Configuration Error: Area " << model->id << " is required to have a recruitment model defined\n";
                mas::mas_log << "Configuration Error: Area " << model->id << " is required to have a recruitment model defined\n";
                exit(0);
            }

            rit = (*area_model).value.FindMember("fecundity_model");
            if (rit != (*area_model).value.MemberEnd()) {
                model->fecundity_model_id = (*rit).value.GetInt();
            } else {
                std::cout << "Configuration Error: Area " << model->id << " is required to have a fecundity model defined\n";
                mas::mas_log << "Configuration Error: Area " << model->id << " is required to have a fecundity model defined\n";
                exit(0);
            }

            model->id = model_id;
            area_iterator it = this->areas.find(model->id);
            if (it != this->areas.end()) {
                std::cout << "Configuration Error: More than one area with the same identifier defined. Areas require a unique id.\n";
                mas::mas_log << "Configuration Error: More than one area with the same identifier defined. Areas require a unique id.\n";

                exit(0);
            } else {
                this->areas[model->id] = model;
            }



        }

        void HandleSelectivityModel(rapidjson::Document::MemberIterator& selectivity_model) {
            rapidjson::Document::MemberIterator rit;
            rit = (*selectivity_model).value.FindMember("model");

            if (rit == (*selectivity_model).value.MemberEnd()) {
                std::cout << "Configuration Error: Selectivity is required to have a model specified.\n";
                mas::mas_log << "Configuration Error: Selectivity is required to have a model specified.\n";
                exit(0);
            }


            std::shared_ptr<mas::SelectivityBase<REAL_T> > model(NULL);
            std::string smodel = std::string((*rit).value.GetString());

            rit = (*selectivity_model).value.FindMember("id");
            int model_id = 0;
            if (rit == (*selectivity_model).value.MemberEnd()) {
                std::cout << "Configuration Error: Selectivity is required to have a unique identifier\n";
                mas::mas_log << "Configuration Error: Selectivity is required to have a unique identifier\n";
                exit(0);
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
                    exit(0);
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
                    exit(0);
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
                exit(0);
            } else {
                this->selectivity_models[model->id] = model;
            }


        }

        void HandleGrowthModel(rapidjson::Document::MemberIterator& growth_model) {
            rapidjson::Document::MemberIterator rit;
            rit = (*growth_model).value.FindMember("model");

            if (rit == (*growth_model).value.MemberEnd()) {
                std::cout << "Configuration Error: Growth is required to have a model specified.\n";
                mas::mas_log << "Configuration Error: Growth is required to have a model specified.\n";
                exit(0);
            }


            std::shared_ptr<mas::GrowthBase<REAL_T> > model(NULL);
            std::string smodel = std::string((*rit).value.GetString());

            rit = (*growth_model).value.FindMember("id");
            int model_id = 0;
            if (rit == (*growth_model).value.MemberEnd()) {
                std::cout << "Configuration Error: Growth is required to have a unique identifier\n";
                mas::mas_log << "Configuration Error: Growth is required to have a unique identifier\n";
                exit(0);
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
                    exit(0);
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
                    exit(0);
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
                    exit(0);
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
                    exit(0);
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
                    exit(0);
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
                exit(0);
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
                    pm = (*amin_it).value.FindMember("phase");
                    if (pm != (*amin_it).value.MemberEnd()) {
                        phase = (*pm).value.GetInt();
                    }
                    //register alpha
                    model->Register(model->a_min, phase);

                }
            }

            rapidjson::Document::MemberIterator amax_it = (*pit).value.FindMember("amin");

            if (amax_it == (*pit).value.MemberEnd()) {
                std::cout << "Configuration Error: Growth is required to have a amax specified.\n";
                mas::mas_log << "Configuration Error: Growth is required to have a amax specified.\n";
                exit(0);
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

                exit(0);
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
                exit(0);
            }

            std::shared_ptr<mas::RecruitmentBase<REAL_T> > model(NULL);
            std::string smodel = std::string((*rit).value.GetString());

            rit = (*recruitment_model).value.FindMember("id");
            int model_id = 0;
            if (rit == (*recruitment_model).value.MemberEnd()) {
                std::cout << "Configuration Error: Recruitment is required to have a unique identifier\n";
                mas::mas_log << "Configuration Error: Recruitment is required to have a unique identifier\n";
                exit(0);
            } else {
                model_id = (*rit).value.GetInt();
            }


            if (smodel == std::string("beverton_holt")) {
                model = std::make_shared<mas::BevertonHolt<REAL_T> >();
                mas::BevertonHolt<REAL_T>* bh = (mas::BevertonHolt<REAL_T>*)model.get();
                std::stringstream ss;
                ss << "beverton_holt_alpha_" << model_id;
                VariableTrait<REAL_T>::SetName(bh->alpha, ss.str());
                ss.str("");
                ss << "beverton_holt_beta_" << model_id;
                VariableTrait<REAL_T>::SetName(bh->beta, ss.str());

                rapidjson::Document::MemberIterator pit = (*recruitment_model).value.FindMember("parameters");

                if (pit == (*recruitment_model).value.MemberEnd()) {
                    std::cout << "Configuration Error: Recruitment model \"Beverton-Holt\" has no parameter definitions.\n";
                    mas::mas_log << "Configuration Error: Recruitment model \"Beverton-Holt\" has no parameter definitions.\n";
                    exit(0);
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

                rapidjson::Document::MemberIterator pit = (*recruitment_model).value.FindMember("parameters");

                if (pit == (*recruitment_model).value.MemberEnd()) {
                    std::cout << "Configuration Error: Recruitment model \"Beverton-Holt\" has no parameter definitions.\n";
                    mas::mas_log << "Configuration Error: Recruitment model \"Beverton-Holt\" has no parameter definitions.\n";
                    exit(0);
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

                rapidjson::Document::MemberIterator pit = (*recruitment_model).value.FindMember("parameters");

                if (pit == (*recruitment_model).value.MemberEnd()) {
                    std::cout << "Configuration Error: Recruitment model \"Beverton-Holt Dep\" has no parameter definitions.\n";
                    mas::mas_log << "Configuration Error: Recruitment model \"Beverton-Holt Dep\" has no parameter definitions.\n";
                    exit(0);
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

                rapidjson::Document::MemberIterator pit = (*recruitment_model).value.FindMember("parameters");

                if (pit == (*recruitment_model).value.MemberEnd()) {
                    std::cout << "Configuration Error: Recruitment model \"Ricker\" has no parameter definitions.\n";
                    mas::mas_log << "Configuration Error: Recruitment model \"Ricker\" has no parameter definitions.\n";
                    exit(0);
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

                rapidjson::Document::MemberIterator pit = (*recruitment_model).value.FindMember("parameters");

                if (pit == (*recruitment_model).value.MemberEnd()) {
                    std::cout << "Configuration Error: Recruitment model \"Ricker Alt\" has no parameter definitions.\n";
                    mas::mas_log << "Configuration Error: Recruitment model \"Ricker Alt\" has no parameter definitions.\n";
                    exit(0);
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

                rapidjson::Document::MemberIterator pit = (*recruitment_model).value.FindMember("parameters");

                if (pit == (*recruitment_model).value.MemberEnd()) {
                    std::cout << "Configuration Error: Recruitment model \"Shepherd\" has no parameter definitions.\n";
                    mas::mas_log << "Configuration Error: Recruitment model \"Shepherd\" has no parameter definitions.\n";
                    exit(0);
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

                rapidjson::Document::MemberIterator pit = (*recruitment_model).value.FindMember("parameters");

                if (pit == (*recruitment_model).value.MemberEnd()) {
                    std::cout << "Configuration Error: Recruitment model \"Deriso\" has no parameter definitions.\n";
                    mas::mas_log << "Configuration Error: Recruitment model \"Deriso\" has no parameter definitions.\n";
                    exit(0);
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


            model->id = model_id;
            recruitment_model_iterator it = this->recruitment_models.find(model->id);
            if (it != this->recruitment_models.end()) {
                std::cout << "Configuration Error: More than one recruitment model with the same identifier defined. Recruitment models require a unique id.\n";
                mas::mas_log << "Configuration Error: More than one recruitment model with the same identifier defined. Recruitment models require a unique id.\n";

                exit(0);
            } else {
                this->recruitment_models[model->id] = model;
            }



        }

        void HandleSeason(rapidjson::Document::MemberIterator & season) {

            rapidjson::Document::MemberIterator sit;
            std::shared_ptr<mas::Season<REAL_T> > s(new mas::Season<REAL_T>());

            sit = (*season).value.FindMember("id");
            if (sit != (*season).value.MemberEnd()) {
                s->id = (*sit).value.GetInt();
            } else {
                std::cout << "Configuration Error: Seasons are required to have a unique identifier.\n";
                mas::mas_log << "Configuration Error: Seasons are required to have a unique identifier.\n";
                exit(0);
            }

            sit = (*season).value.FindMember("name");
            if (sit != (*season).value.MemberEnd()) {
                s->name = std::string((*sit).value.GetString());
            } else {
                std::cout << "Configuration Warning: Season has no name specified.\n";
                mas::mas_log << "Configuration Warning: Season has no name specified.\n";
            }

            sit = (*season).value.FindMember("months");
            if (sit != (*season).value.MemberEnd()) {
                s->months = (*sit).value.GetInt();
            } else {
                std::cout << "Configuration Warning: Season month duration specified.\n";
                mas::mas_log << "Configuration Warning: Season month duration specified.\n";
            }


            seasons_iterator it = this->seasons.find(s->id);
            if (it != this->seasons.end()) {
                std::cout << "Configuration Error: More than one season with the same identifier defined. Seasons require unique id.\n";
                mas::mas_log << "Configuration Error: More than one season with the same identifier defined. Seasons require unique id.\n";
                exit(0);
            } else {
                this->seasons[s->id] = s;
            }

        }

        void ParseData(const std::string & path) {

        }

        void CreateModel() {

            area_iterator it;
            for(it = this->areas.begin(); it != this->areas.end(); ++it){
                mas::Area<REAL_T>* area = (*it).second.get();
                std::cout<<"area"<<area->growth_model_id<<"\n";
            }
            
        }

    private:

    };


}


#endif /* MAS_INFORMATION_HPP */

