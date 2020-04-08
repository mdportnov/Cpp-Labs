#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <malloc.h>
#include <cstring>
const int p = 17;
const int m = 10;
const char* MSGS[] = { "1 - Add element to tabl.","2 - Delete element from tabl.","3 - Table to screen.","4 - Find element with key from one of Key Spaces and print the element.","5 - File Compression.""\n\n0 - Exit." };


typedef struct ElPk {
    int key,len,ofset;
    short int state;//1 - если удалён, 0 - если нет.
}ElPk;

typedef struct Table {
    ElPk* PK[2];
    FILE* fd;
    char* fname;
}Table;

struct info {
    char* str;
    int ind1, ind2;
};

typedef struct Item {
    int key1, key2;
    info info;
} Item;

int Compression(Table*);
int random(int, int);
int h1(int);
int h(int, int);
int search(int , ElPk* );
void Creator(ElPk**);
int GetInt(int*);
int InsertPK(Table*, Item*);
int DeletePK(int , Table* , int ) ;
int AddElement(Table* );
int ExpelElement(Table *Ptab);
int PrintTable(Table *Ptab);
void Clear(ElPk**);
char* GetStr();
int FindElement(Table* );
void PrintElement(Table*, int , int );
int dialog();
int load(Table*);
int New(Table*);
void Upload(Table*);


int a = random(1, 16);
int b = random(0, 16);

int main()
{
    int rc = 0;
    auto* pTable = (Table*)malloc(sizeof(Table));
    Creator(pTable->PK);
    while (rc == 0) {
        printf("Press 1 to create a new file, 2 to open file");
        GetInt(&rc);
        if (rc == 1)
            rc = New(pTable);
        if (rc == 2)
            rc = load(pTable);
    }
    int (*fptr[])(Table*) = {NULL, AddElement, ExpelElement, PrintTable, FindElement, Compression };
    int  ppp = 1;
    while ((rc = dialog()) && ppp)
        ppp = fptr[rc](pTable);
    Upload(pTable);

    fclose(pTable->fd);

    Clear(pTable->PK);

    free(pTable->fname);
    free(pTable);
}
void Creator(ElPk** PK) {
    PK[0] = (ElPk*)calloc(m, sizeof(ElPk));
    PK[1] = (ElPk*)calloc(m, sizeof(ElPk));
}

int dialog() {
    int n, rc;
    printf("********** MENU **********\n\n");
    for (int i = 0;i <= 5;i++)
        printf("%s\n", MSGS[i]);
    printf("**************************\n\n");
    printf(">>>Please,input number of function,which you want to use: ");
    while (n = GetInt(&rc), (rc < 0) || (rc > 5))
        printf(">>>This variant does not exist, please, try agin: ");
    if (n == 0) {
        printf("Good bye!");
        return 0;
    }
    else return rc;
}

int load(Table* Ptab) {
    char* name;
    int n;
    printf("please, input a FileName");
    name = GetStr();
    fopen_s(&(Ptab->fd), name, "r+b");
    if (Ptab->fd == NULL) {
        printf("this file does not exist.\n");
        return 0;
    }

    fread(&a, sizeof(int), 1, Ptab->fd);
    fread(&b, sizeof(int), 1, Ptab->fd);
    fread(&n, sizeof(int), 1, Ptab->fd);
    fread(Ptab->PK[0], sizeof(ElPk), m, Ptab->fd);
    fread(Ptab->PK[1], sizeof(ElPk), m, Ptab->fd);
    Ptab->fname = name;
    return 1;

}

int New(Table* Ptab) {
    printf("please, input a FileName");
    char* name = GetStr();
    if ((fopen_s(&(Ptab->fd), name, "w+b")) != 0) {
        printf("error");
        return 0;
    }
    ElPk* it = (ElPk*)calloc(2 * m, sizeof(ElPk));
    fwrite(&a, sizeof(int), 1, Ptab->fd);
    fwrite(&b, sizeof(int), 1, Ptab->fd);
    fwrite(&m, sizeof(int), 1, Ptab->fd);
    fwrite(it, sizeof(ElPk), 2 * m, Ptab->fd);
    Ptab->fname = name;
    return 1;
}

int h1(int k) {
    // return((a * k + b) % p) % m;//универслаьное хэширование
    return k % m;
}
int h(int k, int i) {
    return (h1(k) + i) % m;
}
int random(int a, int b) {
    return (time(nullptr)) % (b - a + 1) + a;
}
int GetInt(int* a)
{
    int n;
    do {
        n = scanf("%d", a);
        if (n == 0) {
            printf("error\n");
            scanf("%*[^\n]");
        }
    } while (n == 0);
    return n < 0 ? 0 : 1;
}
char* GetStr() {
    char buf[30] = { "" };
    int n;
    char* a = (char*)calloc(1, sizeof(char));
    printf(">>>Input text (string) information:  ");
    scanf("%*[\n]");
    while (n = scanf("%29[^\n]", buf), n > 0) {
        int c = strlen(buf) + strlen(a);
        a = (char*)realloc(a, c+1);
        //a[c - strlen(buf)] = '\0';
        strcat(a, buf);
    }
    if (n < 0) {
        free(a);
        exit(1);
    }
    return a;
}

int search(int k, ElPk* T) {
    int i = 0, j;
    do {
        j = h(k, i);
        if ((T[j].key == k) && (T[j].state == 0))
            return j;
        else i++;
    } while ((i != m) && (T[j].key != 0));
    return -1;
}
int InsertPK(Table* Ptab, Item*X) {
    int i = 0;
    do {
        int j = h(X->key1, i);
        if ((Ptab->PK[0][j].key == 0) || (Ptab->PK[0][j].state)) {
            Ptab->PK[0][j].key = X->key1;
            Ptab->PK[0][j].len = strlen(X->info.str)+1;
            X->info.ind1 = j;
            fseek(Ptab->fd, 0, SEEK_END);
            Ptab->PK[0][j].ofset = ftell(Ptab->fd);
            Ptab->PK[0][j].state = 0;
            break;
        }
        else i++;
    } while (i != m);
    if (i == m) {
        printf(">>>The tabl if full.Can't add the element.\n\n");
        return -1;
    }
    i = 0;
    do {
        int j = h(X->key2, i);
        if ((Ptab->PK[1][j].key == 0) || (Ptab->PK[1][j].state)) {
            Ptab->PK[1][j].key = X->key2;
            Ptab->PK[1][j].len = strlen(X->info.str)+1;
            X->info.ind2 = j;
            Ptab->PK[1][j].ofset=ftell(Ptab->fd);
            fwrite(X->info.str, 1, Ptab->PK[1][j].len, Ptab->fd);
            fwrite(&(X->info.ind1), sizeof(int), 1, Ptab->fd);
            fwrite(&(X->info.ind2), sizeof(int), 1, Ptab->fd);
            int n = ftell(Ptab->fd);
            Ptab->PK[1][j].state = 0;
            return 1;
        }
        else i++;
    } while (i != m);
    printf(">>>The table if full.Can't add the element.\n\n");
    return -1;
}
int DeletePK(int a, Table* Ptab, int k) {
    int i = 0, j;
    do {
        j = h(k, i);
        if ((Ptab->PK[a - 1][j].key == k) && (Ptab->PK[a - 1][j].state == 0)) {
            Ptab->PK[a - 1][j].state = 1;
            info* it = (info*)malloc(sizeof(info));
            it->str = (char*)malloc(Ptab->PK[a - 1][j].len);
            fseek(Ptab->fd, Ptab->PK[a - 1][j].ofset, SEEK_SET);
            fread(it->str, 1, Ptab->PK[a - 1][j].len, Ptab->fd);
            fread(&(it->ind1), sizeof(int), 1, Ptab->fd);
            fread(&(it->ind2), sizeof(int), 1, Ptab->fd);

            Ptab->PK[a &1][a==2?it->ind1:it->ind2].state=1;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            free(it->str);
            free(it);
            printf(">>>The element has successfully deleted.\n\n");
            return 1;
        }
        else i++;
    } while (i != m);
    printf(">>>That element does not exist or key hasn't link to information.\n\n");
    return -1;
}
int AddElement(Table* Ptab) {
    Item* X = (Item*)malloc(sizeof(Item));
    printf(">>>Input key in first key space: ");
    if (GetInt(&(X->key1)) == 0) {
        printf(">>>Back to main menu...\n\n");
        return 0;
    }
    printf(">>>Input key in second key space: ");
    if (GetInt(&(X->key2)) == 0) {
        printf(">>>Back to main menu...\n\n");
        return 0;
    }
    //X->info = GetStr();ФУНКЦИЯ ВВОДА СТРОКИ = ИНФО!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    if ((search(X->key1, Ptab->PK[0]) == -1) && (search(X->key2, Ptab->PK[1]) == -1)) {
        X->info.str = GetStr();
        if (InsertPK(Ptab,X) == -1) {
            free(X->info.str);
            free(X);
            return 1;
        }

        printf(">>>The element has successfully added.\n\n");
    }
    else {
        printf("One of this keys is duplicated in its space . Cancel...\n");
        free(X);

    }
    return 1;
}
int ExpelElement(Table* Ptab) {
    int a;
    printf(">>>Input number of key space: ");
    while (GetInt(&a), a < 1 || a>2)
        printf("Please, input 1 or 2: ");
    //из какого пространства ключей ключ
    int key;
    printf(">>>Input a key: ");
    GetInt(&key);//сам ключ

    a = DeletePK(a, Ptab, key);
    return 1;
}
int FindElement(Table* Ptab) {
    int a;
    printf(">>>Input number of key space: ");
    while (GetInt(&a), a < 1 || a>2)
        printf("Please, input 1 or 2: ");//из какого пространства ключей ключ
    int key;
    printf(">>>Input a key: ");
    GetInt(&key);//сам ключ
    int j = search(key, Ptab->PK[a - 1]);
    if (j != -1) {
        printf("%d\n", j);
        PrintElement(Ptab,j,a);
        return 1;
    }
    printf(">>>Element has not been found.\n\n");
    return -1;
}
void PrintElement(Table* Ptab, int j, int a) {
    printf("Element: \n");
    fseek(Ptab->fd, Ptab->PK[a-1][j].ofset, SEEK_SET);
    info* it = (info*)malloc(sizeof(info));
    it->str = (char*)malloc(Ptab->PK[a - 1][j].len);
    fread(it->str, 1, Ptab->PK[a - 1][j].len, Ptab->fd);
    fread(&(it->ind1), sizeof(int), 1, Ptab->fd);
    fread(&(it->ind2), sizeof(int), 1, Ptab->fd);

    printf_s("key1: %d\n", Ptab->PK[0][it->ind1].key);
    printf_s("key2: %d\n", Ptab->PK[1][it->ind2].key);
    printf_s("info: %s\n", it->str);
    printf_s("ind1: %d\n", it->ind1);
    printf_s("ind2: %d\n\n", it->ind2);
    free(it->str);
    free(it);
}
int PrintTable(Table* Ptab) {
    int l = 0, b = 0;
    printf_s("=====================\n\n");
    for (int i = 0;b < m;i = (i + 1) % m, b++) {
        if ((Ptab->PK[1][i].state == 0) && (Ptab->PK[1][i].key != 0)) {
            printf("%d\n",i);
            PrintElement(Ptab,i,2);
            l++;
        }
    }
    if (l == 0)
        printf_s("The table is empty.\n\n");
    printf_s("=====================\n\n");
    return 1;
}
void Clear(ElPk** PK) {
    free(PK[0]);
    free(PK[1]);
}
void Upload(Table* Ptab) {
    fseek(Ptab->fd, sizeof(int) * 3, SEEK_SET);
    fwrite(Ptab->PK[0], sizeof(ElPk), m, Ptab->fd);
    fwrite(Ptab->PK[1], sizeof(ElPk), m, Ptab->fd);
}


int Compression(Table* Ptab) {
    FILE* fd2;
    fclose(Ptab->fd);
    fopen_s(&fd2, R"(C:\Users\Mi\Downloads\t.tab)", "wb");
    fopen_s(&(Ptab->fd),Ptab->fname , "rb");
    if (Ptab->fd == NULL)
        return 0;
    info* it = (info*)malloc(sizeof(info));

    int k = 0;
    fseek(fd2, 0, SEEK_SET);
    fwrite(&a, sizeof(int), 1, fd2);
    fwrite(&b, sizeof(int), 1, fd2);
    fwrite(&m, sizeof(int), 1, fd2);
    fwrite(Ptab->PK[0], sizeof(ElPk), m, fd2);
    fwrite(Ptab->PK[1], sizeof(ElPk), m, fd2);

    for (int i = 0;i < m;i++) {
        if ((Ptab->PK[0][i].state == 0)&&(Ptab->PK[0][i].key!=0)) {
            fseek(Ptab->fd, Ptab->PK[0][i].ofset, SEEK_SET);
            it->str = (char*)calloc(Ptab->PK[0][i].len,1);
            fread(it->str, 1, Ptab->PK[0][i].len, Ptab->fd);
            fread(&(it->ind1), sizeof(int), 1, Ptab->fd);
            fread(&(it->ind2), sizeof(int), 1, Ptab->fd);
            fseek(fd2, 0, SEEK_END);
            Ptab->PK[1][it->ind2].ofset = ftell(fd2);
            Ptab->PK[0][i].ofset = ftell(fd2);
            fwrite(it->str, 1, Ptab->PK[0][i].len, fd2);
            fwrite(&(it->ind1), sizeof(int), 1, fd2);
            fwrite(&(it->ind2), sizeof(int), 1, fd2);
            free(it->str);
            k++;

        }
    }
    if(k!=0)
        free(it);
    fclose(Ptab->fd);
    remove(Ptab->fname);
    fclose(fd2);
    rename(R"(C:\Users\Mi\Downloads\t.tab)", Ptab->fname);
    fopen_s(&(Ptab->fd), Ptab->fname, "r+b");
    Ptab->fd = fd2;

    return 1;
}