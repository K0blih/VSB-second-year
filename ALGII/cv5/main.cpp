#include <iostream>
#include <vector>

double polyEvaluator(const std::vector<double>& coeff, const double x)
{
    double res = 0;
    double xPower = 1;

    for (size_t i = 0; i < coeff.size(); i++) {
        res += coeff[i] * xPower;
        xPower *= x;
    }

    return res;
}

double horner(const std::vector<double>& coeff, const double x)
{
    double res = coeff.back();

    for (int i = coeff.size()- 2; i < coeff.size(); i--) {
        res = res * x + coeff[i];
    }

    return res;
}

std::vector<std::vector<int>> combinations(const int n, const int k) {
    std::vector<std::vector<int>> res;
    if (k < 0 || k > n) {
        return res;
    }

    std::vector<int> current;
    current.reserve(k);
    for (int i = 0; i < k; i++) {
        current.push_back(i);
    }

    while (true) {
        res.push_back(current);

        int i = k - 1;
        while (i >= 0 && current[i] == (n + 1 - k)) {
            i--;
        }
        if (i < 0) {
            break;
        }
        current[i] += 1;
        for (int j = i + 1; j < current.size(); j++){
            current[j] = current[j - 1] + 1;
        }
    }

    return res;
}

void testPoly()
{
    std::vector<double> c1 = {1, 1, 1};
    std::vector<double> c2 = {1, 2, 3};

    double r1 = polyEvaluator(c1, 1.0);
    double r2 = polyEvaluator(c2, 0.0);

    std::cout << r1 << " " << r2 << "\n";
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char const *argv[])
{
    const auto rr = combinations(5, 3);

    for (const auto& line : rr) {
        for (const auto& item : line) {
            std::cout << item << " ";
        }
        std::cout << "\n";
    }
    
    return 0;
}
