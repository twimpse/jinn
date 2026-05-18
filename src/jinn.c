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
bool is_tainted = false;
bool is_monitored = false;
bool is_genie = false;
bool is_proxy = false;
bool is_mud false;
bool is_shooting_pengguins = false;
int start_time = NULL;
int cpu_start_time = NULL;
bool user_has_logged_in false;
bool is_slim_shady = false;
int banner_key = 0;
char canary[512]
char code_key[] = CODE_KEY;


int main_loop() {
  mode(int mode_type);

};

int signal_catcher() {

};

int main() {
  start_time = time(NULL)
  cpu_start_time = -1 // get cpu cycles or something
  // read system key from .rodata
  //
};
