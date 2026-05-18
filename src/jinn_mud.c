// MuD game goes here
#DEFINE ROOM_DESCRIPTION_MAX_LEN 128


int mud_health
int mud_gold
int mud_potions
int mud_progress
int mud_level
char *mud_hero_name[32];
char *mud_villan_name[32];
char *mud_dragon_name[32];
char *nud_wizards_name[32];
char *mud_elf_name[32];


typedef struct Room {
  char description[ROOM_DESCRIPTION_MAX_LEN]
  struct Room *north;
  struct Room *south;
  struct Room *east;
  struct Room *west;
  int has_monster;
  int has_chest;
  int has_loot;
