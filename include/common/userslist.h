#ifndef LIST_H
#define LIST_H

#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "protocol.h"

#define MIN_PTS 5
#define MIN_PFS (MIN_PTS+1)

/* Структура, представляющая пользователя */
struct User {
    char name[MAX_USER_NAME_SZ];
    long PTS;
    long PFS; // PTS + 1
};

// Структура, представляющая узел двусвязного списка
struct Node {
    struct User* user;   // Структура пользователя
    struct Node *prev;  // Указатель на предыдущий узел
    struct Node *next;  // Указатель на следующий узел
};

// Структура, представляющая двусвязный список
struct List {
    struct Node *head;  // Указатель на начало списка
    struct Node *tail;  // Указатель на конец списка
};

struct User* create_new_user(char* name, long PTS, long PFS);
struct User* create_new_user_auto(struct List* list, char* name);

void initialize_list(struct List *list);
struct Node* append(struct List *list, struct User* user);
void remove_node(struct List *list, struct Node *node);
struct Node *search_by_name(struct List *list, char *name);
struct Node *search_by_PTS(struct List *list, int PTS);
struct Node *search_by_PFS(struct List *list, int PFS);
void freeList(struct List *list);

#endif//LIST_H