#include "template-lib.h"

int main() {
    foo1(0);
    foo2<int>(0);
//    foo2<double>(0.0);

    return 0;
}
