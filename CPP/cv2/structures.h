#pragma once

struct Bar {
    // This function is implicitly inline
    int fun1() {
        return 1;
    }

    // This function must be implemented (=defined) elsewhere
    int fun2();

    static int static_fun() {
        // this is not available inside of this function
        return 0;
    }
};
