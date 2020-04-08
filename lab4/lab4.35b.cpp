#include <cstring>
#include "iostream"
#include "string"

using namespace std;
const int SIZEP = 10;
const int SIZEC = 4;

// Перемешивание сцеплением.

struct Item {
    int key;
    char *info;

    long offset; //место первой буквы в ПЗУ
    int size; //длина строки
};

struct Parent_Item : Item {
    Parent_Item *next;
};

struct Child_Item : Item {
    int pkey; // Ключ элемента родительской таблицы
    Child_Item *next;
};

struct Table {
};

struct Child_Table : Table {
    Child_Item *cItem[SIZEC]{};
    int n = SIZEC;
};

struct Parent_Table : Table {
    Parent_Item *pItem[SIZEP]{};
    int n = SIZEP;
};

int getNaturalInt(int *a);

int hashFunction(int key, int table_type);

char *getStr();

const char *msgs[] = {" 0. Quit", " 1. Add Parent Item", " 2. Add Child Item", " 3. Delete Item from Parent Table ",
                      " 4. Show Parent Table", " 5. Show Child Table"};

const int NMsgs = sizeof(msgs) / sizeof(msgs[0]);

int dialog(const char *msgs[], int N);

int add_parent_element(Child_Table *, Parent_Table *), add_child_element(Child_Table *, Parent_Table *ptable),
        delete_element(Child_Table *, Parent_Table *), show_parent_table(Child_Table *, Parent_Table *),
        show_child_table(Child_Table *, Parent_Table *);

int findByKey(Parent_Table *, int), findByKeyInChild(Child_Table *, int);

int (*table_functions[])(Child_Table *, Parent_Table *) ={nullptr, add_parent_element, add_child_element,
                                                          delete_element, show_parent_table, show_child_table};

int main() {
    auto *pTable = new Parent_Table;
    auto *cTable = new Child_Table;
    FILE *pi_file=nullptr, *pt_file=nullptr;
//    FILE *ci_file=nullptr, *ct_file=nullptr;

    char pt_filename[] = R"(C:\Users\Mi\Downloads)" , pt_info_filename[50];          /* Имя файла и вспомогательная переменная */

    int rc=0;

    while(rc==0){
        printf("1. Create a new files\n2. Open an existed files");
        getNaturalInt(&rc);
        if(rc==1){
            rc=createTables(pTable, cTable);
        }
        if(rc==2){
            rc==loadTables(pTable, cTable);
        }
    }


    printf("\nEnter file name --> ");
    scanf("%s", pt_info_filename);                                                                                  //Ввод имени файла
    strcat(pt_filename, pt_info_filename);
    pt_file = fopen(pt_filename, "rb+");                                                                                  //Попытка открытия чтения-записи для файла


    while ((rc = dialog(msgs, NMsgs)))
        if (!table_functions[rc](cTable, pTable))
            break;
    printf("That's all. Bye!\n");
}


int load_parent_table(Child_Table *, Parent_Table *parentTable){
    FILE *parent_info_file=nullptr, *parent_table_file=nullptr;
    puts("Input table file name and info file name, please!");
    char* table_path = getStr();
    char* info_path = getStr();

    if ((table_path == nullptr) || (info_path == nullptr)) {
        printf("Wrong filename or EOF. ");
        printf("Exit");
        return 0;
    }

    //Открытие файла
    fopen_s(&parent_info_file, info_path, "r+b"); // открываем
    if (!parent_info_file) // если не существует, то создаём
        fopen_s(&parent_info_file, info_path, "w+b");

    fopen_s(&parent_table_file, table_path, "r+b");
    if (!parent_table_file)
        fopen_s(&parent_table_file, table_path, "w+b");


    //Работа с файлом
    if (parent_info_file && parent_table_file) {
        Parent_Table table = {};
//        table = read_table(parent_table_file);
//        putTable(&table, &parent_info_file);
        int rc;
        while ((rc = dialog(msgs, NMsgs)))
//            if (!table_functions[rc](&table, &parent_info_file))
//                break;
//        write_table(&table, parent_table_file);

        fclose(parent_table_file);
        fclose(parent_info_file);

        printf("Exit");
//        delTable(&table);
    }
    else
        printf("Something gone wrong, check access permissions.\n");
}

int write_table(Parent_Table* ptab, FILE* bfp) {
    auto *curr = new Parent_Item[ptab->n];
    curr=*ptab->pItem;

    fseek(bfp, 0, SEEK_SET);
    for (; curr; curr = curr->next) {
        fwrite(&curr->key, sizeof(int), 1, bfp);
        fwrite(&curr->offset, sizeof(long), 1, bfp);
        fwrite(&curr->size, sizeof(int), 1, bfp);
    }
    return 0;
}

int load_child_table(Child_Table *, Parent_Table *parentTable){

}

int add_parent_element(Child_Table *, Parent_Table *table) {
    int key;
    printf(" Input key: ");
    getNaturalInt(&key);
    cin.sync(); // очистка буфера

    char *info = nullptr;
    printf("Input information: ");
    info = getStr();

    if (!findByKey(table, key)) {
        int position = hashFunction(key, 0);

        Parent_Item *tmpItem;
        auto *item = new Parent_Item();

        item->key = key;
        item->info = info;

        if (table->pItem[position] == nullptr) {
            item->next = nullptr;
            table->pItem[position] = item;
        } else {
            tmpItem = table->pItem[position];
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
    } else {
        printf("\n Item with this key = [%d] was founded \n Item not added\n\n", key);
    }
    return 1;
}

int add_child_element(Child_Table *table, Parent_Table *ptable) {
    int key, pkey;
    printf("Input key: \n");
    getNaturalInt(&key);
    cin.sync(); // очистка буфера

    printf("Input parent key: \n");
    getNaturalInt(&pkey);
    cin.sync();

    char *info = nullptr;
    printf("Input information: ");
    info = getStr();

    if (!findByKeyInChild(table, key) && findByKey(ptable, pkey)) {
        int position = hashFunction(key, 1);

        Child_Item *tmpItem;
        auto *item = new Child_Item();

        item->key = key;
        item->pkey = pkey;
        item->info = info;

        if (table->cItem[position] == nullptr) {
            item->next = nullptr;
            table->cItem[position] = item;
        } else {
            tmpItem = table->cItem[position];
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
        cout << endl << "Unique item successfully added!\n\n";
    } else {
        printf("\n Item not added\n\n", pkey);
    }
    return 1;
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

    for (int i = 0; i < table->n; i++) {
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

int delete_element(Child_Table *childTable, Parent_Table *parentTable) {
    int pkey, position;
    printf("Input key for delete element in Parent Table: \n");
    getNaturalInt(&pkey);
    cin.sync(); // очистка буфера

    Parent_Item *tmpItem;
    position = hashFunction(pkey, 0);

    if (!findByKeyInChildforPkey(childTable, pkey)) {
        if (parentTable->pItem[position] == nullptr) {
            printf("\nItem in this cell doesn't exist\n\n");
            return 1; // Не найдено
        } else if (parentTable->pItem[position]->key == pkey) { // Если первый элемент подходит
            parentTable->pItem[position] = parentTable->pItem[position]->next;
            printf("\nThe element was successfully removed \n\n");
        } else {
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

int show_parent_table(Child_Table *, Parent_Table *table) {
    Parent_Item *tmpItem;
    int size = table->n;
    cout << endl;

    for (int i = 0; i < size; i++) {
        if ((table->pItem[i]) != nullptr) { // Проверка на пустоту списков элементов
            cout << "[" << i << "]";
            tmpItem = table->pItem[i];

            if (tmpItem->next != nullptr)
                while (tmpItem) {
                    cout << "\n    [key: " << tmpItem->key << " | info: " << tmpItem->info << "] ";
                    tmpItem = tmpItem->next;
                }
            else
                while (tmpItem) {
                    cout << " [key: " << tmpItem->key << " | info: " << tmpItem->info << "] ";
                    tmpItem = tmpItem->next;
                }
            cout << endl;
        } else cout << "[" << i << "] " << "..." << endl;
    }

}

int show_child_table(Child_Table *table, Parent_Table *) {
    Child_Item *tmpItem;
    int size = table->n;
    cout << endl;

    for (int i = 0; i < size; i++) {
        if ((table->cItem[i]) != nullptr) { // Проверка на пустоту списков элементов
            cout << "[" << i << "]";
            tmpItem = table->cItem[i];

            if (tmpItem->next != nullptr)
                while (tmpItem) {
                    cout << "\n    [key: " << tmpItem->key << " | info: " << tmpItem->info << "] ";
                    tmpItem = tmpItem->next;
                }
            else
                while (tmpItem) {
                    cout << " [key: " << tmpItem->key << " | pkey: " << tmpItem->pkey << " | info: " << tmpItem->info
                         << "] ";
                    tmpItem = tmpItem->next;
                }
            cout << endl;
        } else cout << "[" << i << "] " << "..." << endl;
    }
}

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

