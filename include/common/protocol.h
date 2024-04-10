#ifndef PROTOCOL_H
#define PROTOCOL_H

#define MAX_USER_NAME_SZ    16
#define MAX_MESSAGE_SZ      512
#define MAX_HISTORY_SZ      4096
#define MAX_USERS_COUNT     16

struct Notification {
    long mtype;  // Кому (Приоритет сообщения)
    int type;   // Notification type
};

// ====== Notification types
// Connection establish
#define NT_CONNECT_REQUEST 10       // Requires TransactionConnect
#define NT_USER_CONNECTED 11        // Requires TransactionUser
#define NT_USER_DISCONNECTED 12     // Requires TransactionUser
#define NT_NEW_MESSAGE 13           // Requires TransactionMessage

// ====== Transactions
struct TransactionConnect {
    long mtype;  // Кому (Приоритет сообщения)
    long PTS;    // Приоритет отправки на сервер
    long PFS;    //
    char history[MAX_HISTORY_SZ];
    char users_list[MAX_USERS_COUNT][MAX_USER_NAME_SZ];
};

struct TransactionMessage {
    long mtype;
    char message[MAX_MESSAGE_SZ];
};

struct TransactionUser {
    long mtype;
    char name[MAX_USER_NAME_SZ];
};

#endif//PROTOCOL_H