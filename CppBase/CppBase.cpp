//// CppBase.cpp : This file contains the 'main' function. Program execution begins and ends there.
////
//
//#include "CSVHelper.h"
//#include <iostream>
//#include <list>
//#include <string>
//#include "json.hpp"
//
////std::vector<double> splitString(std::string input) {
////    std::vector<double> result;
////
////    input.erase(std::remove(input.begin(), input.end(), '['), input.end());
////    input.erase(std::remove(input.begin(), input.end(), ']'), input.end());
////
////    std::stringstream stream(input);
////    std::string current;
////    
////    while (getline(stream, current, ',')) {
////        result.push_back(std::stod(current));
////    }
////
////    return result;
////}
////
////std::list<std::string> splitStringMatrix(std::string input) {
////    std::vector<std::vector<double>> result;
////
////    // Remove the first and last chars, which are respectively [ and ]
////    input = input.substr(1, input.size() - 2);
////
////    std::stringstream stream(input);
////    std::string current;
////
////    while (s)
////}
//
//using json = nlohmann::json;
//
//std::vector<std::any> convertInputsFromJson(
//    std::list<std::string> inputs,
//    crow::json::rvalue jsonBody
//) {
//    std::vector<std::any> converted_inputs;
//
//    for (const std::string input : inputs) {
//        crow::json::rvalue input_rval = jsonBody[input];
//        std::vector<double> resVector;
//        std::vector<std::vector<double>> resMatrix;
//
//        switch (input_rval.t()) {
//        case crow::json::type::Number:
//            // Convert to correct number type
//            break;
//        case crow::json::type::String:
//            std::string input_str = input_rval.s();
//
//            if (input_str[0] == '[') {
//                if (input_str[1] == '[') {
//                    // It is a matrix
//                    json json_matrix = json::parse(input_str);
//
//                    std::vector<std::vector<double>> matrix = json_matrix.get<std::vector<std::vector<double>>>();
//
//                    converted_inputs.push_back(matrix);
//                }
//                else {
//                    // It is a vector
//                    json json_vector = json::parse(input_str);
//                    
//                    std::vector<double> vector = json_vector.get<std::vector<double>>();
//
//                    converted_inputs.push_back(vector);
//                }
//            }
//            break;
//        }
//    }
//    return converted_inputs;
//}
//
//std::vector<std::any> convertInputs(
//        std::list<std::string> inputs,
//        crow::json::rvalue jsonBody
//    ) {
//    std::vector<std::any> converted_inputs;
//
//    // Get JSON for each input for each function and convert
//    for (const std::string input : inputs) {
//        crow::json::rvalue input_rval = jsonBody[input];
//        std::vector<double> resVector;
//        std::vector<std::vector<double>> resMatrix;
//
//        std::cout << crow::json::get_type_str(input_rval.t()) << std::endl;
//
//        switch (input_rval.t()) {
//        case crow::json::type::Number:
//            // It is a single number
//            break;
//        case crow::json::type::List:
//            // It is either a single string with a number, or comma separated numbers, assuming decimal numbers are written with a dot, e.g. 2.345
//            std::string row = input_rval.lo()[0].s();
//
//            if (row.find(',') == std::string::npos) {
//                // Comma not found, single number, convert to Vector
//                resVector = convertToDoubleVector(input_rval.lo());
//                converted_inputs.push_back(resVector);
//            }
//            else {
//                // Comma found, convert to Matrix (2D vector)
//                std::vector<std::string> resTemp;
//                for (const crow::json::rvalue row : input_rval.lo()) {
//                    std::string rowStr = row.s();
//                    resTemp.push_back(rowStr);
//                }
//                resMatrix = convertToDoubleMatrix(resTemp);
//                converted_inputs.push_back(resMatrix);
//            }
//            break;
//        }
//    }
//
//    return converted_inputs;
//}
//
//#pragma region Target CPP functions
//
//std::vector<std::vector<double>> rad_bot(
//    const std::vector<double>& zmax,
//    const std::vector<std::vector<double>>& kd,
//    const std::vector<std::vector<double>>& par,
//    const std::vector<std::vector<double>>& height) {
//
//    int nt = kd.size();           // Number of rows
//    int ns = zmax.size();          // Number of columns
//    std::vector<std::vector<double>> rad(nt, std::vector<double>(ns, 0.0));
//
//    for (int is = 0; is < ns; ++is) {
//        for (int it = 0; it < nt; ++it) {
//            double zt = zmax[is] + height[it][is];
//            if (zt > 0.0) {
//                rad[it][is] = par[it][is] * std::exp(-kd[it][is] * zt);
//            }
//            else {
//                rad[it][is] = par[it][is];
//            }
//        }
//    }
//    return rad;
//}
//
//#pragma endregion
//
//// This file functions as a boiler plate for the CppModifier.
//int main()
//{
//
//    std::cout << "Hello World!\n";
//
//    crow::SimpleApp app;
//
//    CROW_ROUTE(app, "/")([]() {
//        return "Hello, Crow!";
//    });
//
//
//    CROW_ROUTE(app, "/test")([]() {
//        return "Hello test";
//    });
//
//    // # MARKER FOR AUTO GENERATION #
//
//
//    CROW_ROUTE(app, "/upload_csv").methods("POST"_method)
//        ([](const crow::request& req) {
//
//        // Extract CSV
//        std::string data = req.body;
//
//        //std::cout << data << std::endl;
//
//        //std::vector<std::vector<double>> result = parseCSVFromString(data);
//
//        return crow::response(200, "Received CSV file");
//    });
//
//    CROW_ROUTE(app, "/rad_bot")
//        .methods("POST"_method)
//        ([](const crow::request& req) {
//
//        std::cout << "Received request for rad_bot" << std::endl;
//
//        crow::json::rvalue jsonBody = crow::json::load(req.body);
//
//        if (!jsonBody) {
//            throw std::invalid_argument("Invalid JSON format");
//        }
//
//        // ---- Auto generate inputs -----
//        std::list<std::string> inputs = { "zmax", "kd", "par", "height" };
//        // ----- End of auto generate inputs -----
//
//        std::vector<std::any> converted_inputs = convertInputsFromJson(inputs, jsonBody);
//
//        // ----- Auto generate assigned converted results and function call -----
//        std::vector<double> zmax = std::any_cast<std::vector<double>>(converted_inputs[0]);
//        std::vector<std::vector<double>> kd = std::any_cast<std::vector<std::vector<double>>>(converted_inputs[1]);
//        std::vector<std::vector<double>> pa = std::any_cast<std::vector<std::vector<double>>>(converted_inputs[2]);
//        std::vector<std::vector<double>> height = std::any_cast<std::vector<std::vector<double>>>(converted_inputs[3]);
//
//        std::vector<std::vector<double>> result = rad_bot(zmax, kd, pa, height);
//        // ----- End of auto generate assigned converted results and function call -----
//        
//
//        json final_result = result;
//        std::string csv_result = toCSV(result);
//
//        stringToCSV(csv_result, "test_csv_result.csv");
//
//        crow::response response(final_result.dump());
//        response.add_header("Content-Type", "application/json");
//
//        std::cout << "Sending response for rad_bot" << std::endl;
//
//        return response;
//    });
//
//    // For each function, create a crow route
//
//    app.port(8080).multithreaded().run();
//}
//
//// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
//// Debug program: F5 or Debug > Start Debugging menu
//
//// Tips for Getting Started: 
////   1. Use the Solution Explorer window to add/manage files
////   2. Use the Team Explorer window to connect to source control
////   3. Use the Output window to see build output and other messages
////   4. Use the Error List window to view errors
////   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
////   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

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
std::vector<std::vector<double>> intPP_mixed(
    const std::vector<double>& zmax,
    const std::vector<std::vector<double>>& kd,
    const std::vector<std::vector<double>>& par,
    const std::vector<std::vector<double>>& alfa,
    const std::vector<std::vector<double>>& eopt,
    const std::vector<std::vector<double>>& pmax,
    const std::vector<std::vector<double>>& height) {

    static const double xg[] = { 0.04691008, 0.23076534, 0.5, 0.76923466, 0.95308992 };
    static const double wg[] = { 0.1184634, 0.2393143, 0.2844444, 0.2393143, 0.1184634 };

    int nt = kd.size();            // number of rows
    int ns = zmax.size();           // number of columns
    std::vector<std::vector<double>> pp(nt, std::vector<double>(ns, 0.0));

    for (int is = 0; is < ns; ++is) {
        for (int it = 0; it < nt; ++it) {
            double zt = zmax[is] + height[it][is];
            double total = 0.0;
            if (zt > 0.0 && (par[it][is] > 0)) {
                double zg = -std::log(0.001) / kd[it][is];
                if (zg > zt) zg = zt;

                double f1 = 1.0 / (alfa[it][is] * std::pow(eopt[it][is], 2.0));
                double f2 = 1.0 / pmax[it][is] - 2.0 / (alfa[it][is] * eopt[it][is]);
                double f3 = 1.0 / alfa[it][is];

                for (int j = 0; j < 5; ++j) {
                    double zz = xg[j] * zg;
                    double parz = par[it][is] * std::exp(-kd[it][is] * zz);
                    double ps = parz / (f1 * std::pow(parz, 2.0) + f2 * parz + f3);
                    total += wg[j] * ps * zg;
                }
            }
            pp[it][is] = total;
        }
    }
    return pp;
}

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

std::vector<std::vector<double>> intPP_exp(
    const std::vector<double>& zmax,
    const std::vector<double>& kd,
    const std::vector<double>& pMud,
    const std::vector<std::vector<double>>& par,
    const std::vector<std::vector<double>>& alfa,
    const std::vector<std::vector<double>>& eopt,
    const std::vector<std::vector<double>>& pmax) {

    static const double xg[] = { 0.04691008, 0.23076534, 0.5, 0.76923466, 0.95308992 };
    static const double wg[] = { 0.1184634, 0.2393143, 0.2844444, 0.2393143, 0.1184634 };

    int nt = par.size();          // Number of rows
    int ns = zmax.size();          // Number of columns
    std::vector<std::vector<double>> pp(nt, std::vector<double>(ns, 0.0));

    for (int is = 0; is < ns; ++is) {
        for (int it = 0; it < nt; ++it) {
            double zt = zmax[is];
            double total = 0.0;
            if (zt > 0.0 && (par[it][is] > 0)) {
                double zg = -std::log(0.001) / kd[is];
                if (zg > zt) zg = zt;

                for (int j = 0; j < 5; ++j) {
                    double zz = xg[j] * zg;
                    double pChl_z = (1 - pMud[is]) + pMud[is] * 2.0 * std::exp(1.0) * std::exp(-2.0 * std::exp(1.0) * zz / zt);
                    double alfa_z = alfa[it][is] * pChl_z;
                    double pmax_z = pmax[it][is] * pChl_z;
                    double eopt_z = eopt[it][is];

                    double f1 = 1.0 / (alfa_z * std::pow(eopt_z, 2.0));
                    double f2 = 1.0 / pmax_z - 2.0 / (alfa_z * eopt_z);
                    double f3 = 1.0 / alfa_z;

                    double parz = par[it][is] * std::exp(-kd[is] * zz);
                    double ps = parz / (f1 * std::pow(parz, 2.0) + f2 * parz + f3);
                    total += wg[j] * ps * zg;
                }
            }
            pp[it][is] = total;
        }
    }
    return pp;
}

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
    CROW_ROUTE(app, "/intPP_mixed")
        .methods("POST"_method)
        ([](const crow::request& req) {

        std::cout << "Received request for intPP_mixed" << std::endl;

        crow::json::rvalue jsonBody = crow::json::load(req.body);

        if (!jsonBody) {
            throw std::invalid_argument("Invalid JSON format");
        }

        // ----- Auto generate inputs -----
        std::list<std::string> inputs = { "zmax","kd","par","alfa","eopt","pmax","height" };
        // ----- End of auto generate inputs -----

        auto start_conv = std::chrono::high_resolution_clock::now();
        std::vector<std::any> converted_inputs = convertInputs(inputs, jsonBody);

        // ----- Auto generate assigned converted results and function call -----
        const std::vector<double>& zmax = std::any_cast<const std::vector<double> &>(converted_inputs[0]);
        const std::vector<std::vector<double>>& kd = std::any_cast<const std::vector<std::vector<double>> &>(converted_inputs[1]);
        const std::vector<std::vector<double>>& par = std::any_cast<const std::vector<std::vector<double>> &>(converted_inputs[2]);
        const std::vector<std::vector<double>>& alfa = std::any_cast<const std::vector<std::vector<double>> &>(converted_inputs[3]);
        const std::vector<std::vector<double>>& eopt = std::any_cast<const std::vector<std::vector<double>> &>(converted_inputs[4]);
        const std::vector<std::vector<double>>& pmax = std::any_cast<const std::vector<std::vector<double>> &>(converted_inputs[5]);
        const std::vector<std::vector<double>>& height = std::any_cast<const std::vector<std::vector<double>> &>(converted_inputs[6]);
        auto end_conv = std::chrono::high_resolution_clock::now();
        auto duration_conv = std::chrono::duration_cast<std::chrono::milliseconds>(end_conv - start_conv);
        std::cout << "Conversion from json for intPP_mixed took: " << duration_conv.count() << std::endl;


        auto start_call = std::chrono::high_resolution_clock::now();
        std::vector<std::vector<double>> result = intPP_mixed(zmax, kd, par, alfa, eopt, pmax, height);
        auto end_call = std::chrono::high_resolution_clock::now();
        auto duration_call = std::chrono::duration_cast<std::chrono::milliseconds>(end_call - start_call);
        std::cout << "Call of function intPP_mixed took: " << duration_call.count() << std::endl;

        // ----- End of auto generate assigned converted results and function call -----

        json json_result = result;
        crow::response response(json_result.dump());
        response.add_header("Content-Type", "application/json");
        std::cout << "Sending response for intPP_mixed" << std::endl;

        return response;
    });

    CROW_ROUTE(app, "/rad_bot")
        .methods("POST"_method)
        ([](const crow::request& req) {

        std::cout << "Received request for rad_bot" << std::endl;

        crow::json::rvalue jsonBody = crow::json::load(req.body);

        if (!jsonBody) {
            throw std::invalid_argument("Invalid JSON format");
        }

        // ----- Auto generate inputs -----
        std::list<std::string> inputs = { "zmax","kd","par","height" };
        // ----- End of auto generate inputs -----

        std::vector<std::any> converted_inputs = convertInputs(inputs, jsonBody);

        // ----- Auto generate assigned converted results and function call -----
        const std::vector<double>& zmax = std::any_cast<const std::vector<double> &>(converted_inputs[0]);
        const std::vector<std::vector<double>>& kd = std::any_cast<const std::vector<std::vector<double>> &>(converted_inputs[1]);
        const std::vector<std::vector<double>>& par = std::any_cast<const std::vector<std::vector<double>> &>(converted_inputs[2]);
        const std::vector<std::vector<double>>& height = std::any_cast<const std::vector<std::vector<double>> &>(converted_inputs[3]);
        std::vector<std::vector<double>> result = rad_bot(zmax, kd, par, height);
        // ----- End of auto generate assigned converted results and function call -----

        json json_result = result;
        crow::response response(json_result.dump());
        response.add_header("Content-Type", "application/json");
        std::cout << "Sending response for rad_bot" << std::endl;

        return response;
    });

    CROW_ROUTE(app, "/intPP_exp")
        .methods("POST"_method)
        ([](const crow::request& req) {

        std::cout << "Received request for intPP_exp" << std::endl;

        crow::json::rvalue jsonBody = crow::json::load(req.body);

        if (!jsonBody) {
            throw std::invalid_argument("Invalid JSON format");
        }

        // ----- Auto generate inputs -----
        std::list<std::string> inputs = { "zmax","kd","pMud","par","alfa","eopt","pmax" };
        // ----- End of auto generate inputs -----

        std::vector<std::any> converted_inputs = convertInputs(inputs, jsonBody);

        // ----- Auto generate assigned converted results and function call -----
        const std::vector<double>& zmax = std::any_cast<const std::vector<double> &>(converted_inputs[0]);
        const std::vector<double>& kd = std::any_cast<const std::vector<double> &>(converted_inputs[1]);
        const std::vector<double>& pMud = std::any_cast<const std::vector<double> &>(converted_inputs[2]);
        const std::vector<std::vector<double>>& par = std::any_cast<const std::vector<std::vector<double>> &>(converted_inputs[3]);
        const std::vector<std::vector<double>>& alfa = std::any_cast<const std::vector<std::vector<double>> &>(converted_inputs[4]);
        const std::vector<std::vector<double>>& eopt = std::any_cast<const std::vector<std::vector<double>> &>(converted_inputs[5]);
        const std::vector<std::vector<double>>& pmax = std::any_cast<const std::vector<std::vector<double>> &>(converted_inputs[6]);
        std::vector<std::vector<double>> result = intPP_exp(zmax, kd, pMud, par, alfa, eopt, pmax);
        // ----- End of auto generate assigned converted results and function call -----

        json json_result = result;
        crow::response response(json_result.dump());
        response.add_header("Content-Type", "application/json");
        std::cout << "Sending response for intPP_exp" << std::endl;

        return response;
    });



    app.port(8080).multithreaded().run();
}

