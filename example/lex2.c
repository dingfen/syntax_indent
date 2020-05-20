void foo(int arg1, char arg2)
{
    printf("hello world\n");
    printf("%d %c\n", arg1, arg2);
}

int main()
{
    int a = 2;
    int b = 3;
    int c;
    char *d = "asbsdq";
    c = a + b * 2;
    if (c >= 3)
        foo(c, d[2]);
    else {
        foo(14, d[1]);
    }
    for(a = 0; a < 10; a=a+1) {
        b = b + a;
        printf("%d\n", b);
    }
    return 0;
}