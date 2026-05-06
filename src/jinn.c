#include <stdio.h>

int main(int argc, char *argv[]) {
    printf("Welcome to jinn\n");
    
    if (argc > 1) {
        printf("Arguments received:\n");
        for (int i = 1; i < argc; i++) {
            printf("  argv[%d] = %s\n", i, argv[i]);
        }
    } else {
        printf("No additional arguments provided.\n");
    }
    
    return 0;
}
