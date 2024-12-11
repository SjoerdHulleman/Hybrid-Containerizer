#pragma once
#include<iostream>
#include "crow_all.h"

// ----- Conversion functions from CSV to vector/matrix ------

// Conversion from CSV matrix string, in Crow format, to vector
std::vector<double> convertToDoubleVector(const std::vector<crow::json::rvalue> jsonVec) {
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

// Conversion from CSV matrix to matrix (2D vector)
std::vector<std::vector<double>> convertToDoubleMatrix(const std::vector<std::string> strVec) {
    std::vector<std::vector<double>> doubleVec2D;

    for (std::string rowStr : strVec) {
        std::vector<double> doubleRow;
        std::stringstream ss(rowStr);
        std::string elem;

        // Split the string by commas and convert to double
        while (std::getline(ss, elem, ',')) {
            try {
                doubleRow.push_back(std::stod(elem)); // Convert the string element to double
            }
            catch (const std::invalid_argument e) {
                doubleRow.push_back(0);
            }
            catch (...) {
                std::cerr << "Unknown eception on input:" << elem << std::endl;
            }
        }

        if (doubleRow.size() > 0) {
            doubleVec2D.push_back(doubleRow);
        }
    }

    return doubleVec2D;
}

// ----- End of conversion functions from CSV to vector/matrix -----



// ----- Conversion functions from vector/matrix to CSV -----

// Conversion of Vector of any type to CSV format
template <typename T>
std::string toCSV(const std::vector<T> vec) {
    std::stringstream ss;

    // For each entry of the vector
    for (size_t i = 0; i < vec.size(); ++i) {
        ss << vec[i];
        if (i != vec.size() - 1) {
            ss << ",";
        }
    }

    ss << "\n";
    return ss.str();
}

// Conversion of Matrix of any type to CSV format
template <typename T>
std::string toCSV(const std::vector<std::vector<T>> matrix) {
    std::stringstream ss;

    // For each row of the matrix
    for (std::vector<T> row : matrix) {
        // Loop over each row entry
        for (size_t i = 0; i < row.size(); ++i) {
            ss << row[i];
            if (i != row.size() - 1) {
                ss << ",";
            }
        }
        ss << "\n";
    }

    return ss.str();
}

// Conversion of string data to CSV file
void stringToCSV(const std::string csvData, const std::string filename) {
    std::ofstream csvFile(filename);

    if (!csvFile.is_open()) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return;
    }

    csvFile << csvData;

    csvFile.close();
}


// ----- End of conversion functions from vector/matrix to CSV -----


#pragma region Old testing functions


//// TODO: Make more efficient/readable
//std::vector<double> parseCSVLine(const std::string& csv) {
//    std::vector<double> row;
//
//    std::stringstream lineStream(csv);
//    std::string cell;
//
//    while (std::getline(lineStream, cell, ',')) {
//        try {
//            // Convert string in cell to double
//            row.push_back(std::stod(cell));
//        }
//        catch (const std::invalid_argument& e) {
//            // Handle invalid conversion (e.g., non-numeric data)
//            std::cerr << "Warning: Invalid number found, setting to 0.0: " << cell << std::endl;
//            row.push_back(0.0);  // Handle invalid values by setting them to 0.0
//        }
//        catch (const std::out_of_range& e) {
//            // Handle numbers that are too large to fit in a double
//            std::cerr << "Warning: Out of range number, setting to 0.0: " << cell << std::endl;
//            row.push_back(0.0);  // Handle out-of-range values
//        }
//    }
//
//    return row;
//}

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

#pragma endregion
