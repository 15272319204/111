#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_io.h"
#include "user_list.h"
#include "transfer.h"

void save_users_to_file(const char *filename)
{
    FILE *fp = fopen(filename, "w");
    if (fp == NULL)
    {
        printf("文件 %s 打开失败！\n", filename);
        return;
    }

    User *p = g_head;
    int count = 0;
    while (p != NULL)
    {
        count++;
        fprintf(fp, "[%d] 昵称: %s | 账号: %s | 密码: %s\n",
                count, p->nickname, p->account, p->password);
        p = p->next;
    }
    fprintf(fp, "========== 共 %d 位用户 ==========\n", count);

    fclose(fp);
    printf("用户信息已保存到文件：%s\n", filename);
}

void load_users_from_file(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        /* 文件不存在，首次运行，忽略 */
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        char nickname[32] = {0};
        char account[32]  = {0};
        char password[32] = {0};
        int  num = 0;

        /* 匹配格式: [N] 昵称: X | 账号: Y | 密码: Z */
        int ret = sscanf(line, "[%d] 昵称: %31[^|] | 账号: %31[^|] | 密码: %31s",
                         &num, nickname, account, password);
        if (ret == 4)
        {
            /* 去除昵称尾部空格 */
            int len = strlen(nickname);
            while (len > 0 && nickname[len - 1] == ' ')
            {
                nickname[--len] = '\0';
            }
            /* 去除账号尾部空格 */
            len = strlen(account);
            while (len > 0 && account[len - 1] == ' ')
            {
                account[--len] = '\0';
            }

            add_user(nickname, account, password);
        }
    }

    fclose(fp);
    printf("已从文件 %s 加载用户数据\n", filename);
}

void transfer_file_to_pc(const char *filename)
{
#ifdef TRANSFER_CMD
    printf("正在将文件回传到电脑...\n");
    printf("执行命令: %s\n", TRANSFER_CMD);
    int ret = system(TRANSFER_CMD);
    if (ret == 0)
    {
        printf("文件回传成功！\n");
    }
    else
    {
        printf("文件回传失败（返回码: %d），请手动拷贝 %s\n", ret, filename);
    }
#else
    printf("============================================\n");
    printf("提示：请配置 transfer.h 中的 TRANSFER_CMD 宏\n");
    printf("      以实现文件自动回传功能。\n");
    printf("      当前请手动拷贝文件：%s\n", filename);
    printf("============================================\n");
#endif
}
