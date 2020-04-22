#include <cstring>
#include "iostream"
#include "string"

using namespace std;
const int SIZEP = 10;
const int SIZEC = 4;

// Перемешивание сцеплением.

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
    FILE* ft{};
    char* fname{};
    int n = SIZEP;
};

struct Child_Table {
    Child_Item *cItem[SIZEC]{};
    FILE* file{};
    char* fname{};
    int n = SIZEC;
};
Parent_Item* getLast(Parent_Item *head) {
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
    auto *tmp = (Parent_Item*) malloc(sizeof(Parent_Item));
    tmp->key = parentItem->key;
    tmp->size = parentItem->size;
    tmp->offset = parentItem->offset;
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
char* findInfo(Parent_Table *pTable, int key);
Parent_Item* findItemByKey(Parent_Table *pTable, int key);

int findByKey(Parent_Table *, int), findByKeyInChild(Child_Table *, int);

int add_parent_element(Child_Table *, Parent_Table *), add_child_element(Child_Table *, Parent_Table *ptable),
        delete_element(Child_Table *, Parent_Table *), show_parent_table(Child_Table *, Parent_Table *),
        show_child_table(Child_Table *, Parent_Table *), insertInParentTable(Child_Table *cTable, Parent_Table *pTable);
int load(Parent_Table *pTable, char *fname);

int save(Child_Table *, Parent_Table *pTable);

int (*table_functions[])(Child_Table *, Parent_Table *) ={nullptr, insertInParentTable, add_child_element,delete_element, show_parent_table, show_child_table, save};


int main() {
    int rc=0;
    auto *pTable = new Parent_Table;
    auto *cTable = new Child_Table;

    char * filename;
    printf("Input table name: \n");
    filename=getStr();

    load(pTable, filename);

    while ((rc = dialog(msgs, NMsgs))) {
        if (!table_functions[rc](cTable, pTable))
            break;
    }
    printf("That's all. Bye!\n");

    fclose(pTable->ft);
    free(pTable);
}

int load(Parent_Table *pTable, char *fname){
    FILE *fd = nullptr;
    int len = strlen(fname);  // длина основной части имени файла
    pTable->fname=fname;

    char* fdname=(char*)malloc((len + 4) * sizeof(char)); // Файл информации таблицы (сохраняется в insert)
    strcpy(fdname, fname);
    strcat(fdname, ".dat");

    char* ftname=(char*)malloc(sizeof(char)*(len+4));  // Файл структуры таблицы, отступы, размеры (сохраняется в save)
    strcpy(ftname, fname);
    strcat(ftname, ".tab");

    // открываем существующий файл таблицы
    fopen_s(&fd, fdname, "rb");

    // Если он существует, то работаем с ним
    if(fd != nullptr){ // считываем данные
        printf("Files have already created: %s & %s\n", fdname, ftname);
        // открываем файл с отступами на чтение и запись

        fopen_s(&(pTable->ft), ftname, "r+b"); // бинарный

            if (pTable->ft == nullptr){
                free(ftname); // если нет, то ошибка
                cout<<"\nError: cannot find .tab file \n";
                return 0;
            }

        auto *item = (Parent_Item*)malloc(sizeof(Parent_Item));
        int position;

        // вставляем элемент из файла в таблицу
        while (fread(&item->size, sizeof(int), 1, pTable->ft) &&
                fread(&item->offset, sizeof(int), 1, pTable->ft) &&
                fread(&item->key, sizeof(int), 1, pTable->ft)){

            printf("%d %d %d\n", item->size, item->offset, item->key);

            position = hashFunction(item->key, 0);

            cout<<"\nPos: "<<position<<"\n";

            if(pTable->pItem[position]==nullptr){ // Если первая ячейка пуста
                item->next = nullptr;

                pTable->pItem[position]=new Parent_Item;

                pTable->pItem[position]->size=item->size;
                pTable->pItem[position]->offset=item->offset;
                pTable->pItem[position]->key=item->key;
                pTable->pItem[position]->next=nullptr;

                cout<<"\nOK\n";
            } else{ // Если нет
                    pushBack(pTable->pItem[position], item);
            }
            cout << endl << "Element from your file was added!\n" << endl;
        }
        free(item);

        fclose(pTable->ft);
        fclose(fd); // закрываем файл
    }else{ // иначе создаем файл данных
        fopen_s(&(pTable->ft), ftname, "w+b");
        if(pTable->ft!= nullptr) {
            printf("New files: %s & %s was created\n\n", fdname, ftname);
            fclose(pTable->ft); // Закрываем, иначе в insert не сработает
            fclose(fd);
            return 1;
        }
    }
    free(ftname);
    return 1;
}

// Заносим в таблицу ft данные о смещениях и в таблицу fd саму информацию
int insertInParentTable(Child_Table *cTable, Parent_Table *pTable){
    int key;
    printf(" Input key: ");
    getNaturalInt(&key);
    cin.sync(); // очистка буфера
    char *str = nullptr;
    printf("Input information: ");
    str = getStr();
    if(findByKey(pTable, key)) {
        printf("\n Item with this key = [%d] was founded \n Item not added\n\n", key);
        return 1; // Вставить нельзя, ошибка
    }

    int position=hashFunction(key, 0);

    char* ftname=(char*)malloc(sizeof(char)*(strlen(pTable->fname)+4));
    strcpy(ftname, pTable->fname);
    strcat(ftname, ".tab");

    // Открыл файл для записи отступов
    fopen_s(&(pTable->ft), ftname, "w+b");

    if(pTable->ft == nullptr){
        printf("\nCan't open\n");
        return 0;
    }

    char* fdname=(char*)malloc((strlen(pTable->fname) + 4) * sizeof(char));
    strcpy(fdname, pTable->fname);
    strcat(fdname, ".dat");

    FILE* fd; // Запись в файл со словами
    fopen_s(&fd, fdname, "a+t");

    auto *item=(Parent_Item*)malloc(sizeof(Parent_Item)); // создание вставляемого элемента
    item->key=key;
    item->size=(int)strlen(str) + 1;
    fseek(fd, 0, SEEK_END);
    item->offset = ftell(fd);
    printf("Offset is: %d", item->offset);

    Parent_Item* tmpItem= nullptr;
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

int save(Child_Table *cTable, Parent_Table *pTable){
    char* ftname=(char*)malloc((strlen(pTable->fname) + 4) * sizeof(char));
    strcpy(ftname, pTable->fname);
    strcat(ftname, ".tab");
    printf("\nSaving to %s\n", ftname);

    fopen_s(&(pTable->ft), ftname, "w+b");

    if(pTable->ft == nullptr){
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
//    fseek(pTable->ft, 0, SEEK_END);

    fclose(pTable->ft);
    printf("\nFile \"%s\" was saved\n", ftname);

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
                printf("\n    [key: %d | size of info: %d | info: %s ]", tmpItem->key, tmpItem->size-1, findInfo(table, tmpItem->key));
//                    cout << "\n    [key: " << tmpItem->key << " | info: " << findInfo(table, tmpItem->key) << "] ";
                tmpItem = tmpItem->next;
            }
            cout << endl;
        } else cout << "[" << i << "] " << "..." << endl;
    }
    return 1;
}

int show_child_table(Child_Table *table, Parent_Table *){}
int add_child_element(Child_Table *table, Parent_Table *ptable){}
int delete_element(Child_Table *childTable, Parent_Table *parentTable){}
int findByKeyInChild(Child_Table *table, int key){}
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
int findByKeyInChildforPkey(Child_Table *table, int pkey){}

// Возвращает элемент по ключу
Parent_Item* findItemByKey(Parent_Table *pTable, int key){
    int index=hashFunction(key, 0);
    Parent_Item* cur;
//    for(; i < pTable.n; ++i)
    for (cur=pTable->pItem[index]; cur; cur=cur->next) {
        if(cur->key == key)
            return cur;
    }
    return nullptr;
}

//  Возвращает инфу по ключу
char* findInfo(Parent_Table *pTable, int key){
    char *info = nullptr;
    Parent_Item  *tmpItem=findItemByKey(pTable, key);
//    printf("\nSize: %d | Offset: %d | Key: %d: \n", tmpItem->size, tmpItem->offset, tmpItem->key);
    FILE *fd=nullptr;
    char* fdname=(char*)malloc((strlen(pTable->fname) + 4) * sizeof(char)); // Файл информации таблицы (сохраняется в insert)
    strcpy(fdname, pTable->fname);
    strcat(fdname, ".dat");
    fopen_s(&fd, fdname, "rb");

        info = (char *)malloc(tmpItem->size);
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