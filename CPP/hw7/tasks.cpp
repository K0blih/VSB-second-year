#include "tasks.h"

#include <iterator>

std::ostream& operator<<(std::ostream& os, const Student& student)
{
    os << student.name;
    return os;
}

std::ostream& operator<<(std::ostream& os, const Exam& exam)
{
    os << exam.subject;
    return os;
}

Score calculate_score(Student student, Exam exam) {
    int math_diff = static_cast<int>(student.math_skill) * std::max(1, 100 - static_cast<int>(exam.math_difficulty));
    int coding_diff = static_cast<int>(student.coding_skill) * std::max(1, 100 - static_cast<int>(exam.coding_difficulty));

    int diff = math_diff + coding_diff;
    return diff / 50;
}

std::vector<std::pair<Student, Score>> find_best_n_students(
    std::vector<Student> students,
    const Exam& exam,
    size_t n
) {
    std::vector<std::pair<Student, Score>> ranked_students;
    ranked_students.reserve(students.size());

    std::transform(
        students.begin(),
        students.end(),
        std::back_inserter(ranked_students),
        [&](const Student& student) {
            return std::pair<Student, Score>{student, calculate_score(student, exam)};
        }
    );

    std::sort(
        ranked_students.begin(),
        ranked_students.end(),
        [](const auto& lhs, const auto& rhs) {
            return lhs.second != rhs.second ? lhs.second > rhs.second : lhs.first.name < rhs.first.name;
        }
    );

    ranked_students.erase(
        ranked_students.begin() + static_cast<std::ptrdiff_t>(std::min(n, ranked_students.size())),
        ranked_students.end()
    );
    return ranked_students;
}

size_t max_score_difference(
    const std::vector<Student>& students,
    const std::vector<Exam>& exams
) {
    if (students.size() < 2 || exams.empty()) {
        return 0;
    }

    std::vector<size_t> exam_differences(exams.size());
    std::transform(
        exams.begin(),
        exams.end(),
        exam_differences.begin(),
        [&](const Exam& exam) {
            std::vector<Score> scores(students.size());
            std::transform(
                students.begin(),
                students.end(),
                scores.begin(),
                [&](const Student& student) {
                    return calculate_score(student, exam);
                }
            );

            std::sort(scores.begin(), scores.end(), std::greater<>{});

            std::vector<size_t> differences(scores.size() - 1);
            std::transform(
                scores.begin(),
                std::prev(scores.end()),
                std::next(scores.begin()),
                differences.begin(),
                [](Score higher, Score lower) {
                    return static_cast<size_t>(higher - lower);
                }
            );

            return *std::max_element(differences.begin(), differences.end());
        }
    );

    return *std::max_element(exam_differences.begin(), exam_differences.end());
}

std::pair<std::unordered_set<Student>, std::unordered_set<Student>> filter_students(
    const std::vector<Student>& students,
    const std::vector<Exam>& exams
) {
    std::unordered_set<Student> passed_all;
    std::unordered_set<Student> passed_one;

    std::copy_if(
        students.begin(),
        students.end(),
        std::inserter(passed_all, passed_all.end()),
        [&](const Student& student) {
            return std::all_of(
                exams.begin(),
                exams.end(),
                [&](const Exam& exam) {
                    return calculate_score(student, exam) >= 100;
                }
            );
        }
    );

    std::copy_if(
        students.begin(),
        students.end(),
        std::inserter(passed_one, passed_one.end()),
        [&](const Student& student) {
            return std::any_of(
                exams.begin(),
                exams.end(),
                [&](const Exam& exam) {
                    return calculate_score(student, exam) >= 100;
                }
            );
        }
    );

    return {passed_all, passed_one};
}

Leaderboard get_leaderboard_of_each_subject(
    const std::vector<Student>& students,
    const std::vector<Exam>& exams
) {
    std::unordered_map<Subject, std::vector<Exam>> exams_by_subject;
    std::for_each(
        exams.begin(),
        exams.end(),
        [&](const Exam& exam) {
            exams_by_subject[exam.subject].push_back(exam);
        }
    );

    Leaderboard leaderboard;
    std::for_each(
        exams_by_subject.begin(),
        exams_by_subject.end(),
        [&](const auto& subject_entry) {
            const auto& [subject, subject_exams] = subject_entry;
            auto& scores = leaderboard[subject];
            scores.reserve(students.size());

            std::transform(
                students.begin(),
                students.end(),
                std::back_inserter(scores),
                [&](const Student& student) {
                    return std::pair<Student, Score>{
                        student,
                        std::accumulate(
                            subject_exams.begin(),
                            subject_exams.end(),
                            0,
                            [&](Score total, const Exam& exam) {
                                return total + calculate_score(student, exam);
                            }
                        )
                    };
                }
            );

            std::sort(
                scores.begin(),
                scores.end(),
                [](const auto& lhs, const auto& rhs) {
                    return lhs.second != rhs.second ? lhs.second > rhs.second : lhs.first.name < rhs.first.name;
                }
            );
        }
    );

    return leaderboard;
}
