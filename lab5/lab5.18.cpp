#include <cstring>
#include "iostream"
#include "string"

using namespace std;

typedef struct Node{
    int color;
    int key;

    struct Node *left, *right;
    struct Node *parent;
} Node;

const char* msgs[] = { "0. Quit", "1. Add", "2. Find", "3. Find max key", "4. Delete",
                       "5. Show", "6. Timing", "7. Load from file", "8. Save to file"};

const int NMsgs = sizeof(msgs) / sizeof(msgs[0]);

int getNaturalInt(int *a);
char *getStr();
int dialog(const char *msgs[], int N);

int addElement(Node *);
int deleteElement(Node *);
int findElement(Node *);
int findElementWithMaxKey(Node *);
int showTable(Node *);

int timing(Node* );

int loadFromFile(Node *);
int saveToFile(Node *);

int (*table_functions[])(Node*)={nullptr, addElement, findElement, findElementWithMaxKey,
                                 deleteElement, showTable, timing, loadFromFile, saveToFile};

int main(){
    Node* rbTable= nullptr;
    int rc;
    while((rc=dialog(msgs, NMsgs)))
        if(!table_functions[rc](rbTable))
            break;

    printf("That's all. Bye!\n");
}



int addElement(Node *){};
int deleteElement(Node *){};
int findElement(Node *){};
int findElementWithMaxKey(Node *){};
int showTable(Node *){};

int timing(Node* ){};

int loadFromFile(Node *){};
int saveToFile(Node *){};



int dialog(const char *msgs[], int N) {
    string errmsg = "";
    int rc, n;

    do {
        cout<<errmsg;
        errmsg = "You are wrong. Repeat, please\n";
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
            scanf("%*[^\n]");
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
