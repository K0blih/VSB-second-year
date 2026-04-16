#pragma once

#include <array>
#include <concepts>
#include <exception>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <ostream>
#include <type_traits>
#include <utility>
#include <vector>

class ArrayException: public std::exception {
public:
    explicit ArrayException(const char* message): message(message) {}

    const char* what() const noexcept override {
        return this->message;
    }
private:
    const char* message;
};

template <typename T>
class NumpyArray;

template <typename T>
class NumpySlice;

template <typename Iterator>
class Iterable;

template <typename Base, typename Predicate>
class FilterIterable;

template <typename Base, typename Mapper>
class MapIterable;

template <typename T>
concept ArrayLike = requires(const std::remove_reference_t<T>& value, size_t index) {
    { value.size() } -> std::convertible_to<size_t>;
    value[index];
};

template <typename T>
concept ScalarLike = !ArrayLike<T>;

template <typename T>
struct IsNumpyExpression: std::false_type {};

template <typename Iterator>
class Iterable {
public:
    Iterable(Iterator begin, Iterator end): begin_(begin), end_(end) {}

    Iterator begin() const {
        return this->begin_;
    }

    Iterator end() const {
        return this->end_;
    }

    template <typename Predicate>
    auto filter(Predicate predicate) const {
        return FilterIterable<Iterable, Predicate>(*this, std::move(predicate));
    }

    template <typename Mapper>
    auto map(Mapper mapper) const {
        return MapIterable<Iterable, Mapper>(*this, std::move(mapper));
    }

    template <typename Reducer>
    auto reduce(Reducer reducer) const {
        auto it = this->begin();
        if (it == this->end()) {
            throw ArrayException("Cannot reduce empty iterable");
        }

        std::decay_t<decltype(*it)> accumulator = *it;
        ++it;
        for (; it != this->end(); ++it) {
            reducer(accumulator, *it);
        }
        return accumulator;
    }

private:
    Iterator begin_;
    Iterator end_;
};

template <typename Base, typename Predicate>
class FilterIterable {
    class Iterator {
    public:
        Iterator(decltype(std::declval<Base>().begin()) current,
                 decltype(std::declval<Base>().end()) end,
                 Predicate* predicate):
            current_(current), end_(end), predicate_(predicate) {
            this->skip_rejected();
        }

        decltype(auto) operator*() const {
            return *this->current_;
        }

        Iterator& operator++() {
            ++this->current_;
            this->skip_rejected();
            return *this;
        }

        bool operator==(const Iterator& other) const {
            return this->current_ == other.current_;
        }

        bool operator!=(const Iterator& other) const {
            return !(*this == other);
        }

    private:
        void skip_rejected() {
            while (this->current_ != this->end_ && !std::invoke(*this->predicate_, *this->current_)) {
                ++this->current_;
            }
        }

        decltype(std::declval<Base>().begin()) current_;
        decltype(std::declval<Base>().end()) end_;
        Predicate* predicate_;
    };

public:
    FilterIterable(Base base, Predicate predicate):
        base_(std::move(base)), predicate_(std::move(predicate)) {}

    Iterator begin() const {
        return Iterator(this->base_.begin(), this->base_.end(), &this->predicate_);
    }

    Iterator end() const {
        return Iterator(this->base_.end(), this->base_.end(), &this->predicate_);
    }

    template <typename NextPredicate>
    auto filter(NextPredicate predicate) const {
        return FilterIterable<FilterIterable, NextPredicate>(*this, std::move(predicate));
    }

    template <typename Mapper>
    auto map(Mapper mapper) const {
        return MapIterable<FilterIterable, Mapper>(*this, std::move(mapper));
    }

    template <typename Reducer>
    auto reduce(Reducer reducer) const {
        auto it = this->begin();
        if (it == this->end()) {
            throw ArrayException("Cannot reduce empty iterable");
        }

        std::decay_t<decltype(*it)> accumulator = *it;
        ++it;
        for (; it != this->end(); ++it) {
            reducer(accumulator, *it);
        }
        return accumulator;
    }

private:
    Base base_;
    mutable Predicate predicate_;
};

template <typename Base, typename Mapper>
class MapIterable {
    class Iterator {
    public:
        Iterator(decltype(std::declval<Base>().begin()) current, Mapper* mapper):
            current_(current), mapper_(mapper) {}

        decltype(auto) operator*() const {
            return std::invoke(*this->mapper_, *this->current_);
        }

        Iterator& operator++() {
            ++this->current_;
            return *this;
        }

        bool operator==(const Iterator& other) const {
            return this->current_ == other.current_;
        }

        bool operator!=(const Iterator& other) const {
            return !(*this == other);
        }

    private:
        decltype(std::declval<Base>().begin()) current_;
        Mapper* mapper_;
    };

public:
    MapIterable(Base base, Mapper mapper): base_(std::move(base)), mapper_(std::move(mapper)) {}

    Iterator begin() const {
        return Iterator(this->base_.begin(), &this->mapper_);
    }

    Iterator end() const {
        return Iterator(this->base_.end(), &this->mapper_);
    }

    template <typename Predicate>
    auto filter(Predicate predicate) const {
        return FilterIterable<MapIterable, Predicate>(*this, std::move(predicate));
    }

    template <typename NextMapper>
    auto map(NextMapper mapper) const {
        return MapIterable<MapIterable, NextMapper>(*this, std::move(mapper));
    }

    template <typename Reducer>
    auto reduce(Reducer reducer) const {
        auto it = this->begin();
        if (it == this->end()) {
            throw ArrayException("Cannot reduce empty iterable");
        }

        std::decay_t<decltype(*it)> accumulator = *it;
        ++it;
        for (; it != this->end(); ++it) {
            reducer(accumulator, *it);
        }
        return accumulator;
    }

private:
    Base base_;
    mutable Mapper mapper_;
};

template <typename T>
class NumpySlice {
public:
    using const_iterator = typename std::vector<T>::const_iterator;

    NumpySlice(const_iterator begin, const_iterator end): begin_(begin), end_(end) {}

    size_t size() const {
        return static_cast<size_t>(std::distance(this->begin_, this->end_));
    }

    decltype(auto) operator[](size_t index) const {
        if (index >= this->size()) {
            throw ArrayException("Index out of range");
        }
        return *(this->begin_ + static_cast<typename std::iterator_traits<const_iterator>::difference_type>(index));
    }

    const_iterator begin() const {
        return this->begin_;
    }

    const_iterator end() const {
        return this->end_;
    }

    auto iter() const {
        return Iterable(this->begin(), this->end());
    }

    NumpySlice slice() const {
        return this->slice(0, this->size());
    }

    NumpySlice slice(size_t from) const {
        return this->slice(from, this->size());
    }

    NumpySlice slice(size_t from, size_t to) const {
        if (from > to || from > this->size() || to > this->size()) {
            throw ArrayException("Invalid slice indices");
        }
        return NumpySlice(this->begin_ + static_cast<typename std::iterator_traits<const_iterator>::difference_type>(from),
                          this->begin_ + static_cast<typename std::iterator_traits<const_iterator>::difference_type>(to));
    }

private:
    const_iterator begin_;
    const_iterator end_;
};

template <typename T>
class NumpyArray {
public:
    using iterator = typename std::vector<T>::iterator;
    using const_iterator = typename std::vector<T>::const_iterator;

    NumpyArray(std::vector<T> data): data_(std::move(data)) {}

    NumpyArray(std::initializer_list<T> data): data_(data) {}

    // Bonus: N-dimensional indexing shape constructor.
    NumpyArray(std::vector<T> data, std::vector<size_t> shape):
        data_(std::move(data)), shape_(std::move(shape)) {
        this->validate_shape();
    }

    // Bonus: N-dimensional indexing shape constructor.
    NumpyArray(std::initializer_list<T> data, std::vector<size_t> shape):
        data_(data), shape_(std::move(shape)) {
        this->validate_shape();
    }

    size_t size() const {
        return this->data_.size();
    }

    decltype(auto) operator[](size_t index) {
        if (index >= this->size()) {
            throw ArrayException("Index out of range");
        }
        return this->data_[index];
    }

    decltype(auto) operator[](size_t index) const {
        if (index >= this->size()) {
            throw ArrayException("Index out of range");
        }
        return this->data_[index];
    }

    iterator begin() {
        return this->data_.begin();
    }

    iterator end() {
        return this->data_.end();
    }

    const_iterator begin() const {
        return this->data_.begin();
    }

    const_iterator end() const {
        return this->data_.end();
    }

    auto iter() {
        return Iterable(this->begin(), this->end());
    }

    auto iter() const {
        return Iterable(this->begin(), this->end());
    }

    NumpySlice<T> slice() const {
        return this->slice(0, this->size());
    }

    NumpySlice<T> slice(size_t from) const {
        return this->slice(from, this->size());
    }

    NumpySlice<T> slice(size_t from, size_t to) const {
        if (from > to || from > this->size() || to > this->size()) {
            throw ArrayException("Invalid slice indices");
        }
        return NumpySlice<T>(
            this->data_.begin() + static_cast<typename std::iterator_traits<const_iterator>::difference_type>(from),
            this->data_.begin() + static_cast<typename std::iterator_traits<const_iterator>::difference_type>(to));
    }

    // Bonus: N-dimensional indexing.
    template <size_t N>
    decltype(auto) nd_index1(const std::array<size_t, N>& indices) const {
        if (N != this->shape_.size()) {
            throw ArrayException("Invalid number of indices");
        }

        size_t flat_index = 0;
        size_t stride = 1;
        for (size_t i = this->shape_.size(); i > 0; --i) {
            const size_t dim_index = i - 1;
            if (indices[dim_index] >= this->shape_[dim_index]) {
                throw ArrayException("Index out of range");
            }
            flat_index += indices[dim_index] * stride;
            stride *= this->shape_[dim_index];
        }

        return (*this)[flat_index];
    }

private:
    void validate_shape() const {
        if (this->shape_.empty()) {
            throw ArrayException("Shape cannot be empty");
        }

        size_t product = 1;
        for (const size_t dimension: this->shape_) {
            product *= dimension;
        }

        if (product != this->data_.size()) {
            throw ArrayException("Shape does not match data size");
        }
    }

    std::vector<T> data_;
    std::vector<size_t> shape_;
};

template <typename T>
struct IsNumpyExpression<NumpyArray<T>>: std::true_type {};

template <typename T>
struct IsNumpyExpression<NumpySlice<T>>: std::true_type {};

template <typename T>
concept NumpyExpression = ArrayLike<T> && IsNumpyExpression<std::remove_cvref_t<T>>::value;

template <ArrayLike Array>
decltype(auto) array_value_at(const Array& array, size_t index) {
    return array[index];
}

template <typename Left, typename Right>
size_t broadcast_size(const Left& left, const Right& right) {
    const size_t left_size = left.size();
    const size_t right_size = right.size();

    if (left_size == right_size) {
        return left_size;
    }
    if (left_size == 1) {
        return right_size;
    }
    if (right_size == 1) {
        return left_size;
    }

    throw ArrayException("Arrays have incompatible sizes");
}

template <ArrayLike Left, ArrayLike Right, typename Operation>
auto binary_array_operation(const Left& left, const Right& right, Operation operation) {
    using Result = std::decay_t<std::invoke_result_t<Operation, decltype(array_value_at(left, 0)), decltype(array_value_at(right, 0))>>;

    const size_t result_size = broadcast_size(left, right);
    std::vector<Result> result;
    result.reserve(result_size);

    const size_t left_size = left.size();
    const size_t right_size = right.size();
    for (size_t i = 0; i < result_size; ++i) {
        const size_t left_index = left_size == 1 ? 0 : i;
        const size_t right_index = right_size == 1 ? 0 : i;
        result.push_back(std::invoke(operation, array_value_at(left, left_index), array_value_at(right, right_index)));
    }

    return NumpyArray<Result>(std::move(result));
}

template <ArrayLike Array, ScalarLike Scalar, typename Operation>
auto binary_array_scalar_operation(const Array& array, const Scalar& scalar, Operation operation) {
    using Result = std::decay_t<std::invoke_result_t<Operation, decltype(array_value_at(array, 0)), const Scalar&>>;

    std::vector<Result> result;
    result.reserve(array.size());
    for (size_t i = 0; i < array.size(); ++i) {
        result.push_back(std::invoke(operation, array_value_at(array, i), scalar));
    }
    return NumpyArray<Result>(std::move(result));
}

template <ScalarLike Scalar, ArrayLike Array, typename Operation>
auto binary_scalar_array_operation(const Scalar& scalar, const Array& array, Operation operation) {
    using Result = std::decay_t<std::invoke_result_t<Operation, const Scalar&, decltype(array_value_at(array, 0))>>;

    std::vector<Result> result;
    result.reserve(array.size());
    for (size_t i = 0; i < array.size(); ++i) {
        result.push_back(std::invoke(operation, scalar, array_value_at(array, i)));
    }
    return NumpyArray<Result>(std::move(result));
}

template <NumpyExpression Left, NumpyExpression Right>
requires requires(const Left& left, const Right& right) { array_value_at(left, 0) + array_value_at(right, 0); }
auto operator+(const Left& left, const Right& right) {
    return binary_array_operation(left, right, [](const auto& a, const auto& b) {
        return a + b;
    });
}

template <NumpyExpression Array, ScalarLike Scalar>
requires requires(const Array& array, const Scalar& scalar) { array_value_at(array, 0) + scalar; }
auto operator+(const Array& array, const Scalar& scalar) {
    return binary_array_scalar_operation(array, scalar, [](const auto& a, const auto& b) {
        return a + b;
    });
}

template <ScalarLike Scalar, NumpyExpression Array>
requires requires(const Scalar& scalar, const Array& array) { scalar + array_value_at(array, 0); }
auto operator+(const Scalar& scalar, const Array& array) {
    return binary_scalar_array_operation(scalar, array, [](const auto& a, const auto& b) {
        return a + b;
    });
}

template <NumpyExpression Left, NumpyExpression Right>
requires requires(const Left& left, const Right& right) { array_value_at(left, 0) * array_value_at(right, 0); }
auto operator*(const Left& left, const Right& right) {
    return binary_array_operation(left, right, [](const auto& a, const auto& b) {
        return a * b;
    });
}

template <NumpyExpression Array, ScalarLike Scalar>
requires requires(const Array& array, const Scalar& scalar) { array_value_at(array, 0) * scalar; }
auto operator*(const Array& array, const Scalar& scalar) {
    return binary_array_scalar_operation(array, scalar, [](const auto& a, const auto& b) {
        return a * b;
    });
}

template <ScalarLike Scalar, NumpyExpression Array>
requires requires(const Scalar& scalar, const Array& array) { scalar * array_value_at(array, 0); }
auto operator*(const Scalar& scalar, const Array& array) {
    return binary_scalar_array_operation(scalar, array, [](const auto& a, const auto& b) {
        return a * b;
    });
}

// Bonus: boolean array/slice logical AND.
template <NumpyExpression Left, NumpyExpression Right>
requires (std::same_as<std::decay_t<decltype(array_value_at(std::declval<const Left&>(), 0))>, bool> &&
          std::same_as<std::decay_t<decltype(array_value_at(std::declval<const Right&>(), 0))>, bool>)
auto operator&&(const Left& left, const Right& right) {
    return binary_array_operation(left, right, [](bool a, bool b) {
        return a && b;
    });
}

// Bonus: boolean array/slice logical AND.
template <NumpyExpression Array>
requires std::same_as<std::decay_t<decltype(array_value_at(std::declval<const Array&>(), 0))>, bool>
auto operator&&(const Array& array, bool scalar) {
    return binary_array_scalar_operation(array, scalar, [](bool a, bool b) {
        return a && b;
    });
}

// Bonus: boolean array/slice logical AND.
template <NumpyExpression Array>
requires std::same_as<std::decay_t<decltype(array_value_at(std::declval<const Array&>(), 0))>, bool>
auto operator&&(bool scalar, const Array& array) {
    return binary_scalar_array_operation(scalar, array, [](bool a, bool b) {
        return a && b;
    });
}

template <ArrayLike Array>
std::ostream& print_array_like(std::ostream& os, const Array& array) {
    for (size_t i = 0; i < array.size(); ++i) {
        os << array[i];
        if (i + 1 < array.size()) {
            os << ", ";
        }
    }
    return os;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const NumpyArray<T>& array) {
    return print_array_like(os, array);
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const NumpySlice<T>& slice) {
    return print_array_like(os, slice);
}
