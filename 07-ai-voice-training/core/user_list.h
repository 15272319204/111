#ifndef USER_LIST_H
#define USER_LIST_H

typedef struct User {
    char nickname[32];
    char account[32];
    char password[32];
    struct User *next;
} User;

extern User *g_head;

User* find_user(const char *account);
void  add_user(const char *nickname, const char *account, const char *password);
void  print_all_users(void);
void  free_all_users(void);

#endif
