#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "user_list.h"

User *g_head = NULL;

User* find_user(const char *account)
{
    User *p = g_head;
    while (p != NULL)
    {
        if (strcmp(p->account, account) == 0)
        {
            return p;
        }
        p = p->next;
    }
    return NULL;
}

void add_user(const char *nickname, const char *account, const char *password)
{
    User *new_user = (User *)malloc(sizeof(User));
    if (new_user == NULL)
    {
        printf("内存分配失败！\n");
        return;
    }

    strncpy(new_user->nickname, nickname, sizeof(new_user->nickname) - 1);
    new_user->nickname[sizeof(new_user->nickname) - 1] = '\0';
    strncpy(new_user->account,  account,  sizeof(new_user->account)  - 1);
    new_user->account[sizeof(new_user->account) - 1] = '\0';
    strncpy(new_user->password, password, sizeof(new_user->password) - 1);
    new_user->password[sizeof(new_user->password) - 1] = '\0';
    new_user->next = NULL;

    if (g_head == NULL)
    {
        g_head = new_user;
    }
    else
    {
        User *p = g_head;
        while (p->next != NULL)
        {
            p = p->next;
        }
        p->next = new_user;
    }
}

void print_all_users(void)
{
    printf("\n========== 所有注册用户信息 ==========\n");
    if (g_head == NULL)
    {
        printf("（暂无注册用户）\n");
        return;
    }

    User *p = g_head;
    int count = 0;
    while (p != NULL)
    {
        count++;
        printf("[%d] 昵称: %s | 账号: %s | 密码: %s\n",
               count, p->nickname, p->account, p->password);
        p = p->next;
    }
    printf("========== 共 %d 位用户 ==========\n\n", count);
}

void free_all_users(void)
{
    User *p = g_head;
    while (p != NULL)
    {
        User *tmp = p;
        p = p->next;
        free(tmp);
    }
    g_head = NULL;
}
