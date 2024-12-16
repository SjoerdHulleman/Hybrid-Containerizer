#include <vector>

std::vector<std::vector<double>> est_bc_source(const std::vector<std::vector<double>>& geno) {
    int n_ind = geno.size();
    if (n_ind == 0) return {};
    int n_mar = geno[0].size();

    std::vector<std::vector<double>> rec(n_mar, std::vector<double>(n_mar, 0.0));

    for (int i = 0; i < n_mar - 1; i++) {
        for (int j = i + 1; j < n_mar; j++) {
            double ct = 0.0;
            for (int k = 0; k < n_ind; k++) {
                if (geno[k][i] != geno[k][j]) {
                    ct++;
                }
            }
            double recombination_rate = ct / n_ind;
            rec[i][j] = rec[j][i] = recombination_rate;
        }
    }

    return rec;
}
