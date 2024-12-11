// CppBase.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "CSVHelper.h"
#include <iostream>
#include <list>
#include <string>


std::vector<std::any> convertInputs(
        std::list<std::string> inputs,
        crow::json::rvalue jsonBody
    ) {
    std::vector<std::any> converted_inputs;

    // Get JSON for each input for each function and convert
    for (const std::string input : inputs) {
        crow::json::rvalue input_rval = jsonBody[input];
        std::vector<double> resVector;
        std::vector<std::vector<double>> resMatrix;

        switch (input_rval.t()) {
        case crow::json::type::Number:
            // It is a single number
            break;
        case crow::json::type::List:
            // It is either a single string with a number, or comma separated numbers, assuming decimal numbers are written with a dot, e.g. 2.345
            std::string row = input_rval.lo()[0].s();

            if (row.find(',') == std::string::npos) {
                // Comma not found, single number, convert to Vector
                resVector = convertToDoubleVector(input_rval.lo());
                converted_inputs.push_back(resVector);
            }
            else {
                // Comma found, convert to Matrix (2D vector)
                std::vector<std::string> resTemp;
                for (const crow::json::rvalue row : input_rval.lo()) {
                    std::string rowStr = row.s();
                    resTemp.push_back(rowStr);
                }
                resMatrix = convertToDoubleMatrix(resTemp);
                converted_inputs.push_back(resMatrix);
            }
            break;
        }
    }

    return converted_inputs;
}

// # MARKER: TARGET CPP FUNCTIONS #

// This file functions as a boiler plate for the CppModifier.
int main()
{

    std::cout << "Hello World!\n";

    crow::SimpleApp app;

    // Endpoints to test API running
    CROW_ROUTE(app, "/")([]() {
        return "Hello, Crow!";
    });


    CROW_ROUTE(app, "/ping")([]() {
        return "pong";
    });

    // # MARKER: API ENDPOINTS #
    

    app.port(8080).multithreaded().run();
}
