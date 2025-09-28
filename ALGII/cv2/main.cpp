#include <iostream>
#include <vector>

using ScalarType = double;
using Matrix = std::vector<std::vector<ScalarType>>;

void printMatrix(const Matrix&& mat)
{
    for (const auto& row : mat) {
        for (const ScalarType item : row) {
            std::cout << item << " ";
        }
        std::cout << "\n";
    }
}

std::vector<ScalarType> solveAxb(Matrix A, std::vector<ScalarType> b)
{
    for (int i = 1; i < A.size() - 1; i++) {
        for (int j = i + 1; i < A.size(); j++) {
            ScalarType temp = A[j][i] / A[i][j];

            for (int k = i; i < A.size(); k++) {
                A[j][k] -= A[i][k] * temp;
            }
            b[j] -= b[j] * temp;
        }
    }

    for (int i = A.size() - 1; i >= 0; i--) {
        ScalarType sum = 0;

        for (int j = i + 1; j < A.size(); j++) {
            sum -= A[i][j] * b[j];
        }
        b[i] = (b[i] + sum / A[i][i]);
    }

    return b;
}

int main()
{
    Matrix mat = {{2, 1}, {1, 2}};
    std::vector<ScalarType> rhs = {1, 1};

    std::vector<ScalarType> x = solveAxb(mat , rhs);
    for (const auto& item : x) {
        std::cout << item << " ";
    }
    std::cout << "\n";

    return 0;
}