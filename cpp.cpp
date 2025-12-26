#include <iostream>
#include <string>
#include <vector>
#include <algorithm> 
#include <iomanip>   
#include <limits>  

using namespace std;

// Константы ограничений выплат
const double MIN_REWARD = 5000.0;
const double MAX_REWARD = 1000000.0;

// Структура для хранения входных данных
struct InputData {
    string productionType;  
    int severity;           
    double damageAmount;    
    int infoValue;          
    double partCoeff;       
};

// Структура для хранения результатов расчета
struct CalculationResult {
    double reward;          
    double percent;         
    string categoryMsg;     
    string coeffMsg;        
};

// --- Прототипы функций ---
void show_menu();
void run_calculator_mode();
void run_test_mode();
CalculationResult calculate_reward(const InputData& data);
double get_base_percent(const string& type);

// --- Главная функция ---
int main() {
    // Настройка локали для поддержки кириллицы
    setlocale(LC_ALL, "Russian");

    int choice;
    bool running = true;

    while (running) {
        show_menu();
        
        // Защищенный ввод выбора меню
        if (!(cin >> choice)) {
            cin.clear(); 
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
            choice = 0; 
        }

        switch (choice) {
        case 1:
            run_calculator_mode();
            break;
        case 2:
            run_test_mode();
            break;
        case 3:
            cout << "Завершение работы программы..." << endl;
            running = false;
            break;
        default:
            cout << "Ошибка: Неверный пункт меню. Попробуйте снова." << endl;
        }
        cout << endl;
    }

    return 0;
}

// --- Реализация функций ---

// Отображение главного меню
void show_menu() {
    cout << "==========================================" << endl;
    cout << " КАЛЬКУЛЯТОР ВОЗНАГРАЖДЕНИЯ (УИБО-15-24)" << endl;
    cout << "==========================================" << endl;
    cout << "1. Выполнение расчета (ввод данных)" << endl;
    cout << "2. Автоматическое тестирование" << endl;
    cout << "3. Выход" << endl;
    cout << "------------------------------------------" << endl;
    cout << "Выберите действие: ";
}

// Определение базового процента по типу дела
double get_base_percent(const string& type) {
    // Приводим к нижнему регистру для удобства (упрощенно)
    // В реальном проекте лучше использовать перечисления (enum)
    if (type == "уголовное" || type == "Уголовное") return 0.10; // 
    if (type == "административное" || type == "Административное") return 0.05; 
    if (type == "гражданское" || type == "Гражданское") return 0.03; 
    return 0.03; 
}

// Основная логика расчета
CalculationResult calculate_reward(const InputData& data) {
    CalculationResult res;
    
    // 1. Определение базового процента
    double base = get_base_percent(data.productionType);
    res.categoryMsg = "Категория дела: " + data.productionType;

    // 2. Корректировка на тяжесть: Base * (1 + (sev - 1) * 0.2)
    // Пример: Тяжесть 1 -> коэф 1.0. Тяжесть 5 -> коэф 1.8.
    double severityMult = 1.0 + (data.severity - 1) * 0.2;
    double p1 = base * severityMult;

    // 3. Корректировка на ценность информации: P1 * (val / 3)
    // Ценность 3 -> коэф 1.0.
    double valueMult = (double)data.infoValue / 3.0;
    double correctedPercent = p1 * valueMult;
    
    res.percent = correctedPercent; 

    // 4. Расчет вознаграждения: Сумма * Процент * КоэфУчастия
    double rawReward = data.damageAmount * correctedPercent * data.partCoeff;

    // 5. Применение законодательных ограничений
    if (rawReward < MIN_REWARD) {
        res.reward = MIN_REWARD;
    } else if (rawReward > MAX_REWARD) {
        res.reward = MAX_REWARD;
    } else {
        res.reward = rawReward;
    }

    // Формируем строку с информацией о коэффициентах для отчета
    res.coeffMsg = "Коэф. значимости: " + to_string(valueMult) + 
                   ", Участие: " + to_string(data.partCoeff);

    return res;
}

// Режим ручного ввода
void run_calculator_mode() {
    InputData d;
    
    cout << "\n--- РЕЖИМ РАСЧЕТА ---" << endl;
    
    // Ввод типа производства
    cout << "Введите вид производства (уголовное/административное/гражданское): ";
    cin >> d.productionType;

    // Ввод тяжести с проверкой
    cout << "Тяжесть правонарушения (1-5): ";
    while (!(cin >> d.severity) || d.severity < 1 || d.severity > 5) {
        cout << "Ошибка! Введите число от 1 до 5: ";
        cin.clear(); cin.ignore(1000, '\n');
    }

    // Ввод суммы ущерба
    cout << "Сумма ущерба (руб): ";
    while (!(cin >> d.damageAmount) || d.damageAmount < 0) {
        cout << "Ошибка! Введите положительное число: ";
        cin.clear(); cin.ignore(1000, '\n');
    }

    // Ввод ценности информации
    cout << "Ценность информации (1-5): ";
    while (!(cin >> d.infoValue) || d.infoValue < 1 || d.infoValue > 5) {
        cout << "Ошибка! Введите число от 1 до 5: ";
        cin.clear(); cin.ignore(1000, '\n');
    }

    // Ввод коэффициента участия
    cout << "Коэффициент участия (0.1 - 1.0): ";
    while (!(cin >> d.partCoeff) || d.partCoeff < 0.1 || d.partCoeff > 1.0) {
        cout << "Ошибка! Введите число от 0.1 до 1.0: ";
        cin.clear(); cin.ignore(1000, '\n');
    }

    // Выполнение расчета
    CalculationResult res = calculate_reward(d);

    // Вывод результата
    cout << fixed << setprecision(2);
    cout << "\n--- РЕЗУЛЬТАТЫ ---" << endl;
    cout << "Размер вознаграждения: " << res.reward << " руб." << endl;
    cout << "Процент от суммы (расчетный): " << res.percent * 100 << "%" << endl;
    cout << res.categoryMsg << endl;
    cout << res.coeffMsg << endl;
    cout << "------------------" << endl;
}

// Режим автоматического тестирования
void run_test_mode() {
    cout << "\n--- ЗАПУСК АВТОМАТИЧЕСКИХ ТЕСТОВ ---" << endl;

    // Тест 1: Уголовное, стандарт
    InputData t1 = {"уголовное", 1, 500000, 3, 0.8};
    // Расчет: Base 0.10. SevMult (1+(0)*0.2)=1. ValMult 3/3=1. 
    // Pct = 0.10. Reward = 500000 * 0.10 * 0.8 = 40000.
    // Limits: 40000 > 5000 и < 1000000. Итог 40000.
    CalculationResult r1 = calculate_reward(t1);
    
    cout << "Тест 1 (Уголовное): ";
    if (abs(r1.reward - 40000.0) < 1.0) cout << "ПРОЙДЕН";
    else cout << "ПРОВАЛЕН (Ожидалось 40000, получено " << r1.reward << ")";
    cout << endl;

    // Тест 2: Проверка минимума
    InputData t2 = {"гражданское", 1, 1000, 1, 0.1};
    // Очень малая выплата, должна сработать MIN_REWARD (5000)
    CalculationResult r2 = calculate_reward(t2);
    
    cout << "Тест 2 (Минимум 5000): ";
    if (r2.reward == 5000.0) cout << "ПРОЙДЕН";
    else cout << "ПРОВАЛЕН (Получено " << r2.reward << ")";
    cout << endl;

    // Тест 3: Проверка максимума
    InputData t3 = {"уголовное", 5, 100000000, 5, 1.0};
    // Огромная сумма, должна сработать MAX_REWARD (1000000)
    CalculationResult r3 = calculate_reward(t3);
    
    cout << "Тест 3 (Максимум 1млн): ";
    if (r3.reward == 1000000.0) cout << "ПРОЙДЕН";
    else cout << "ПРОВАЛЕН (Получено " << r3.reward << ")";
    cout << endl;
    
    cout << "------------------------------------" << endl;
}
