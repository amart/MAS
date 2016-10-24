/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: matthewsupernaw
 *
 * Created on September 14, 2016, 9:57 AM
 */

#include <cstdlib>
#include "Population.hpp"
#include "Growth.hpp"
#include "Recruitment.hpp"
#include "Information.hpp"
#include <iostream>
using namespace std;

void create() {

    int fisheries = 2;
    int nindixes = 2;
    int npops = 2;
    int nareas = 2;
    int start_date = -9;
    int end_date = 20;
    int data_length_start = 0;
    int data_length_end = 100;
    int data_age_start = 0;
    int data_age_end = 9;

    std::vector<float> area1_biomass = {13199.2, 12841.9, 12556.2, 12331.0, 12138.5, 11979.5, 11859.1, 11771.5, 11709.3, 11659.9, 40784.0, 32307.0, 28392.8, 26300.2, 24956.9, 23962.3, 23253.2, 22716.0, 22331.0, 22030.5, 11848.1, 13367.5, 14157.3, 14599.9, 14921.5, 15191.7, 15391.1, 15552.8, 15670.1, 15766.2};
    std::vector<float> area2_biomass = {9608.6, 9963.1, 10246.3, 10455.0, 10547.4, 10538.9, 10506.4, 10472.3, 10438.4, 10409.0, 19897.4, 18393.6, 16830.1, 15682.1, 14889.3, 14303.0, 13850.3, 13509.7, 13248.6, 13052.0, 12900.9, 12785.6, 12950.7, 13150.4, 13317.6, 13473.5, 13620.2, 13739.7, 13839.8, 13918.6};

    std::vector<float> area1_survey_biomass = {127.0, 123.6, 120.9, 118.7, 116.9, 115.4, 114.3, 113.5, 112.9, 112.4, 91.3, 73.2, 64.9, 60.4, 57.4, 55.2, 53.7, 52.5, 51.6, 51.0, 57.4, 64.2, 67.8, 69.8, 71.3, 72.6, 73.5, 74.2, 74.8, 75.2};
    std::vector<float> area2_survey_biomass = {93.7, 97.1, 99.8, 101.9, 102.7, 102.5, 102.2, 101.8, 101.5, 101.3, 94.4, 87.5, 80.4, 75.2, 71.6, 68.9, 66.8, 65.2, 64.0, 63.0, 62.3, 61.8, 62.6, 63.5, 64.2, 65.0, 65.7, 66.2, 66.7, 67.0};

    std::vector<std::vector<float> > area1_catch_proportion = {
        {0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000},
        {0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000},
        {0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000},
        {0.146, 0.145, 0.146, 0.146, 0.144, 0.145, 0.146, 0.147, 0.147, 0.148, 0.157, 0.195, 0.217, 0.232, 0.235, 0.241, 0.243, 0.247, 0.249, 0.251, 0.243, 0.217, 0.206, 0.201, 0.202, 0.199, 0.198, 0.196, 0.196, 0.195},
        {0.221, 0.221, 0.221, 0.221, 0.222, 0.219, 0.220, 0.220, 0.222, 0.222, 0.221, 0.235, 0.268, 0.282, 0.298, 0.298, 0.304, 0.303, 0.308, 0.307, 0.314, 0.308, 0.288, 0.281, 0.275, 0.277, 0.274, 0.275, 0.272, 0.273},
        {0.164, 0.164, 0.164, 0.164, 0.164, 0.165, 0.162, 0.162, 0.162, 0.164, 0.161, 0.149, 0.149, 0.163, 0.166, 0.175, 0.172, 0.176, 0.173, 0.177, 0.177, 0.193, 0.195, 0.186, 0.184, 0.180, 0.183, 0.180, 0.182, 0.179},
        {0.122, 0.122, 0.122, 0.121, 0.122, 0.122, 0.122, 0.120, 0.120, 0.120, 0.120, 0.108, 0.096, 0.092, 0.099, 0.098, 0.105, 0.101, 0.105, 0.100, 0.106, 0.110, 0.126, 0.125, 0.122, 0.121, 0.118, 0.121, 0.118, 0.121},
        {0.090, 0.090, 0.090, 0.090, 0.090, 0.091, 0.091, 0.091, 0.089, 0.088, 0.087, 0.082, 0.068, 0.061, 0.056, 0.061, 0.059, 0.064, 0.059, 0.063, 0.059, 0.068, 0.070, 0.083, 0.080, 0.081, 0.079, 0.078, 0.080, 0.078},
        {0.067, 0.067, 0.067, 0.067, 0.067, 0.067, 0.067, 0.067, 0.067, 0.066, 0.064, 0.059, 0.053, 0.042, 0.040, 0.034, 0.038, 0.035, 0.040, 0.035, 0.040, 0.037, 0.046, 0.045, 0.056, 0.051, 0.055, 0.051, 0.053, 0.053},
        {0.191, 0.191, 0.191, 0.191, 0.191, 0.192, 0.192, 0.192, 0.192, 0.192, 0.189, 0.172, 0.149, 0.128, 0.107, 0.093, 0.079, 0.073, 0.067, 0.066, 0.062, 0.067, 0.069, 0.078, 0.081, 0.091, 0.093, 0.099, 0.099, 0.101}
    };

    std::vector<std::vector<float> > area2_catch_proportion = {
        {0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.002, 0.002, 0.002, 0.002, 0.002, 0.003, 0.003, 0.003, 0.003, 0.003, 0.002, 0.002, 0.002, 0.002, 0.002, 0.002, 0.002, 0.002, 0.002, 0.002},
        {0.007, 0.007, 0.007, 0.007, 0.007, 0.007, 0.007, 0.007, 0.007, 0.007, 0.009, 0.010, 0.011, 0.012, 0.012, 0.012, 0.013, 0.013, 0.013, 0.013, 0.012, 0.011, 0.010, 0.010, 0.010, 0.010, 0.010, 0.010, 0.010, 0.010},
        {0.035, 0.035, 0.036, 0.035, 0.035, 0.036, 0.036, 0.036, 0.036, 0.036, 0.044, 0.054, 0.060, 0.062, 0.064, 0.065, 0.066, 0.067, 0.067, 0.068, 0.060, 0.055, 0.052, 0.052, 0.051, 0.051, 0.050, 0.050, 0.050, 0.050},
        {0.156, 0.156, 0.157, 0.157, 0.155, 0.156, 0.157, 0.158, 0.159, 0.159, 0.173, 0.211, 0.233, 0.248, 0.250, 0.256, 0.258, 0.262, 0.263, 0.265, 0.254, 0.230, 0.219, 0.213, 0.214, 0.211, 0.211, 0.209, 0.209, 0.208},
        {0.201, 0.200, 0.200, 0.200, 0.200, 0.198, 0.198, 0.199, 0.201, 0.200, 0.195, 0.204, 0.230, 0.241, 0.254, 0.253, 0.258, 0.257, 0.261, 0.260, 0.271, 0.269, 0.252, 0.247, 0.241, 0.244, 0.241, 0.242, 0.239, 0.240},
        {0.155, 0.155, 0.155, 0.155, 0.155, 0.155, 0.153, 0.153, 0.153, 0.155, 0.149, 0.135, 0.133, 0.145, 0.148, 0.156, 0.153, 0.156, 0.153, 0.156, 0.159, 0.176, 0.178, 0.171, 0.169, 0.165, 0.167, 0.165, 0.167, 0.164},
        {0.115, 0.115, 0.115, 0.115, 0.115, 0.116, 0.116, 0.114, 0.113, 0.113, 0.112, 0.098, 0.087, 0.083, 0.088, 0.088, 0.094, 0.090, 0.093, 0.089, 0.096, 0.100, 0.115, 0.115, 0.113, 0.111, 0.109, 0.111, 0.109, 0.112},
        {0.085, 0.085, 0.085, 0.086, 0.086, 0.086, 0.086, 0.086, 0.085, 0.084, 0.081, 0.074, 0.062, 0.055, 0.050, 0.054, 0.052, 0.057, 0.053, 0.056, 0.054, 0.062, 0.065, 0.077, 0.074, 0.075, 0.073, 0.072, 0.074, 0.072},
        {0.063, 0.063, 0.063, 0.063, 0.064, 0.064, 0.064, 0.064, 0.064, 0.063, 0.060, 0.054, 0.048, 0.038, 0.035, 0.030, 0.034, 0.031, 0.035, 0.031, 0.036, 0.034, 0.043, 0.041, 0.052, 0.047, 0.051, 0.047, 0.049, 0.049},
        {0.181, 0.181, 0.181, 0.181, 0.182, 0.182, 0.182, 0.182, 0.182, 0.182, 0.176, 0.157, 0.134, 0.115, 0.096, 0.083, 0.071, 0.065, 0.059, 0.059, 0.056, 0.062, 0.063, 0.072, 0.075, 0.084, 0.086, 0.092, 0.092, 0.094}
    };

    std::vector<std::vector<float> > area1_survey_proportion = {
        {0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.002, 0.002, 0.002, 0.002, 0.002, 0.003, 0.003, 0.003, 0.003, 0.003, 0.002, 0.002, 0.002, 0.002, 0.002, 0.002, 0.002, 0.002, 0.002, 0.002},
        {0.007, 0.007, 0.007, 0.007, 0.007, 0.007, 0.007, 0.007, 0.007, 0.007, 0.009, 0.010, 0.011, 0.012, 0.012, 0.012, 0.013, 0.013, 0.013, 0.013, 0.012, 0.011, 0.010, 0.010, 0.010, 0.010, 0.010, 0.010, 0.010, 0.010},
        {0.035, 0.035, 0.036, 0.035, 0.035, 0.036, 0.036, 0.036, 0.036, 0.036, 0.044, 0.054, 0.060, 0.062, 0.064, 0.065, 0.066, 0.067, 0.067, 0.068, 0.060, 0.055, 0.052, 0.052, 0.051, 0.051, 0.050, 0.050, 0.050, 0.050},
        {0.156, 0.156, 0.157, 0.157, 0.155, 0.156, 0.157, 0.158, 0.159, 0.159, 0.173, 0.211, 0.233, 0.248, 0.250, 0.256, 0.258, 0.262, 0.263, 0.265, 0.254, 0.230, 0.219, 0.213, 0.214, 0.211, 0.211, 0.209, 0.209, 0.208},
        {0.201, 0.200, 0.200, 0.200, 0.200, 0.198, 0.198, 0.199, 0.201, 0.200, 0.195, 0.204, 0.230, 0.241, 0.254, 0.253, 0.258, 0.257, 0.261, 0.260, 0.271, 0.269, 0.252, 0.247, 0.241, 0.244, 0.241, 0.242, 0.239, 0.240},
        {0.155, 0.155, 0.155, 0.155, 0.155, 0.155, 0.153, 0.153, 0.153, 0.155, 0.149, 0.135, 0.133, 0.145, 0.148, 0.156, 0.153, 0.156, 0.153, 0.156, 0.159, 0.176, 0.178, 0.171, 0.169, 0.165, 0.167, 0.165, 0.167, 0.164},
        {0.115, 0.115, 0.115, 0.115, 0.115, 0.116, 0.116, 0.114, 0.113, 0.113, 0.112, 0.098, 0.087, 0.083, 0.088, 0.088, 0.094, 0.090, 0.093, 0.089, 0.096, 0.100, 0.115, 0.115, 0.113, 0.111, 0.109, 0.111, 0.109, 0.112},
        {0.085, 0.085, 0.085, 0.086, 0.086, 0.086, 0.086, 0.086, 0.085, 0.084, 0.081, 0.074, 0.062, 0.055, 0.050, 0.054, 0.052, 0.057, 0.053, 0.056, 0.054, 0.062, 0.065, 0.077, 0.074, 0.075, 0.073, 0.072, 0.074, 0.072},
        {0.063, 0.063, 0.063, 0.063, 0.064, 0.064, 0.064, 0.064, 0.064, 0.063, 0.060, 0.054, 0.048, 0.038, 0.035, 0.030, 0.034, 0.031, 0.035, 0.031, 0.036, 0.034, 0.043, 0.041, 0.052, 0.047, 0.051, 0.047, 0.049, 0.049},
        {0.181, 0.181, 0.181, 0.181, 0.182, 0.182, 0.182, 0.182, 0.182, 0.182, 0.176, 0.157, 0.134, 0.115, 0.096, 0.083, 0.071, 0.065, 0.059, 0.059, 0.056, 0.062, 0.063, 0.072, 0.075, 0.084, 0.086, 0.092, 0.092, 0.094}
    };


    std::vector<std::vector<float> > area2_survey_proportion = {
        {0.002, 0.002, 0.002, 0.002, 0.002, 0.002, 0.002, 0.002, 0.002, 0.002, 0.002, 0.002, 0.002, 0.002, 0.002, 0.002, 0.002, 0.002, 0.002, 0.002, 0.002, 0.002, 0.002, 0.002, 0.002, 0.002, 0.002, 0.002, 0.002, 0.002},
        {0.008, 0.008, 0.008, 0.008, 0.008, 0.008, 0.008, 0.008, 0.008, 0.008, 0.008, 0.009, 0.009, 0.010, 0.010, 0.010, 0.011, 0.011, 0.011, 0.011, 0.011, 0.011, 0.011, 0.011, 0.011, 0.011, 0.011, 0.011, 0.011, 0.011},
        {0.042, 0.041, 0.040, 0.038, 0.038, 0.038, 0.038, 0.038, 0.038, 0.038, 0.041, 0.043, 0.047, 0.049, 0.050, 0.052, 0.052, 0.053, 0.054, 0.054, 0.055, 0.055, 0.054, 0.054, 0.054, 0.053, 0.053, 0.053, 0.053, 0.052},
        {0.157, 0.157, 0.157, 0.157, 0.149, 0.150, 0.151, 0.151, 0.151, 0.151, 0.156, 0.165, 0.178, 0.189, 0.195, 0.197, 0.201, 0.203, 0.205, 0.206, 0.208, 0.209, 0.206, 0.202, 0.201, 0.202, 0.200, 0.200, 0.198, 0.198},
        {0.196, 0.197, 0.198, 0.198, 0.200, 0.190, 0.192, 0.193, 0.193, 0.194, 0.193, 0.198, 0.206, 0.222, 0.230, 0.235, 0.235, 0.240, 0.239, 0.243, 0.242, 0.244, 0.244, 0.239, 0.236, 0.234, 0.236, 0.233, 0.235, 0.232},
        {0.153, 0.153, 0.154, 0.154, 0.156, 0.158, 0.150, 0.152, 0.153, 0.151, 0.152, 0.148, 0.147, 0.150, 0.161, 0.164, 0.168, 0.165, 0.169, 0.167, 0.170, 0.168, 0.172, 0.172, 0.168, 0.167, 0.165, 0.168, 0.165, 0.167},
        {0.114, 0.114, 0.114, 0.115, 0.116, 0.118, 0.119, 0.113, 0.114, 0.115, 0.112, 0.112, 0.104, 0.101, 0.101, 0.109, 0.108, 0.112, 0.108, 0.112, 0.108, 0.112, 0.110, 0.117, 0.115, 0.113, 0.112, 0.111, 0.114, 0.111},
        {0.085, 0.085, 0.085, 0.085, 0.086, 0.087, 0.088, 0.089, 0.084, 0.086, 0.085, 0.081, 0.079, 0.070, 0.068, 0.067, 0.073, 0.070, 0.074, 0.069, 0.074, 0.069, 0.074, 0.072, 0.080, 0.076, 0.076, 0.075, 0.074, 0.077},
        {0.063, 0.063, 0.063, 0.063, 0.064, 0.065, 0.065, 0.066, 0.067, 0.063, 0.064, 0.062, 0.057, 0.054, 0.045, 0.046, 0.043, 0.048, 0.044, 0.049, 0.043, 0.048, 0.043, 0.050, 0.046, 0.055, 0.049, 0.052, 0.050, 0.050},
        {0.179, 0.179, 0.180, 0.180, 0.182, 0.185, 0.187, 0.189, 0.191, 0.192, 0.188, 0.181, 0.170, 0.153, 0.138, 0.119, 0.107, 0.096, 0.093, 0.087, 0.087, 0.082, 0.084, 0.081, 0.087, 0.087, 0.095, 0.095, 0.099, 0.099}
    };


    netcdf_output out("mas_case_1.nc");
    out.add_global_attribute("model", "mas case study 1");
    out.add_global_attribute("workgroup", "mas worksop");
    out.add_global_attribute("date", "10/19/2016");

    out.add_dimension("years", 30);
    out.add_dimension("seasons", 1);
    out.add_dimension("ages", 10);

    //
    out.add_variable("catch_biomass_area_1", "float",{"years"});
    out.add_variable_attribute("catch_biomass_area_1", "units", "MT");
    out.add_variable_attribute("catch_biomass_area_1", "data_object_type", "catch_biomass");
    out.add_variable_attribute("catch_biomass_area_1", "sex", "undifferentiated");
    out.add_variable_attribute("catch_biomass_area_1", "area", "1");
    //
    out.add_variable("catch_biomass_area_2", "float",{"years"});
    out.add_variable_attribute("catch_biomass_area_2", "data_object_type", "catch_biomass");
    out.add_variable_attribute("catch_biomass_area_2", "sex", "undifferentiated");
    out.add_variable_attribute("catch_biomass_area_2", "units", "MT");
    out.add_variable_attribute("catch_biomass_area_2", "area", "2");

    out.add_variable("survey_biomass_area_1", "float",{"years"});
    out.add_variable_attribute("survey_biomass_area_1", "data_object_type", "catch_biomass");
    out.add_variable_attribute("survey_biomass_area_1", "sex", "undifferentiated");
    out.add_variable_attribute("survey_biomass_area_1", "units", "kg");
    out.add_variable_attribute("survey_biomass_area_1", "area", "1");

    out.add_variable("survey_biomass_area_2", "float",{"years"});
    out.add_variable_attribute("survey_biomass_area_2", "data_object_type", "catch_biomass");
    out.add_variable_attribute("survey_biomass_area_2", "sex", "undifferentiated");
    out.add_variable_attribute("survey_biomass_area_2", "units", "kg");
    out.add_variable_attribute("survey_biomass_area_2", "area", "2");


    out.add_variable("catch_proportions_at_age_area_1", "float",{"years", "seasons", "ages"});
    out.add_variable_attribute("catch_proportions_at_age_area_1", "data_object_type", "catch_proportion_at_age");
    out.add_variable_attribute("catch_proportions_at_age_area_1", "units", "proportion");
    out.add_variable_attribute("catch_proportions_at_age_area_1", "area", "1");

    out.add_variable("catch_proportions_at_age_area_2", "float",{"years", "seasons", "ages"});
    out.add_variable_attribute("catch_proportions_at_age_area_2", "units", "proportion");
    out.add_variable_attribute("catch_proportions_at_age_area_2", "data_object_type", "catch_proportion_at_age");
    out.add_variable_attribute("catch_proportions_at_age_area_2", "area", "2");

    out.add_variable("survey_proportions_at_age_area_1", "float",{"years", "seasons", "ages"});
    out.add_variable_attribute("survey_proportions_at_age_area_1", "units", "proportion");
    out.add_variable_attribute("survey_proportions_at_age_area_1", "data_object_type", "catch_proportion_at_age");
    out.add_variable_attribute("survey_proportions_at_age_area_1", "area", "1");

    out.add_variable("survey_proportions_at_age_area_2", "float",{"years", "seasons", "ages"});
    out.add_variable_attribute("survey_proportions_at_age_area_2", "data_object_type", "catch_proportion_at_age");
    out.add_variable_attribute("survey_proportions_at_age_area_2", "units", "proportion");
    out.add_variable_attribute("survey_proportions_at_age_area_2", "area", "2");

    //
    out.create();


    for (int i = 0; i < out.dimension_size("years"); i++) {
        out.write_float("catch_biomass_area_1", area1_biomass[i], i);
        out.write_float("catch_biomass_area_2", area2_biomass[i], i);
        out.write_float("survey_biomass_area_1", area1_survey_biomass[i], i);
        out.write_float("survey_biomass_area_2", area2_survey_biomass[i], i);
    }

    std::cout << area1_catch_proportion.size() << " x " << area1_catch_proportion[0].size() << "\n";

    for (int i = 0; i < out.dimension_size("years"); i++) {
        for (int j = 0; j < out.dimension_size("seasons"); j++) {
            for (int k = 0; k < out.dimension_size("ages"); k++) {
                out.write_float("catch_proportions_at_age_area_1", area1_catch_proportion[k][i], i, j, k);
                out.write_float("catch_proportions_at_age_area_2", area2_catch_proportion[k][i], i, j, k);
                out.write_float("survey_proportions_at_age_area_1", area1_survey_proportion[k][i], i, j, k);
                out.write_float("survey_proportions_at_age_area_2", area2_survey_proportion[k][i], i, j, k);
            }
        }
    }
    out.close();

}

/*
 * 
 */
int main(int argc, char** argv) {

    mas::Information<double> info1;
    info1.ParseConfig("mas_cas31.json");
    info1.ParseData("mas_case_1.nc");
    info1.CreateModel();
    typename std::unordered_map<int, std::shared_ptr<mas::Population<double> > >::iterator it;
    std::unordered_map<int, std::shared_ptr<mas::Population<double> > >& pops =
            info1.GetPopulations();

    for (it = pops.begin(); it != pops.end(); ++it) {
        (*it).second->Prepare();
    }


    for (it = pops.begin(); it != pops.end(); ++it) {
        (*it).second->Evaluate();
    }
    
    for (it = pops.begin(); it != pops.end(); ++it) {
        (*it).second->Show();
    }


    exit(0);
    //    
    ////    create();
    //    exit(0);
    //    netcdf_output out("test.nc");
    //    out.add_dimension("x", 100);
    //    out.add_dimension("y", 100);
    //    out.add_variable("catch", "float",{"x", "y"});
    //    out.add_variable_attribute("catch", "units", "metric_tonnes");
    //    out.add_global_attribute("what up?", "nothing!");
    //    out.create();
    //    for (int i = 0; i < out.dimension_size("x"); i++) {
    //        for (int j = 0; j < out.dimension_size("y"); j++) {
    //            out.write_float("catch", static_cast<float> (i * out.dimension_size("x") + j), i, j);
    //        }
    //    }
    //
    //    out.close();
    //
    //
    //
    //    netcdf_input input("test.nc");
    //
    //    std::multimap<std::string, NcVar> vars = input.get_variables();
    //    typename std::multimap<std::string, NcVar>::iterator iit;
    //    for (iit = vars.begin(); iit != vars.end(); ++iit) {
    //        std::cout << (*iit).first << "\n";
    //
    //        int dims = (*iit).second.getDimCount();
    //
    //        switch (dims) {
    //
    //            case 1:
    //
    //                for (int i = 0; i < (*iit).second.getDim(0).getSize(); i++) {
    //                    std::cout << input.read_float((*iit).second.getName(), i) << " ";
    //                }
    //                break;
    //            case 2:
    //                for (int i = 0; i < (*iit).second.getDim(0).getSize(); i++) {
    //                    for (int j = 0; j < (*iit).second.getDim(1).getSize(); j++) {
    //                        std::cout << input.read_float((*iit).second.getName(), i, j) << " ";
    //                    }
    //                    std::cout << "\n";
    //                }
    //                break;
    //            case 3:
    //
    //                for (int i = 0; i < (*iit).second.getDim(0).getSize(); i++) {
    //                    for (int j = 0; j < (*iit).second.getDim(1).getSize(); j++) {
    //                        for (int k = 0; k < (*iit).second.getDim(2).getSize(); k++) {
    //                            std::cout << input.read_float((*iit).second.getName(), i, j, k) << " ";
    //                        }
    //                        std::cout << "\n";
    //                    }
    //                    std::cout << "\n";
    //                }
    //                break;
    //
    //            case 4:
    //                for (int i = 0; i < (*iit).second.getDim(0).getSize(); i++) {
    //                    for (int j = 0; j < (*iit).second.getDim(1).getSize(); j++) {
    //                        for (int k = 0; k < (*iit).second.getDim(2).getSize(); k++) {
    //                            for (int l = 0; l < (*iit).second.getDim(3).getSize(); l++) {
    //                                std::cout << input.read_float((*iit).second.getName(), i, j, k, l) << " ";
    //                            }
    //                            std::cout << "\n";
    //                        }
    //                        std::cout << "\n";
    //                    }
    //                    std::cout << "\n";
    //                }
    //                break;
    //
    //        }
    //        std::cout << "\n";
    //
    //    }
    //
    //    exit(0);



    mas::Information<double> info;
    info.ParseConfig("config.json");
    //    info.ParseData("mas")
    info.CreateModel();

    std::unordered_map<int, std::shared_ptr<mas::Population<double> > >& popps = info.GetPopulations();
    typename std::unordered_map<int, std::shared_ptr<mas::Population<double> > >::iterator pit;

    for (pit = popps.begin(); pit != popps.end(); ++pit) {
        (*pit).second->Prepare();
    }

    for (pit = popps.begin(); pit != popps.end(); ++pit) {
        (*pit).second->Evaluate();
    }
    exit(0);

    std::shared_ptr<mas::GrowthBase<double> > growth(new mas::VonBertalanffy<double>());
    mas::VonBertalanffy<double>* vb = (mas::VonBertalanffy<double>*)growth.get();

    vb->l_inf = 50.0;

    vb->a_min = -.02;

    for (double a = 0; a < 6; a++) {
        vb->k = 1.0;
        std::cout << vb->Evaluate(a) << ",";
        vb->k = .5;
        std::cout << vb->Evaluate(a) << ",";
        vb->k = .2;
        std::cout << vb->Evaluate(a) << "\n";
    }

    return 0;
}

