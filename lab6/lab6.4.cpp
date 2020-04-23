#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <iostream>
#include <ctime>
#include <math.h>

using namespace std;

enum state {
    INTERSECTION, EXIT, DEADLOCK
};

// distance from source to vertex
typedef struct adjs{
    int vertex;
    double distance;
} adjs;

typedef struct vertex {
    adjs* adj_list;
    int number;
    int x, y;
    state state;
} vertex;


int dialog(const char *msgs[], int N);

int getNaturalInt(int *a);

int getInt(int *a);

char *getStr();

int add_vertex();

int add_corridor();

int delete_vertex();

int find_way();

int show_labyrinth();

int d_add_vertex();

int d_add_corridor();

int loadFromFile();

double getDistance(vertex v1, vertex v2){
    return sqrt(pow((v1.x-v2.x), 2)+pow((v1.y-v2.y), 2));
}

const char *msgs[] = {"0. Quit", "1. Add vertex", "2. Add corridor", "3. Delete corridor between 2 vertex",
                      "4. Find way",
                      "5. Show labyrinth"};

const int NMsgs = sizeof(msgs) / sizeof(msgs[0]);

int (*graph_functions[])() ={nullptr, d_add_vertex, d_add_corridor, delete_vertex, find_way, show_labyrinth};

int main() {

    loadFromFile();

    int rc;
    while ((rc = dialog(msgs, NMsgs)))
        if (!graph_functions[rc]())
            break;
    printf("That's all. Bye!\n");
}

int loadFromFile() {
    vertex* adjacent_list;
    adjacent_list->number = 4;
    printf("Input file name:\n");
    char *fname = getStr();
    strcat(fname, ".txt");
    cout << "\n" << fname;

    adjacent_list->number = 5;
    FILE *f = NULL;
    errno_t err = fopen_s(&f, fname, "r");

    if (err) {
        printf("\nFile does not exist \n");
    }
    rewind(f);

    char *info = (char *) calloc(100, sizeof(char));
    int k;

    while (fscanf(f, "%d%s", &k, info) != EOF) {
        vertex *newNode = (vertex *) malloc(sizeof(vertex));

        char *str = (char *) calloc(100, sizeof(char));

    }
    free(info);
    fclose(f);

}

vertex find_vertex(int num, vertex* graph){
}

int add_vertex(vertex* graph) {

}

int add_corridor() {

}

int delete_vertex() {

}

int find_way() {

}

int show_labyrinth() {

}

int d_add_vertex() {
    printf("\nAdding new vertex\n");

}

int d_add_corridor() {

}


int dialog(const char *msgs[], int N) {
    char *errmsg = "";
    int rc, n;
    printf("\n");
    do {
        printf("%s", errmsg);
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

int getInt(int *a) {
    int n;
    do {
        n = scanf("%d", a);
        if (n < 0)
            return 0;
        if (n == 0) {
            printf("%s\n", "Error! Repeat input");
            scanf("%*[^\n]", 0);
        }
    } while (n == 0);
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