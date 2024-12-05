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

#pragma region Target CPP functions

std::vector<std::vector<double>> rad_bot(
    const std::vector<double>& zmax,
    const std::vector<std::vector<double>>& kd,
    const std::vector<std::vector<double>>& par,
    const std::vector<std::vector<double>>& height) {

    int nt = kd.size();           // Number of rows
    int ns = zmax.size();          // Number of columns
    std::vector<std::vector<double>> rad(nt, std::vector<double>(ns, 0.0));

    for (int is = 0; is < ns; ++is) {
        for (int it = 0; it < nt; ++it) {
            double zt = zmax[is] + height[it][is];
            if (zt > 0.0) {
                rad[it][is] = par[it][is] * std::exp(-kd[it][is] * zt);
            }
            else {
                rad[it][is] = par[it][is];
            }
        }
    }
    return rad;
}

#pragma endregion

// This file functions as a boiler plate for the CppModifier.
int main()
{

    std::cout << "Hello World!\n";

    crow::SimpleApp app;

    CROW_ROUTE(app, "/")([]() {
        return "Hello, Crow!";
    });


    CROW_ROUTE(app, "/test")([]() {
        return "Hello test";
    });

    // # MARKER FOR AUTO GENERATION #


    CROW_ROUTE(app, "/upload_csv").methods("POST"_method)
        ([](const crow::request& req) {

        // Extract CSV
        std::string data = req.body;

        //std::cout << data << std::endl;

        //std::vector<std::vector<double>> result = parseCSVFromString(data);

        return crow::response(200, "Received CSV file");
    });

    CROW_ROUTE(app, "/rad_bot")
        .methods("POST"_method)
        ([](const crow::request& req) {

        std::cout << "Received request for rad_bot" << std::endl;

        crow::json::rvalue jsonBody = crow::json::load(req.body);

        if (!jsonBody) {
            throw std::invalid_argument("Invalid JSON format");
        }

        // ---- Auto generate inputs -----
        std::list<std::string> inputs = { "zmax", "kd", "par", "height" };
        // ----- End of auto generate inputs -----

        std::vector<std::any> converted_inputs = convertInputs(inputs, jsonBody);

        // ----- Auto generate assigned converted results and function call -----
        std::vector<double> zmax = std::any_cast<std::vector<double>>(converted_inputs[0]);
        std::vector<std::vector<double>> kd = std::any_cast<std::vector<std::vector<double>>>(converted_inputs[1]);
        std::vector<std::vector<double>> pa = std::any_cast<std::vector<std::vector<double>>>(converted_inputs[2]);
        std::vector<std::vector<double>> height = std::any_cast<std::vector<std::vector<double>>>(converted_inputs[3]);

        std::vector<std::vector<double>> result = rad_bot(zmax, kd, pa, height);
        // ----- End of auto generate assigned converted results and function call -----
        
        std::string csv_result = toCSV(result);

        stringToCSV(csv_result, "test_csv_result.csv");

        crow::response response(csv_result);
        response.add_header("Content-Type", "text/csv");

        std::cout << "Sending response for rad_bot" << std::endl;

        return response;
    });

    // For each function, create a crow route

    app.port(8080).multithreaded().run();
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
