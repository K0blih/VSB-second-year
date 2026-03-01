#include <algorithm>

// Copy-and-swap idiom (see https://stackoverflow.com/a/3279550/1107768)
class Resource {};

class RAII
{
public:
    RAII() : p{new Resource{}}
    {}

    ~RAII()
    {
        delete p;
    }

    RAII(const RAII& other) : p{new Resource{*(other.p)}}
    {}

    RAII(RAII&& other) noexcept: p{other.p}
    {
        other.p = nullptr;
    }

    RAII& operator=(RAII other)
    {
        swap(*this, other);
        return *this;
    }

private:
    static void swap(RAII& first, RAII& second)
    {
        std::swap(first.p, second.p);
    }

    Resource* p;
};

int main()
{
    RAII raii;

    // Do not mix up new/delete[] or new[]/delete => UB!
    int* p = new int[3];
    delete p; // UB

    return 0;
}
