// Copyright 2022 <elektrab>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<fcntl.h>
#define X 80  // Ширина поля
#define Y 25  // Высота поля

void draw(short field[Y][X]);  // Отрисовка поля по заданному массиву с координатами 1 0
void input(short field[Y][X]);  // Инициализация поля значениями из файла
int step(short field[Y][X]);  // Следующее поколение
int who_is_alive(short y, short x, short field[Y][X]);  // Опр для ячейки с коорд x y кол-во живых соседей
int listen(double *speed);  // Считываем команды пользователя

int main() {
    double speed = 1;  // Начальная скорость
    short field[Y][X];  // Создание поля

    input(field);

    if (freopen("/dev/tty", "r", stdin) == NULL) {
        printf("Не удалось переключить поток ввода");
    }
    system("stty cbreak");

    draw(field);

    while (1) {  // Запуск игры
        if (step(field) == 1) {  // Если изменения произошли, то отрисовываем новый кадр,
            draw(field);
            printf("\nСкорость %.1lf", speed);
            if (listen(&speed) == 1) {
                printf("\nИгра окончена\n");
                break;
            }
            usleep(1 / speed * 100000);
            printf("\e[H\e[2J\e[3J");
        } else {  // иначе завершаем игру
            printf("Игра окончена\n");
            break;
        }
    }
    return 0;
}

int listen(double *speed) {
    int is_ended = 0;
    char symb;
    int old = fcntl(0, F_GETFL);
    fcntl(0, F_SETFL, old | O_NONBLOCK);
    symb = getchar();
    fcntl(0, F_SETFL, old);
    if (symb == ' ') {
        symb = 'x';
        while (symb != ' ') {
            old = fcntl(0, F_GETFL);
            fcntl(0, F_SETFL, old | O_NONBLOCK);
            symb = getchar();
            fcntl(0, F_SETFL, old);
        }
    } else if (symb == '.') {
        if (*speed < 9.9) {  // Если скорость меньше 9.9, то ее можно увеличить
            *speed += 0.1;  // Увеличивает скорость
        }
    } else if (symb == ',') {
        if (*speed > 0.2) {
            *speed -= 0.1;  // Уменьшает скорость
        }
    } else if (symb == 'e') {
        is_ended = 1;
    }
    return is_ended;
}

void draw(short field[Y][X]) {
    for (int y = 0; y < Y; y++) {
        for (int x = 0; x < X; x++) {
            if (field[y][x] == 0) {  // Мертвая клетка
                printf(" ");
            } else if (field[y][x] == 1) {  // Живая клетка
                printf("0");
            }
        }
        printf("\n");
    }
}

void input(short field[Y][X]) {
    char c;
    for (int y = 0; y < Y; y++) {
        for (int x = 0; x < X; x++) {
            scanf("%c", &c);
            if (c == '0') {
                field[y][x] = 0;
            } else if (c == '1') {
                field[y][x] = 1;
            }
        }
        scanf("%c", &c);
    }
}

int step(short field[Y][X]) {  // Если при смене кадра ничего не изменилось, вернется 0
    int is_changed = 0;
    short tmp_field[Y][X];
    int alive_neighbors;
    for (int y = 0; y < Y; y++) {
        for (int x = 0; x < X; x++) {
            alive_neighbors = who_is_alive(y, x, field);
            if (field[y][x] == 1) {
                if (alive_neighbors != 2 && alive_neighbors != 3) {
                    tmp_field[y][x] = 0;
                    is_changed = 1;
                } else {
                    tmp_field[y][x] = 1;
                }
            } else if (field[y][x] == 0) {
                if (alive_neighbors == 3) {
                    tmp_field[y][x] = 1;
                    is_changed = 1;
                } else {
                    tmp_field[y][x] = 0;
                }
            }
        }
    }

    for (int y = 0; y < Y; y++) {
        for (int x = 0; x < X; x++) {
            field[y][x] = 0;
            if (tmp_field[y][x] == 1) {
                field[y][x] = 1;
            }
        }
    }
    return is_changed;
}

int who_is_alive(short y, short x, short field[Y][X]) {
    int c = 0;
    for (int yl = y - 1; yl <= y + 1; yl++) {
        for (int xl = x - 1; xl <= x + 1; xl++) {
            if (xl != x || yl != y) {
                if (field[(Y + (yl % Y)) % Y][(X + (xl % X)) % X] == 1) {
                    c++;
                }
            }
        }
    }
    return c;
}
