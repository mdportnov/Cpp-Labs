#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <iostream>
#include <ctime>

// 1. Каждый узел покрашен либо в черный, либо в красный цвет
// 2. Листьями объявляются NIL-узлы (т.е. "виртуальные" узлы, наследники узлов,
//    которые обычно называют листьями; на них "указывают" NULL указатели). Листья покрашены в черный цвет
// 3. Если узел красный, то оба его потомка черны (Родитель каждого красного узла - черный)
// 4. На всех ветвях дерева, ведущих от его корня к листьям, число черных узлов одинаково
// 5. Корень - черный

#define RED 0
#define BLACK 1

using namespace std;

typedef struct Node {
    int key;
    int color;
    char *info;

    Node *parent, *left, *right;
} Node;

/* Глобальные переменные - все функции имеют к ним доступ */
Node *NIL = (Node *) malloc(sizeof(Node));
Node *ROOT = NIL;
int counter = 0;

int loadFromFile();

int dialog(const char *msgs[], int N);

int getNaturalInt(int *a);

int getInt(int *a);

char *getStr();

void left_rotate(Node *x);

void right_rotate(Node *x);

int dialog_show();

int rb_insert(Node *);

int dialog_delete();

int dialog_insert();

int dialog_search();

int dialog_max_search();

int timing();

Node *tree_search(int key);

Node *tree_minimum(Node *x);

void rb_insert_fixup(Node *z);

void rb_delete_fixup(Node *x);

void rb_replace(Node *u, struct Node *v);

int rb_tear_down(Node *ptr, Node **root);

int show_as_tree(int count, Node *nd) {
    int i = 0;
    if (nd == NIL) return 1;
    show_as_tree(count + 1, nd->left);
    for (; i < count; ++i) {
        printf("   ");
    }
    printf("%d;%d;%s\n", nd->key, nd->color, nd->info);
    show_as_tree(count + 1, nd->right);
    return 1;
}

int d_show_as_tree() {
    puts("Tree:");
    show_as_tree(0, ROOT);
    return 1;
}

const char *msgs[] = {"0. Quit", "1. Add", "2. Find", "3. Find max key", "4. Delete",
                      "5. Show", "6. Timing", "7. Show as a tree"};

const int NMsgs = sizeof(msgs) / sizeof(msgs[0]);

int (*tree_functions[])() ={nullptr, dialog_insert, dialog_search, dialog_max_search,
                            dialog_delete, dialog_show, timing, d_show_as_tree};

int main() {
    NIL->color = BLACK;
    NIL->left = NIL;
    NIL->right = NIL;

    loadFromFile();

    int rc;
    while ((rc = dialog(msgs, NMsgs)))
        if (!tree_functions[rc]())
            break;
    printf("That's all. Bye!\n");
    rb_tear_down(ROOT, &ROOT);


}

int rb_show(Node *x) {
    if (x != NIL) {
        rb_show(x->left);
        printf("[key: %d | info: %s]\n", x->key, x->info);
        rb_show(x->right);
    }
    return 1;
}

int dialog_show() {
    rb_show(ROOT);
    return 1;
}

Node *tree_search(int key) {
    Node *x = ROOT;
    while (x != NIL && x->key != key) {
        if (key < x->key) {
            x = x->left;
        } else {
            x = x->right;
        }
    }
    return x;
}

Node *find_node(int key) {
    Node *cur = ROOT;
    while (cur != NIL) {
        counter++;
        if (cur->key == key) return cur;
        if (key < cur->key) cur = cur->left;
        else cur = cur->right;
    }
    return nullptr;
}

int dialog_search() {
    printf("Input key:\n");
    int key;
    getInt(&key);

    if (tree_search(key) == NIL) {
        printf("\nThere is no element with this key\n");
    } else {
        Node *ptr = tree_search(key);
        printf("Info of element with key \"%d\" is \"%s\n", key, ptr->info);
    }

    return 1;
}

Node *tree_minimum(Node *x) {
    while (x->left != NIL) {
        x = x->left;
    }
    return x;
}

Node *tree_maximum() {
    Node *x = ROOT;
    while (x->right != NIL) {
        x = x->right;
    }
    return x;
}

int dialog_max_search() {
    if (tree_maximum() == NIL) {
        printf("\nThere is no tree exists\n");
    } else
        printf("Element with the max key (%d): %s\n", tree_maximum()->key, tree_maximum()->info);
    return 1;
}

// Вставка производится по той же процедуре, что и для BST.
// За исключением того, что новый узел окрашивается в красный цвет.
// Так как он окрашен в красный цвет, он может нарушить 3 или 5.
// По этой причине для исправления нарушения вызывается вспомогательная процедура rb_insert_fixup.

int dialog_insert() {
    Node *z;
    z = (Node *) malloc(sizeof(Node));

    int key;
    printf("Input key:\n");
    getInt(&key);

    if (find_node(key) != nullptr) {
        printf("\nElement with this key already exists\n");
        return 1;
    }

    z->key = key;
    z->color = RED;
    z->left = NIL;
    z->right = NIL;

    std::cin.sync(); // очистка буфера

    printf("Input info:\n");
    z->info = getStr();

    rb_insert(z);
    return 1;
}

int rb_insert(Node *z) {
    Node *x, *y;

    x = ROOT;
    y = NIL;

    // Просматриваем дерево до тех пор, пока не достигнем листа (NIL)
    // Y используется для отслеживания последнего не NIL узла, который будет родителем Z.

    while (x != NIL) {
        y = x;
        if (z->key <= x->key) {
            x = x->left;
        } else {
            x = x->right;
        }
    }

    if (y == NIL) {
        ROOT = z;
    } else if (z->key <= y->key) {
        y->left = z;
    } else {
        y->right = z;
    }

    z->parent = y;

    rb_insert_fixup(z);
    return 0;
}

void rb_insert_fixup(Node *z) {
    while (z->parent->color == RED) { // Пока предок нового узла - красный, нарушено свойство 3

        // Если родитель Z - левый сын дедушки
        if (z->parent == z->parent->parent->left) {

            // Перекрашивание предусматривает два случая

            /* Дядя Z - красный */
            if (z->parent->parent->right->color == RED) {
                z->parent->color = BLACK;
                z->parent->parent->right->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            }
                /* Дядя Z - НЕ красный */
            else {
                /* Если Z - правый ребёнок родителя */
                if (z == z->parent->right) {
                    z = z->parent;
                    left_rotate(z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                right_rotate(z->parent->parent);
            }
        } else { // Если родитель Z - левый правый дедушки
            /* Дядя Z - красный */
            if (z->parent->parent->left->color == RED) {
                z->parent->color = BLACK;
                z->parent->parent->left->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            }
                /* Дядя Z - НЕ красный */
            else {
                /* z is z's parents left child */
                if (z == z->parent->left) {
                    z = z->parent;
                    right_rotate(z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                left_rotate(z->parent->parent);
            }
        }
    } // На данный момент может быть нарушено только свойство 5, поэтому сделаем корень чёрным
    ROOT->color = BLACK;
}

/*
 * Y - правый ребёнок X. Поворачивая влево от Х, делаем X левым ребёнком Y.
 * Левый ребёнок Y становится правым ребёнком X.
 *
 * 		x									y
 *	   / \                                 / \
 *    A   y			----------->		  x   C
 *		 / \							 / \
 *	    B   C						    A   B
 */

void left_rotate(struct Node *x) {
    struct Node *y;

    /* Делаем левого ребёнка Y правым ребёнком X */
    y = x->right;
    x->right = y->left;
    if (y->left != NIL) {
        y->left->parent = x;
    }

    /* Делаем родителя X родителем Y и Y - ребёнком родителя X */
    y->parent = x->parent;
    if (y->parent == NIL) {
        ROOT = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }

    /* Делаем X левым ребенком Y и Y родителем X */
    y->left = x;
    x->parent = y;
}

/*
 * Y - левый ребёнок X. Поворачивая вправо от Х, делаем X правым ребёнком Y.
 * Правый ребёнок Y становится левым ребёнком X.
 *
 *			|											|
 *			x											y
 *		   / \										   / \
 *		  y   A		---------------->			      B	  x
 *		 / \											 / \
 *	    B   C										    C   A
 */

void right_rotate(struct Node *x) {
    struct Node *y;

    /* Делаем правого ребёнка Y левым ребёнком X */
    y = x->left;
    x->left = y->right;
    if (y->right != NIL) {
        y->right->parent = x;
    }

    /* Делаем родителя X родителем и Y ребёнком родителя X */
    y->parent = x->parent;
    if (y->parent == NIL) {
        ROOT = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }

    /* Сделать Y родителем X и X ребёнком Y */
    y->right = x;
    x->parent = y;
}

// Удаление производится тем же механизмом, что и удаление BST. Если у Z нет ребенка, то Z удалён.
// Если Z имеет одного ребенка, Z заменяется его ребенком. Иначе z заменяется на его преемника
// Если преемник не является прямым ребенком z, то он заменяется сначала на своего собственного ребенка.
// Затем Z заменяется преемником.
//
// Указатель Y используется для отслеживания. В первых двух случаях Y - это Z. В третьем случае Y -  преемник Z
// Итак, в первых двух случаях "Y" удаляется. В третьем случае Y перемещается.
//
// Другой указатель X используется для отслеживания узла, который заменяет Y.
//
// Так как удаление или перемещение Y может повредить свойства красно-черного дерева,
// переменная yOriginalColor используется для отслеживания оригинального цвета. Если он ЧЕРНЫЙ, тогда
// удаление или перемещение - плохо для свойств красно-черного дерева.
// В этом случае для восстановления вызывается процедура red_black_delete_fixup(x).

int rb_delete(Node *z) {
    Node *y, *x;
    int yOriginalColor;

    y = z;
    yOriginalColor = y->color;

    if (z->left == NIL) {
        x = z->right;
        rb_replace(z, z->right);
    } else if (z->right == NIL) {
        x = z->left;
        rb_replace(z, z->left);
    } else {
        y = tree_minimum(z->right);
        yOriginalColor = y->color;

        x = y->right;

        if (y->parent == z) {
            x->parent = y;
        } else {
            rb_replace(y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }

        rb_replace(z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }

    if (yOriginalColor == BLACK) {
        rb_delete_fixup(x);
    }
    return 1;
}

void rb_delete_fixup(Node *ptr) {
    Node *p, *w;
    while (ptr != ROOT && ptr->color == BLACK) {
        p = ptr->parent;
        if (ptr == p->left) {
            w = p->right;
            if (w->color == RED) {
                w->color = BLACK;
                p->color = RED;
                left_rotate(p);
                w = p->right;
            }
            if (w->left->color == BLACK && w->right->color == BLACK) {
                w->color = RED;
                ptr = p;
            } else {
                if (w->right->color == BLACK) {
                    w->color = RED;
                    w->left->color = BLACK;
                    right_rotate(w);
                    w = p->right;
                }
                w->color = p->color;
                p->color = BLACK;
                w->right->color = BLACK;
                left_rotate(p);
                ptr = ROOT;
            }
        } else {
            w = p->left;
            if (w->color == RED) {
                w->color = BLACK;
                p->color = RED;
                right_rotate(p);
                w = p->left;
            }
            if (w->right->color == BLACK && w->left->color == BLACK) {
                w->color = RED;
                ptr = p;
            } else {
                if (w->left->color == BLACK) {
                    w->color = RED;
                    w->right->color = BLACK;
                    left_rotate(w);
                    w = p->left;
                }
                w->color = p->color;
                p->color = BLACK;
                w->left->color = BLACK;
                right_rotate(p);
                ptr = ROOT;
            }
        }
    }
    ptr->color = BLACK;
}

int dialog_delete() {
    printf("Input key:\n");
    int key;
    getInt(&key);

    if (tree_search(key) == NIL) {
        printf("\nThere is no element with this key\n");
        return 1;
    }

    cout << "info: " << tree_search(key)->info << "\n";
    rb_delete(tree_search(key));
    return 1;
}

// Так как Y был черным и удаленным, Х приобретает дополнительную черноту Y.
// Перемещаем "лишнюю" черноту X до тех пор, пока
// 1. X не станет корнем. В таком случае просто уберите лишнюю черноту
// 2. X не становится красным и чёрным узлом. В таком случае просто сделайте x Чёрным.
//
// Сначала проверим, левый или правый ребенок являются ли родителями X. Скажем, X - левый ребенок
//
// Есть 4 случая.
//
// Случай 1: родной брат Х - КРАСНЫЙ. Преобразовать случай 1 в случай 2 путем перекрашивания
// W и родителя X. Затем левый поворот родителя X.
//
// Случай 2: брат X - черный, оба ребенка X - черные. Переместите X и W
// черный - родителю Х, окрашивая W в красный цвет, а родитель X - в черный.
// Сделаем родителя X новым X.
//
// Случай 3: W черный, левый ребенок w красный, правый ребенок черный. Трансформируйте
//         случай 3 в случай 4, перекрашивая W и левого ребенка W, затем поворот вправо от W.
//
// Случай 4: W - черный, правый ребенок W - красный. Перекрасим W в цвет родителя X.
// Сделаем родителя X черным, правого ребенка - черным. Теперь сделаем левый поворот от родителя X.
// Сделать X указывать на корень. Таким образом, цикл будет остановлен в следующей итерации.
// Если x - его правый ребенок, сделаем то же самое, поменяв местами повороты влево<->правое.

void rb_replace(Node *u, Node *v) {
    if (u->parent == NIL) {
        ROOT = v;
    } else if (u == u->parent->left) {
        u->parent->left = v;
    } else {
        u->parent->right = v;
    }
    v->parent = u->parent;
}

int rb_tear_down(Node *ptr, Node **root) {
    if (ptr == NIL) return 1;
    rb_tear_down(ptr->left, root);
    rb_tear_down(ptr->right, root);
    rb_delete(ptr);
    return 1;
}

int timing() {
    Node *ptr, *nd, *cur;
    int n = 10, key[10000], k, cnt = 1000000, i, m, j;
    clock_t first, last;
    srand(time(nullptr));
    while (n-- > 0) {
        for (i = 0; i < 10000; ++i)
            key[i] = rand() * rand();
        for (i = 0; i < cnt;) {
            k = rand() * rand();
            ptr = (Node *) malloc(sizeof(Node));
            ptr->right = NIL;
            ptr->left = NIL;
            ptr->key = k;
            nd = find_node(k);
            if (!nd) {
                if (!rb_insert(ptr))
                    ++i;
            }
        }
        m = 0;
        first = clock();
        for (i = 0; i < 10000; ++i) {
            if (find_node(key[i])) {
                ++m;
            }
        }
        last = clock();
        printf("%d items were found\n", m);
        printf("test #%d, number of nodes = %d, time = %ld\n",
               10 - n, (10 - n) * cnt, (last - first));
        cur = ROOT;
        for (j = 0; cur != NIL; ++j) {
            cur = cur->left;
        }
        printf("tree height: %d\n", j);
        printf("checked elements: %d\n\n", counter);
    }
    rb_tear_down(ROOT, &ROOT);
    return 1;
}

int loadFromFile() {
    printf("Input file name:\n");
    char *fname = getStr();
    strcat(fname, ".txt");
    cout << "\n" << fname;

    FILE *f = NULL;
    errno_t err = fopen_s(&f, fname, "r");

    if (err) {
        printf("\nFile does not exist \n");
        return 1;
    }
    rewind(f);

    char *info = (char *) calloc(100, sizeof(char));
    int k;

    while (fscanf(f, "%d%s", &k, info) != EOF) {
        Node *newNode = (Node *) malloc(sizeof(Node));

        char *str = (char *) calloc(100, sizeof(char));

        strcpy(str, info);

        newNode->color = RED;
        newNode->left = NIL;
        newNode->right = NIL;
        newNode->key = k;
        newNode->info = str;

        rb_insert(newNode);
    }
    free(info);
    fclose(f);
    return 1;
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