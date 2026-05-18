#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>
#include <openssl/sha.h>
#include "jinn.h"
#include "seed.h"

bool is_innocent = false;
int innocent_heuristics = 0;
int times_innocent = 0;

