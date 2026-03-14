#include <vector>
#include <cstdint>
#include <ostream>
#include <iostream>
#include <variant>

/*
 * This file shows how you can implement a hierarchy of types using two approaches:
 * 1) OOP: using runtime polymorphism (virtual methods)
 * 2) (Functional programming:) using algebraic data types (std::variant)
 *
 * These two approaches have various advantages and disadvantages:
 * 1) It's easy to add new types, because all functionality is located in a single place inside
 * the class. But it's difficult to add new behavior (methods), because you have to modify all
 * classes in the hierarchy. Also, there is a lot of coupling, because the types have to know about
 * all their behavior. In other words, it's not easy to add new behaviour to a class from the outside.
 * This can be alleviated e.g. with a Visitor design pattern.
 *
 * 2) It's easy to add new behavior, because all types are located in a single place (`std::visit`) call.
 * It's difficult to add new types, because you have to modify all places where you use `std::visit`
 * on the variant type. A nice thing is that you can add behaviour to the types from the outside,
 * without them knowing about this.
 *
 * I recomment you to watch this talk by Klaus Iglberger: https://www.youtube.com/watch?v=4eeESJQk-mw.
 * There are also other approaches, like type erasure (https://www.youtube.com/watch?v=qn6OqefuH08,
 * type-erasure.cpp). Task: try to implement this hierarchy using type-erasure.
 */

struct CPU {
public:
    explicit CPU(size_t register_count): registers(register_count, 0), instruction_pointer(0) {}

    std::vector<uint32_t> registers;
    size_t instruction_pointer;
};

// OOP approach: easy to add new data types
namespace oop {
    class Instruction {
    public:
        virtual void execute(CPU& cpu) const = 0;
        virtual void print(std::ostream& os) const = 0;
        virtual ~Instruction() = default;
    };

    class Add: public Instruction {
    public:
        Add(uint32_t left, uint32_t right, uint32_t result): left(left), right(right), result(result) {}

        void execute(CPU& cpu) const override {
            auto a = cpu.registers[this->left];
            auto b = cpu.registers[this->right];
            cpu.registers[this->result] = a + b;
            cpu.instruction_pointer++;
        }

        void print(std::ostream& os) const override {
            os << "ADD" << std::endl;
        }

    private:
        uint32_t left;
        uint32_t right;
        uint32_t result;
    };

    class Move: public Instruction {
    public:
        Move(uint32_t source, uint32_t result): source(source), result(result) {}

        void execute(CPU& cpu) const override {
            cpu.registers[this->result] = cpu.registers[this->source];
            cpu.instruction_pointer++;
        }

        void print(std::ostream& os) const override {
            os << "MOVE" << std::endl;
        }

    private:
        uint32_t source;
        uint32_t result;
    };

    class Set: public Instruction {
    public:
        Set(uint32_t result, uint32_t value): value(value), result(result) {}

        void execute(CPU& cpu) const override {
            cpu.registers[this->result] = this->value;
            cpu.instruction_pointer++;
        }

        void print(std::ostream& os) const override {
            os << "SET" << std::endl;
        }

    private:
        uint32_t value;
        uint32_t result;
    };

    struct Program {
        std::vector<Instruction*> instructions;

        Program(Program&) = delete;
        Program& operator=(const Program&) = delete;
        ~Program() {
            for (auto inst : this->instructions) {
                delete inst;
            }
        }

        void execute(CPU& cpu) {
            while (cpu.instruction_pointer < this->instructions.size()) {
                auto* instruction = this->instructions[cpu.instruction_pointer];
                instruction->execute(cpu);
            }
        }
    };
}

// Algebraic data type approach: easy to add new operations
namespace func {
    template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
    template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

    struct Move {
        uint32_t source;
        uint32_t target;
    };
    struct Set {
        uint32_t target;
        uint32_t value;
    };
    struct Add {
        uint32_t left;
        uint32_t right;
        uint32_t target;
    };

    using Instruction = std::variant<Move, Set, Add>;

    ssize_t execute(const Instruction& instruction, CPU& cpu) {
        return std::visit(overloaded {
                [&cpu](const Move& move) {
                    cpu.registers[move.target] = cpu.registers[move.source];
                    return 1;
                },
                [&cpu](const Add& add) {
                    auto a = cpu.registers[add.left];
                    auto b = cpu.registers[add.right];
                    cpu.registers[add.target] = a + b;
                    return 1;
                },
                [&cpu](const Set& set) {
                    cpu.registers[set.target] = set.value;
                    return 1;
                }
        }, instruction);
    }

    void run_program(CPU& cpu, std::vector<Instruction> instructions) {
        size_t ip = 0;
        while (ip < instructions.size()) {
            const auto& instruction = instructions[ip];
            ip += execute(instruction, cpu);
        }
    }

    void print(Instruction instruction, std::ostream& os) {
        std::visit(overloaded {
            [&os](const Move& move) { os << "MOVE" << std::endl; },
            [&os](const Add& move) { os << "ADD" << std::endl; },
            [&os](const Set& move) { os << "SET" << std::endl; }
        }, instruction);
    }
}

void program_oop() {
    using namespace oop;

    CPU cpu{10};
    oop::Program program{{
        new Set(0, 5),
        new Move(0, 1),
        new Add(0, 1, 2)
    }};
    program.execute(cpu);

    program.instructions[0]->print(std::cout);

    for (auto reg: cpu.registers) {
        std::cout << reg << " ";
    }
    std::cout << std::endl;
}
void program_func() {
    using namespace func;

    CPU cpu{10};
    std::vector<func::Instruction> instructions{
        Set{0, 5},
        Move{0, 1},
        Add{0, 1, 2}
    };
    run_program(cpu, instructions);

    print(instructions[0], std::cout);

    for (auto reg: cpu.registers) {
        std::cout << reg << " ";
    }
    std::cout << std::endl;
}

int main() {
    program_oop();
    program_func();

    return 0;
}
