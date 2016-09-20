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

/*
 * 
 */
int main(int argc, char** argv) {
    
    mas::Information<double> info;
    info.ParseConfig("config.json");
    info.CreateModel();
    
    std::shared_ptr<mas::GrowthBase<double> > growth(new mas::VonBertalanffy<double>());
    mas::VonBertalanffy<double>* vb = (mas::VonBertalanffy<double>*)growth.get();
    
    vb->l_inf = 50.0;
   
    vb->a_min = -.02;
    
    for(double a=0;  a < 6; a++){
         vb->k = 1.0;
        std::cout<<vb->Evaluate(a)<<",";
         vb->k = .5;
        std::cout<<vb->Evaluate(a)<<",";
        vb->k = .2;
        std::cout<<vb->Evaluate(a)<<"\n";
    }
    
    return 0;
}

