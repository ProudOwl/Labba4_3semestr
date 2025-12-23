#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <chrono>
#include <random>
#include <iomanip>
#include <algorithm>
#include "DynamicArray.h"
#include "Map.h"

using namespace std;

struct ExamResult {
    string subject;
    int score;
    
    ExamResult(const string& subj = "", int sc = 0) : subject(subj), score(sc) {}
    
    ExamResult(const ExamResult& other) : subject(other.subject), score(other.score) {}
    
    ExamResult& operator=(const ExamResult& other) {
        if (this != &other) {
            subject = other.subject;
            score = other.score;
        }
        return *this;
    }
    
    ExamResult(ExamResult&& other) noexcept : subject(std::move(other.subject)), score(other.score) {}
    
    ExamResult& operator=(ExamResult&& other) noexcept {
        if (this != &other) {
            subject = std::move(other.subject);
            score = other.score;
        }
        return *this;
    }
};

struct Student {
    string name;
    int age;
    int school_id;
    DynamicArray<ExamResult> exam_results;
    
    Student() : name(""), age(0), school_id(0) {}
    
    Student(const string& n, int a, int sid) : name(n), age(a), school_id(sid) {}
    
    Student(const Student& other) : 
        name(other.name), 
        age(other.age), 
        school_id(other.school_id),
        exam_results(other.exam_results) {}
    
    Student& operator=(const Student& other) {
        if (this != &other) {
            name = other.name;
            age = other.age;
            school_id = other.school_id;
            exam_results = other.exam_results;
        }
        return *this;
    }
    
    Student(Student&& other) noexcept : 
        name(std::move(other.name)), 
        age(other.age), 
        school_id(other.school_id),
        exam_results(std::move(other.exam_results)) {}
    
    Student& operator=(Student&& other) noexcept {
        if (this != &other) {
            name = std::move(other.name);
            age = other.age;
            school_id = other.school_id;
            exam_results = std::move(other.exam_results);
        }
        return *this;
    }

    bool hasPerfectScore() const {
        for (int i = 0; i < exam_results.get_size(); ++i) {
            if (exam_results[i].score == 100) {
                return true;
            }
        }
        return false;
    }
};

struct SchoolStats {
    int student_count = 0;
    int perfect_score_students = 0;
    
    SchoolStats() : student_count(0), perfect_score_students(0) {}
};

DynamicArray<Student> generate_students(int num_students, int num_schools) {
    DynamicArray<Student> students;
    srand(time(nullptr));
    
    string subjects[] = {"Математика", "Русский язык", "Физика", "Химия", 
                        "Биология", "История", "Английский язык", "Информатика"};
    int num_subjects = 8;
    
    string names[] = {"Иван", "Мария", "Алексей", "Анна", "Дмитрий", "Екатерина", 
                     "Сергей", "Ольга", "Павел", "Наталья"};
    int num_names = 10;
    
    for (int i = 0; i < num_students; ++i) {
        Student s;
        s.name = names[rand() % num_names] + " " + names[rand() % num_names] + "ов";
        s.age = 16 + rand() % 3;
        s.school_id = rand() % num_schools + 1;
        
        int num_exams = 1 + rand() % 5;
        for (int j = 0; j < num_exams; ++j) {
            string subject = subjects[rand() % num_subjects];
            int score = 60 + rand() % 41; // 60-100 баллов
            
            if (rand() % 10 == 0) score = 100;
            
            s.exam_results.push_back(ExamResult(subject, score));
        }
        
        students.push_back(s);
    }
    
    return students;
}

Map<int, SchoolStats> process_single_thread(DynamicArray<Student>& students) {
    Map<int, SchoolStats> school_stats;
    
    for (int i = 0; i < students.get_size(); ++i) {
        const Student& s = students[i];
        SchoolStats& stats = school_stats[s.school_id];
        stats.student_count++;
        
        if (s.hasPerfectScore()) {
            stats.perfect_score_students++;
        }
    }
    
    return school_stats;
}

void process_chunk(DynamicArray<Student>& students, int start, int end, 
                   Map<int, SchoolStats>& local_stats) {
    for (int i = start; i < end; ++i) {
        const Student& s = students[i];
        SchoolStats& stats = local_stats[s.school_id];
        stats.student_count++;
        
        if (s.hasPerfectScore()) {
            stats.perfect_score_students++;
        }
    }
}

int main() {
    int num_students, num_threads;
    
    cout << "Введите количество школьников (например, 100000): ";
    if (!(cin >> num_students)) num_students = 100000;
    
    cout << "Введите количество потоков для обработки: ";
    if (!(cin >> num_threads)) num_threads = 4;
    
    const int NUM_SCHOOLS = 100;
    
    cout << "\nГенерация данных " << num_students << " школьников..." << endl;
    DynamicArray<Student> students = generate_students(num_students, NUM_SCHOOLS);
    
    // Однопоточная обработка
    cout << "\nОднопоточная обработка..." << endl;
    auto start_single = chrono::high_resolution_clock::now();
    
    Map<int, SchoolStats> single_result = process_single_thread(students);
    
    auto end_single = chrono::high_resolution_clock::now();
    auto single_time = chrono::duration<double, milli>(end_single - start_single).count();
    cout << "Время выполнения: " << fixed << setprecision(2) << single_time << " мс" << endl;
    
    // Многопоточная обработка
    cout << "\nМногопоточная обработка (" << num_threads << " потоков)..." << endl;
    auto start_multi = chrono::high_resolution_clock::now();
    
    DynamicArray<thread> threads;
    DynamicArray<Map<int, SchoolStats>> local_stats(num_threads);
    
    for (int i = 0; i < num_threads; ++i) {
        local_stats.push_back(Map<int, SchoolStats>());
    }
    
    int chunk_size = num_students / num_threads;
    
    for (int i = 0; i < num_threads; ++i) {
        int start = i * chunk_size;
        int end = (i == num_threads - 1) ? num_students : (i + 1) * chunk_size;
        
        threads.emplace_back(process_chunk, ref(students), start, end, ref(local_stats[i]));
    }
    
    Map<int, SchoolStats> multi_result;
    
    for (int i = 0; i < num_threads; ++i) {
        threads[i].join();
        
        for (auto it = local_stats[i].begin(); it != local_stats[i].end(); ++it) {
            int school_id = it->key;
            SchoolStats& local = it->value;
            SchoolStats& global = multi_result[school_id];
            
            global.student_count += local.student_count;
            global.perfect_score_students += local.perfect_score_students;
        }
    }
    
    auto end_multi = chrono::high_resolution_clock::now();
    auto multi_time = chrono::duration<double, milli>(end_multi - start_multi).count();
    cout << "Время выполнения: " << fixed << setprecision(2) << multi_time << " мс" << endl;
    
    // Вычисление ускорения
    double speedup = single_time / multi_time;
    cout << "\nУскорение: " << fixed << setprecision(2) << speedup << "x" << endl;
    
    // Находим топ-3 школы по количеству школьников, сдавших хотя бы один экзамен на 100 баллов
    cout << "\nТОП-3 школы по количеству школьников с хотя бы одним экзаменом на 100 баллов:" << endl;
    
    // Создаем массив пар (ID школы, количество отличников) для сортировки
    struct SchoolRank {
        int school_id;
        int perfect_students;
        int total_students;
        double percentage;
    };
    
    DynamicArray<SchoolRank> rankings;
    
    for (auto it = multi_result.begin(); it != multi_result.end(); ++it) {
        if (it->value.student_count > 0) {
            SchoolRank rank;
            rank.school_id = it->key;
            rank.perfect_students = it->value.perfect_score_students;
            rank.total_students = it->value.student_count;
            rank.percentage = (static_cast<double>(rank.perfect_students) / rank.total_students) * 100;
            
            rankings.push_back(rank);
        }
    }
    
    // Сортировка по количеству отличников (по убыванию)
    for (int i = 0; i < rankings.get_size() - 1; ++i) {
        for (int j = i + 1; j < rankings.get_size(); ++j) {
            if (rankings[j].perfect_students > rankings[i].perfect_students) {
                // Обмен местами
                SchoolRank temp = rankings[i];
                rankings[i] = rankings[j];
                rankings[j] = temp;
            }
        }
    }
    
    // Вывод топ-3
    int top_n = min(3, rankings.get_size());
    for (int i = 0; i < top_n; ++i) {
        cout << i + 1 << ". Школа №" << rankings[i].school_id << ": " 
             << rankings[i].perfect_students << " чел. (" 
             << fixed << setprecision(1) << rankings[i].percentage << "% из " 
             << rankings[i].total_students << ")" << endl;
    }
}
