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

std::vector<ScalarType> solveAxb(Matrix A, std::vector<ScalarType> b){
    Matrix L(A.size(), std::vector<ScalarType>(A.size(), 0.0));
    for (size_t i = 0; i < L.size(); i++){
        L[i][i] = 1;
    }

    for (size_t i = 0; i < A.size(); i++) {
        size_t pivot = i;
        ScalarType pivotValue = std::abs(A.at(i).at(i));
        for (size_t l = i + 1; l < A.size(); l++){
            if (std::abs(A.at(l).at(i)) > pivotValue){
                pivot = l;
                pivotValue = std::abs(A.at(l).at(i));
            }
        }

        if(pivot != i){
            std::swap(A[i], A[pivot]);
            std::swap(b[i], b[pivot]);
        }

        for (size_t j = i + 1; j < A.size(); j++) {
            ScalarType temp = A.at(j).at(i) / A.at(i).at(i);
            L[j][i] = temp;
            for (size_t k = i; k < A.size(); k++){
                A[j][k] = A.at(j).at(k) - temp * A.at(i).at(k);
            }
            b[j] = b.at(j) - temp * b.at(i);
        }
    }

    Matrix U = A;

    for (int i = A.size() - 1; i >=0; i--){
        double sum = 0;
        for (size_t j = i + 1; j < A.size(); j++){
            sum += A.at(i).at(j) * b.at(j);
        }
        b[i] = (b.at(i) - sum)/A.at(i).at(i);
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