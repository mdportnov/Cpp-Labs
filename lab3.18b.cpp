#include <iostream>
using namespace std;
struct Item {
    Item(char i, Item* pVoid) {
        s=i;
        next=pVoid;
    }
    Item() {}
    char s;
    Item *next;
};
int getList(Item **pptr);
Item *deleteList(Item *ptr);

void form(Item *in, Item *&outDigits, Item *&outLetters);
Item *skip(Item *str);
Item *copyDigits(Item *&from, Item *&to, int *nflag);
Item *copyLetters(Item *&from, Item *&to, int *cflag);
void showItem(Item *list);
void printList(const char *msg, Item *list);

int is_digit(Item a) {
    if (a.s >= 48 && a.s <= 57) {
        return 1;
    } else return 0;
}

int is_alpha(Item a) {
    if ((a.s >= 65 && a.s <= 90) || (a.s >= 97 && a.s <= 122)) {
        return 1;
    } else return 0;
}

int main(){
    Item *in = nullptr, *outDigits = nullptr, *outLetters = nullptr;

    while(cout << "Enter string:\n", getList(&in)){
        printList("Entered string: ", in);

        form(in, outDigits, outLetters);

        printList("Result string of nums: ", outDigits);
        printList("Result string of words: ", outLetters);

        deleteList(outDigits);
        deleteList(outLetters);
        deleteList(in);
    }
}

void form(Item *in, Item *&outDigits, Item *&outLetters){
    int nflag=0, cflag=0;

    Item headDigits = {'*', nullptr};
    Item headLetters = {'*', nullptr};

    outDigits = &headDigits;
    outLetters = &headLetters;

    while((in = skip(in))){
        if(isdigit(in->s)){
            if(nflag) {
                outDigits = copyDigits(in, outDigits, &nflag);
                nflag=1;
            }
            if(!nflag)
                outDigits = copyDigits(in, outDigits, &nflag);
        } else {
            if(cflag){
                outLetters=copyLetters(in, outLetters, &cflag);
                cflag=1;
            }
            if(!cflag)
                outLetters=copyLetters(in, outLetters, &cflag);
        }
    }

    outDigits = headDigits.next;
    outLetters = headLetters.next;
}

Item *copyDigits(Item *&from, Item *&to, int *nflag) {
    if (*nflag == 0) {
        while (from != nullptr && (is_digit(*from))) {
//            Item *k = new Item;
//            k->s = from->s;
//            k->next = nullptr;
//            to->next = k;
//            to = k;
//            from = from->next;
//            to=to->next;
            to->next=from;
            from = from->next;
            to=to->next;
        }
        to->next= nullptr;
        *nflag=1;
    } else {
        to->next = new Item(' ', nullptr);
        to = to->next;
        to->s = ' ';
        to->next = nullptr;

        while (from != nullptr && (is_digit(*from))) {
            to->next=from;
            from = from->next;
            to=to->next;
        }
        to->next= nullptr;
    }

    return to;
}

Item *copyLetters(Item *&from, Item *&to, int *cflag){
    if(*cflag==0){
        while(from!=nullptr && (is_alpha(*from))){
            to->next=from;
            from = from->next;
            to=to->next;
        }
        to->next= nullptr;

        *cflag=1;
    } else{
        to->next = new Item(' ', nullptr);
        to = to->next;
        to->s = ' ';
        to->next = nullptr;

        while(from!=nullptr && (is_alpha(*from))){
            to->next=from;
            from = from->next;
            to=to->next;
        }
        to->next= nullptr;
    }

    return to;
}

Item *skip(Item *str)
{
    while(str!=nullptr && !is_digit(*str) && !is_alpha(*str)){
        str = str->next;
    }
    return str;
}

void printList(const char *msg, Item *list) {
    cout << msg << "\"";
    while (list) {
        cout << list->s;
        list = list->next;
    }
    cout << "\"\n";
}

Item *deleteList(Item *ptr) {
    Item *tmp = nullptr;
    while (ptr != nullptr) {
        tmp = ptr;
        ptr = ptr->next;
        free(tmp);
    }
    return ptr;
}

int getList(Item **pptr)
{
    char buf[81], *str;
    Item head = {'*', NULL };
    Item *last = &head;
    int n, rc = 1;
    do{
        n = scanf("%80[^\n]", buf);
        if(n < 0){
            deleteList(head.next);
            head.next = NULL;
            rc = 0;
            continue;
        }
        if(n > 0){
            for(str = buf; *str != '\0'; ++str){
                last->next = (Item *)malloc(sizeof(Item));
                last = last->next;
                last->s = *str;
            }
            last->next = NULL;
        }
        else
            scanf("%*c");
    } while(n > 0);
    *pptr = head.next;
    return rc;
}
