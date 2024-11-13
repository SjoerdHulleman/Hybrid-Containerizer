#include <cmath>
#include <vector>

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

    int nt = kd.size();           // number of rows
    int ns = zmax.size();          // number of columns
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

// [[Rcpp::export]]
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

    int nt = par.size();           // Number of rows
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
