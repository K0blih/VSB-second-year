#pragma once

#include <array>
#include <cstddef>
#include <type_traits>

template <typename T, std::size_t Rows, std::size_t Cols>
struct Matrix {
    std::array<T, Rows * Cols> values;

    constexpr const T& at(std::size_t row, std::size_t col) const {
        return values[row * Cols + col];
    }

    constexpr T& at(std::size_t row, std::size_t col) {
        return values[row * Cols + col];
    }
};

template <typename T, std::size_t Rows, std::size_t Cols>
constexpr auto operator+(const Matrix<T, Rows, Cols>& lhs, const Matrix<T, Rows, Cols>& rhs)
        -> Matrix<T, Rows, Cols> {
    Matrix<T, Rows, Cols> result{{}};

    for (std::size_t i = 0; i < Rows * Cols; ++i) {
        result.values[i] = lhs.values[i] + rhs.values[i];
    }

    return result;
}

template <typename T, std::size_t Rows, std::size_t Inner, std::size_t Cols>
constexpr auto operator*(const Matrix<T, Rows, Inner>& lhs, const Matrix<T, Inner, Cols>& rhs)
        -> Matrix<T, Rows, Cols> {
    Matrix<T, Rows, Cols> result{{}};

    for (std::size_t row = 0; row < Rows; ++row) {
        for (std::size_t col = 0; col < Cols; ++col) {
            T sum{};
            for (std::size_t i = 0; i < Inner; ++i) {
                sum += lhs.at(row, i) * rhs.at(i, col);
            }
            result.at(row, col) = sum;
        }
    }

    return result;
}

struct Null {};

template <int N, typename Next>
struct List {};

template <typename ListT>
struct ListFirst {};

template <int N, typename Next>
struct ListFirst<List<N, Next>> {
    static constexpr int Value = N;
};

template <typename ListT>
struct ListLast {};

template <int N>
struct ListLast<List<N, Null>> {
    static constexpr int Value = N;
};

template <int N, typename Next>
struct ListLast<List<N, Next>> {
    static constexpr int Value = ListLast<Next>::Value;
};

template <typename ListT>
struct ListLength {};

template <>
struct ListLength<Null> {
    static constexpr std::size_t Value = 0;
};

template <int N, typename Next>
struct ListLength<List<N, Next>> {
    static constexpr std::size_t Value = 1 + ListLength<Next>::Value;
};

template <typename ListT>
struct ListSum {};

template <>
struct ListSum<Null> {
    static constexpr int Value = 0;
};

template <int N, typename Next>
struct ListSum<List<N, Next>> {
    static constexpr int Value = N + ListSum<Next>::Value;
};

template <typename ListT>
struct ListMax {};

template <int N>
struct ListMax<List<N, Null>> {
    static constexpr int Value = N;
};

template <int N, typename Next>
struct ListMax<List<N, Next>> {
    static constexpr int Rest = ListMax<Next>::Value;
    static constexpr int Value = N > Rest ? N : Rest;
};

template <typename ListT, typename Accumulator>
struct ListReverseHelper {};

template <typename Accumulator>
struct ListReverseHelper<Null, Accumulator> {
    using Value = Accumulator;
};

template <int N, typename Next, typename Accumulator>
struct ListReverseHelper<List<N, Next>, Accumulator> {
    using Value = typename ListReverseHelper<Next, List<N, Accumulator>>::Value;
};

template <typename ListT>
struct ListReverse {
    using Value = typename ListReverseHelper<ListT, Null>::Value;
};

template <typename ...Ts> struct Stack {};

template <typename ...Ts, typename ...As>
constexpr auto push(Stack<Ts...>, As...) -> Stack<As..., Ts...>;

template <typename T, typename ...Ts>
constexpr auto pop(Stack<T, Ts...>) -> Stack<Ts...>;

template <typename T, typename ...Ts>
constexpr auto top(Stack<T, Ts...>) -> T;

template <std::size_t N>
struct Number {};

template <bool B>
struct Bool {};

template <std::size_t N>
struct PushNumber {};

template <bool B>
struct PushBool {};

struct Add {};
struct Max {};
struct Pop {};
struct Compare {};
struct Select {};

template <typename InstructionStack, typename ValueStack>
struct Interpret {};

template <std::size_t N>
struct Interpret<Stack<>, Stack<Number<N>>> {
    static constexpr std::size_t Result = N;
};

template <bool B>
struct Interpret<Stack<>, Stack<Bool<B>>> {
    static constexpr bool Result = B;
};

template <std::size_t N, typename ...Instructions, typename ValueStack>
struct Interpret<Stack<PushNumber<N>, Instructions...>, ValueStack>
        : Interpret<Stack<Instructions...>, decltype(push(ValueStack{}, Number<N>{}))> {};

template <bool B, typename ...Instructions, typename ValueStack>
struct Interpret<Stack<PushBool<B>, Instructions...>, ValueStack>
        : Interpret<Stack<Instructions...>, decltype(push(ValueStack{}, Bool<B>{}))> {};

template <typename ...Instructions, std::size_t A, std::size_t B, typename ...Values>
struct Interpret<Stack<Add, Instructions...>, Stack<Number<A>, Number<B>, Values...>>
        : Interpret<Stack<Instructions...>, Stack<Number<A + B>, Values...>> {};

template <typename ...Instructions, std::size_t A, std::size_t B, typename ...Values>
struct Interpret<Stack<Max, Instructions...>, Stack<Number<A>, Number<B>, Values...>>
        : Interpret<Stack<Instructions...>, Stack<Number<(A > B ? A : B)>, Values...>> {};

template <typename ...Instructions, typename Value, typename ...Values>
struct Interpret<Stack<Pop, Instructions...>, Stack<Value, Values...>>
        : Interpret<Stack<Instructions...>, Stack<Values...>> {};

template <typename ...Instructions, std::size_t A, std::size_t B, typename ...Values>
struct Interpret<Stack<Compare, Instructions...>, Stack<Number<A>, Number<B>, Values...>>
        : Interpret<Stack<Instructions...>, Stack<Bool<A == B>, Values...>> {};

template <typename A, typename B, typename ...Instructions, typename ...Values>
struct Interpret<Stack<Select, A, B, Instructions...>, Stack<Bool<true>, Values...>>
        : Interpret<Stack<A, Instructions...>, Stack<Values...>> {};

template <typename A, typename B, typename ...Instructions, typename ...Values>
struct Interpret<Stack<Select, A, B, Instructions...>, Stack<Bool<false>, Values...>>
        : Interpret<Stack<B, Instructions...>, Stack<Values...>> {};
