#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <conio.h>
#include <windows.h>
#include <ctime>
#include "blocks.h"
using namespace std;

/* Тетрис для ОС Windows */

const int WIDTH = 10;
const int HEIGHT = 20;
const int MAX_NAME_LENGTH = 15;

// Игровой прогресс
int score;
int level; // Уровень повышается, когда текущий счет становится больше либо равен следующей степени двойки
int waiting_time;
bool tetris;

// Данные таблицы рекордов
string names[5];
int scores[5];
int levels[5];

int field[HEIGHT][WIDTH]; // Поле тетриса

// Падающий блок
bool item[4][4];
int item_type; // 1..7
int item_turn; // 1..4
int item_X, item_Y;
int item_size_X, item_size_Y;
bool item_was_moved;

// Следующий блок
int next_item_type;
int next_item[4][4];

// Объекты для файлового ввода-вывода
ifstream fin;
ofstream fout;

// Системная переменная-идентификатор окна
HANDLE h;

// Функция вставки объекта в массив
// Используется при обновлении таблицы рекордов
template <typename T>
void insert_to_array(T *arr, int arr_size, T obj, int pos) {
    for (int i = arr_size-1; i >= pos; --i)
        arr[i] = arr[i-1];
    arr[pos-1] = obj;
}

// Функция записи структуры отображения блока в массив
void copy_display(bool item[][4], int turn, int type) {
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            item[i][j] = displays[turn-1][type-1][i][j];
}

// Функция заполнения массива next_item
void set_color(bool item_display[][4]) {
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            next_item[i][j] = (item_display[i][j]) ? next_item_type : 0;
}

// Функция вывода определенного кол-ва пробелов
void print_spaces(int amount) {
    for (int i = 0; i < amount; ++i)
        cout << ' ';
}

// Функция подсчета кол-ва цифр в числе
int amount_of_digits(int num) {
    if (num < 10) return 1;
    return amount_of_digits(num / 10) + 1;
}

// Функция очистки входного потока
// Используется при генерации нового блока для того, чтобы программа не считывала команды, введенные пользователем ранее
void clear_input_stream() {
    while (kbhit()) getch();
}

// Функция проверки имени на корректность
bool check_name(string name) {
    try {
        if (name == "") throw "Error! Name must contain at least 1 symbol.";
        if (name.length() > MAX_NAME_LENGTH) throw "Error! Name is too long.";
        for (string::const_iterator it = name.begin(); it != name.end(); ++it)
            if (*it == ' ') throw "Error! Name contains space.";
    } catch (const char *error_msg) {
        cout << error_msg << endl;
        Sleep(2000);
        return false;
    }
    return true;
}

// Функция инициализации таблицы рекордов значениями по умолчанию
void set_high_scores_by_default() {
    for (int i = 0, j = 5; i < 5; ++i, --j) {
        names[i] = "Smirnov_A._A.";
        scores[i] = j;
        levels[i] = (int)log2(scores[i])+1;
    }
}

// Функция записи данных в файл с таблицей рекордов
void update_high_score_list() {
    fout.open("high_scores.txt");
    for (int i = 0; i < 5; ++i)
        fout << names[i] << ' ' << scores[i] << ' ' << levels[i] << ' ';
    fout.close();
}

// Функция инициализации таблицы рекордов
void get_high_scores() {
    fin.open("high_scores.txt");
    if (!fin.is_open()) {
        set_high_scores_by_default();
        update_high_score_list();
    } else
        for (int i = 0; i < 5; ++i)
            fin >> names[i] >> scores[i] >> levels[i];
    fin.close();
}

// Функция определяет, попал ли игрок в таблицу рекордов
// Если попал, то возвращается его позиция в таблице, иначе возвращается 0
int check_for_new_high_score() {
    int pos = -1;
    for (int i = 4; i >= 0; --i)
        (score >= scores[i]) ? pos = i : i = -1;
    return (pos == -1) ? 0 : pos + 1;
}

// Функция вывода таблицы рекордов
void show_high_scores() {
    int mode = 0;
    bool chosen = false;
    do {
        system("cls");
        cout << "------- The Best Players -------" << endl;
        cout << "#  Name";
        print_spaces(MAX_NAME_LENGTH+1);
        cout << "Score";
        print_spaces(5);
        cout << "Level" << endl;
        // Чтобы таблица выглядила красиво, каждый шаг цикла выводим нужное кол-во пробелов,...
        // ...которое зависит от длины имени и набранных очков
        for (int i = 0; i < 5; ++i) {
            cout << i+1 << ". " << names[i];
            print_spaces(MAX_NAME_LENGTH-names[i].length()+5);
            cout << scores[i];
            print_spaces(MAX_NAME_LENGTH-amount_of_digits(scores[i])-5);
            cout << levels[i] << endl;
        }
        // Предлогаем пользователю либо вернуться в главное меню, либо очистить таблицу
        cout << endl;
        char c = 0;
        cout << (mode == 0 ? '>' : ' ');
        cout << " Return to main menu" << endl;
        cout << (mode == 1 ? '>' : ' ');
        cout << " Clear High Scores" << endl;

        c = getch();
        switch (c) {
            case 'w':
            case 's': mode = (mode == 0) ? 1 : 0; break;
            case '\r': chosen = true;
        }
    } while (!chosen);

    if (mode == 1) {
        set_high_scores_by_default();
        update_high_score_list();
        cout << endl << "High Score list has been cleared!" << endl;
        Sleep(2000);
    }
}

// Функция удаления линии и сдвига остальных линий вниз
void delete_line(int line) {
    for (int i = line-1; i >= 0; --i)
        for (int j = 0; j < WIDTH; ++j)
            field[i+1][j] = field[i][j];

    for (int i = 0; i < WIDTH; ++i)
        field[0][i] = 0;
}

// Функция обновления игрового счета
void update_score(int deleted_lines) {
    switch (deleted_lines) {
        case 1: score += 1; break;
        case 2: score += 3; break;
        case 3: score += 5; break;
        case 4: score += 10;
                tetris = true;
    }
    level = (int)log2(score)+1;
    waiting_time = 1100 - level*100;
}

// Начальная инициализация поля
void init_field() {
    for (int i = 0; i < HEIGHT; ++i)
        for (int j = 0; j < WIDTH; ++j)
            field[i][j] = 0;
}

// Отрисовка своеобразной "рамочки"
void draw_frame(int fr_size) {
    print_spaces(4);
    cout << '+';
    for (int i = 0; i < fr_size+1; ++i)
        cout << '-';
    cout << '+';
}

// Отрисовка отдельной точки
void draw_point(int point) {
    switch (point) {
        case 0:
            cout << '.'; break;
        default:
            WORD Attribute = point + 8;
            SetConsoleTextAttribute(h, Attribute);
            cout << 'X';
            SetConsoleTextAttribute(h, 0x0008);
    }
    cout << ' ';
}

// Функция отрисовки поля
void draw_field() {
    bool next_item_display[4][4];
    copy_display(next_item_display, 1, next_item_type); // получаем отображение следующего блока
    set_color(next_item_display); // задаем ему нужный цвет

    print_spaces(4);
    draw_frame(HEIGHT);
    draw_frame(8);
    cout << endl;

    for (int i = 0; i < HEIGHT; ++i) {
        print_spaces(8);
        cout << "| ";
        for (int j = 0; j < WIDTH; ++j)
            draw_point(field[i][j]);
        cout << '|';
        if (i < 4) { // отрисовываем следующий блок
            cout << "    | ";
            for (int j = 0; j < 4; ++j)
                draw_point(next_item[i][j]);
            cout << '|' << endl;
        } else {
            switch (i) {
                case 4: draw_frame(8); break;
                case 6: cout << "    Score: " << score; break;
                case 7: cout << "    High Score: " << max(score, scores[0]); break;
                case 8: cout << "    Level: " << level; break;
                case 10: if (tetris) {
                             SetConsoleTextAttribute(h, 0x000C);
                             cout << "    TETRIS!!!";
                             SetConsoleTextAttribute(h, 0x0008);
                         }
            }
            cout << endl;
        }
    }

    print_spaces(4);
    draw_frame(HEIGHT);
    cout << endl;

    if (tetris) {
        Sleep(2000);
        tetris = false;
    }
}

// Функция проверки поля на заполненные линии
void check_lines() {
    int count = 0; // кол-во удаленных линий. От него зависит кол-во начисляемых очков
    for (int i = HEIGHT-1; i;) {
        bool is_full = true;
        for (int j = 0; j < WIDTH && is_full; ++j)
            if (!field[i][j])
                is_full = false;
        if (is_full) {
            delete_line(i);
            count++;
        } else i--;
    }
    if (count) update_score(count); // обновляем игровой счет

}

// Функция проверки коллизий блока с лежащими на поле элементами
bool has_collision(char command, bool prev_item[][4], bool item[][4], int x, int y, int size_x, int size_y) {
    bool result = false;
    if (x < 0 || y+size_y > HEIGHT || x+size_x > WIDTH) // если блок вышел за границы поля
        result = true;
    for (int i = 0; i < size_y && !result; ++i)
        for (int j = 0; j < size_x && !result; ++j)
            // Условие выхода за границы блока определяется тем, как мы хотим его сдвинуть, т.е. командой
            switch (command) { // проверка коллизий
                case 'a': // слева
                    if (!j || !prev_item[i][j-1]) result = (item[i][j] && field[y+i][x+j]);
                    break;
                case 'w': // при повороте
                    if (!prev_item[i][j]) result = (item[i][j] && field[y+i][x+j]);
                    break;
                case 'd': // справа
                    if (j+1 > 3 || !prev_item[i][j+1]) result = (item[i][j] && field[y+i][x+j]);
                    break;
                case 's': // снизу
                    if (i+1 > 3 || !prev_item[i+1][j]) result = (item[i][j] && field[y+i][x+j]);
            }
    return result;
}

// Функция генерации нового блока
void generate_new_item(bool *exit_game) {
    item_type = next_item_type;
    item_turn = 1;
    copy_display(item, item_turn, item_type);
    next_item_type = 1 + rand() % 7;

    item_size_X = sizes[item_turn-1][item_type-1][0];
    item_size_Y = sizes[item_turn-1][item_type-1][1];

    item_was_moved = false;
    clear_input_stream();

    item_Y = 0;
    switch (item_type) {
        case 1: item_X = rand() % 10; break;
        case 2:
        case 3:
        case 4: item_X = rand() % 9; break;
        default: item_X = rand() % 8;
    }

    if (exit_game != NULL) {
        bool empty_arr[4][4] = {false};
        if (has_collision('w', empty_arr, item, item_X, item_Y, item_size_X, item_size_Y))
            *exit_game = true;
    }
}

// Функция отрисовки блока на поле
void insert_item_to_field(bool item[][4], int x, int y, int type) {
    for (int i = 0; i < item_size_Y; ++i)
        for (int j = 0; j < item_size_X; ++j)
            if (item[i][j])
                field[y+i][x+j] = type;
}

// Функция удаления блока с поля
void remove_item_from_field(bool item[][4], int x, int y) {
    for (int i = 0; i < item_size_Y; ++i)
        for (int j = 0; j < item_size_X; ++j)
            if (item[i][j])
                field[y+i][x+j] = 0;
}

// Реализация падения блока вниз
void process_fall(int *y) {
    (*y)++;
}

// Функция сдвига блока влево
void move_left() {
    if (!has_collision('a', item, item, item_X-1, item_Y, item_size_X, item_size_Y)) {
        item_X--;
        item_was_moved = true;
    }
}

// Функция сдвига блока вправо
void move_right() {
    if (!has_collision('d', item, item, item_X+1, item_Y, item_size_X, item_size_Y)) {
        item_X++;
        item_was_moved = true;
    }
}

// Функция поворота блока
void rotate() {
    int new_turn = (item_turn == 4) ? 1 : item_turn + 1;
    int new_size_x = sizes[new_turn-1][item_type-1][0];
    int new_size_y = sizes[new_turn-1][item_type-1][1];
    bool rotated_item[4][4];
    copy_display(rotated_item, new_turn, item_type);

    if (!has_collision('w', item, rotated_item, item_X, item_Y, new_size_x, new_size_y)) {
        item_turn = new_turn;
        item_size_X = new_size_x;
        item_size_Y = new_size_y;
        copy_display(item, item_turn, item_type);
        item_was_moved = true;
    }
}

// Функция сбрасывания блока
void drop() {
    item_Y--; // коррекция положения блока на поле
    while (!has_collision('s', item, item, item_X, item_Y+1, item_size_X, item_size_Y)) {
           remove_item_from_field(item, item_X, item_Y);
           process_fall(&item_Y);
           insert_item_to_field(item, item_X, item_Y, item_type);
    }
    item_was_moved = true;
}

// Фукнция паузы игры
bool pause() {
    int mode = 0;
    bool chosen = false;
    char c = 0;
    do {
        system("cls");
        cout << "  Game paused." << endl << endl;
        cout << (mode == 0 ? '>' : ' ');
        cout << " Continue game" << endl;
        cout << (mode == 1 ? '>' : ' ');
        cout << " Leave game" << endl;

        c = getch();
        switch (c) {
            case 'w':
            case 's': mode = (mode == 0) ? 1 : 0; break;
            case '\r': chosen = true;
        }

    } while (!chosen);

    item_Y--; // возвращение к предыдущему кадру
    return mode;
}

// Функция считывания команд, введеных пользователем, и выполнения соответствующих им действий
bool process_keyboard() {
    char c = 0;
    if (kbhit()) c = getch();

    item_was_moved = false;
    switch (c) {
        case 'a': move_left(); break;
        case 'd': move_right(); break;
        case 'w': rotate(); break;
        case 's': drop(); break;
        case 'p': return pause();
    }
    return false;
}

// Процесс обратного отсчета перед игрой
void count_down() {
    for (int i = 20; i > 0; --i) {
        system("cls");
        cout << "Game will start in " << i << " seconds..." << endl << endl;
        cout << "Controls:" << endl
             << "a - turn left" << endl << "w - rotate" << endl << "d - turn right" << endl
             << "s - drop" << endl << "p - pause" << endl << endl;
        SetConsoleTextAttribute(h, 0x000C);
        cout << "Be sure that you have shifted language to English and Caps Lock is disabled!" << endl;
        cout << "Otherwise controls are not gonna work!" << endl << endl;
        SetConsoleTextAttribute(h, 0x0008);
        cout << "Press any key to speed up count down..." << endl;
        if (kbhit()) {
            getch();
            i -= 2;
        } else Sleep(1000);
    }
}

// Функция подведения итогов после завершения игры
void game_over() {
    Sleep(2000);
    int pos = check_for_new_high_score(); // смотрим, занял ли пользователь место в таблице рекордов
    if (pos) { // если занял,
        // 1. Выводим соответствующие сообщения и запрашиваем у пользователя имя
        string name;
        bool is_correct = true;
        clear_input_stream();
        do {
            system("cls");
            cout << "Game Over!" << endl << "Final score: " << score << endl << endl;
            cout << "Congratulations! You got into High Score list! :)" << endl;
            // Решил немного облегчить себе задачу и поставил условие, что имя не должно содержать пробелов
            cout << "Enter your name (it can\'t contain any spaces and be longer than 15 sybmols): " << endl;
            getline(cin, name);
            is_correct = check_name(name);
        } while (!is_correct);
        // 2. Обновляем переменные
        insert_to_array(scores, 5, score, pos);
        insert_to_array(levels, 5, level, pos);
        insert_to_array(names, 5, name, pos);
        // 3. Обновляем файл
        update_high_score_list();
        // 4. Показываем пользователю обновленную таблицу
        show_high_scores();
    } else { // иначе переходим в главное меню
        system("cls");
        cout << "Game Over!" << endl << "Final score: " << score << endl << endl;
        cout << "Press any key to return to main menu..." << endl;
        getch();
    }
}

// Функция определяет, нужно ли продолжать игру
// Игра заканчивается, когда на верхней линии поля есть хотя бы один элемент
bool check_exit() {
    bool rez = false;
    for (int i = 0; i < WIDTH && !rez; ++i)
        if (field[0][i])
            rez = true;
    return rez;
}

void show_about() {
    system("cls");
    cout << "This program is a low-level analogue of well known game \"Tetris\"." << endl;
    cout << "The author of algorithms and code of this program is Aslanov Alisher." << endl;
    cout << "MSHP Online-22" << endl << endl;
    cout << "Press any key to return to main menu..." << endl;
    getch();
}

// Процесс игры
void game_loop() {
    score = 0;
    level = 1;
    waiting_time = 1000;
    tetris = false;
    next_item_type = 1 + rand() % 7;

    init_field();
    generate_new_item(NULL);
    insert_item_to_field(item, item_X, item_Y, item_type);
    count_down();

    bool exit = false;
    do {
        if (!has_collision('s', item, item, item_X, item_Y+1, item_size_X, item_size_Y)) {
            remove_item_from_field(item, item_X, item_Y);
            process_fall(&item_Y);
            if (process_keyboard()) return;
        } else {
            check_lines();
            exit = check_exit();
            generate_new_item(&exit);
        }

        if (!item_was_moved)
            Sleep(waiting_time);
        insert_item_to_field(item, item_X, item_Y, item_type);

        system("cls");
        draw_field();

    } while (!exit);
    game_over();
}

// Функция main() будет выполнять роль главного меню
int main() {
    h = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(h, 0x0008);
    srand(time(NULL));
    get_high_scores();

    int mode = 0;
    do {
        bool chosen = false;
        char c = 0;
        mode = 0;
        do {
            system("cls");
            cout << "------- Welcome to Tetris :) -------" << endl << endl;
            cout << (mode == 0 ? '>' : ' ');
            cout << " New game" << endl;
            cout << (mode == 1 ? '>' : ' ');
            cout << " High Scores" << endl;
            cout << (mode == 2 ? '>' : ' ');
            cout << " About" << endl;
            cout << (mode == 3 ? '>' : ' ');
            cout << " Exit game" << endl << endl;
            cout << "Controls (works only with English keyboard and disabled Caps Lock):" << endl
                 << "w - Up" << endl << "s - Down" << endl << "Enter - Choose" << endl;

            c = getch();
            switch (c) {
                case 'w': mode = (mode == 0) ? 3 : mode - 1; break;
                case 's': mode = (mode == 3) ? 0 : mode + 1; break;
                case '\r': chosen = true;
            }
        } while (!chosen);

        switch (mode) {
            case 0: game_loop(); break;
            case 1: show_high_scores(); break;
            case 2: show_about(); break;
            case 3: system("cls"); cout << "Thanks for playing! :)" << endl << "Press Enter to exit" << endl;
        }
    } while (mode != 3);
    return 0;
}
