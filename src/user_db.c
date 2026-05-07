#include <openssl/sha.h>
#include "user_db.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

static void sha256_hash(const char* input, char* output) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, input, strlen(input));
    SHA256_Final(hash, &sha256);
    
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(output + (i * 2), "%02x", hash[i]);
    }
}

// Authentication function
static int authenticate_user(user_t *u, const char *password)
{
    char test_hash[MAX_PASSWORD_HASH];
    sha256_hash(password, test_hash);  // Changed from one_way_hash to sha256_hash

    if (strcmp(u->password_hash, test_hash) == 0)
    {
        printf("[OK] Authentication successful for %s (Level %d)\n",
               u->username, u->user_level);
        return 1;
    }
    else
    {
        printf("[FAIL] Authentication failed for %s\n", u->username);
        return 0;
    }
}

// Update login information
static void update_user_login(user_t *u, const char *location, connection_type_t conn_type)
{
    u->last_login_time = time(NULL);
    strncpy(u->last_location, location, MAX_HOSTNAME - 1);
    u->last_location[MAX_HOSTNAME - 1] = '\0';
    u->last_connection_type = conn_type;
}

// Internal function to initialize a single user
static void init_user(user_t *u, const char *username, const char *password, int level, int group)
{
    strncpy(u->username, username, MAX_USERNAME - 1);
    u->username[MAX_USERNAME - 1] = '\0';

    sha256_hash(password, u->password_hash);  // Changed from one_way_hash to sha256_hash
    u->user_level = level;
    u->group_number = group;
    u->last_login_time = 0;
    strcpy(u->last_location, "never");
    u->last_connection_type = CONN_INTERNET;

    // Assign function pointers
    u->authenticate = authenticate_user;
    u->update_login = update_user_login;
}

// Initialize the database with admin user
int init_user_db(user_db_t *db)
{
    if (!db)
        return -1;

    db->user_count = 0;

    // Add the admin user from defines
    return add_user(db, ADMIN_USER, ADMIN_PASS, USER_LEVEL_ADMIN, 0);
}

// Add a new user to the database
int add_user(user_db_t *db, const char *username, const char *password,
             int level, int group)
{
    if (!db || db->user_count >= MAX_USERS)
        return -1;

    // Check if user already exists
    if (find_user(db, username) != NULL)
    {
        printf("Error: User '%s' already exists!\n", username);
        return -1;
    }

    init_user(&db->users[db->user_count], username, password, level, group);
    db->user_count++;

    printf("Added user: %s (Level %d, Group %d)\n", username, level, group);
    return 0;
}

// Find user by username
user_t *find_user(user_db_t *db, const char *username)
{
    if (!db)
        return NULL;

    for (int i = 0; i < db->user_count; i++)
    {
        if (strcmp(db->users[i].username, username) == 0)
        {
            return &db->users[i];
        }
    }
    return NULL;
}

// Authenticate and update login in one call
int authenticate_user_login(user_db_t *db, const char *username,
                            const char *password, const char *location,
                            connection_type_t conn_type)
{
    user_t *user = find_user(db, username);
    if (!user)
    {
        printf("User '%s' not found\n", username);
        return 0;
    }

    if (user->authenticate(user, password))
    {
        user->update_login(user, location, conn_type);
        return 1;
    }
    return 0;
}

// Display user information - Simple and clean version
void print_user_info(user_t *user)
{
    if (!user)
        return;

    const char *conn_names[] = { "Internet", "Socket", "Pipe" };
    char time_buffer[64];
    struct tm *time_info;

    printf("\n========================================\n");
    printf("         USER INFORMATION              \n");
    printf("========================================\n");
    printf("Username:      %s\n", user->username);
    printf("User Level:    %d (", user->user_level);
    
    // Show level description
    switch(user->user_level) {
        case 0: printf("Admin"); break;
        case 2: printf("Moderator"); break;
        case 5: printf("User"); break;
        case 9: printf("Guest"); break;
        default: printf("Custom");
    }
    printf(")\n");
    
    printf("Group Number:  %d\n", user->group_number);
    
    if (user->last_login_time > 0)
    {
        time_info = localtime(&user->last_login_time);
        strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", time_info);
        printf("Last Login:    %s\n", time_buffer);
    }
    else
    {
        printf("Last Login:    Never\n");
    }
    
    printf("Last Location: %s\n", user->last_location);
    printf("Connection:    %s\n", conn_names[user->last_connection_type]);
    printf("========================================\n");
}