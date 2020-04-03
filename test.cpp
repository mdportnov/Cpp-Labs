#include <cstdio>

int main(){
    FILE *fd;
    char fname[]="../files.txt";
    long size;
    fd=fopen(fname,"w+b");
    char input_buf[]="data for testing";
    char output_buf[20];

    if(fd==nullptr){
        fprintf(stderr, "can't open file %s\n", fname);
        return 1;
    }
    printf("size of data: %d\n", sizeof(input_buf));
    fwrite(input_buf, 1, sizeof(input_buf), fd);

    fseek(fd, 0, SEEK_END);
    size=ftell(fd);

    printf("File %s has %ld bytes \n", fname, size);

    rewind(fd);

    printf("reading from file\n");

    while(size=fread(output_buf, 1, sizeof(output_buf), fd), size>0)
        fwrite(output_buf, 1, size, stdout);

    printf("\n");
    fclose(fd);






    return 0;
}