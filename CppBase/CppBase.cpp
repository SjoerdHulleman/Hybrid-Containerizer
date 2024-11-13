// CppBase.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <crow.h>


std::vector<std::vector<double>> parseCSVFromString(const std::string csv) {
    std::vector<std::vector<double>> data;

    std::stringstream stream = std::stringstream(csv);
    std::string line;

    while (stream >> line) {
        std::stringstream lineStream = std::stringstream(line);
        std::string cell;
        std::vector<double> row;

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

        data.push_back(row);
    }

    return data;
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

        std::vector<std::vector<double>> result = parseCSVFromString(data);

        return crow::response(200, "Received CSV file");
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
