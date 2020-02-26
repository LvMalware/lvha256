clean:
    rm -vrf ./~*
all:
    gcc -o lvha256sum test.c lvha256.c