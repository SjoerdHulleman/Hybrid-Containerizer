// CppBase.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <crow.h>
#include <Rcpp.h>
using namespace Rcpp;

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
