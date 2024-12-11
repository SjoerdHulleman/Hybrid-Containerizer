// CppBase.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "crow_all.h"
#include <iostream>
#include <list>
#include <string>
#include "json.hpp"

using json = nlohmann::json;

std::vector<std::any> convertInputs(
    std::list<std::string> inputs,
    crow::json::rvalue jsonBody
) {
    std::vector<std::any> converted_inputs;

    for (const std::string input : inputs) {
        crow::json::rvalue input_rval = jsonBody[input];
        std::vector<double> resVector;
        std::vector<std::vector<double>> resMatrix;

        switch (input_rval.t()) {
        case crow::json::type::Number:
            // Convert to correct number type
            break;
        case crow::json::type::String:
            std::string input_str = input_rval.s();

            if (input_str[0] == '[') {
                if (input_str[1] == '[') {
                    // It is a matrix
                    json json_matrix = json::parse(input_str);

                    std::vector<std::vector<double>> matrix = json_matrix.get<std::vector<std::vector<double>>>();

                    converted_inputs.push_back(matrix);
                }
                else {
                    // It is a vector
                    json json_vector = json::parse(input_str);

                    std::vector<double> vector = json_vector.get<std::vector<double>>();

                    converted_inputs.push_back(vector);
                }
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
