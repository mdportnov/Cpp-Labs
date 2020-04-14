#include <cstring>
#include "iostream"
#include "string"

using namespace std;
const int SIZEP = 10;
const int SIZEC = 4;

// Перемешивание сцеплением - надо хранить информацию в двух файлах,
// поскольку мы не знаем заранее возможное кол-во элементов
// Файл fd (.dat) создаётся и изменяется только в момент вставки элемента в любую из таблиц (insert)
// Файл ft (.tab) создаётся в начале, хранит в себе отступы, ключи, а так же размер данных, хранимых в (.dat), а обновляется при save
// Функция удаления не подверглась никаким изменениям после 4а - метаинформация остаётся мусором в .tab файле

struct Item {
    int size; //длина строки
//    char *info;
    int offset; //место первой буквы в ПЗУ
    int key;
};

struct Parent_Item : Item {
    Parent_Item *next;
};
struct Child_Item : Item {
    int pkey; // Ключ элемента родительской таблицы
    Child_Item *next;
};

struct Parent_Table {
    Parent_Item *pItem[SIZEP]{};
    FILE *ft{};
    char *fname{};
    int n = SIZEP;
};

struct Child_Table {
    Child_Item *cItem[SIZEC]{};
    FILE *ft{};
    char *fname{};
    int n = SIZEC;
};

Parent_Item *getLast(Parent_Item *head) {
    if (head == nullptr) {
        return nullptr;
    }
    while (head->next) {
        head = head->next;
    }
    return head;
}
Child_Item *getLast_c(Child_Item *head) {
    if (head == nullptr) {
        return nullptr;
    }
    while (head->next) {
        head = head->next;
    }
    return head;
}

void pushBack(Parent_Item *head, Parent_Item *parentItem) {
    Parent_Item *last = getLast(head);
    auto *tmp = (Parent_Item *) malloc(sizeof(Parent_Item));
    tmp->key = parentItem->key;
    tmp->size = parentItem->size;
    tmp->offset = parentItem->offset;
    tmp->next = nullptr;
    last->next = tmp;
}
void pushBack_c(Child_Item *head, Child_Item *childItem) {
    Child_Item *last = getLast_c(head);
    auto *tmp = (Child_Item *) malloc(sizeof(Child_Item));
    tmp->key = childItem->key;
    tmp->size = childItem->size;
    tmp->offset = childItem->offset;
    tmp->next = nullptr;
    last->next = tmp;
}

const char *msgs[] = {" 0. Exit", " 1. Add Parent Item", " 2. Add Child Item", " 3. Delete Item from Parent Table ",
                      " 4. Show Parent Table", " 5. Show Child Table", " 6. Save"};

const int NMsgs = sizeof(msgs) / sizeof(msgs[0]);

int getNaturalInt(int *a);

int hashFunction(int key, int table_type);

char *getStr();

int dialog(const char *msgs[], int N);

char *findInfo(Parent_Table *pTable, int key);
char *findcInfo(Child_Table *pTable, int key);

Parent_Item *findItemByKey(Parent_Table *pTable, int key);

int findByKey(Parent_Table *, int), findByKeyInChild(Child_Table *, int);
int findByKeyInChildforPkey(Child_Table *table, int key);

int delete_element(Child_Table *, Parent_Table *), show_parent_table(Child_Table *, Parent_Table *),
        show_child_table(Child_Table *, Parent_Table *), insertInChildTable(Child_Table *table, Parent_Table *ptable), insertInParentTable(Child_Table *cTable, Parent_Table *pTable);

int load(Parent_Table *pTable, Child_Table *cTable, char *fname);

int save(Child_Table *, Parent_Table *pTable);

int (*table_functions[])(Child_Table *, Parent_Table *) ={nullptr, insertInParentTable, insertInChildTable, delete_element,
                                                      show_parent_table, show_child_table, save};

int main() {
    int rc = 0;
    auto *pTable = new Parent_Table;
    auto *cTable = new Child_Table;

    char *filename;
    printf("Input table name: \n");
    filename = getStr();

    load(pTable, cTable, filename);

    while ((rc = dialog(msgs, NMsgs))) {
        if (!table_functions[rc](cTable, pTable))
            break;
    }
    printf("That's all. Bye!\n");

    fclose(pTable->ft);
    free(pTable);
    fclose(cTable->ft);
    free(cTable);
}

int load(Parent_Table *pTable, Child_Table *cTable, char *fname) {
    FILE *fd = nullptr;
    int len = strlen(fname);  // длина основной части имени файла
    pTable->fname = fname;
    cTable->fname = fname;
//    strcpy(pTable->fname, fname);
//    strcpy(cTable->fname, fname);

    FILE *fdc = nullptr;
    // имена файлов для ДОЧЕРНЕЙ таблицы

    char *fdcname = (char *) malloc((len + 5) * sizeof(char)); // Файл информации таблицы (сохраняется в insert)
    strcpy(fdcname, "c");
    strcat(fdcname, cTable->fname);
    strcat(fdcname, ".dat");

    char *ftcname = (char *) malloc(sizeof(char) * (len + 5));  // Файл структуры таблицы, отступы, размеры (сохраняется в save)
    strcpy(ftcname, "c");
    strcat(ftcname, cTable->fname);
    strcat(ftcname, ".tab");

    // имена файлов для РОДИТЕЛЬСКОЙ таблицы
    char *fdname = (char *) malloc((len + 4) * sizeof(char)); // Файл информации таблицы (сохраняется в insert)
    strcpy(fdname, pTable->fname);
    strcat(fdname, ".dat");

    char *ftname = (char *) malloc(
            sizeof(char) * (len + 4));  // Файл структуры таблицы, отступы, размеры (сохраняется в save)
    strcpy(ftname, pTable->fname);
    strcat(ftname, ".tab");

    // открываем существующий файл РОДИТЕЛЬСКОЙ таблицы (со словами) для ЧТЕНИЯ как бинарный
    fopen_s(&fd, fdname, "rb");
    fopen_s(&fdc, fdcname, "rb");
//    printf("New files: %s, %s & %s, %s was created\n\n", fdname, ftname, fdcname, ftcname);

    // Если он существует, то работаем с ним
    if (fd != nullptr) { // считываем данные
        printf("Files have already created: %s & %s , %s & %s\n", fdname, ftname, fdcname, ftcname);

        // открываем файлы с отступами на чтение и запись (бинарные)
        fopen_s(&(pTable->ft), ftname, "r+b");

        if (pTable->ft == nullptr) {
            free(ftname); // если нет, то ошибка
            cout << "\nError: cannot find .tab file \n";
            return 0;
        }

        // РОДИТЕЛЬСКАЯ таблица
        auto *item = (Parent_Item *) malloc(sizeof(Parent_Item));
        int position;

        // вставляем элемент из файла в таблицу
        while (fread(&item->size, sizeof(int), 1, pTable->ft) &&
               fread(&item->offset, sizeof(int), 1, pTable->ft) &&
               fread(&item->key, sizeof(int), 1, pTable->ft)) {

            printf("%d %d %d\n", item->size, item->offset, item->key);
            position = hashFunction(item->key, 0);

            if (pTable->pItem[position] == nullptr) { // Если первая ячейка пуста
                item->next = nullptr;
                pTable->pItem[position] = new Parent_Item;
                pTable->pItem[position]->size = item->size;
                pTable->pItem[position]->offset = item->offset;
                pTable->pItem[position]->key = item->key;
                pTable->pItem[position]->next = nullptr;

                cout << "\nOK\n";
            } else { // Если нет
                pushBack(pTable->pItem[position], item);
            }
            cout << endl << "Element from your file was added!\n" << endl;
        }
        free(item);
        fclose(pTable->ft);
        fclose(fd); // закрываем файл


        fopen_s(&(cTable->ft), ftcname, "r+b");

        // ДОЧЕРНЯЯ таблица
        auto *c_item = (Child_Item *) malloc(sizeof(Child_Item));
        // вставляем элемент из файла в таблицу
        while (fread(&c_item->size, sizeof(int), 1, cTable->ft) &&
               fread(&c_item->offset, sizeof(int), 1, cTable->ft) &&
               fread(&c_item->key, sizeof(int), 1, cTable->ft) &&
               fread(&c_item->pkey, sizeof(int), 1, cTable->ft)) {

            printf("%d %d %d %d\n", c_item->size, c_item->offset, c_item->key, c_item->pkey);
            position = hashFunction(item->key, 1);

            if (cTable->cItem[position] == nullptr) { // Если первая ячейка пуста
                c_item->next = nullptr;
                cTable->cItem[position] = new Child_Item;
                cTable->cItem[position]->size = c_item->size;
                cTable->cItem[position]->offset = c_item->offset;
                cTable->cItem[position]->key = c_item->key;
                cTable->cItem[position]->pkey = c_item->pkey;
                cTable->cItem[position]->next = nullptr;
            } else { // Если нет
                pushBack_c(cTable->cItem[position], c_item);
            }
            cout << endl << "Element from your file was added!\n" << endl;
        }
        free(c_item);
        fclose(cTable->ft);
        fclose(fdc); // закрываем файл

        return 1;
    } else { // иначе создаем файл данных
        fopen_s(&(pTable->ft), ftname, "w+b");
        fopen_s(&(cTable->ft), ftcname, "w+b");
        if (pTable->ft != nullptr && cTable->ft != nullptr) {
            printf("New files: %s, %s & %s, %s was created\n\n", fdname, ftname, fdcname, ftcname);
            fclose(pTable->ft);
            fclose(cTable->ft);
            fclose(fd);
            fclose(fdc);
            return 1;
        }
    }
    free(ftname);
    free(ftcname);
    return 1;
}

// Заносим в таблицу ft данные о смещениях и в таблицу fd саму информацию
int insertInParentTable(Child_Table *cTable, Parent_Table *pTable) {
    int key;
    printf(" Input key: ");
    getNaturalInt(&key);
    cin.sync(); // очистка буфера
    char *str = nullptr;
    printf("Input information: ");
    str = getStr();
    if (findByKey(pTable, key)) {
        printf("\n Item with this key = [%d] was founded \n Item not added\n\n", key);
        return 1; // Вставить нельзя, ошибка
    }

    int position = hashFunction(key, 0);

    char *ftname = (char *) malloc(sizeof(char) * (strlen(pTable->fname) + 4));
    strcpy(ftname, pTable->fname);
    strcat(ftname, ".tab");

    // Открыл файл для записи отступов
    fopen_s(&(pTable->ft), ftname, "w+b");

    if (pTable->ft == nullptr) {
        printf("\nCan't open\n");
        return 0;
    }

    char *fdname = (char *) malloc((strlen(pTable->fname) + 4) * sizeof(char));
    strcpy(fdname, pTable->fname);
    strcat(fdname, ".dat");

    FILE *fd; // Запись в файл со словами
    fopen_s(&fd, fdname, "a+t");

    auto *item = (Parent_Item *) malloc(sizeof(Parent_Item)); // создание вставляемого элемента
    item->key = key;
    item->size = (int) strlen(str) + 1;
    fseek(fd, 0, SEEK_END);
    item->offset = ftell(fd);
    printf("Offset is: %d", item->offset);

    Parent_Item *tmpItem = nullptr;
    if (pTable->pItem[position] == nullptr) {
        item->next = nullptr;
        pTable->pItem[position] = item;
    } else {
        tmpItem = pTable->pItem[position];
        while (tmpItem) {
            if (tmpItem->key == key) {
                if (tmpItem->next != nullptr && tmpItem->next->key != key) break;
            }
            if (tmpItem->next == nullptr) break;
            tmpItem = tmpItem->next;
        }
        item->next = tmpItem->next;
        tmpItem->next = item;
    }
    cout << endl << "Unique item successfully added!\n" << endl;

    fwrite(str, sizeof(char), item->size, fd);

    fclose(fd);
    fclose(pTable->ft);
    return 1;
}

int insertInChildTable(Child_Table *cTable, Parent_Table *pTable) {
    int key, pkey;
    printf("Input key: \n");
    getNaturalInt(&key);
    cin.sync(); // очистка буфера

    printf("Input parent key: \n");
    getNaturalInt(&pkey);
    cin.sync();

    char *str = nullptr;
    printf("Input information: ");
    str=getStr();

    int position = hashFunction(key, 1);

    if(!findByKeyInChild(cTable, key) && findByKey(pTable, pkey)) {

        char *ftcname = (char *) malloc(sizeof(char) * (strlen(cTable->fname) + 4));
        strcpy(ftcname, "c");
        strcat(ftcname, cTable->fname);
        strcat(ftcname, ".tab");

        // Открыл файл для записи отступов
        fopen_s(&(cTable->ft), ftcname, "w+b");

        if (cTable->ft == nullptr) {
            printf("\nCan't open\n");
            return 0;
        }

        char *fdcname = (char *) malloc((strlen(cTable->fname) + 5) * sizeof(char));
        strcpy(fdcname, "c");
        strcat(fdcname, pTable->fname);
        strcat(fdcname, ".dat");

        FILE *fdc; // Запись в файл со словами
        fopen_s(&fdc, fdcname, "a+t");

        auto *item = (Child_Item *) malloc(sizeof(Child_Item)); // создание вставляемого элемента
        item->key = key;
        item->pkey = pkey;
        item->size = (int)strlen(str) + 1;
        fseek(fdc, 0, SEEK_END);
        item->offset = ftell(fdc);
        printf("Offset is: %d", item->offset);

        Child_Item *tmpItem = nullptr;
        if (cTable->cItem[position] == nullptr) {
            item->next = nullptr;
            cTable->cItem[position] = item;
        } else {
            tmpItem = cTable->cItem[position];
            while (tmpItem) {
                if (tmpItem->key == key) {
                    if (tmpItem->next != nullptr && tmpItem->next->key != key) break;
                }
                if (tmpItem->next == nullptr) break;
                tmpItem = tmpItem->next;
            }
            item->next = tmpItem->next;
            tmpItem->next = item;
        }
        cout << endl << "Unique item successfully added!\n" << endl;

        fwrite(str, sizeof(char), item->size, fdc);

        fclose(fdc);
        fclose(cTable->ft);
        return 1;
    } else{
        cout << endl << "Item not added!\n" << endl;
        return 1;
    }

}

int save(Child_Table *cTable, Parent_Table *pTable) {
    char *ftname = (char *) malloc((strlen(pTable->fname) + 4) * sizeof(char));
    strcpy(ftname, pTable->fname);
    strcat(ftname, ".tab");

    char *ftcname = (char *) malloc((strlen(cTable->fname) + 4) * sizeof(char));
    strcpy(ftcname, "c");
    strcat(ftcname, cTable->fname);
    strcat(ftcname, ".tab");

    printf("\nSaving to %s & %s\n", ftname, ftcname);

    fopen_s(&(pTable->ft), ftname, "w+b");
    fopen_s(&(cTable->ft), ftcname, "w+b");

    if (pTable->ft == nullptr || cTable->ft== nullptr) {
        printf("\nIs Null\n");
        return 0; // ошибка при создании файла
    }

    Parent_Item *cur = nullptr;
    // запись файла в таблицу
    for (int i = 0; i < pTable->n; i++) {
        for (cur = pTable->pItem[i]; cur; cur = cur->next) {
            fwrite(&(cur->size), sizeof(int), 1, pTable->ft);
            fwrite(&(cur->offset), sizeof(int), 1, pTable->ft);
            fwrite(&(cur->key), sizeof(int), 1, pTable->ft);
        }
    }

    Child_Item *ccur = nullptr;
    // запись файла в таблицу
    for (int i = 0; i < cTable->n; i++) {
        for (ccur = cTable->cItem[i]; ccur; ccur = ccur->next) {
            fwrite(&(ccur->size), sizeof(int), 1, cTable->ft);
            fwrite(&(ccur->offset), sizeof(int), 1, cTable->ft);
            fwrite(&(ccur->key), sizeof(int), 1, cTable->ft);
            fwrite(&(ccur->pkey), sizeof(int), 1, cTable->ft);
        }
    }

    fclose(pTable->ft);
    fclose(cTable->ft);
    printf("\nFile \"%s\" & \"%s\" was saved\n", ftname, ftcname);

    return 1;
}

int show_parent_table(Child_Table *, Parent_Table *table) {
    Parent_Item *tmpItem;
    int size = table->n;
    cout << endl;

    for (int i = 0; i < size; i++) {
        if ((table->pItem[i]) != nullptr) { // Проверка на пустоту списков элементов
            cout << "[" << i << "]";
            tmpItem = table->pItem[i];

            while (tmpItem) {
                printf("\n    [key: %d | size of info: %d | info: %s ]", tmpItem->key, tmpItem->size - 1,
                       findInfo(table, tmpItem->key));
//                    cout << "\n    [key: " << tmpItem->key << " | info: " << findInfo(table, tmpItem->key) << "] ";
                tmpItem = tmpItem->next;
            }
            cout << endl;
        } else cout << "[" << i << "] " << "..." << endl;
    }
    return 1;
}

int show_child_table(Child_Table *table, Parent_Table *) {
    Child_Item *tmpItem;
    int size = table->n;
    cout << endl;

    for (int i = 0; i < size; i++) {
        if ((table->cItem[i]) != nullptr) { // Проверка на пустоту списков элементов
            cout << "[" << i << "]";
            tmpItem = table->cItem[i];

            while (tmpItem) {
                printf("\n    [key: %d | pkey: %d | size of info: %d | info: %s ]", tmpItem->key, tmpItem->pkey, tmpItem->size - 1,
                       findcInfo(table, tmpItem->key));
                tmpItem = tmpItem->next;
            }
            cout << endl;
        } else cout << "[" << i << "] " << "..." << endl;
        cout<<"\n";
    }
    return 1;
}

int delete_element(Child_Table* childTable, Parent_Table * parentTable){
    int pkey, position;
    printf("Input key for delete element in Parent Table: \n");
    getNaturalInt(&pkey);
    cin.sync(); // очистка буфера

    Parent_Item *tmpItem;
    position=hashFunction(pkey, 0);

    if(!findByKeyInChildforPkey(childTable, pkey)) {
        if (parentTable->pItem[position] == nullptr) {
            printf("\nItem in this cell doesn't exist\n\n");
            return 1; // Не найдено
        }else if (parentTable->pItem[position]->key == pkey) { // Если первый элемент подходит
            parentTable->pItem[position] = parentTable->pItem[position]->next;
            printf("\nThe element was successfully removed \n\n");
        }else {
            tmpItem = parentTable->pItem[position];
            while (tmpItem) {
                if (tmpItem->next->key == pkey) {
                    tmpItem->next = tmpItem->next->next;
//                cout << endl << "[k: " << tmpItem->key << " r: " << tmpItem->release << " i: " << tmpItem->info << "] ";
                    printf("\nThe element was successfully removed \n\n");
                    return 1; // Найдено
                }
                tmpItem = tmpItem->next;
            }
        }

    } else printf("\nThe child table has a related element\n\n");

    return 1;
}

int findByKey(Parent_Table *table, int key) {
    int position = hashFunction(key, 0);
    Parent_Item *tmpItem;

    if (table->pItem[position] == nullptr) {
        return 0; // Не найдено
    } else {
        tmpItem = table->pItem[position];
        while (tmpItem) {
            if (tmpItem->key == key) {
//                cout << endl << "[k: " << tmpItem->key << " r: " << tmpItem->release << " i: " << tmpItem->info << "] ";
                return 1; // Найдено
            }
            tmpItem = tmpItem->next;
        }
    }
    return 0; // Не найдено
}

int findByKeyInChild(Child_Table *table, int key) {
    int position = hashFunction(key, 1);
    Child_Item *tmpItem;

    if (table->cItem[position] == nullptr) {
        return 0; // Не найдено
    } else {
        tmpItem = table->cItem[position];
        while (tmpItem) {
            if (tmpItem->key == key) {
//                cout << endl << "[k: " << tmpItem->key << " r: " << tmpItem->release << " i: " << tmpItem->info << "] ";
                return 1; // Найдено
            }
            tmpItem = tmpItem->next;
        }
    }
    return 0; // Не найдено
}

int findByKeyInChildforPkey(Child_Table *table, int pkey) {
    Child_Item *tmpItem;

    for (int i = 0; i < table->n ; i++) {
        tmpItem = table->cItem[i];
        while (tmpItem) {
            if (tmpItem->pkey == pkey) {
//                cout << endl << "[k: " << tmpItem->key << " r: " << " i: " << tmpItem->info << "] ";
                return 1; // Найдено
            }
            tmpItem = tmpItem->next;
        }
    }
    return 0; // Нe найдено
}

// Возвращает элемент по ключу
Parent_Item *findItemByKey(Parent_Table *pTable, int key) {
    int index = hashFunction(key, 0);
    Parent_Item *cur;
//    for(; i < pTable.n; ++i)
    for (cur = pTable->pItem[index]; cur; cur = cur->next) {
        if (cur->key == key)
            return cur;
    }
    return nullptr;
}

Child_Item *findItemByKeyInChild(Child_Table *cTable, int key) {
    int index = hashFunction(key, 1);
    Child_Item *cur;
    for (cur = cTable->cItem[index]; cur; cur = cur->next) {
        if (cur->key == key)
            return cur;
    }
    return nullptr;
}

//  Возвращает инфу по ключу
char *findInfo(Parent_Table *pTable, int key) {
    char *info = nullptr;
    Parent_Item *tmpItem = findItemByKey(pTable, key);
//    printf("\nSize: %d | Offset: %d | Key: %d: \n", tmpItem->size, tmpItem->offset, tmpItem->key);
    FILE *fd = nullptr;
    char *fdname = (char *) malloc(
            (strlen(pTable->fname) + 4) * sizeof(char)); // Файл информации таблицы (сохраняется в insert)
    strcpy(fdname, pTable->fname);
    strcat(fdname, ".dat");
    fopen_s(&fd, fdname, "rb");

    info = (char *) malloc(tmpItem->size);
    fseek(fd, tmpItem->offset, SEEK_SET);
    fread(info, sizeof(char), tmpItem->size, fd);
    fclose(fd);
//    printf("\nInfo: %s\n", info);
    return info;
}

char *findcInfo(Child_Table *cTable, int key){
    char *info = nullptr;
    Child_Item *tmpItem = findItemByKeyInChild(cTable, key);
//    printf("\nSize: %d | Offset: %d | Key: %d: \n", tmpItem->size, tmpItem->offset, tmpItem->key);
    FILE *fd = nullptr;
    char *fdname = (char *) malloc(
            (strlen(cTable->fname) + 5) * sizeof(char)); // Файл информации таблицы (сохраняется в insert)
    strcpy(fdname, "c");
    strcat(fdname, cTable->fname);
    strcat(fdname, ".dat");

    fopen_s(&fd, fdname, "rb");

    info = (char *) malloc(tmpItem->size);
    fseek(fd, tmpItem->offset, SEEK_SET);
    fread(info, sizeof(char), tmpItem->size, fd);
    fclose(fd);
//    printf("\nInfo: %s\n", info);
    return info;
}

// Utils
int dialog(const char *msgs[], int N) {
    string errmsg = "";
    int rc, n;

    do {
        cout << errmsg;
        errmsg = "You are wrong. Repeat, please\n";
        for (int j = 0; j < N; ++j)
            puts(msgs[j]);
        puts("Make your choice: --> ");

        n = getNaturalInt(&rc);
        if (n == 0)
            rc = 0;
    } while (rc < 0 || rc >= N);
    return rc;
}

int getNaturalInt(int *a) {
    int n;
    do {
        n = scanf("%d", a);
        if (n < 0)
            return 0;
        if (n == 0 || *a < 0) {
            printf("%s\n", "Error! Please, repeat your input: ");
            scanf("%*[^\n]");
        }
    } while (n == 0 || *a < 0);
    return 1;
}

char *getStr() {
    char *ptr = (char *) malloc(1);
    char buf[81];
    int n, len = 0;
    *ptr = '\0';
    do {
        n = scanf("%80[^\n]", buf);
        if (n < 0) {
            free(ptr);
            return nullptr;
        } else if (n) {
            len += strlen(buf);
            ptr = (char *) realloc(ptr, len + 1);
            int k = strcspn(buf, "\0"); // длина слова
            strncat(ptr, buf, k);
        }
    } while (n > 0);
    scanf("%*c");
    return ptr;
}

int hashFunction(int key, int table_type) {
    if (table_type == 0)
        return key % SIZEP;
    if (table_type == 1)
        return key % SIZEC;
}