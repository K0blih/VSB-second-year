#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>
#include <set>
#include <algorithm>

std::vector<std::vector<int>> combinations(const int n, const int k)
{
    std::vector<std::vector<int>> res;
    if (k < 0 || k > n)
    {
        return res;
    }

    std::vector<int> current(k);
    for (int i = 0; i < k; ++i)
    {
        current[i] = i;
    }

    while (true)
    {
        res.push_back(current);

        int i = k - 1;
        while (i >= 0 && current[i] == n - k + i)
        {
            i--;
        }
        if (i < 0)
        {
            break;
        }

        current[i]++;
        for (int j = i + 1; j < k; ++j)
        {
            current[j] = current[j - 1] + 1;
        }
    }

    return res;
}

std::set<std::vector<int>> hranice(const std::vector<std::vector<int>> &sit)
{
    std::map<std::vector<int>, int> counts;

    for (const auto &simplex : sit)
    {
        int n = simplex.size();
        if (n <= 1)
        {
            continue;
        }

        auto combs = combinations(n, n - 1);

        for (const auto &idxs : combs)
        {
            std::vector<int> face;
            for (int id : idxs)
            {
                face.push_back(simplex[id]);
            }

            std::vector<int> face_sorted = face;
            std::sort(face_sorted.begin(), face_sorted.end());
            counts[face_sorted]++;
        }
    }

    std::set<std::vector<int>> boundary;
    for (const auto &kv : counts)
    {
        if (kv.second == 1)
        {
            boundary.insert(kv.first);
        }
    }

    return boundary;
}

std::map<int, std::set<std::vector<int>>> computeSimplicesByDimension(const std::vector<std::vector<int>> &sit)
{
    std::map<int, std::set<std::vector<int>>> simplices_by_dim;

    for (const auto &simplex : sit)
    {
        int n = simplex.size();
        if (n == 0)
        {
            continue;
        }

        for (int k = 1; k <= n; k++)
        {
            auto combs = combinations(n, k);
            for (const auto &idxs : combs)
            {
                std::vector<int> sub;
                for (int id : idxs)
                {
                    sub.push_back(simplex[id]);
                }
                std::sort(sub.begin(), sub.end());
                simplices_by_dim[k - 1].insert(sub);
            }
        }
    }

    return simplices_by_dim;
}

void printTopologySummary(const std::vector<std::vector<int>> &sit)
{
    auto simplices_by_dim = computeSimplicesByDimension(sit);

    int max_dim = simplices_by_dim.empty() ? 0 : simplices_by_dim.rbegin()->first;
    std::vector<int> counts(max_dim + 1, 0);

    for (int d = 0; d <= max_dim; d++)
    {
        counts[d] = simplices_by_dim[d].size();
    }

    std::cout << "Verticies: " << counts[0] << "\n";
    if (counts.size() > 1)
    {
        std::cout << "Edges: " << counts[1] << "\n";
    }
    if (counts.size() > 2)
    {
        std::cout << "Triangles: " << counts[2] << "\n";
    }
    if (counts.size() > 3)
    {
        std::cout << "Tetrahedrons: " << counts[3] << "\n";
    }

    int chi = 0;
    for (int d = 0; d <= max_dim; d++)
    {
        chi += (d % 2 == 0 ? counts[d] : -counts[d]);
    }

    std::cout << "chi: " << chi << "\n";
}

void printBoundaries(const std::vector<std::vector<int>> &sit)
{
    auto boundaries = hranice(sit);
    if (!boundaries.empty())
    {
        for (const auto &item : boundaries)
        {
            for (const auto &boundary : item)
            {
                if (item.back() != boundary)
                {
                    std::cout << boundary << " ";
                }
                else
                {
                    std::cout << boundary;
                }
            }
            std::cout << "\n";
        }
    }
    else
    {
        std::cout << "Boundary:\n";
        std::cout << "is empty\n";
    }
}

std::vector<std::vector<int>> readIntegersFromFile(const std::string &filename)
{
    std::ifstream file(filename);
    std::vector<std::vector<int>> numbersVec;

    if (!file.is_open())
    {
        std::cerr << "Unable to open file: " << filename << std::endl;
        return numbersVec;
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::stringstream lineAsStream(line);
        std::vector<int> lineNumbers;
        int num;

        while (lineAsStream >> num)
        {
            lineNumbers.push_back(num);
        }

        if (!lineNumbers.empty())
        {
            numbersVec.push_back(lineNumbers);
        }
    }

    file.close();
    return numbersVec;
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char const *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Program takes exactly 1 argument\n";
        return 1;
    }

    auto data = readIntegersFromFile(argv[1]);
    printTopologySummary(data);
    std::cout << "\n";
    printBoundaries(data);

    return 0;
}
