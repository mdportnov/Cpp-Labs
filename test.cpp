#include <stdio.h>
#include <string.h>
/* Перемешанная таблица с использованием перемешивания сложением, работа по запросам оператора, считывается и записывается в файл */
typedef struct Item {
    int ver;          /* Версия */
    int busy;         /* Занятость */
    int key;          /* Ключ */
    char info[100];   /* Информация элемента */
} Item;

const int SIZE = 10;                        /* Размер таблицы */


/* Описание функции: */                      /* Описание входа: */          /* Описание выхода: */


int I(int k);                               /* Хеш-функция */                            /* Ключ */                     /* Хешированный ключ */

int menu();                                 /* Функция меню */                           /*  */                         /* 0 при завершении работы */

int insert(Item *ptable);                   /*Добавить элемент*/                         /* Таблица */                  /* 0 при успешном вводе, -1 при переполнении */

void output(Item *ptable, int t);           /*Вывод таблицы*/                            /* Таблица, тип таблицы */     /*  */

void search(Item *ptable, Item *stable);    /*Поиск всех версий элемента*/               /* Таблица, таблица поиска */  /*  */

void purification(Item *ptable);            /*Очистка таблицы*/                          /* Таблица */                  /*  */

void deleting(Item *ptable);                /*Удаление всех версий элемента*/            /* Таблица */                  /*  */

void insertkey(int *a);                     /*Ввод ключа*/                               /* Ключ */                     /*  */


void insertkey(int *a) {
    while (scanf("%i", a) != 1) {
        printf("Enter the correct key -->");
        fflush(stdin);
    }
}

int I(int k) {
    return (k % SIZE);
}

int menu() {
    int a;
    FILE *f = NULL;                                                                                                     /* Файл */
    char filename[] = R"(C:\Users\Mi\Downloads)" , helpfilename[50];          /* Имя файла и вспомогательная переменная */
    Item ptable[SIZE];                                                                                                  /* Таблица */
    Item uber[SIZE];                                                                                                    /* Таблица поиска */
    printf("\nHello!\n");
    printf("\nEnter file name --> ");
    scanf("%s", helpfilename);                                                                                  //Ввод имени файла
    strcat(filename, helpfilename);                                                                                     //Получение пути
    printf("\nFile reading...\n");
    f = fopen(filename, "rb+");                                                                                  //Попытка открытия чтения-записи для файла
    fseek(f, 0, SEEK_END);                                                                                       //Установка указателя в конец
    if (ftell(f) == -1){                                                                                                //Если файл не существует
        printf("\nFile with the specified name does not exist\n\nFile creation...\n\nFile was created successfully\n");
        fclose(f);                                                                                                      //Закрытие файла
        f = fopen(filename, "w+");                                                                               //Создание файла и его открытие для чтения-записи
    }
    fseek(f, 0, SEEK_END);                                                                                       //Установка указателя в конец
    printf("\nFile Size -> %li\n", ftell(f));
    if (ftell(f) != 0){                                                                                                 //Если файл не пуст
        fseek(f, 0, SEEK_SET);                                                                                   //Установка указателя в начало
        fread(ptable, sizeof(Item), SIZE, f);                                                                           //Считывание файла
        printf("\nReading was performed successfully\n\n");
    }
    else{
        printf("\nFile is empty\n\n");
    }
    printf("_________________________________________\n");
    while (printf(
            "Select Action:\n1)Add Element\n2)Search\n3)Delete\n4)Output\n5)Purification\n6)Exit\nEnter action -->")) {
        while ((scanf("%i", &a) != 1) || ((a != 1) && (a != 2) && (a != 3) && (a != 4) && (a != 5) && (a != 6))) {
            fflush(stdin);
            printf("Enter the correct action -->");
        }
        if (a == 1) {
            if (insert(ptable) == -1){
                printf("Table is overfull\n");
            }
            else{
                printf("Element was added successfully\n");
            }
        }
        if (a == 2) {
            printf("_________________________________________\n");
            search(ptable, uber);
            output(uber, 2);
            purification(uber);
            printf("_________________________________________\n");
            continue;                                   //Чтобы не производить лишнюю запись в файл
        }
        if (a == 3) {
            deleting(ptable);
        }
        if (a == 4) {
            printf("_________________________________________\n");
            output(ptable, 1);
        }
        if (a == 5) {
            purification(ptable);
        }
        if (a == 6) {
            fclose(f);                                  //Закрытие файла для чтения-записи
            printf("\nGood Bye!\n");
            printf("_________________________________________\n");
            return 0;
        }
        fseek(f, 0, SEEK_SET);                  //Установка указателя в начало
        fwrite(ptable, sizeof(Item), SIZE, f);         //Запись изменений в файл
        printf("\nChanges were recorded successfully\n");
        printf("_________________________________________\n");
    }
    return 0;                                          //Чтобы компилятор не ругался
}

int insert(Item *ptable) {
    int k, v=0, strt, i, h = 1;      /* k - ключ, v - версия, h - шаг перемешивания, strt для определения переполнения таблицы */
    char in[100];                    /* Информация */
    printf("Insert key -->");
    insertkey(&k);
    fflush(stdin);
    printf("Insert info -->");
    gets(in);
    strt = I(k);                     /* Вычисление исходной позиции таблицы */
    i = I(k);
    while (ptable[i].busy == 1) {    /* Позиция занята */
        if (ptable[i].key == k) {    /* Элемент с заданным ключом есть в таблице - +1 к версии*/
            v++;
        }
        i = (i + h) % SIZE;          /* Следующая позиция */
        if (i == strt) {             /* Верулись в исходную позицию - таблица полна, возвращаем -1 */
            return -1;
        }
    }
    ptable[i].key = k;               /* Занесение нового элемента */
    ptable[i].ver = v;
    ptable[i].busy = 1;
    strcpy(ptable[i].info, in);
    return 0;                        /* Ввод совершён успешно - возвращаем 0 */
}

void output(Item *ptable, int t) {
    printf("#...key...ver...info\n");
    for (int i = 0; i < SIZE; ++i) {
        if (ptable[i].busy == 1) {                     /* Если элемент есть */
            if (ptable[i].key < 10) {
                printf("%i... %i ... %i ...%s\n", i, ptable[i].key, ptable[i].ver, ptable[i].info);
            }
            if ((ptable[i].key > 9) && (ptable[i].key < 100)) {
                printf("%i...%i ... %i ...%s\n", i, ptable[i].key, ptable[i].ver, ptable[i].info);
            }                                          /* 3 варианта вывода для красоты */
            if (ptable[i].key > 99) {
                printf("%i...%i... %i ...%s\n", i, ptable[i].key, ptable[i].ver, ptable[i].info);
            }
        }
        if ((ptable[i].busy != 1) && (t == 1)) {       /*Если элемента нет и работа не с таблицей поиска*/
            printf("%i...___...___...____\n", i);
        }
    }
}

void search(Item *ptable, Item *stable) {
    int k, j = 0;                                               /* k - ключ, j -счётчик таблицы вывода */
    printf("Insert key -->");
    insertkey(&k);
    for (int i = 0; i < SIZE; i++) {
        if ((ptable[i].busy == 1) && (ptable[i].key == k)) {    /*Если найден элемент с данным ключом*/
            strcpy(stable[j].info, ptable[i].info);             /*Добавляем его в таблицу вывода*/
            stable[j].busy = ptable[i].busy;
            stable[j].key = ptable[i].key;
            stable[j].ver = ptable[i].ver;
            j++;
        }
    }
}

void purification(Item *ptable) {
    for (int i = 0; i < SIZE; i++) {
        ptable[i].busy = 0;
    }
}

void deleting(Item *ptable) {
    int k;                                                      /* k - ключ */
    printf("Insert key -->");
    insertkey(&k);
    for (int i = 0; i < SIZE; i++) {
        if ((ptable[i].busy == 1) && (ptable[i].key == k)) {    /*Если найден элемент с данным ключом*/
            ptable[i].busy = 0;                                 /*Очищаем элемент*/
        }
    }
}

int main() {
    return menu();
}