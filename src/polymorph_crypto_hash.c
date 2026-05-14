// Polymorphic sha512 hash function
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const char *loops[] = {
    "for (int i = 0; i < 80; i++) { %s }",
    "int i = 0; while (i++ < 80) { %s }"
};

const char *rotr_impls[] = {
    "#define ROTR(x,n) (((x) >> (n)) | ((x) << (64-(n))))",
    "#define ROTR(x,n) ( (n==0) ? x : ((x>>1)|((x&1)<<63)) ROTR(x,n-1) )"
};

/*

int main() {
    srand(time(NULL));

    printf("#include <stdint.h>\n");
    printf("%s\n\n", rotr_impls[rand() % 2]);
    printf("void sha512_transform(uint64_t *state, const uint64_t *block) {\n");
    printf("    %s\n", loops[rand() % 2]);
    printf("}\n");

    return 0;
}

*/

int jinn_sha512() {

}
