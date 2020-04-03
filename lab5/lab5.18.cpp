
typedef struct Node{
    int color;
    int key;

    struct Node *left, *right;
    struct Node *parent;
} Node;


const char* msgs[] = { "0.Quit", "1.Add", "2.Find", "3.Find max key", "4.Delete",
                       "5.Show", "6.Timing", "7.Input from file", "8.Output in file" };
const int NMsgs = sizeof(msgs) / sizeof(msgs[0]);
int dialog(const char* msgs[], int N);

int main(){

}