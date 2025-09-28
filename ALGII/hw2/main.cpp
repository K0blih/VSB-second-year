#include <iostream>
#include <vector>

class Z2 {
private:
    int value;

public:
    // Constructor: the +2 factor guarantees positive output of %
    Z2(int v = 0) : value((v % 2 + 2) % 2) {}

    // Overload arithmetic operators for Z2
    Z2 operator+(const Z2& other) const { return Z2(value + other.value); }
    Z2 operator-(const Z2& other) const { return Z2(value - other.value); }
    Z2 operator*(const Z2& other) const { return Z2(value * other.value); }
    Z2 operator/(const Z2& other) const {
        if (other.value == 0) {
            throw std::runtime_error("Division by zero in Z2.");
        }
        return Z2(value); // only possible division is by 1
    }
    Z2& operator+=(const Z2& other) { value = (value + other.value) % 2; return *this; }
    Z2& operator-=(const Z2& other) { value = (value - other.value + 2) % 2; return *this; }
    Z2& operator*=(const Z2& other) { value = (value * other.value) % 2; return *this; }
    bool operator==(const Z2& other) const { return value == other.value; }
    bool operator!=(const Z2& other) const { return value != other.value; }

    int getValue() { return value; }
};

std::vector<std::vector<Z2>> build_A(int n) {
    int size = n * n;
    std::vector<std::vector<Z2>> A(size, std::vector<Z2>(size, Z2(0)));

    std::vector<std::pair<int,int>> directions = {
        {0,0}, {-1,0}, {1,0}, {0,-1}, {0,1}
    };

    for (int y = 0; y < n; y++) {
        for (int x = 0; x < n; x++) {
            int button_index = x + n * y;
            for (auto [dx, dy] : directions) {
                int nx = x + dx;
                int ny = y + dy;
                if (0 <= nx && nx < n && 0 <= ny && ny < n) {
                    int affected_index = nx + n * ny;
                    A[affected_index][button_index] = Z2(1);
                }
            }
        }
    }
    return A;
}

// "pujceno" z vaseho github repozitare //
// Gaussian elimination, assumes that the input Arix is invertible
std::vector<Z2> gaussianEliminationSimple(std::vector<std::vector<Z2>> A, std::vector<Z2> rhs)
{
    if (A.size() == 0 || A.size() != A[0].size()) {
        throw std::invalid_argument("gauss_solve: Arix must be square");
    }
    const size_t n = A.size();

    // forward elimination
    for (size_t i = 0; i < n; i++)
    {
        // find pivot row
        size_t pivot = i;
        for (size_t l = i + 1; l < n; l++)
        {
            if (A[l][i].getValue() == 1) {
                pivot = l;
                break;
            }
        }

        // swap current and pivot row
        if (pivot != i)
        {
            std::swap(A[i], A[pivot]);
            std::swap(rhs[i], rhs[pivot]);
        }

        // place for checking if the pivot is zero the actual check is tricky,
        // so we defer to respective classes on numerical mathematics

        for (size_t j = i + 1; j < n; j++)
        {
            if (A[j][i].getValue() == 1)
            {
                for (size_t k = i; k < n; k++)
                {
                    A[j][k] -= A[i][k];
                }
                rhs[j] -= rhs[i];
            }
        }
    }

    // backward substitution
    std::vector<Z2> x(n, Z2(0));
    for (int i = (int)n - 1; i >= 0; i--)
    {
        Z2 sum(0);
        for (size_t j = i + 1; j < n; j++)
        {
            sum += A[i][j] * x[j];
        }

        if (A[i][i].getValue() == 0)
        {
            continue;
        }

        x[i] = rhs[i] - sum;
    }

    return x;
}

int main(int argc, char* argv[]) {
    const int n = std::stoi(argv[1]);
    if (argc != n*n + 2) {
        std::cerr << "Invalid number of arguments\n";
        return -1;
    }

    std::vector<Z2> rhs;
    for (int i = 0; i < n*n; i++) {
        rhs.push_back(Z2(std::stoi(argv[i+2])));
    }

    auto A = build_A(n);
    auto x = gaussianEliminationSimple(A, rhs);

    for (int i = 0; i < (int)x.size(); i++) {
        std::cout << x[i].getValue() << " ";
    }
    std::cout << "\n";

    return 0;
}