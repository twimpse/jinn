#ifndef USER_DB_H
#define USER_DB_H

#include <time.h>

// Constants
#define MAX_USERNAME 16
#define MAX_PASSWORD_HASH 128
#define MAX_HOSTNAME 100
#define MAX_USERS 128

// Admin credentials
#define ADMIN_USER "bob"
#define ADMIN_PASS "BoBizBack!"

// User levels (lower number = higher access)
#define USER_LEVEL_ADMIN     0
#define USER_LEVEL_MODERATOR 2
#define USER_LEVEL_USER      5
#define USER_LEVEL_GUEST     9

// Connection types
typedef enum
{
  CONN_INTERNET,
  CONN_SOCKET,
  CONN_PIPE
} connection_type_t;

// Forward declaration
typedef struct user user_t;

// Function pointer types
typedef int (*auth_function)(user_t *, const char *password);
typedef void (*update_login_function)(user_t *, const char *location,
                                      connection_type_t conn_type);

// User struct
struct user
{
  // Data fields
  char username[MAX_USERNAME];
  char password_hash[MAX_PASSWORD_HASH];
  int user_level;               // 0 = highest access
  int group_number;             // 0 = admin group
  time_t last_login_time;
  char last_location[MAX_HOSTNAME];
  connection_type_t last_connection_type;

  // Methods (function pointers)
  auth_function authenticate;
  update_login_function update_login;
};

// Database struct
typedef struct
{
  user_t users[MAX_USERS];
  int user_count;
} user_db_t;

// Public functions
int init_user_db(user_db_t * db);
int add_user(user_db_t * db, const char *username, const char *password,
             int level, int group);
user_t *find_user(user_db_t * db, const char *username);
int authenticate_user_login(user_db_t * db, const char *username,
                            const char *password, const char *location,
                            connection_type_t conn_type);
void print_user_info(user_t * user);

#endif
