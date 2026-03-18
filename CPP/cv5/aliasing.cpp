#include <vector>
#include <iostream>
#include <cassert>
#include <algorithm>
#include <cmath>

/*
 * Aliasing is a situation where we refer to the same variable (address, piece of memory, ...)
 * through multiple handles (pointers, references, variable names).
 *
 * Aliasing by itself is fine, but it can cause issues when it is combined with mutation.
 * It can lead to many nasty bugs, and also performance pessimizations.
 * These bugs can cause UB, but they can also cause logical bugs without any UB.
 * Combining aliasing with mutation can create errors similar to race conditions,
 * but also in single-threaded code!
 *
 * If it's possible, we should try to use value semantics - pass objects by value and avoid references
 * and pointers, unless it's truly necessary. If we pass arguments as values, then no aliasing happens
 * across function boundaries.
 * With move semantics and copy elision, copying stuff around is often almost as cheap as passing
 * a reference/pointer, and in some cases it can actually improve performance, because when the compiler
 * can prove that no aliasing happens, it can sometimes generate better code.
 *
 * This problem is not specific to C++, it happens in many (most?) mainstream languages.
 * Some languages (e.g. Rust) take care to avoid these issues and enforce a rule that you can either
 * alias, or mutate, but not both at the same time.
 *
 * You can find a nice presentation about aliasing in C++ here:
 * https://www.youtube.com/watch?v=zHkmk1Y-gqM
 */

void what_is_aliasing() {
    int x = 1;
    int* p = &x; // Here both *p and x aliases the memory of the local variable `x`
    // It can seem harmless, but as you can see below, aliasing can cause unintended problems in real
    // code.

    int& ref = x; // Another aliasing (reference to x)
}

// (Just a helper function to print vectors)
void print_vec(const std::vector<int>& items) {
    for (size_t i = 0; i < items.size(); i++) {
        std::cout << items[i];
        if (i != items.size() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << std::endl;
}

/*
 * This is one of the most classical aliasing + mutation errors that happens in many languages.
 * We have a collection that we iterate through and during the iteration, we mutate the collection.
 */
void mutate_during_iteration() {
    std::vector<int> x{1, 2, 3};

    // Increase all odd elements of the vector by 1, and remove all even elements
    for (auto it = x.begin(); it != x.end(); it++) {
        if ((*it % 2) == 0) {
            // After this command, we trigger UB, because std::vector::erase invalidates all
            // iterators from the passed position forwards (https://cplusplus.com/reference/vector/vector/erase/#validity).
            x.erase(it);
        } else {
            (*it)++;
        }
    }
    // (UB happened, so whatever may happen)
    // Maybe x is {2, 3}, because `it` skipped 3, which was moved into the place of `2` during erase
    print_vec(x);

    // Where was the aliasing?
    // We could access the elements of the vector both through `x` and through the iterator.
    // We modified the elements using `x`, calling erase, and we accessed them using `it`.
}

/*
 * Find which of the two numbers is smaller/larger and store the result into `min`/`max`.
 * This might seem like a perfectly reasonable function, but with aliasing it gets problematic
 * (see `use_get_min_max`).
 */
void get_min_max(const int& num1, const int& num2, int& min, int& max) {
    if (num1 < num2) {
        min = num1;
        max = num2;
    }
    else if (num1 >= num2) {
        min = num2;
        max = num1;
    }
}

void use_get_min_max() {
    int a = 5;
    int b = 1;

    // Use this function to "sort" a, b so that a will contain the smaller value.
    get_min_max(a, b, a, b);

    // Oops: prints a=1, b=1
    std::cout << "a=" << a << ", b=" << b << std::endl;

    // Where is the aliasing? When we pass `a` to the function, it is stored under two references
    // - num1 and min. One of these is mutable, so we combine aliasing with mutation.
    // There is no UB here, but still a logical error happens.
}

/*
 * This function takes a vector of integers and adds an integer to each element of the vector.
 * Again, seems harmless, but what happens in `use_add_numbers`?
 */
void add_numbers(std::vector<int>& items, const int& value) {
    for (auto& item: items) {
        item += value;
    }
}

void use_add_numbers() {
    std::vector<int> items{1, 2, 3};

    // Add the first item of the vector to all elements of the vector
    add_numbers(items, items[0]);

    // Oops, `items` is {2, 4, 5} instead of {2, 3, 4}
    print_vec(items);

    // Again, no UB, but a logical error happens.

    // Notice that `value` was passed as a const int& - a constant reference.
    // But it was still changed during the execution of `add_numbers`!
    // That is because in C++, both const references and non-const references to the same memory
    // can be created freely at the same time, and thus aliasing with mutation can easily happen.
}

/*
 * Copy `count` integers from the address `from` to the address `to`.
 * See any bugs? Try to think about how could this function be used wrongly. Look at `use_copy_ints`.
 */
void copy_ints(const int* from, int* to, size_t count) {
    for (size_t i = 0; i < count; i++) {
        to[i] = from[i];
    }
}

void use_copy_ints() {
    std::vector<int> items{1, 2, 3, 4, 5, 6, 7};

    // Move 3 integers to the right by one index and then put zero at the beginning
    copy_ints(items.data(), items.data() + 1, 3);
    items[0] = 0;

    // We would expect {0, 1, 2, 3, 5, 6, 7}, but we get {0, 1, 1, 1, 5, 6, 7}
    print_vec(items);

    // Where is the aliasing? In `copy_ints`, we have two pointers to the same data, which is
    // overlapping in this case. We mutate through one of the pointers: aliasing + mutation happens.
}

/*
 * For curious programmers: how can aliasing affect performance?
 * In C++ there is a rule that all pointers/references to the same type can alias, and that they
 * cannot alias with any other type other than `char/unsigned char/std::byte`.
 * Therefore, any `int&` can alias with `int&` or `int*`.
 * `int&` can also alias `char*` or `std::byte&`.
 * But it cannot alias `double&`.
 *
 * The compiler makes assumptions that these rules hold.
 * That means two things:
 * 1) If you break the assumptions, you cause UB. For example, if you cast `int*` to `double*` and then
 * you access the `double*` value, it is UB, because the compiler assumes that this will not happen.
 * 2) The compiler assumes that the aliasing can happen. Therefore, even if it does not happen in
 * some specific case, where the compiler could optimize if aliasing does not happen, the compiler
 * will not do this, because it has to uphold its own rules and assume that it *can* happen. This
 * can impede performance in some cases (see below).
 */
void apply_function(std::vector<double>& items, const double& value) {
    for (auto& item: items) {
        // std::sin(value) is expensive
        // Normally, the compiler could just perform it once before the loop, store it into a register
        // and then multiply all items with the register, without recomputing the sinus function.
        // But remember, item can potentially alias with value!
        // In that case the multiplication could change the value of the sinus result in the next
        // iteration.
        // So the compiler will not do this, because it could change the behaviour of the program.
        // You can check the assembly here: https://godbolt.org/z/84WxnG754
        // The program calls the sin function in each iteration of the loop.
        item *= std::sin(value);
    }

    // In this case, we could solve this with marking `value` with `restrict`, which tells the
    // compiler that the pointer/reference does not alias with anything.
    // Using `const double& __restrict__ value` for the parameter changes the code so that `std::sin`
    // is only called once.
    // However, this is a non-standard extension of C++ compilers, and if you break the rule,
    // you cause UB.
    // Another solution could be to use a different type for `value`, as then C++ would assume that
    // it cannot alias `double&`, because it will have a different type.
}

int main() {
//    mutate_during_iteration();
//    use_get_min_max();
//    use_add_numbers();
    use_copy_ints();

    return 0;
}
