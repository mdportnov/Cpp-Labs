#include <iostream>
using namespace std;
const int SIZE = 80;
void form(char *in, char *outDigits, char *outLetters);
char *skip(char *str);
char *copyDigits(char *&from, char *to);
char *copyLetters(char *&from, char *to);

int main(){
    char buf[SIZE], outDigits[SIZE], outLetters[SIZE];
    while(cout << "Enter string: \n", cin.getline(buf, SIZE)){
        form(buf, outDigits, outLetters);
        cout << "Result string of nums: \"" << outDigits <<"\""<<
        endl <<"Result string of words: \""<< outLetters <<"\""<< endl;
    }
}

void form(char *in, char *outDigits, char *outLetters){
    while( *(in = skip(in)) ){
        if(*in >= '0' && *in <= '9'){
            outDigits = copyDigits(in, outDigits);
        } else {
            outLetters = copyLetters(in, outLetters);
        }
    }
    *outDigits = '\0';
    *outLetters = '\0';
}

char *copyDigits(char *&from, char *to)
{
    while(*from >= '0' && *from <= '9'){
        *to++ = *from++;
    }
    *to++ = ' ';
    return to;
}

char *skip(char *str)
{
    while(*str && ((*str<'0' || *str>'9') && (*str<'A' || *str>'Z') && (*str<'a' || *str>'z'))){
        str++;
    }
    return str;
}

char *copyLetters(char *&from, char *to)
{
    while(((*from >= 'A' && *from <= 'Z') || (*from >= 'a' && *from <= 'z'))){
        *to++ = *from++;
    }
    *to++ = ' ';
    return to;
}


