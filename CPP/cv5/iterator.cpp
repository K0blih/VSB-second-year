#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <optional>
#include <coroutine>

using namespace std;

/*
 * It is often useful to implement some kind of abstraction for iterating over something.
 * If we want to perform various things with the iterated items, it's not very nice to hardcore
 * these operations and combine them with the iteration logic.
 *
 * It's much nicer to separate the iteration logic (which cares about generating items) and the consuming
 * logic (which wants to perform something with each item).
 *
 * This is not however so trivial in C++. This file shows various approaches to achieve that.
 */

/*
 * Imagine that we want to iterate over words in a sentence. And in our program, we'd like to perform
 * various things with the words - count them, store them in a vector, print them to stdout etc.
 * How can we do this?
 * In `count_words` and `find_words`, the iteration logic is interleaved with the counting/vector
 * gathering logic. This is easy to implement, but the iteration logic is duplicated, which is quite
 * annoying - we'd have to copy-paste it for each consuming logic.
 */
int count_words(std::string sentence) {
    int count = 0;
    std::string word;

    for (auto c: sentence) {
        if (c == ' ') {
            if (!word.empty()) {
                count++;
            }
            word = "";
        } else {
            word += c;
        }
    }
    if (!word.empty()) {
        count++;
    }

    return count;
}
std::vector<std::string> find_words(std::string sentence) {
    std::vector<std::string> words;
    std::string word;

    for (auto c: sentence) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
            }
            word = "";
        } else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }

    return words;
}


/*
 * So, how can we avoid this? There are several ways. One of them is so-called "internal iteration".
 * We put the iteration logic in a function, and then we pass the consuming logic through a callback
 * (a function pointer or a lambda function). The iteration is then "driven" by the iterating function,
 * that's why its called internal.
 *
 * Advantages: Iteration logic is simple to write, and it is not duplicated
 * Disadvantage: It is hard to use this API, because you need to provide a lambda function, and you
 * cannot use normal control flow statements (break/continue) to control the loop.
 *
 * So this approach is easy to implement for the (library code) author, but hard to use for the end
 * users of the API.
 */
void iterate_words_internal(std::string input, std::function<void(std::string)> fn) {
    std::string word;

    for (auto c: input) {
        if (c == ' ') {
            if (!word.empty()) {
                fn(word);
            }
            word = "";
        } else {
            word += c;
        }
    }
    if (!word.empty()) {
        fn(word);
    }
}
void use_internal_iteration() {
    int count = 0;
    iterate_words_internal("ahoj svete", [&count](std::string word) {
        count++;
    });

    std::vector<std::string> words;
    iterate_words_internal("ahoj svete", [&words](std::string word) {
        words.push_back(std::move(word));
    });
}

/*
 * Another approach is "external iteration". This is basically the opposite of the above approach.
 * Here the end user drives the iteration (hence the name external). But for this to work, you need
 * to implement the iterator in a complicated way - instead of just writing a function, you need to
 * manually split the iteration into its state (variables) and several functions that will be called
 * by the consuming user.
 *
 * Advantages: It's much easier for the end user to use this iterator, and break/continue can be used.
 * Disadvantages: It can be incredibly annoying to implement this iteration style by hand. You basically
 * have to implement just the "inner body" of the iteration loop separately, because the loop itself
 * will be called by the consumer.
 *
 * In C++, external iteration is implemented using the *iterator protocol*, a set of rules that tell
 * us what functions and interface must some class implement in order to be considered as an iterator.
 * If you implement this "protocol", your iterator class will be usable with the "range-based" for loop.
 *
 * An iterator has to support at least these operations:
 * 1) Dereference (`*it`) - gets the current value of the iterator.
 * 2) Move to the next element (`++it`) - moves the iterator to the next element.
 * 3) Inequality comparison (`it1 != it2`) - checks if the iterator is at "the end".
 *
 * To use the iterator protocol, you have to create a class with `begin` and `end` methods, which
 * should return an iterator pointing to the first item and an iterator representing the end.
 * Note that you will have to implement two classes - one with `begin` and `end`, which returns the
 * iterator objects, and one class implementing the Iterator interface itself (`*`, `++` and `!=`).
 *
 * Range-based ("for-each") loops are then translated like this:
 *
 * Iterable my_iterable;
 * for (auto item: my_iterable) {
 *     item.foo();
 * }
 * // is translated into vvv
 * auto it = my_iterable.begin();
 * for (; it != my_iterable.end(); ++it) {
 *    (*it).foo();
 * }
 */
struct WordSplitIterator {
    explicit WordSplitIterator(std::string_view text): text(text) {}

    bool operator==(const WordSplitIterator& other) const {
        return other.text == this->text;
    }
    bool operator!=(const WordSplitIterator& other) const {
        return !(other == *this);
    }

    std::string_view operator*() const {
        const char* word_start = this->text.data();
        const char* word_end = word_start;
        const char* end = word_start + this->text.size();
        while (word_end < end && *word_end != ' ') {
            word_end++;
        }
        size_t word_count = word_end - word_start;
        return std::string_view{word_start, word_count};
    }
    WordSplitIterator& operator++() {
        std::string_view word = this->operator*();
        const char* end = this->text.data() + this->text.size();
        const char* word_start = this->text.data() + word.size();
        while (word_start < end && *word_start == ' ') {
            word_start++;
        }

        size_t size = word_start - this->text.data();

        this->text = std::string_view{word_start, this->text.size() - size};

        return *this;
    }

private:
    std::string_view text;
};

struct Iterable {
    explicit Iterable(std::string str): str(std::move(str)) {}

    WordSplitIterator begin() const {
        return WordSplitIterator{this->str};
    }
    WordSplitIterator end() const {
        return WordSplitIterator{std::string_view{this->str.data() + this->str.size(), 0}};
    }

    std::string str;
};

// Here we can see the trade-off - this is much easier to use than internal iteration.
// But as you can see above, it is also much harder to implement the iterator itself!
void use_external_iteration() {
    Iterable iterable{"ahoj     karle jak se mas?"};
    for (auto word : iterable) {
        cout << word << endl;
    }
    cout << "konec" << endl;
}

/*
 * It would be nice if there was some third alternative that would allow us to have the best of both
 * worlds. Basically, we would like to implement the iterator as in the "internal iterator" example,
 * and consume it as in the "external iterator" example.
 *
 * For this, we'd need a way to write a function that can pause its execution - once it generates
 * an item, it will pause, give the consumer the item for further processing, and once the consumer
 * is done processing, the iterator would resume and continue generating the next item.
 *
 * This can be performed in various programming languages using generators (also called coroutines).
 * These are supported in C++ since C++20, with the keyword `co_yield`. It is easy to write them,
 * however you also need nontrivial boilerplate code to use them.
 * This boilerplate code can be seen below (Future and Generator classes) - it was copied from the
 * internet. For simple generators, you can reuse this same boilerplate code, so you could just put
 * it into some other file and don't think about it.
 */

//------------- Start of boilerplate -------------
template <typename T>
class Future
{
    class Promise
    {
    public:
        using value_type = std::optional<T>;

        Promise() = default;
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void unhandled_exception() {
            std::rethrow_exception(std::move(std::current_exception()));
        }

        std::suspend_always yield_value(T value) {
            this->value = std::move(value);
            return {};
        }

        // void return_value(T value) {
        //     this->value = std::move(value);
        // }

        void return_void() {
            this->value = std::nullopt;
        }

        inline Future get_return_object();

        value_type get_value() {
            return std::move(value);
        }

        bool finished() {
            return !value.has_value();
        }

    private:
        value_type value{};
    };

public:
    using value_type = T;
    using promise_type = Promise;

    explicit Future(std::coroutine_handle<Promise> handle)
            : handle (handle)
    {}

    ~Future() {
        if (handle) { handle.destroy(); }
    }

    Promise::value_type next() {
        if (handle) {
            handle.resume();
            return handle.promise().get_value();
        }
        else {
            return {};
        }
    }

private:
    std::coroutine_handle<Promise> handle;
};

template <typename T>
class Generator
{
    class Promise
    {
    public:
        using value_type = std::optional<T>;

        Promise() = default;
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void unhandled_exception() {
            std::rethrow_exception(std::move(std::current_exception()));
        }

        std::suspend_always yield_value(T value) {
            this->value = std::move(value);
            return {};
        }

        // void return_value(T value) {
        //     this->value = std::move(value);
        // }

        void return_void() {
            this->value = std::nullopt;
        }

        inline Generator get_return_object();

        value_type get_value() {
            return std::move(value);
        }

        bool finished() {
            return !value.has_value();
        }

    private:
        value_type value{};
    };

public:
    using value_type = T;
    using promise_type = Promise;

    explicit Generator(std::coroutine_handle<Promise> handle)
            : handle (handle)
    {}

    ~Generator() {
        if (handle) { handle.destroy(); }
    }

    Promise::value_type next() {
        if (handle) {
            handle.resume();
            return handle.promise().get_value();
        }
        else {
            return {};
        }
    }

    struct end_iterator {};

    class iterator
    {
    public:
        using value_type = Promise::value_type;
        using difference_type =  std::ptrdiff_t;
        using iterator_category = std::input_iterator_tag;

        iterator() = default;
        iterator(Generator& generator) : generator{&generator}
        {}

        value_type operator*() const {
            if (generator) {
                return generator->handle.promise().get_value();
            }
            return {};
        }

        value_type operator->() const {
            if (generator) {
                return generator->handle.promise().get_value();
            }
            return {};
        }

        iterator& operator++() {
            if (generator && generator->handle) {
                generator->handle.resume();
            }
            return *this;
        }

        iterator& operator++(int) {
            if (generator && generator->handle) {
                generator->handle.resume();
            }
            return *this;
        }

        bool operator== (const end_iterator&) const {
            return generator ? generator->handle.promise().finished() : true;
        }

    private:
        Generator* generator{};
    };

    iterator begin() {
        iterator it{*this};
        return ++it;
    }

    end_iterator end() {
        return end_sentinel;
    }

private:
    end_iterator end_sentinel{};
    std::coroutine_handle<Promise> handle;
};

template <typename T>
inline Future<T> Future<T>::Promise::get_return_object()
{
    return Future{ std::coroutine_handle<Promise>::from_promise(*this) };
}

template <typename T>
inline Generator<T> Generator<T>::Promise::get_return_object()
{
    return Generator{ std::coroutine_handle<Promise>::from_promise(*this) };
}
//------------- End of boilerplate -------------

/*
 * This is the generator itself. Apart from the boilerplate, it is very easy to write and gives us
 * exactly what we wanted - a lazy, preemptible function that can generate items one by one and
 * we can use it as a normal iterator.
 */
Generator<std::string> generate_words(std::string input) {
    std::string word;
    for (auto c: input) {
        if (c == ' ') {
            if (!word.empty()) {
                co_yield word;
            }
            word = "";
        } else {
            word += c;
        }
    }
    if (!word.empty()) {
        co_yield word;
    }
}

void use_generator() {
    for (auto word: generate_words("  ahoj svete    cau ")) {
        std::cout << *word << std::endl;
    }
}

int main() {
//    use_internal_iteration();
//    use_external_iteration();
    use_generator();
    return 0;
}
