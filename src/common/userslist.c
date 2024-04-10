#include "../../include/common/userslist.h"

// Инициализация пустого списка
void initialize_list(struct List *list) {
    list->head = NULL;
    list->tail = NULL;
}

/*Создать пользователя
Возвращает
- указатель на пользвателя
- NULL в случае ошибки выделения памяти*/
struct User* create_new_user(char* name, long PTS, long PFS){
    struct User* newUser = (struct User*) malloc(sizeof(struct User));
    if (newUser == NULL){
        return NULL;
    }
    strncpy(newUser->name, name, MAX_USER_NAME_SZ);
    newUser->name[MAX_USER_NAME_SZ-1] = '\0';
    newUser->PTS = PTS;
    newUser->PFS = PFS;

    return newUser;
}

/*Создать пользователя
Автоматически назначает PTS и PFS на основе максимальных 
приоритетов, хранящихся в List 
Возвращает
- указатель на пользвателя
- NULL в случае ошибки выделения памяти*/
struct User* create_new_user_auto(struct List* list, char* name){
    // Найти свободный приоритет (плохое решение, int исчерпается)
    struct Node* current = list->head;
    long maxPFS = MIN_PFS;
    while (current != NULL) {
        if (current->user->PFS > maxPFS){
            maxPFS = current->user->PFS;
        }
        current = current->next;
    }

    struct User* newUser = create_new_user(name, maxPFS + 1, maxPFS + 2);
    if (newUser == NULL){
        return NULL;
    }
    return newUser;
}

/*Добавление структуры User в конец списка
Возвращает
- указатель на узел списка, содержащий User
- NULL - в случае ошибки выделения памяти*/
struct Node* append(struct List *list, struct User* user) {
    // Выделение памяти под новый узел
    struct Node *newNode = (struct Node *)malloc(sizeof(struct Node));
    if (newNode == NULL) {
        return NULL;
    }
    
    // Заполнение нового узла данными пользователя
    newNode->user = user;
    // Установка указателя на следующий узел в NULL, так как новый узел будет последним
    newNode->next = NULL;

    // Если список пустой, устанавливаем новый узел как начало списка
    if (list->head == NULL) {
        list->head = newNode;
        list->tail = newNode;
        newNode->prev = NULL;
    } else {
        // Иначе добавляем новый узел в конец списка
        list->tail->next = newNode;
        newNode->prev = list->tail;
        list->tail = newNode;
    }

    return newNode;
}

// Удаление структуры User из списка
void remove_node(struct List *list, struct Node *node) {
    // Проверка наличия узла для удаления
    if (node == NULL)
        return;

    // Удаление узла из списка
    if (node->prev != NULL)
        node->prev->next = node->next;
    else
        list->head = node->next;

    if (node->next != NULL)
        node->next->prev = node->prev;
    else
        list->tail = node->prev;

    // Освобождение памяти, выделенной для узла
    free(node->user);
    free(node);
}

/*Поиск структуры User по имени
Возвращает
- указатель на узел списка с нужным именем
- NULL - если элемент с именем не найден
*/
struct Node *search_by_name(struct List *list, char *name) {
    struct Node *current = list->head;
    // Проход по списку для поиска пользователя
    while (current != NULL) {
        // Сравнение имени пользователя с искомым именем
        if (strcmp(current->user->name, name) == 0)
            return current;
        current = current->next;
    }
    return NULL; // Возвращаем NULL, если пользователь не найден
}

/*Поиск структуры User по количеству PTS
Возвращает
- указатель на узел списка
- NULL - если пользователь не найден*/
struct Node *search_by_PTS(struct List *list, int PTS) {
    struct Node *current = list->head;
    // Проход по списку для поиска пользователя
    while (current != NULL) {
        // Сравнение количества PTS пользователя с искомым
        if (current->user->PTS == PTS)
            return current;
        current = current->next;
    }
    return NULL; // Возвращаем NULL, если пользователь не найден
}

/*Поиск структуры User по количеству PFS
Возвращает
- указатель на узел списка
- NULL - если пользователь не найден*/
struct Node *search_by_PFS(struct List *list, int PFS) {
    struct Node *current = list->head;
    // Проход по списку для поиска пользователя
    while (current != NULL) {
        // Сравнение количества PFS пользователя с искомым
        if (current->user->PFS == PFS)
            return current;
        current = current->next;
    }
    return NULL; // Возвращаем NULL, если пользователь не найден
}

// Освобождение памяти, выделенной для списка
void freeList(struct List *list) {
    struct Node *current = list->head;
    // Проход по списку и освобождение памяти для каждого узла
    while (current != NULL) {
        struct Node *temp = current;
        current = current->next;
        free(temp->user);
        free(temp);
    }
    // Установка указателей на начало и конец списка в NULL
    list->head = NULL;
    list->tail = NULL;
}

