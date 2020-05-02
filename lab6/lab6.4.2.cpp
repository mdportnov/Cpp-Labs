#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <cmath>
#include <vector>
#include <ctime>

#define BLACK 1;
#define WHITE 0;

const float INF = 100000.f;

using namespace std;

typedef struct vertex {
    int num;
    int x, y;
    char state;
} vertex;

struct adjList;

void add_random_vertexes(int num);

typedef struct adjNode {
    adjList *destination;
    float weight;
    adjNode *next;
} adjNode;

typedef struct adjList {
    int N;
    // fields for dijkstra
    int color; // vertex is passed? BLACK - yes, WHITE - no
    float distance;  // distance between start vertex and current
    adjList *from; // from what vertex where we come from
    vertex *vertex;
    adjNode *head;
    adjList *prev;
    adjList *next;
} adjList;

typedef struct Graph { // Graph structure represents by double linked list
    int N;
    int lastNum;
    adjList *headList;
    adjList *tailList;
} Graph;

Graph *graph;

void printV(vertex *v) {
    printf("\nVertex #%d (%d, %d)\n", v->num, v->x, v->y);
}

#include "init.h"

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
    adjList *tmpList = graph->headList;
    adjNode *tmpNode;

    for (int i = 0; i < graph->N; ++i) {
        tmpNode = tmpList->head;
        for (int j = 0; j < tmpList->N; ++j) {
            tmpNode->weight = getDistance(tmpList->vertex->num, tmpNode->destination->vertex->num);
            tmpNode = tmpNode->next;
        }
        tmpList = tmpList->next;
    }
}

void loadFromFile() {
    char filename[] = R"(C:\Users\Mi\CLionProjects\labs_cpp\lab6\)", helpfilename[50];

    FILE *f;
    graph = (Graph *) malloc(sizeof(Graph));
    graph->N = 0;
    int size;

    printf("Hello!\nEnter file name --> ");
    scanf("%s", helpfilename);
    strcat(filename, helpfilename);
    strcat(filename, ".txt");
    fopen_s(&f, filename, "rb+");
    fseek(f, 0, SEEK_END);
    if (f == nullptr) {
        printf("\nFile with the specified name does not exist\n\nFile creation...\n\nFile was created successfully\n");
        fclose(f);
        f = fopen(filename, "w+");
        size = 0;
    } else {
        fseek(f, 0, SEEK_SET);
        fscanf(f, "%d\n", &size);
    }

    printf("\nGraph size is: %d\n", size);

    graph->headList = (adjList *) malloc(sizeof(adjList));
    graph->tailList = (adjList *) malloc(sizeof(adjList));
    graph->headList->vertex = (vertex *) malloc(sizeof(vertex));
    graph->headList->head = (adjNode *) malloc(sizeof(adjNode));
    graph->headList->next = (adjList *) malloc(sizeof(adjList));
    graph->headList->prev = (adjList *) malloc(sizeof(adjList));

    graph->headList = nullptr;

    int adjListSize;
    for (int i = 0; i < size; ++i) {
        auto *vert = (vertex *) malloc(sizeof(vertex));
        fscanf(f, "%d %d %d %c %d", &vert->num, &vert->x,
               &vert->y, &vert->state, &adjListSize);
        graph->lastNum = vert->num; // set lastNum to current vertex num
        add_vertex(vert, adjListSize);
    }

    adjList *tmpList = graph->headList;
    for (int k = 0; k < graph->N; ++k) {
        adjNode *tmpNode = tmpList->head;
        for (int j = 0; j < tmpList->N; ++j) {
            int vertexNum;
            fscanf(f, "%d ", &vertexNum);
            tmpNode->destination = getAdjListByNum(vertexNum);
            tmpNode->next = (adjNode *) malloc(sizeof(adjNode));
            tmpNode = tmpNode->next;
            tmpNode->next = nullptr;
        }
        tmpList = tmpList->next;
    }
    free(tmpList);
    fclose(f);
}

void loadToFile() {
    FILE *nf;
    char newfilename[] = R"(C:\Users\Mi\CLionProjects\labs_cpp\lab6\)", helpfilename[50], check;
    printf("Press enter to save changes\n");
    fflush(stdin);
    scanf("%c", &check);
    if (check != '\n') return;
    printf("Enter file name -->");
    scanf("%s", helpfilename);
    strcat(newfilename, helpfilename);
    strcat(newfilename, ".txt");

    fopen_s(&nf, newfilename, "w+");
    printf("\nFile name is: %s", newfilename);

    adjList *tmpList1 = graph->headList;
    adjNode *tmpNode;

    fprintf(nf, "%d\n", graph->N);
    for (int i = 0; i < graph->N; ++i) {
        fprintf(nf, "%d %d %d %c %d\n", tmpList1->vertex->num, tmpList1->vertex->x, tmpList1->vertex->y,
                tmpList1->vertex->state, tmpList1->N);
        tmpList1 = tmpList1->next;
    }
    auto tmpList2 = graph->headList;
    for (int k = 0; k < graph->N; ++k) {
        tmpNode = tmpList2->head;
        for (int j = 0; j < tmpList2->N; ++j) {
            fprintf(nf, "%d ", tmpNode->destination->vertex->num);
            tmpNode = tmpNode->next;
        }
        tmpList2=tmpList2->next;
        fputc('\n', nf);
    }
    fclose(nf);
}

int show_labyrinth() {
    printf("\nLabyrinth of %d vertices show:", graph->N);
    adjList *ptr = graph->headList;
    for (int i = 0; i < graph->N; ++i) {
        printVertex(ptr);
        ptr = ptr->next;
    }
    printf("\nLast number is %d\n", graph->lastNum);
    return 1;
}

void add_vertex(vertex *item, int adjListSize) {
    auto *newAdjList = new adjList();
    newAdjList->vertex = item;
    newAdjList->color = WHITE; // field, which response for passing current node in dijkstra
    newAdjList->distance = INF;
    newAdjList->N = adjListSize;
    newAdjList->from = nullptr;
    newAdjList->prev = (adjList *) malloc(sizeof(adjList));
    newAdjList->head = new adjNode;
//    printV(newAdjList->vertex);

    newAdjList->next = (adjList *) malloc(sizeof(adjList));
    newAdjList->next = nullptr;

    graph->tailList = (adjList *) malloc(sizeof(adjList));
    graph->tailList = newAdjList;
    if (graph->headList == nullptr) {
        graph->N++;
        graph->lastNum++;
        newAdjList->prev = nullptr;
        graph->headList = newAdjList;
        return;
    }

    adjList *last = graph->headList;

    for (int i = 0; i < graph->N - 1; ++i) {
        last = last->next;
//        printV(last->vertex);
    }

    last->next = (adjList *) malloc(sizeof(adjList));
    last->next = newAdjList;
    newAdjList->prev = last;
    graph->N++;
    graph->lastNum++;

    return;
}

void sortedInsert(adjList *insertedList) {
    adjList *current;

    if (graph->N == 0) // if list is empty
    {
//        printf("\nERR\n");
        insertedList->prev = nullptr;
        insertedList->next = nullptr;
        graph->headList = insertedList;
    }
        // if the node is to be inserted at the beginning
        // of the doubly linked list
    else if (graph->headList->distance >= insertedList->distance) {
        insertedList->prev = nullptr;
        graph->headList->prev = insertedList;
        insertedList->next = graph->headList;
        graph->headList = insertedList;
    } else {
        current = graph->headList;
        // locate the node after which the new node
        // is to be inserted
        while (current->next != NULL &&
               current->next->distance < insertedList->distance)
            current = current->next;
        /* Make the appropriate links */
        insertedList->next = current->next;

        // if the new node isn't inserted
        // at the end of the list
        if (current->next != NULL)
            insertedList->next->prev = insertedList;

        current->next = insertedList;
        insertedList->prev = current;
    }
    graph->N++;
    graph->lastNum++;
    printf("\nSize: %d\n", graph->N);
}

int d_add_vertex() {
    printf("Adding new vertex:\n");
    auto *item = new vertex();
    adjList *check = graph->headList;

    printf("Input X -->");
    getInt(&item->x);
    printf("Input Y -->");
    getInt(&item->y);
    if (abs(item->y) > 99 or abs(item->x) > 99) {
        printf("Coordinates Limit (-99; 99)\n");
        return 1;
    }
    for (int j = 0; j < graph->N; ++j) {
        if (item->x == check->vertex->x and item->y == check->vertex->y) {
            printf("The vertex with these coordinates is already there\n");
            return 1;
        }
        check = check->next;
    }
    item->num = graph->lastNum;

    char test;
    printf("If the vertex is not the exit, press enter\nOtherwise, keep any symbol\n");
    fflush(stdin);
    scanf("%c", &test);
    fflush(stdin);
    if (test != '\n') item->state = 'E';
    else item->state = '_';
//    int n = 0;
    auto *newAdjList = new adjList();
    newAdjList->vertex = item;
    newAdjList->color = WHITE; // field, which response for passing current node in dijkstra
    newAdjList->distance = INF;
    newAdjList->N = 0;
    newAdjList->from = nullptr;
    newAdjList->prev = (adjList *) malloc(sizeof(adjList));
    newAdjList->prev = nullptr;
    newAdjList->head = new adjNode;
    printV(newAdjList->vertex);

    newAdjList->next = (adjList *) malloc(sizeof(adjList));
    newAdjList->next = nullptr;

    graph->tailList = (adjList *) malloc(sizeof(adjList));
    graph->tailList = newAdjList;
//    add_vertex(item, n);
    sortedInsert(newAdjList);
    makeConnections();
    return 1;
}

adjList *getAdjListByNum(int num) {
    adjList *ptr = graph->headList;
    for (int i = 0; i < graph->N; ++i) {
        if (ptr->vertex->num == num) {
            return ptr;
        }
        ptr = ptr->next;
    }
    return nullptr;
}

adjNode *getAdjNodeByNum(adjList *adjList, int num) {
    adjNode *ptr = adjList->head;
    int counter = num;
    while (counter--) {
        ptr = ptr->next;
    }
    return ptr;
}

int d_add_corridor() {
    printf("Adding new corridor:\n");
    if (graph->N < 2) {
        printf("You need more than 1 vertex\n");
        return 1;
    }
    int v1, v2;
    printf("Input first vertex -->");
    getNaturalInt(&v1);
    printf("Input second vertex -->");
    getNaturalInt(&v2);
    if (v1 == v2) {
        printf("The entered vertexes must be different\n");
        return 1;
    }
    if (getAdjListByNum(v1) == nullptr || getAdjListByNum(v2) == nullptr) {
        printf("There is no vertex with this number\n");
        return 1;
    }

    add_corridor(v1, v2);
    add_corridor(v2, v1);
    return 1;
//    if (add_corridor(v1, v2)) return 1;
//    else return 1;
}

int add_corridor(int v1, int v2) {
    adjList *tmpAdj = getAdjListByNum(v1);

    if (tmpAdj->N == 0) {
        tmpAdj->head = (adjNode *) malloc(sizeof(adjNode));
        tmpAdj->head->weight = getDistance(v1, v2);
        tmpAdj->head->destination = getAdjListByNum(v2);
        tmpAdj->N++;
        if (tmpAdj->vertex->state == '_') tmpAdj->vertex->state = 'D';
        return 1;
    } else {
        adjNode *tmpNode1 = tmpAdj->head;
        if (tmpNode1->destination->vertex->num == v2) {
            printf("These vertexes are already connected\n", v1, v2);
            return 0;
        }
        for (int j = 0; j < tmpAdj->N - 1; ++j) {
            if (tmpNode1->next->destination->vertex->num == v2) {
                printf("These vertexes are already connected\n", v1, v2);
                return 0;
            } else {
                tmpNode1 = tmpNode1->next;
            }
        }
        tmpNode1->next = (adjNode *) malloc(sizeof(adjNode));
        tmpNode1 = tmpNode1->next;
        tmpNode1->weight = getDistance(v1, v2);
        tmpNode1->destination = getAdjListByNum(v2);
        tmpAdj->N++;
        if (tmpAdj->vertex->state == 'D') tmpAdj->vertex->state = 'I';
        return 1;
    }
}

int d_add_random_vertexes() {
    printf("Adding random vertexes:\n");
    int num;

    printf("How many vertexes do you want to generate? -->");
    getNaturalInt(&num);
    add_random_vertexes(num);
    return 1;
}

void add_random_vertexes(int num) {
    int test;
    auto *item = new vertex();
    auto *newAdjList = new adjList();
    adjList *check;
    for (int i = 0; i < num; ++i) {
        item->x = rand() % 199 - 99;
        item->y = rand() % 199 - 99;
        check = graph->headList;
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
        item->num = graph->lastNum;
        item->state = '_';
        newAdjList->vertex = item;
        newAdjList->color = WHITE; // field, which response for passing current node in dijkstra
        newAdjList->distance = INF;
        newAdjList->N = 0;
        newAdjList->from = nullptr;
        newAdjList->prev = (adjList *) malloc(sizeof(adjList));
        newAdjList->prev = nullptr;
        newAdjList->head = new adjNode;
        printV(newAdjList->vertex);

        newAdjList->next = (adjList *) malloc(sizeof(adjList));
        newAdjList->next = nullptr;

        graph->tailList = (adjList *) malloc(sizeof(adjList));
        graph->tailList = newAdjList;
        sortedInsert(newAdjList);
        item = new vertex();
        newAdjList = new adjList();
    }
}

// Problem of very slowly adding new link, because  nodes hasn't order
int add_random_corridors() {
    int num, fCounter = 0, v1, v2;
    printf("Adding random corridors:\n");
    if (graph->N < 2) {
        printf("You need more than 1 vertex\n");
        return 1;
    }
    printf("How many corridors do you want to generate? -->");
    getNaturalInt(&num);
    srand(time(nullptr));
    for (int i = 0; i < num; ++i) {
        if (fCounter == graph->N * 10) {
            printf("\n%d corridors successfully generated\nCoincidence limit exceeded\n", i);
            return 1;
        }
        v1 = rand() % graph->lastNum;
        v2 = rand() % graph->lastNum;

        if (getAdjListByNum(v1) == nullptr || getAdjListByNum(v2) == nullptr
            || v1 == v2 || add_corridor_for_random(v1, v2) == 0) {
            i--;
            fCounter++;
            continue;
        }
        add_corridor_for_random(v2, v1);
    }
    return 1;
}

int change_vertex_state() {
    printf("Changing vertex state:\n");
    int num;
    printf("Enter vertex number -->");
    getNaturalInt(&num);
    adjList *ptr=getAdjListByNum(num);
    if (ptr== nullptr) {
        printf("\nThere is no vertex with this number\n");
        return 1;
    }
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
    printf("\nState was successfully changed\n");

    return 1;
}

int find_way() {
    printf("Input start vertex number -->");
    int start;
    getNaturalInt(&start);
    spf(start);
    return 1;
}

vector<int> getExits() {
    vector<int> exits;
    adjList* tmpList=graph->headList;
    for (int i = 0; i < graph->N; ++i) {
        if (tmpList->vertex->state == 'E') {
            exits.push_back(tmpList->vertex->num);
        }
        tmpList = tmpList->next;
    }
    return exits;
}

void pullOutList(adjList *changedList) {
    if (graph->headList == NULL || changedList == NULL)
        return;

    /* If node to be deleted is head node */
    if (graph->headList == changedList) {
        if (graph->headList->next != nullptr) // if not alone
        {
//            printf("\nERR2\n");
            graph->headList = graph->headList->next;
            graph->headList->prev = nullptr;
        }
    }

    /* Change next only if node to be
    deleted is NOT the last node */
    if (changedList->next != NULL)
        changedList->next->prev = changedList->prev;

    /* Change prev only if node to be
    deleted is NOT the first node */
    if (changedList->prev != NULL)
        changedList->prev->next = changedList->next;
    graph->N--;

    /* Finally, free the memory occupied by del*/
}

int d_delete_vertex() {
    int num;
    adjList *delList, *changedList;
    printf("Deleting:\n");
    printf("Input vertex number -->");
    getNaturalInt(&num);
    delList = getAdjListByNum(num);

    if (delList == nullptr) {
        printf("There is no vertex with this number\n");
        return 1;
    }


    adjNode *pNode = delList->head;
    for (int j = 0; j < delList->N; ++j) { //Пробегаемся по всем смежным удаляемой вершинам
        changedList = getAdjListByNum(pNode->destination->vertex->num);
        adjNode *tmpNode = changedList->head;
        // Case 1
        if (changedList->head->destination->vertex->num == num) {
            changedList->head = changedList->head->next;
        } else {// Case 2
            for (int i = 0; i < changedList->N; ++i) {
                if (tmpNode->next->destination->vertex->num == num) {
                    tmpNode->next = tmpNode->next->next;
                    break;
                }
                tmpNode = tmpNode->next;
            } // Case 3

            if (tmpNode->destination->vertex->num == num) {
                printf("\nERR\n");
                tmpNode->destination = nullptr;
            }
        }
        changedList->N--;
        pNode = pNode->next; //Переход к следующей смежной удаляемой вершине

        if (changedList->vertex->state != 'E') {  //Изменение состояния вершины, потерявшей смежную
            if (changedList->N == 1) changedList->vertex->state = 'D';
            if (changedList->N == 0) changedList->vertex->state = '_';
        } else if (changedList->N == 0) changedList->vertex->state = '_';
    }

    adjNode *delNode = delList->head;
    adjNode *ptr;
    while (delNode) {
        ptr = delNode;
        delNode = delNode->next;
        free(ptr);
    }

    pullOutList(delList);
    free(delList);
//    if (graph->headList == NULL)
//        return 1;
//
//    /* If node to be deleted is head node */
//    if (graph->headList == delList) {
//        graph->headList = graph->headList->next;
//        graph->headList->prev = nullptr;
//        free(delList->vertex);
//        free(delList);
//        graph->N--;
//        return 1;
//    } else if (delList->next != NULL)
//        delList->next->prev = delList->prev;
//    else if (delList->prev != NULL)
//        delList->prev->next = delList->next;
//    free(delList->vertex);
//    free(delList);
    return 1;
}

int add_corridor_for_random(int v1, int v2) {
    adjList *tmpAdj = getAdjListByNum(v1);

    if (tmpAdj->N == 0) {
        tmpAdj->head = (adjNode *) malloc(sizeof(adjNode));
        tmpAdj->head->weight = getDistance(v1, v2);
        tmpAdj->head->destination = getAdjListByNum(v2);
        tmpAdj->N++;
        if (tmpAdj->vertex->state == '_') tmpAdj->vertex->state = 'D';
        return 1;
    } else {
        adjNode *tmpNode1 = tmpAdj->head;
        if (tmpNode1->destination->vertex->num == v2) return 0;
        for (int j = 0; j < tmpAdj->N - 1; ++j) {
            if (tmpNode1->next->destination->vertex->num == v2) return 0;
            else {
                tmpNode1 = tmpNode1->next;
            }
        }
        tmpNode1->next = (adjNode *) malloc(sizeof(adjNode));
        tmpNode1 = tmpNode1->next;
        tmpNode1->weight = getDistance(v1, v2);
        tmpNode1->destination = getAdjListByNum(v2);
        tmpAdj->N++;
        if (tmpAdj->vertex->state == 'D') tmpAdj->vertex->state = 'I';
        return 1;
    }
}

int add_random_corridors_for_timing(int count) {
    int v1, v2;
    if (graph->N < 2) {
        return 1;
    }
    for (int i = 0; i < count; ++i) {
        v1 = rand() % graph->lastNum;
        v2 = rand() % graph->lastNum;
        add_corridor_for_random(v2, v1);
    }
    return 1;
}

void replaceList(adjList *replacedList) {

    // Pulling out
    if (graph->headList == NULL || replacedList == NULL)
        return;

    /* If node to be deleted is head node */
    if (graph->headList == replacedList) {
        graph->headList = graph->headList->next;
        graph->headList->prev = nullptr;
    }

    /* Change next only if node to be
    deleted is NOT the last node */
    if (replacedList->next != NULL)
        replacedList->next->prev = replacedList->prev;

    /* Change prev only if node to be
    deleted is NOT the first node */
    if (replacedList->prev != NULL)
        replacedList->prev->next = replacedList->next;


    // Push with right order
    adjList *current;

    if (graph->headList == nullptr) // if list is empty
    {
        graph->headList = replacedList;
    }
        // if the node is to be inserted at the beginning
        // of the doubly linked list
    else if (graph->headList->distance >= replacedList->distance) {
        replacedList->prev = nullptr;
        graph->headList->prev = replacedList;
        replacedList->next = graph->headList;
        graph->headList = replacedList;
    } else {
        current = graph->headList;
        // locate the node after which the new node
        // is to be inserted
        while (current->next != NULL &&
               current->next->distance < replacedList->distance)
            current = current->next;
        /* Make the appropriate links */
        replacedList->next = current->next;

        // if the new node isn't inserted
        // at the end of the list
        if (current->next != NULL)
            replacedList->next->prev = replacedList;

        current->next = replacedList;
        replacedList->prev = current;
    }
}

void spf(int start) {
    adjList *tmpList = graph->headList; // launch initialization
    for (int i = 0; i < graph->N; ++i) {
        tmpList->distance = INF;
        tmpList->color = WHITE;
        tmpList->from= new adjList;
        tmpList = tmpList->next;
    }
    free(tmpList);

    vector<int> exits = getExits();
    if(exits.size()==0){
        printf("\nThere is no exits\n");
        return;
    }

    adjList *curList = getAdjListByNum(start);

    if (curList == nullptr || curList->head == nullptr) {
        printf("\nThere is no ways from this vertex\n");
        return;
    }
    // So we have list with priority and we need to move
    // our start vertex at the top of list

    curList->distance = 0;
    replaceList(curList);
    // Black = 1 = attended
    // White = 0 = not attended
    printf("\nSearching shortest way from vertex #%d...\n", curList->vertex->num);

    // every time a cycle is repeated we need to move on the next
    // NOT attended vertex and lift current vertex up in priority list;

    while (curList != nullptr) {
        printf("\nV: %d", curList->vertex->num);
        adjNode *adjNode = curList->head; // current vertex, which has link with curList vertex
        for (int i = 0; i < curList->N; ++i) {
            if (adjNode->destination->color != 1 &&
                curList->distance + adjNode->weight <= adjNode->destination->distance) {
                adjNode->destination->distance = adjNode->weight + curList->distance; // set new distance
                adjNode->destination->from = curList;
                replaceList(adjNode->destination);
                printf("\nAdj V: %d", adjNode->destination->vertex->num);
            }
            adjNode = adjNode->next;
        }
        curList->color = BLACK;
        curList = curList->next;
    }

    adjList *ptrList=graph->headList;
    for (int k = 0; k < graph->N; k++) {
        printf("\nShortest way from %d to %d = %.2f", start, ptrList->vertex->num, ptrList->distance);
        ptrList=ptrList->next;
    }

    printf("\nExit vertex numbers: ");

    auto iter1 = exits.begin();
    while (iter1 != exits.end()) {
        printf("%d ", *iter1);
        iter1++;
    }

    float minPath = INF;
    int exitNum;
    auto iter2 = exits.begin();

    while (iter2 != exits.end()) {
        if (getAdjListByNum(*iter2)->distance < minPath) {
            minPath = getAdjListByNum(*iter2)->distance;
            exitNum = *iter2;
        }
        iter2++;
    }
    adjList* pathList=getAdjListByNum(exitNum);
    if(pathList->distance==INF){
        printf("\n\nThere is no way from this vertex\n");
        return;
    }

    printf("\nPath from %d to nearest exit %d:\n", start, exitNum);

    printf("%d", exitNum);
    pathList=pathList->from;
    while (pathList->from!= nullptr){
        printf(" <-- %d", pathList->vertex->num);
        pathList=pathList->from;
    }
    printf(" | Distance: %.2f\n", minPath);

}

int timing() {
    clockid_t first, last;
    int n = 10, count = 500;
    while (n--) {
        add_random_vertexes(count);
        add_random_corridors_for_timing(count);
        first = clock();
        // Зависимость времени поиска выхода от количества узлов в графе
        spf(0);
        last = clock();

        printf("test #%d, time = %ld, volume = %d\n",
               10 - n, (last - first), graph->N);
        count += 500;
    }
    return 1;
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