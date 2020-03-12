#include <cstring>
#include "iostream"
#include "string"

using namespace std;
const int SIZEP=10;
const int SIZEC=4;

// Перемешивание сцеплением.

struct Item{
    int key;
    int release;
    char *info;
};

struct Parent_Item : Item{
    Parent_Item *next;
};

struct Child_Item : Item{
    int pkey; // Ключ элемента родительской таблицы
    Child_Item *next;
};

struct Table {
    int n;
};

struct Child_Table : Table {
    public:
        Child_Item *cItem[SIZEC]{};
        int n=SIZEC;
};

struct Parent_Table : Table {
    public:
        Parent_Item *pItem[SIZEP]{};
        int n=SIZEP;
};

int getNaturalInt(int *a);
int hashFunction(int key, int table_type);

char *getStr();

const char *msgs[] = {"0. Quit", "1. Add", "2. Find", "3. Delete", "4. Show"};

const int NMsgs = sizeof(msgs) / sizeof(msgs[0]);

int dialog(const char *msgs[], int N);

void add_parent_element(Parent_Table *), add_child_element(Child_Table *, Parent_Table *ptable), delete_element(Parent_Table *), show_parent_table(Parent_Table *);

int findByKey(Parent_Table *, int);
int findByKeyInChild(Child_Table *, int);
//int (*ftpr[])(Parent_Table *)={nullptr, add_parent_element, find_element, delete_element, show_parent_table};

int main() {

    auto *parent_table=new Parent_Table;
    auto *child_table=new Child_Table;

    add_parent_element(parent_table);

    add_child_element(child_table, parent_table);

    show_parent_table(parent_table);

//    int rc;
//    while((rc=dialog(msgs, NMsgs)))
//        if(!ftpr[rc](parent_table))
//            break;
//    printf("That's all. Bye!\n");

}

void add_parent_element(Parent_Table *table) {
    int key;
    printf("Input key: ");
    getNaturalInt(&key);
//    printf("%d", key);
    cin.sync(); // очистка буфера

    char *info = nullptr;
    printf("Input information: ");
    info=getStr();
//    printf("\n");
//    printf("%s", info);

    if(!findByKey(table, key)) {
        int position = hashFunction(key, 0);
        int release; // Индикатор заполненности

        Parent_Item *tmpItem;
        auto *item = new Parent_Item();

        item->key = key;
        item->info = info;

        if (table->pItem[position] == nullptr) {
            item->release = 1;
            item->next = nullptr;
            table->pItem[position] = item;
        } else {
            tmpItem = table->pItem[position];
            release = 0;
            while (tmpItem) {
                if (tmpItem->key == key) {
                    release = tmpItem->release;
                    if (tmpItem->next != nullptr && tmpItem->next->key != key) break;
                }
                if (tmpItem->next == nullptr) break;
                tmpItem = tmpItem->next;
            }
            item->release = ++release;
            item->next = tmpItem->next;
            tmpItem->next = item;
        }
    }
}

void add_child_element(Child_Table *table, Parent_Table *ptable) {
    int key, pkey;
    printf("Input key: ");
    getNaturalInt(&key);
    cin.sync(); // очистка буфера

    printf("Input parent key: ");
    getNaturalInt(&pkey);
    cin.sync();

    char *info = nullptr;
    printf("Input information: ");
    info=getStr();

    if(!findByKeyInChild(table, key) && findByKey(ptable, pkey)) {
        int position = hashFunction(key, 0);
        int release; // Индикатор заполненности

        Child_Item *tmpItem;
        auto *item = new Child_Item();

        item->key = key;
        item->pkey = pkey;
        item->info = info;

        if (table->cItem[position] == nullptr) {
            item->release = 1;
            item->next = nullptr;
            table->cItem[position] = item;
        } else {
            tmpItem = table->cItem[position];
            release = 0;
            while (tmpItem) {
                if (tmpItem->key == key) {
                    release = tmpItem->release;
                    if (tmpItem->next != nullptr && tmpItem->next->key != key) break;
                }
                if (tmpItem->next == nullptr) break;
                tmpItem = tmpItem->next;
            }
            item->release = ++release;
            item->next = tmpItem->next;
            tmpItem->next = item;
        }
    }
}

void show_parent_table(Parent_Table *table) {
        Parent_Item *tmpItem;
        int size=table->n;
        cout << endl;

        for (int i = 0; i < size; i++) {
            if ((table->pItem[i]) != nullptr) { // Проверка на пустоту списков элементов
                cout << "[" << i << "]";
                tmpItem = table->pItem[i];

                if(tmpItem->next!=nullptr)
                    while (tmpItem) {
                        cout << "\n    [key: " << tmpItem->key << " | info: " << tmpItem->info << "] ";
                        tmpItem = tmpItem->next;
                    }
                else
                    while (tmpItem) {
                        cout << " [key: " << tmpItem->key << " | info: " << tmpItem->info << "] ";
                        tmpItem = tmpItem->next;
                    }
                cout<<endl;
            } else cout << "[" << i << "] " << "..." << endl;
        }

}

int findByKeyInChild(Child_Table *table, int key) {
    int position = hashFunction(key, 0);
    Child_Item *tmpItem;

    if (table->cItem[position] == nullptr) {
        cout << endl << "Unique item successfully added!\n";
        return 0; // Не найдено
    } else {
        tmpItem = table->cItem[position];
        while (tmpItem) {
            if (tmpItem->key == key) {
//                cout << endl << "[k: " << tmpItem->key << " r: " << tmpItem->release << " i: " << tmpItem->info << "] ";
                printf("Item with this key = [%d] was founded\n", key);
                printf("Item not added\n");
                return 1; // Найдено
            }
            tmpItem = tmpItem->next;
        }
    }
    return 0; // Не найдено
}

int findByKey(Parent_Table *table, int key) {
    int position = hashFunction(key, 0);
    Parent_Item *tmpItem;

    if (table->pItem[position] == nullptr) {
        cout << endl << "Unique item successfully added!\n";
        return 0; // Не найдено
    } else {
        tmpItem = table->pItem[position];
        while (tmpItem) {
            if (tmpItem->key == key) {
//                cout << endl << "[k: " << tmpItem->key << " r: " << tmpItem->release << " i: " << tmpItem->info << "] ";
                printf("Item with this key = [%d] was founded\n", key);
                printf("Item not added\n");
                return 1; // Найдено
            }
            tmpItem = tmpItem->next;
        }
    }
    return 0; // Не найдено
}

int dialog(const char *msgs[], int N) {
    string errmsg = " ";
    int rc;
    int n;

    do {
        cout<<errmsg;
        errmsg = "You are wrong. Repeat, please";

        for (int j = 0; j < N; ++j)
            puts(msgs[j]);
        puts("Make your choice: --> ");

        n=getNaturalInt(&rc);
        if(n==0)
            rc=0;
    }while (rc<0 || rc>=N);
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
            scanf("%*[^\n]", 0);
        }
    } while (n == 0 || *a < 0);
    return 1;
}

char *getStr() {
    char *ptr = (char *)malloc(1);
    char buf[81];
    int n, len = 0;
    *ptr = '\0';
    do {
        n = scanf("%80[^\n]", buf);
        if (n < 0) {
            free(ptr);
            return nullptr;
        }
        else
        if (n) {
            len += strlen(buf);
            ptr = (char *)realloc(ptr, len + 1);
            int k = strcspn(buf, "\0"); // длина слова
            strncat(ptr, buf, k);
        }
    } while (n > 0);
    scanf("%*c");
    return ptr;
}

int hashFunction(int key, int table_type) {
    if(table_type==0)
        return key % SIZEP;
    else
        return key % SIZEC;
}

