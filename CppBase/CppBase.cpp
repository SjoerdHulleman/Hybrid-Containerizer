// CppBase.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <crow.h>



//std::vector<std::vector<double>> parseCSVFromString(const std::string csv) {
//    std::vector<std::vector<double>> data;
//
//    std::stringstream stream = std::stringstream(csv);
//    std::string line;
//
//    while (stream >> line) {
//        std::stringstream lineStream = std::stringstream(line);
//        std::string cell;
//        std::vector<double> row;
//
//        while (std::getline(lineStream, cell, ',')) {
//            try {
//                // Convert string in cell to double
//                row.push_back(std::stod(cell));
//            }
//            catch (const std::invalid_argument& e) {
//                // Handle invalid conversion (e.g., non-numeric data)
//                std::cerr << "Warning: Invalid number found, setting to 0.0: " << cell << std::endl;
//                row.push_back(0.0);  // Handle invalid values by setting them to 0.0
//            }
//            catch (const std::out_of_range& e) {
//                // Handle numbers that are too large to fit in a double
//                std::cerr << "Warning: Out of range number, setting to 0.0: " << cell << std::endl;
//                row.push_back(0.0);  // Handle out-of-range values
//            }
//        }
//
//        data.push_back(row);
//    }
//
//    return data;
//}


//template <typename T>
//std::string toCSV(const std::vector<T>& vec) {
//    std::stringstream ss;
//    for (size_t i = 0; i < vec.size(); ++i) {
//        ss << vec[i];
//        if (i != vec.size() - 1) {
//            ss << ",";  // Add comma separator for each element except the last one
//        }
//    }
//    ss << "\n";  // New line after the vector
//    return ss.str();
//}
//
//template <typename T>
//std::string toCSV(const std::vector<std::vector<T>>& matrix) {
//    std::stringstream ss;
//    for (const auto& row : matrix) {
//        for (size_t i = 0; i < row.size(); ++i) {
//            ss << row[i];
//            if (i != row.size() - 1) {
//                ss << ",";  // Add comma separator for each element except the last one
//            }
//        }
//        ss << "\n";  // New line after each row
//    }
//    return ss.str();
//}

// TODO: Make more efficient/readable
std::vector<double> parseCSVLine(const std::string& csv) {
    std::vector<double> row;

    std::stringstream lineStream(csv);
    std::string cell;

    while (std::getline(lineStream, cell, ',')) {
        try {
            // Convert string in cell to double
            row.push_back(std::stod(cell));
        }
        catch (const std::invalid_argument& e) {
            // Handle invalid conversion (e.g., non-numeric data)
            std::cerr << "Warning: Invalid number found, setting to 0.0: " << cell << std::endl;
            row.push_back(0.0);  // Handle invalid values by setting them to 0.0
        }
        catch (const std::out_of_range& e) {
            // Handle numbers that are too large to fit in a double
            std::cerr << "Warning: Out of range number, setting to 0.0: " << cell << std::endl;
            row.push_back(0.0);  // Handle out-of-range values
        }
    }

    return row;
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

// TODO: Make more efficient/readable/generic
std::vector<std::vector<double>> convertToDouble2D(const std::vector<std::string>& strVec) {
    std::vector<std::vector<double>> doubleVec2D;

    for (const auto& rowStr : strVec) {
        std::vector<double> doubleRow;
        std::stringstream ss(rowStr);
        std::string element;

        // Split the string by commas and convert to double
        while (std::getline(ss, element, ',')) {
            try {
                doubleRow.push_back(std::stod(element)); // Convert the string element to double
            }
            catch (const std::invalid_argument& e) {
                doubleRow.push_back(0);
                std::cerr << "Invalid input string: '" << element << "' could not be converted to double." << std::endl;
            }
            catch (const std::out_of_range& e) {
                std::cerr << "Input string: '" << element << "' is out of range for a double." << std::endl;
            }
        }

        if (doubleRow.size() > 0) {
            doubleVec2D.push_back(doubleRow);
        }
    }

    return doubleVec2D;
}
// TODO: Make more efficient/readable/generic
std::vector<double> convertToDouble(const std::vector<crow::json::rvalue>& jsonVec) {
    std::vector<double> doubleVec;
    for (const auto& jsonVal : jsonVec) {
        if (jsonVal.t() == crow::json::type::Number) {
            doubleVec.push_back(jsonVal.d());
        }
        else if (jsonVal.t() == crow::json::type::String) {
            try {
                doubleVec.push_back(std::stod(jsonVal.s()));
            }
            catch (const std::invalid_argument& e) {
                std::cerr << "Invalid input string: '" << jsonVal.s() << "' could not be converted to double." << std::endl;
            }
            catch (const std::out_of_range& e) {
                std::cerr << "Input string: '" << jsonVal.s() << "' is out of range for a double." << std::endl;
            }
        }
        else {
            std::cerr << "Unsupported JSON value type; skipping entry." << std::endl;
        }
    }
    return doubleVec;
}

void stringToCSV(const std::string csvData, const std::string filename) {
    std::ofstream csvFile(filename);

    if (!csvFile.is_open()) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return;
    }

    csvFile << csvData;

    csvFile.close();
}


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

        crow::json::rvalue jsonBody = crow::json::load(req.body);

        if (!jsonBody) {
            throw std::invalid_argument("Invalid JSON format");
        }

#pragma region Auto generate inputs
        std::list<std::string> inputs = { "zmax", "kd", "pa", "height" };
#pragma endregion

        std::vector<std::any> converted_inputs;

        // Get JSON for each input for each function and convert
        for (const std::string input : inputs) {
            crow::json::rvalue input_rval = jsonBody[input];
            std::vector<double> resVector1D;
            std::vector<std::vector<double>> resVector2D;

            switch (input_rval.t()) {
            case crow::json::type::Number:
                // It is a single number
                break;
            case crow::json::type::List:
                // It is either a single string with a number, or comma separated numbers, assuming decimal numbers are written with a dot, e.g. 2.345
                std::string row = input_rval.lo()[0].s();

                if (row.find(',') == std::string::npos) {
                    // Comma not found, single number, convert to 1D array
                    resVector1D = convertToDouble(input_rval.lo());
                    converted_inputs.push_back(resVector1D);
                }
                else {
                    // Comma found, convert to 2D array
                    std::vector<std::string> resTemp;
                    for (const crow::json::rvalue row : input_rval.lo()) {
                        std::string rowStr = row.s();
                        resTemp.push_back(rowStr);
                    }
                    resVector2D = convertToDouble2D(resTemp);
                    converted_inputs.push_back(resVector2D);
                }
                break;
            }
        }

        // Auto-generate this
#pragma region Auto generate assigned converted results and function call
        std::vector<double> zmax = std::any_cast<std::vector<double>>(converted_inputs[0]);
        std::vector<std::vector<double>> kd = std::any_cast<std::vector<std::vector<double>>>(converted_inputs[1]);
        std::vector<std::vector<double>> pa = std::any_cast<std::vector<std::vector<double>>>(converted_inputs[2]);
        std::vector<std::vector<double>> height = std::any_cast<std::vector<std::vector<double>>>(converted_inputs[3]);

        std::vector<std::vector<double>> result = rad_bot(zmax, kd, pa, height);
#pragma endregion
        
        std::string csv_result = toCSV(result);

        stringToCSV(csv_result, "test_csv_result.csv");

        crow::response response(csv_result);
        response.add_header("Content-Type", "text/csv");

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
