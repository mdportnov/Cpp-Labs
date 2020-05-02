
int dialog(const char *msgs[], int N);

int getNaturalInt(int *a);

int getInt(int *a);

void add_vertex(vertex *, int);

adjList *getLastAdjList();

adjList *getAdjListByNum(int num);

int add_corridor(int, int);

int d_add_vertex();

int d_add_corridor();

int d_delete_vertex();

int find_way();

int show_labyrinth();

void loadFromFile();

void loadToFile();

int d_add_random_vertexes();

int add_random_corridors();

int change_vertex_state();

int add_corridor_for_random(int v1, int v2);

void renumber(int num);

int timing();

void printVertex(adjList *ptr) {
    if(ptr== nullptr){
        printf("\nVertex is null");
        return;
    }
    printf("\nVertex #%d (%d, %d) (%c) adjust with: ", ptr->vertex->num, ptr->vertex->x, ptr->vertex->y,
           ptr->vertex->state);
    adjNode *a = ptr->head;
    for (int i = 0; i < ptr->N; ++i) {
        printf("%d (%.2f) ", a->destination->vertex->num, a->weight);
        a = a->next;
    }
    printf(" | distance: %.2f", ptr->distance);
}

adjList *getPrev(int num) {
    printVertex(getAdjListByNum(num)->prev);
    return getAdjListByNum(num)->prev;
}

const char *msgs[] = {"0. Quit", "1. Add vertex", "2. Add corridor", "3. Delete vertex", "4. Find way",
                      "5. Show labyrinth", "6. Add random vertexes", "7. Add random corridors",
                      "8. Change the state of the vertex", "9. Timing"};

const int NMsgs = sizeof(msgs) / sizeof(msgs[0]);

int (*graph_functions[])() ={nullptr, d_add_vertex, d_add_corridor, d_delete_vertex, find_way, show_labyrinth,
                             d_add_random_vertexes, add_random_corridors, change_vertex_state, timing};

void makeConnections();

adjNode *getAdjNodeByNum(adjList *, int);

void spf(int start);

void printNode(adjNode *adjNode) {
    printf("\nTo node #%d weight is %f\n", adjNode->destination, adjNode->weight);
}

float getDistance(int v1, int v2) {
    adjList *ptr1 = graph->headList;
    while(ptr1->vertex->num!=v1){
        ptr1 = ptr1->next;
    }
    adjList *ptr2 = graph->headList;
    while(ptr2->vertex->num!=v2) {
        ptr2 = ptr2->next;
    }
    return (float) sqrt(
            pow((abs(ptr1->vertex->x - ptr2->vertex->x)), 2) + pow(abs(ptr1->vertex->y - ptr2->vertex->y), 2));
}