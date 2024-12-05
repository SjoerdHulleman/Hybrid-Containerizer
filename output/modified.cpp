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
std::vector<std::vector<double>> intPP_mixed(
       const std::vector<double>& zmax,
       const std::vector<std::vector<double>>& kd,
       const std::vector<std::vector<double>>& par,
       const std::vector<std::vector<double>>& alfa,
       const std::vector<std::vector<double>>& eopt,
       const std::vector<std::vector<double>>& pmax,
       const std::vector<std::vector<double>>& height) {

  static const double xg[] = {0.04691008, 0.23076534, 0.5, 0.76923466, 0.95308992};
  static const double wg[] = {0.1184634, 0.2393143, 0.2844444, 0.2393143, 0.1184634};

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
      } else {
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

  static const double xg[] = {0.04691008, 0.23076534, 0.5, 0.76923466, 0.95308992};
  static const double wg[] = {0.1184634, 0.2393143, 0.2844444, 0.2393143, 0.1184634};

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
		std::list<std::string> inputs = {"zmax","kd","par","alfa","eopt","pmax","height"};
		// ----- End of auto generate inputs -----

		std::vector<std::any> converted_inputs = convertInputs(inputs, jsonBody);

		// ----- Auto generate assigned converted results and function call -----
		const std::vector<double> & zmax = std::any_cast<const std::vector<double> &>(converted_inputs[0]);
		const std::vector<std::vector<double>> & kd = std::any_cast<const std::vector<std::vector<double>> &>(converted_inputs[1]);
		const std::vector<std::vector<double>> & par = std::any_cast<const std::vector<std::vector<double>> &>(converted_inputs[2]);
		const std::vector<std::vector<double>> & alfa = std::any_cast<const std::vector<std::vector<double>> &>(converted_inputs[3]);
		const std::vector<std::vector<double>> & eopt = std::any_cast<const std::vector<std::vector<double>> &>(converted_inputs[4]);
		const std::vector<std::vector<double>> & pmax = std::any_cast<const std::vector<std::vector<double>> &>(converted_inputs[5]);
		const std::vector<std::vector<double>> & height = std::any_cast<const std::vector<std::vector<double>> &>(converted_inputs[6]);
		std::vector<std::vector<double>> result = intPP_mixed(zmax,kd,par,alfa,eopt,pmax,height);
		// ----- End of auto generate assigned converted results and function call -----

		std::string csv_result = toCSV(result);

		crow::response response(csv_result);
		response.add_header("Content-Type", "text/csv");

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
		std::list<std::string> inputs = {"zmax","kd","par","height"};
		// ----- End of auto generate inputs -----

		std::vector<std::any> converted_inputs = convertInputs(inputs, jsonBody);

		// ----- Auto generate assigned converted results and function call -----
		const std::vector<double> & zmax = std::any_cast<const std::vector<double> &>(converted_inputs[0]);
		const std::vector<std::vector<double>> & kd = std::any_cast<const std::vector<std::vector<double>> &>(converted_inputs[1]);
		const std::vector<std::vector<double>> & par = std::any_cast<const std::vector<std::vector<double>> &>(converted_inputs[2]);
		const std::vector<std::vector<double>> & height = std::any_cast<const std::vector<std::vector<double>> &>(converted_inputs[3]);
		std::vector<std::vector<double>> result = rad_bot(zmax,kd,par,height);
		// ----- End of auto generate assigned converted results and function call -----

		std::string csv_result = toCSV(result);

		crow::response response(csv_result);
		response.add_header("Content-Type", "text/csv");

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
		std::list<std::string> inputs = {"zmax","kd","pMud","par","alfa","eopt","pmax"};
		// ----- End of auto generate inputs -----

		std::vector<std::any> converted_inputs = convertInputs(inputs, jsonBody);

		// ----- Auto generate assigned converted results and function call -----
		const std::vector<double> & zmax = std::any_cast<const std::vector<double> &>(converted_inputs[0]);
		const std::vector<double> & kd = std::any_cast<const std::vector<double> &>(converted_inputs[1]);
		const std::vector<double> & pMud = std::any_cast<const std::vector<double> &>(converted_inputs[2]);
		const std::vector<std::vector<double>> & par = std::any_cast<const std::vector<std::vector<double>> &>(converted_inputs[3]);
		const std::vector<std::vector<double>> & alfa = std::any_cast<const std::vector<std::vector<double>> &>(converted_inputs[4]);
		const std::vector<std::vector<double>> & eopt = std::any_cast<const std::vector<std::vector<double>> &>(converted_inputs[5]);
		const std::vector<std::vector<double>> & pmax = std::any_cast<const std::vector<std::vector<double>> &>(converted_inputs[6]);
		std::vector<std::vector<double>> result = intPP_exp(zmax,kd,pMud,par,alfa,eopt,pmax);
		// ----- End of auto generate assigned converted results and function call -----

		std::string csv_result = toCSV(result);

		crow::response response(csv_result);
		response.add_header("Content-Type", "text/csv");

		std::cout << "Sending response for intPP_exp" << std::endl;

		return response;
	});

    

    app.port(8080).multithreaded().run();
}
