// MuD game goes here
// ============= CONSTANTS =============
#define ROOM_DESCRIPTION_MAX_LEN 256
#define WEAPON_DESCRIPTION_MAX_LEN 128
#define MAX_INVENTORY 20
#define MAX_ROOMS 50

// Weapon types
#define WEAPON_HANDS 0
#define WEAPON_SWORD 1
#define WEAPON_DAGGER 2
#define WEAPON_BOW 3
#define WEAPON_RUSTY_SPOON 4
#define WEAPON_WOODEN_SPOON 5

// Item types
#define ITEM_WEAPON 0
#define ITEM_POTION 1
#define ITEM_KEY 2
#define ITEM_QUEST 3
#define ITEM_TREASURE 4

// NPC types
#define NPC_ELF 0
#define NPC_WIZARD 1
#define NPC_TROLL 2
#define NPC_DRAGON 3
#define NPC_VILLAIN 4

/*
Small Health Potion +50 25 gold
Large Health Potion +100 45 gold
Bird leg +20 health 10 gold
Lucky Potion +113 to luck multiplexer. Price 250 gold

Shots with arrows cause no flee, one shot kill. 50 injury points

Each round heals +3 health to max 100 unless poisioned

Wolf Strikes cause 5-15 in damage has 20-25 health, drops 5-10 pelts (%75)

Troll strikes cause 15-30 in damage has 30-35 health, dtops 5-30 gold (%50)

Spider attacks cause 1-5 in damage and 50% poisioned for 10 rounds (no health regen) has 5 in health

Spiders can drop arrows. (%10)

Dragom attacks cause 20-40 in damage and has 250 health

Sprained ancle 10-15 damage

Arrows cost 25 gold for 5

Bow cost 81-89 gold

Wooden spoon 0 gold 1-4 damage

Rusty Spoon 10-15 gold 3-6 damage

Dagger does 10-20 damage cost 33-39 gold

Sword does 20-40 damage 62-69 gold

Chests can contain 5-10 gold (25%), potions (3%) or arrows (10%)

Start with 91-120 gold

Maidens trade pelts and gold for potions and room keys

Elfs trade pelts and gold for arrows and rusty keys

Wizards trade in gold for potions, food, weapons, keys

*/

// ============= STRUCTS =============

typedef struct Item {
    char name[64];
    char description[256];
    int item_type;      // 0=weapon, 1=potion, 2=key, 3=quest, 4=treasure
    int value;          // gold value
    int quantity;

    // If weapon
    int min_damage;
    int max_damage;
    int poisoned;
    int poisoned_modifier;
    int magic;
    int magic_modifier;
} Item;

typedef struct Weapon {
    char description[WEAPON_DESCRIPTION_MAX_LEN];
    int max_damage;
    int min_damage;
    int poisoned;
    int poisoned_modifier;
    int magic;
    int magic_modifier;
    int weapon_type;
} Weapon;

typedef struct NPC {
    char name[64];
    int npc_type;       // elf, wizard, troll, dragon, villain
    int health;
    int damage_min;
    int damage_max;
    int experience_reward;
    int gold_reward;
    char dialogue[256];
    int is_hostile;
    int is_defeated;
} NPC;

typedef struct Room {
    char description[ROOM_DESCRIPTION_MAX_LEN];
    char room_name[64];
    int room_id;
    struct Room *north;
    struct Room *south;
    struct Room *east;
    struct Room *west;
    int has_monster;
    int has_chest;
    int has_loot;
    int has_bird;
    int visited;
    int has_npc;
    int npc_id;         // Which NPC (elf, wizard, troll, etc.)
    int is_dark;
    int required_key;   // 0=none, 1=rusty_key, etc.
    Item *items_on_floor[MAX_INVENTORY];
    int item_count_on_floor;
} Room;

typedef struct Inventory {
    Item *items[MAX_INVENTORY];
    int item_count;
} Inventory;

typedef struct GameState {
    // Player stats
    int health;
    int max_health;
    int gold;
    int potions;
    int level;
    int experience;
    int progress;

    // Quest items
    int bird_legs;
    int mud_arrows;

    // Equipment ownership flags
    int have_sword;
    int have_dagger;
    int have_bow;
    int have_rusty_spoon;
    int have_wooden_spoon;
    int have_skel_key;

    // Current equipment
    int current_weapon_type;
    Weapon *current_weapon;

    // Names
    char hero_name[32];
    char villain_name[32];
    char dragon_name[32];
    char wizard_name[32];
    char elf_name[32];

    // Position tracking
    int player_x;
    int player_y;
    Room *current_room;

    // Game progression flags
    int door_unlocked;
    int troll_defeated;
    int wizard_helped;
    int dragon_defeated;

    // Inventory system
    Inventory inventory;
} GameState;

// ============= GLOBAL VARIABLES =============
int mud_health;
int mud_max_health;
int mud_gold;
int mud_potions;
int mud_progress;
int mud_level;
int mud_experience;
int mud_bird_legs;
int mud_arrows;
int mid_luck

char mud_hero_name[32];
char mud_villain_name[32];
char mud_dragon_name[32];
char mud_wizard_name[32];
char mud_elf_name[32];

int mud_have_sword;
int mud_have_dagger;
int mud_have_bow;
int mud_have_rusty_spoon;
int mud_have_wooden_spoon;
int mud_have_skel_key;

int mud_current_weapon;
int mud_defense;
int mud_strength;

// Position tracking
int player_x;
int player_y;
int current_room_id;



char *mud_first_names[] = {
    "Aria", "Thorn", "Elara", "Kael", "Lyra", "Dorn", "Fen", "Mira",
    "Rook", "Sera", "Torvin", "Vex", "Wren", "Zale", "Bren", "Cedra"
};

char *mud_random_letters[] = {
    "an", "ar", "el", "en", "er", "in", "is", "or", "ra", "rin",
    "dor", "fal", "gar", "hal", "kar", "mal", "nar", "tar", "val",
    "bron", "drak", "grim", "keld", "morn", "thorn", "vorn", "zorn"
};

char *mud_lastname_part1[] = {
    "Ash", "Black", "Blood", "Bright", "Dark", "Dawn", "Deep", "Fair",
    "Frost", "Gold", "Gray", "Green", "High", "Iron", "Light", "Low",
    "Moon", "Night", "Red", "Silver", "Star", "Stone", "Storm", "Sun",
    "Thorn", "White", "Wild", "Wind", "Winter", "Wolf", "Wood"
};

char *lastname_part2[] = {
    "blade", "brook", "cloak", "crest", "field", "fire", "flow", "foot",
    "gard", "gate", "hand", "heart", "helm", "hold", "horn", "leaf",
    "mane", "mere", "moor", "river", "shield", "song", "spire", "stalker",
    "stone", "strider", "walker", "ward", "water", "weaver", "wind", "wood"
};
