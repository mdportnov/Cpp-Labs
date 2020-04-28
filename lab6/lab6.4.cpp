#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <ctime>

using namespace std;

typedef struct vertex {                               //Данные вершины
    int num;                                          //Номер
    int x, y;                                         //Координаты
    char state;                                       //Состояние
} vertex;

// distance from source to vertex
typedef struct adjNode {                              //Смежные вершины
    int destination;                                  //Номер
    float weight;                                     //Вес (расстояние)
    adjNode *next;                                    //Указатель на следующую смежную вершину
} adjNode;

// info about vertex, which this list represents
typedef struct adjList {                              //Вершины
    int N;                                            //Количество смежных вершин
    vertex *vertex;                                   //Указатель на данные вершины
    adjNode *head;                                    //Указатель на 1 смежную вершину
    adjList *next;                                    //Указатель на следующую вершину
} adjList;

typedef struct Graph {                                //Граф
    int N;                                            //Количество вершин
    adjList *adjList;                                 //Указатель на 1 вершину
} Graph;

Graph *graph;

int dialog(const char *msgs[], int N);

int getNaturalInt(int *a);

int getInt(int *a);

void add_vertex(adjList *);

void getLastAdjList(adjList *&ptr);

adjList *getAdjListByNum(int num);

int add_corridor(int, int);

int d_add_vertex();

int d_add_corridor();

int d_delete_vertex();

int find_way();

int show_labyrinth();

void loadFromFile();

void loadToFile();

int add_random_vertexes();

int add_random_corridors();

int change_vertex_state();

int add_corridor_for_random(int v1, int v2);

void renumber(int num);

int timing();

float getDistance(int v1, int v2) {
    adjList *ptr1 = graph->adjList;
    for (int i = 0; i < v1; ++i) {
        ptr1 = ptr1->next;
    }
    adjList *ptr2 = graph->adjList;
    for (int i = 0; i < v2; ++i) {
        ptr2 = ptr2->next;
    }
    return (float) sqrt(
            pow((abs(ptr1->vertex->x - ptr2->vertex->x)), 2) + pow(abs(ptr1->vertex->y - ptr2->vertex->y), 2));
}

const char *msgs[] = {"0. Quit", "1. Add vertex", "2. Add corridor", "3. Delete vertex", "4. Find way",
                      "5. Show labyrinth", "6. Add random vertexes", "7. Add random corridors",
                      "8. Change the state of the vertex", "9. Timing"};

const int NMsgs = sizeof(msgs) / sizeof(msgs[0]);

int (*graph_functions[])() ={nullptr, d_add_vertex, d_add_corridor, d_delete_vertex, find_way, show_labyrinth,
                             add_random_vertexes, add_random_corridors, change_vertex_state, timing};

void makeConnections();

void printVertex(adjList *listAdj) {
    printf("\nVertex #%d { X: %d | Y: %d | Type: %c | N: %d}\n", listAdj->vertex->num,
           listAdj->vertex->x, listAdj->vertex->y, listAdj->vertex->state, listAdj->N
    );
}

void printNode(adjNode *adjNode) {
    printf("\nTo node #%d weight is %f\n", adjNode->destination, adjNode->weight);
}

adjNode *getAdjNodeByNum(adjList *, int);

void spf(int start);

int main() {
    loadFromFile();
    makeConnections();
    int rc;
    while ((rc = dialog(msgs, NMsgs)))
        if (!graph_functions[rc]())
            break;
    loadToFile();
    printf("\nThat's all. Bye!\n");
}

void makeConnections() {
    adjList *tmpList = graph->adjList;
    adjNode *tmpNode;

    for (int i = 0; i < graph->N; ++i) {
        tmpNode = tmpList->head;
        for (int j = 0; j < tmpList->N; ++j) {
            tmpNode->weight = getDistance(tmpList->vertex->num, tmpNode->destination);
            tmpNode = tmpNode->next;
        }
        tmpList = tmpList->next;
    }
}

void loadFromFile() {
    char filename[] =  R"(C:\Users\Mi\CLionProjects\lab6\)", helpfilename[50];

    FILE *f;
    graph = (Graph *) malloc(sizeof(Graph));
    graph->adjList = (adjList *) malloc(sizeof(adjList));

    printf("Hello!\nEnter file name --> ");
    scanf("%s", helpfilename);
    strcat(filename, helpfilename);
    strcat(filename, ".txt");
    f = fopen(filename, "rb+");
    fseek(f, 0, SEEK_END);
    if (ftell(f) == -1) {
        printf("\nFile with the specified name does not exist\n\nFile creation...\n\nFile was created successfully\n");
        fclose(f);
        f = fopen(filename, "w+");
    }
    fseek(f, 0, SEEK_END);
    printf("\nFile Size -> %li\n", ftell(f));
    fseek(f, 0, SEEK_SET);
    printf("_________________________________________\n");

    graph->adjList = (adjList *) malloc(sizeof(adjList));
    graph->adjList->vertex = (vertex *) malloc(sizeof(vertex));
    adjList *ptr = graph->adjList;
    graph->N = 0;
    while (fscanf(f, "%d %d %d %c %d", &ptr->vertex->num, &ptr->vertex->x, &ptr->vertex->y, &ptr->vertex->state,
                  &ptr->N) != EOF) {
        ptr->head = (adjNode *) malloc(sizeof(adjNode));
        adjNode *tmpNode = ptr->head;
        tmpNode->next = nullptr;
        for (int i = 0; i < ptr->N; ++i) {
            fscanf(f, "%d", &tmpNode->destination);
            tmpNode->next = (adjNode *) malloc(sizeof(adjNode));
            tmpNode = tmpNode->next;
            tmpNode->next = nullptr;
        }
        ptr->next = (adjList *) malloc(sizeof(adjList));
        ptr->next->vertex = (vertex *) malloc(sizeof(vertex));
        ptr->next->head = (adjNode *) malloc(sizeof(adjNode));
        ptr = ptr->next;
        ptr->next = nullptr;

        graph->N++;
    }
    free(ptr);
    fclose(f);
    printf("\nSize of your graph: %d\n", graph->N);
    printf("_________________________________________");
}

void loadToFile() {
    FILE *nf;
    char newfilename[] =  R"(C:\Users\Mi\CLionProjects\lab6\)", helpfilename[50], check;
    printf("_________________________________________\n");
    printf("Press enter to save changes\n");
    fflush(stdin);
    scanf("%c", &check);
    if (check != '\n') return;
    printf("Enter file name -->");
    scanf("%s", helpfilename);
    strcat(newfilename, helpfilename);
    strcat(newfilename, ".txt");

    nf = fopen(newfilename, "w+");

    adjList *tmpList = graph->adjList;
    adjNode *tmpNode;

    for (int i = 0; i < graph->N; ++i) {
        fprintf(nf, "%d %d %d %c %d", tmpList->vertex->num, tmpList->vertex->x, tmpList->vertex->y,
                tmpList->vertex->state, tmpList->N);
        tmpNode = tmpList->head;
        for (int j = 0; j < tmpList->N; ++j) {
            fprintf(nf, " %d", tmpNode->destination);
            tmpNode = tmpNode->next;
        }
        fputc('\n', nf);
        tmpList = tmpList->next;
    }
    fclose(nf);
}

int show_labyrinth() {
    printf("_________________________________________\n");
    printf("Labyrinth show:");
    adjList *ptr = graph->adjList;
    for (int j = 0; j < graph->N; ++j) {
        printf("\nVertex #%d (%d, %d) (%c) adjust with: ", ptr->vertex->num, ptr->vertex->x, ptr->vertex->y,
               ptr->vertex->state);
        adjNode *a = ptr->head;
        for (int i = 0; i < ptr->N; ++i) {
            printf("%d (%.2f) ", a->destination, a->weight);
            a = a->next;
        }
        ptr = ptr->next;
    }
    printf("\n_________________________________________");
    return 1;
}

int d_add_vertex() {
    printf("_________________________________________\n");
    printf("Adding new vertex:\n");
    auto *item = new vertex();
    adjList *check = graph->adjList;

    printf("Input X -->");
    getInt(&item->x);
    printf("Input Y -->");
    getInt(&item->y);
    if (abs(item->y) > 99 or abs(item->x) > 99) {
        printf("Coordinates Limit (-99; 99)\n_________________________________________");
        return 1;
    }
    for (int j = 0; j < graph->N; ++j) {
        if (item->x == check->vertex->x and item->y == check->vertex->y) {
            printf("The vertex with these coordinates is already there\n_________________________________________");
            return 1;
        }
        check = check->next;
    }
    graph->N++;
    item->num = graph->N - 1;

    char test;
    printf("If the vertex is not the way out, press enter\nOtherwise, keep any symbol");
    fflush(stdin);
    scanf("%c", &test);
    fflush(stdin);
    if (test != '\n') item->state = 'E';
    else item->state = '_';

    auto *newAdjList = new adjList();
    newAdjList->vertex = item;
    newAdjList->head = (adjNode *) malloc(sizeof(adjNode));
    newAdjList->head = nullptr;
    newAdjList->next = (adjList *) malloc(sizeof(adjList));
    newAdjList->next = nullptr;
    newAdjList->N = 0;

    add_vertex(newAdjList);
    makeConnections();
    printf("_________________________________________");
    return 1;
}

void getLastAdjList(adjList *&ptr) { // Получим последний список смежностей
    ptr = graph->adjList;
    int counter = graph->N - 1;
    while (--counter) {
        ptr = ptr->next;
    }
}

adjList *getAdjListByNum(int num) {
    adjList *ptr = graph->adjList;
    int counter = num;
    while (counter--) {
        ptr = ptr->next;
    }
    // printVertex(ptr);
    return ptr;
}

adjNode *getAdjNodeByNum(adjList *adjList, int num) {
    adjNode *ptr = adjList->head;
    int counter = num;
    while (counter--) {
        ptr = ptr->next;
    }
    // printNode(ptr);
    return ptr;
}

void add_vertex(adjList *node) {
    if (graph->N == 1) {
        graph->adjList = node;
        //  printVertex(graph->adjList);
        return;
    }

    auto *lastAdj = new adjList();

    getLastAdjList(lastAdj);

    lastAdj->next = node;
}

int d_add_corridor() {
    printf("_________________________________________\n");
    printf("Adding new corridor:\n");
    if (graph->N < 2) {
        printf("You need more than 1 vertex\n_________________________________________");
        return 1;
    }
    int v1, v2;
    printf("Input first vertex -->");
    getNaturalInt(&v1);
    printf("Input second vertex -->");
    getNaturalInt(&v2);
    if (v1 > graph->N - 1 or v2 > graph->N - 1) {
        printf("There is no vertex with this number\n_________________________________________");
        return 1;
    }
    if (v1 == v2) {
        printf("The entered vertexes must be different\n_________________________________________");
        return 1;
    }
    if (add_corridor(v1, v2) == 0) return 1;
    add_corridor(v2, v1);
    printf("_________________________________________");
    return 1;
}

int add_corridor(int v1, int v2) {
    adjList *tmpAdj = graph->adjList;
    for (int i = 0; i < v1; ++i) {
        tmpAdj = tmpAdj->next;
    }
    if (tmpAdj->N == 0) {
        tmpAdj->head = (adjNode *) malloc(sizeof(adjNode));
        tmpAdj->head->weight = getDistance(v1, v2);
        tmpAdj->head->destination = v2;
        tmpAdj->N++;
        if (tmpAdj->vertex->state == '_') tmpAdj->vertex->state = 'D';
        return 1;
    } else {
        adjNode *tmpNode1 = tmpAdj->head;
        if (tmpNode1->destination == v2) {
            printf("These vertexes are already connected\n_________________________________________);", v1, v2);
            return 0;
        }
        for (int j = 0; j < tmpAdj->N - 1; ++j) {
            if (tmpNode1->next->destination == v2) {
                printf("These vertexes are already connected\n_________________________________________", v1, v2);
                return 0;
            } else {
                tmpNode1 = tmpNode1->next;
            }
        }
        tmpNode1->next = (adjNode *) malloc(sizeof(adjNode));
        tmpNode1 = tmpNode1->next;
        tmpNode1->weight = getDistance(v1, v2);
        tmpNode1->destination = v2;
        tmpAdj->N++;
        if (tmpAdj->vertex->state == 'D') tmpAdj->vertex->state = 'I';
        return 1;
    }
}

int dialog(const char *msgs[], int N) {
    char *errmsg = "";
    int rc, n;
    printf("\n");
    do {
        printf("%s", errmsg);
        errmsg = "You are wrong. Repeat input -->";
        for (int j = 0; j < N; ++j)
            puts(msgs[j]);
        puts("Make your choice --> ");
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
            printf("%s\n", "Error! Repeat input: ");
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


int add_random_vertexes() {
    printf("_________________________________________\n");
    printf("Adding random vertexes:\n");
    int num, test;
    auto *item = new vertex();
    auto *newAdjList = new adjList();
    adjList *check;
    printf("How many vertexes do you want to generate? -->");
    getNaturalInt(&num);
    for (int i = 0; i < num; ++i) {
        item->x = rand() % 199 - 99;
        item->y = rand() % 199 - 99;
        check = graph->adjList;
        test = 0;
        for (int j = 0; j < graph->N; ++j) {
            if (item->x == check->vertex->x and item->y == check->vertex->y) {
                i--;
                test = 1;
                break;
            }
            check = check->next;
        }
        if (test == 1) continue;
        graph->N++;
        item->num = graph->N - 1;
        item->state = '_';

        newAdjList->vertex = item;
        newAdjList->head = (adjNode *) malloc(sizeof(adjNode));
        newAdjList->head = nullptr;
        newAdjList->next = (adjList *) malloc(sizeof(adjList));
        newAdjList->next = nullptr;
        newAdjList->N = 0;

        add_vertex(newAdjList);

        item = new vertex();
        newAdjList = new adjList();
    }
    makeConnections();
    printf("_________________________________________");
    return 1;
}

int change_vertex_state() {
    printf("_________________________________________\n");
    printf("Changing vertex state:\n");
    int num;
    printf("Enter vertex number -->");
    getNaturalInt(&num);
    if (num > graph->N - 1) {
        printf("There is no vertex with this number\n_________________________________________");
        return 1;
    }
    adjList *ptr = graph->adjList;
    for (int i = 0; i < num; ++i) ptr = ptr->next;
    if (ptr->vertex->state == 'D' or ptr->vertex->state == 'I') ptr->vertex->state = 'E';
    else {
        switch (ptr->N) {
            case 0: {
                break;
            }
            case 1: {
                ptr->vertex->state = 'D';
                break;
            }
            default: {
                ptr->vertex->state = 'I';
                break;
            }
        }
    }
    printf("_________________________________________");
    return 1;
}

int add_random_corridors() {
    int num, fCounter = 0, v1, v2;
    printf("_________________________________________\n");
    printf("Adding random corridors:\n");
    if (graph->N < 2) {
        printf("You need more than 1 vertex\n_________________________________________");
        return 1;
    }
    printf("How many corridors do you want to generate? -->");
    getNaturalInt(&num);
    for (int i = 0; i < num; ++i) {
        if (fCounter == graph->N * 10) {
            printf("\n%d corridors successfully generated\nCoincidence limit exceeded\n_________________________________________", i);
            return 1;
        }
        v1 = rand() % graph->N;
        v2 = rand() % graph->N;
        if (v1 == v2) {
            i--;
            fCounter++;
            continue;
        }
        if (add_corridor_for_random(v1, v2) == 0) {
            i--;
            fCounter++;
            continue;
        }
        add_corridor_for_random(v2, v1);
    }
    printf("_________________________________________");
    return 1;
}

int find_way() {
    printf("_________________________________________\n");
    printf("Input start vertex number -->", graph->N - 1);
    int start;
    getNaturalInt(&start);

    spf(start);
    printf("_________________________________________");
    return 1;
}

// Заводятся массивы расстояний dist[], меток u[] и предков p[].
// Затем выполняются n итераций. На каждой итерации сначала находится вершина v,
// имеющая наименьшее расстояние dist[] среди непомеченных вершин. Если расстояние до выбранной
// вершины v оказывается равным бесконечности, то алгоритм останавливается.
// Иначе вершина помечается как помеченная, и просматриваются все рёбра,
// исходящие из данной вершины, и вдоль каждого ребра выполняются релаксации.
// Если релаксация успешна (т.е. расстояние dist[to] меняется), то пересчитывается
// расстояние dist[to] и сохраняется предок p[].

// После выполнения всех итераций в массиве dist[] оказываются длины кратчайших путей
// до всех вершин, а в массиве p[] — предки всех вершин (кроме стартовой s).
// Таким образом, можно восстановить путь до любой вершины

const float INF = 1000000000.f;

vector<int> getExits() {
    vector<int> exits;
    for (int i = 0; i < graph->N; ++i) {
        if (getAdjListByNum(i)->vertex->state == 'E') {
            exits.push_back(getAdjListByNum(i)->vertex->num);
        }
    }
    return exits;
}

// Время работы алгоритма складывается из:
// n раз поиск вершины с наименьшей величиной d[v] среди всех непомеченных вершин,
// т.е. среди O(n) вершин m раз производится попытка релаксаций
// При простейшей реализации этих операций на поиск вершины будет
// затрачиваться O(n) операций, а на одну релаксацию — O(1) операций,
// и итоговая асимптотика алгоритма составляет:  O(n^2+m)

void spf(int start) {

    float dists[graph->N];
    int p[graph->N];
    for (int i = 0; i < graph->N; ++i) {
        dists[i] = INF;
    }
    for (int i = 0; i < graph->N; ++i) {
        p[i] = 0;
    }
    dists[start] = 0;

    char u[graph->N];
    for (int l = 0; l < graph->N; ++l) {
        u[l] = false;
    }

    for (int l = 0; l < graph->N; ++l) {
        u[l] = false;
    }

    for (int i = 0; i < graph->N; ++i) {
        int v = -1;
        for (int j = 0; j < graph->N; ++j)
            if (!u[j] && (v == -1 || (dists[j] < dists[v]))) {
                v = j;
            }
        if (dists[v] == INF) {
            break;
        }
        u[v] = true; // помеченность

        for (size_t r = 0; r < getAdjListByNum(v)->N; ++r) {
            int to = getAdjNodeByNum(getAdjListByNum(v), r)->destination;
            float len = getAdjNodeByNum(getAdjListByNum(v), r)->weight;
            if (dists[v] + len < dists[to]) {
                dists[to] = dists[v] + len;
                p[to] = v;
            }
        }
    }


    for (int k = 0; k < graph->N; k++) {
        printf("Shortest way from %d to %d = %.2f\n", start, k, dists[k]);
    }

    vector<int> exits = getExits();

    printf("\nExit vertex numbers: ");

    auto iter1 = exits.begin();
    while (iter1 != exits.end()) {
        // printf("%d ", *iter1);
        iter1++;
    }
    printf("\n");

    float minPath = INF;
    int exitNum;
    auto iter2 = exits.begin();

    while (iter2 != exits.end()) {
        if (dists[*iter2] < minPath) {
            minPath = dists[*iter2];
            exitNum = *iter2;
        }
        iter2++;
    }

    vector<int> path;
    int t = exitNum;

    printf("\nPath from %d to nearest exit %d:\n", start, t);
    for (int g = t; g != start; g = p[g]) {
        path.push_back(g);
    }
    reverse(path.begin(), path.end());
    printf("%d", start);

    auto iter = path.begin();
    while (iter != path.end()) {
        printf(" --> %d", *iter);
        iter++;
    }
    printf(" | Distance: %.2f\n", minPath);

}

int d_delete_vertex() {
    int num;
    adjList *pList = graph->adjList, *dList;
    printf("_________________________________________\n");
    printf("Deleting:\n");
    printf("Input vertex number -->");
    getNaturalInt(&num);
    if (num > graph->N) {
        printf("There is no vertex with this number\n_________________________________________");
        return 1;
    }
    for (int i = 0; i < num; ++i) {
        pList = pList->next;
    }
    adjNode *pNode = pList->head, *dNode, *mNode;
    for (int j = 0;
         j < pList->N; ++j) {                                    //Пробегаемся по всем смежным удаляемой вершинам
        dList = graph->adjList;
        for (int i = 0; i < pNode->destination; ++i) {       //Выбираем вершину, с которой нужно разорвать связь
            dList = dList->next;
        }
        dNode = dList->head;
        if (dNode->destination == pList->vertex->num) {          //Если первая из списка смежных совпала
            dList->head = dNode->next;
            free(dNode);
        } else {
            for (int k = 0; k < dList->N; ++k) {                 //Если центральная из списка смежных совпала
                if (dNode->next->destination == pList->vertex->num) {
                    mNode = dNode->next;
                    dNode->next = dNode->next->next;
                    free(mNode);
                    break;
                }
                dNode = dNode->next;
            }
            if (dNode->destination == pList->vertex->num) free(dNode);  //Если последняя из списка смежных совпала
        }
        dList->N--;                                          //Уменьшение количества смежных вершин после разрыва связи
        pNode = pNode->next;                                 //Переход к следующей смежной удаляемой вершине
        if (dList->vertex->state != 'E') {                   //Изменение состояния вершины, потерявшей смежную
            if (dList->N == 1) dList->vertex->state = 'D';
            if (dList->N == 0) dList->vertex->state = '_';
        } else if (dList->N == 0) dList->vertex->state = '_';
    }

    if (pList->vertex->num == 0) {                    //Если удаляется нулевая вершина
        graph->adjList = pList->next;
    } else {                                           //Если удаляется не нулевая вершина
        dList = graph->adjList;
        for (int i = 0; i < num - 1; ++i) {
            dList = dList->next;
        }
        dList->next = dList->next->next;
    }
    for (int i = 0; i < pList->N; ++i) {            //Очищаем память удаляемой вершины
        pNode = pList->head;
        pList->head = pList->head->next;
        free(pNode);
    }
    free(pList->vertex);
    free(pList);
    graph->N--;
    renumber(num);
    printf("_________________________________________");
    return 1;
}

int add_corridor_for_random(int v1, int v2) {
    adjList *tmpAdj = graph->adjList;
    for (int i = 0; i < v1; ++i) {
        tmpAdj = tmpAdj->next;
    }
    if (tmpAdj->N == 0) {
        tmpAdj->head = (adjNode *) malloc(sizeof(adjNode));
        tmpAdj->head->weight = getDistance(v1, v2);
        tmpAdj->head->destination = v2;
        tmpAdj->N++;
        if (tmpAdj->vertex->state == '_') tmpAdj->vertex->state = 'D';
        return 1;
    } else {
        adjNode *tmpNode1 = tmpAdj->head;
        if (tmpNode1->destination == v2) return 0;
        for (int j = 0; j < tmpAdj->N - 1; ++j) {
            if (tmpNode1->next->destination == v2) return 0;
            else {
                tmpNode1 = tmpNode1->next;
            }
        }
        tmpNode1->next = (adjNode *) malloc(sizeof(adjNode));
        tmpNode1 = tmpNode1->next;
        tmpNode1->weight = getDistance(v1, v2);
        tmpNode1->destination = v2;
        tmpAdj->N++;
        if (tmpAdj->vertex->state == 'D') tmpAdj->vertex->state = 'I';
        return 1;
    }
}

void renumber(int num) {
    adjList *ptr = graph->adjList;
    adjNode *chNode;
    for (int i = 0; i < graph->N; ++i) {
        ptr->vertex->num = i;
        chNode = ptr->head;
        for (int j = 0; j < ptr->N; ++j) {
            if (chNode->destination > num) chNode->destination--;
            chNode = chNode->next;
        }
        ptr = ptr->next;
    }
}

int add_random_vertexes_for_timing(int count) {
    int test;
    auto *item = new vertex();
    auto *newAdjList = new adjList();
    adjList *check;
    for (int i = 0; i < count; ++i) {
        item->x = rand() % 199 - 99;
        item->y = rand() % 199 - 99;
        check = graph->adjList;
        test = 0;
        for (int j = 0; j < graph->N; ++j) {
            if (item->x == check->vertex->x and item->y == check->vertex->y) {
                i--;
                test = 1;
                break;
            }
            check = check->next;
        }
        if (test == 1) continue;
        graph->N++;
        item->num = graph->N - 1;
        item->state = '_';

        newAdjList->vertex = item;
        newAdjList->head = (adjNode *) malloc(sizeof(adjNode));
        newAdjList->head = nullptr;
        newAdjList->next = (adjList *) malloc(sizeof(adjList));
        newAdjList->next = nullptr;
        newAdjList->N = 0;

        add_vertex(newAdjList);

        item = new vertex();
        newAdjList = new adjList();
    }
    makeConnections();
    return 1;
}

int add_random_corridors_for_timing(int count) {
    int v1, v2;
    if (graph->N < 2) {
        return 1;
    }
    for (int i = 0; i < count; ++i) {
        v1 = rand() % graph->N;
        v2 = rand() % graph->N;
        add_corridor_for_random(v2, v1);
    }
    return 1;
}

int timing(){
    clockid_t first, last;
    int n=10, count = 500;
    while(n--){
        add_random_corridors_for_timing(count);
        add_random_vertexes_for_timing(count);
        first = clock();
        // TODO: Зависимость времени поиска выхода от количества узлов в графе
        spf(0);
        last = clock();

        printf("test #%d, time = %ld, volume = %d\n",
               10 - n, (last - first), graph->N);
        count+=500;
    }
    return 1;
}