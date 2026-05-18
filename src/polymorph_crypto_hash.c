/* Polymorphic sha512 hash function
   Uses a polymorphic approch to creating a sha512 hash function.

   The loop used is varied as are the ROTR impls. Different iteration
   Each run, but very different code run each time. Also random jitter,
   Junk code and delays are added into the mix.

*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "polymorph.h"


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

// a polymorphic generator that randomly selects different implementations for each component.
// That would output valid SHA512 code that's different every time it runs.

int jinn_sha512() {
  jinn_rand_delay((rand() % RAND_DELAY_MAX) +1);

  // random rotr

  jinn_rand_delay((rand() % RAND_DELAY_MAX) +1);

  // random loop
  // jinn_rand_junk_inst() in loop

  jinn_rand_delay((rand() % RAND_DELAY_MAX) +1);

  // random ordering
  // jinn_rand_junk_inst() in loop

  jinn_rand_delay((rand() % RAND_DELAY_MAX) +1);

  // random storage

  jinn_rand_delay((rand() % RAND_DELAY_MAX) +1);

  return sha512hash;
}
