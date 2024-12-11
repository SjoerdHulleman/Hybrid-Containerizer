#pragma once
#include<iostream>
#include "crow_all.h"


std::vector<double> convertJSONToDoubleVector(const std::vector<crow::json::rvalue> jsonVec) {
    std::vector<double> doubleVec;
    for (crow::json::rvalue jsonVal : jsonVec) {
        // Access doubles with the d() function of Crow
        if (jsonVal.t() == crow::json::type::Number) {
            doubleVec.push_back(jsonVal.d());
        }
        // Access Strings with the s() function of Crow
        else if (jsonVal.t() == crow::json::type::String) {
            try {
                doubleVec.push_back(std::stod(jsonVal.s()));
            }
            catch (const std::invalid_argument e) {
                // This is usually the top row with names, we do not include this
               // doubleVec.push_back(0);
            }
            catch (...) {
                std::cerr << "Unknown eception on input:" << jsonVal << std::endl;
            }
        }
        else {
            std::cerr << "Unsupported JSON value type, skipped entry." << std::endl;
        }
    }
    return doubleVec;
}