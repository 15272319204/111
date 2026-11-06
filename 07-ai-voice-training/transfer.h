#ifndef TRANSFER_H
#define TRANSFER_H

/* ===== 文件回传配置 ===== */
/* 根据实际网络环境，取消注释其中一种方式并修改 IP/路径 */

/* 方式1: TFTP */
/* #define TRANSFER_CMD "tftp -p -l user_data.txt -r user_data.txt 192.168.1.100" */

/* 方式2: SCP */
/* #define TRANSFER_CMD "scp user_data.txt user@192.168.1.100:/home/user/" */

/* 方式3: NFS 拷贝 */
/* #define TRANSFER_CMD "cp user_data.txt /mnt/nfs/user_data.txt" */

#endif
