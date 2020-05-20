void foo(int arg1, char arg2)
{
    printf("hello world\n");
    printf("%d %c\n", arg1, arg2);
}

int main()
{
    char *d = "awesbsdq";
    int i;
    for(i = 0; i < strlen(d); i=i+1) {
        if (d[i] > 's') {
            d[i] = d[i]+1;  /* for d[i]++ */
        } else {
            d[i] = 0;
        }
    }
    return 0;
}