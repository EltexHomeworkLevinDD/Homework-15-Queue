#ifndef MSGTTYPES_H
#define MSGTTYPES_H

// ================================= Control messages =================================

#define MAX_CONTENT_SIZE 64    // Max size of field "content"
#define MAX_HISTORY_SIZE 4096
#define MAX_LIST_SIZE 1024

struct ControlMessage {
    long mtype;
    int message;
    char content[MAX_CONTENT_SIZE];
};

struct ControlHistory {
    long mtype;
    char hist[MAX_HISTORY_SIZE];
    int hist_size;
};

struct ControlUsersList {
    long mtype;
    char list[MAX_LIST_SIZE];
    int list_size;
};

// ===== Client --> Server
#define CM_SETUP_CONNECT_REQEST     101
#define CM_GET_HISTORY_REQEST       102
#define CM_GET_USERS_LIST_REQUEST   103

// ===== Server --> Client
#define CM_SETUP_CONNECT_RESPONSE   301
#define CM_GET_HISTORY_RESPONSE     302
#define CM_GET_USERS_LIST_RESPONSE  303

#define CM_NOTIFICATION_USER_CONNECTED      501
#define CM_NOTIFICATION_USER_DISCONNECTED   502

// ===== Bidirectional =====
#define CM_BREAK_CONNECT 10000

/*
У каждого клиента есть PTS (priority to server) для отправки сообщений 
    на сервер и PFS (priority from server) для получения сообщений с сервера.
    Если получен PTS, то PFS = PTS + 1

           Диаграмма установления и разрушения соединения (Control Qeueue)
=============================================================================================================================
| Client |      Struct                  MSG                                               Content                  | Server |
=============================================================================================================================
|  --->  | ControlMessage       CM_SETUP_CONNECT_REQEST             [content: "Name";                  mtype: 1]   |  --->  |
|  <---  | ControlMessage       CM_SETUP_CONNECT_RESPONSE           [content: "Name|PTS";              mtype: 2]   |  <---  |
|  --->  | ControlMessage       CM_GET_HISTORY_REQEST               [content: "";                      mtype: PTS] |  --->  |
|  <---  | ControlHistory       CM_GET_HISTORY_RESPONSE             [content: "(history content)";     mtype: PFS] |  <---  |
|  --->  | ControlMessage       CM_GET_USERS_LIST_REQUEST           [content: "";                      mtype: PTS] |  --->  |
|  <---  | ControlUsersList     CM_GET_USERS_LIST_RESPONSE          [content: "(User|User...)"; s       mtype: PFS] |  <---  |
                                        ...
|  <---  | ControlHistory       CM_NOTIFICATION_USER_CONNECTED      [content: "Name";                  mtype: PFS] |  <---  |
                                        ...
|  <---  | ControlHistory       CM_NOTIFICATION_USER_DISCONNECTED   [content: "Name";                  mtype: PFS] |  <---  |
                                        ...
|  --->  | ControlHistory       CM_BREAK_CONNECT                    [content: "";                      mtype: PTS] |  --->  |
                                ------- OR -------
|  <---  | ControlHistory       CM_BREAK_CONNECT                    [content: "";                      mtype: PFS] |  <---  |
*/

// ================================= Text messages ==========================================================================

#define MAX_TEXT_MSG_SIZE 256 // Максимальный размер текста в передаваемом сообщении (не включая собственное имя)

struct TextMessage {
    long mtype;
    int text_size;
    char text[MAX_TEXT_MSG_SIZE];
};

/*
              Диаграмма обмена сообщениями (Text Qeueue)
=================================================================
| Client |   Struct                 Content            | Server | 
=================================================================
|  --->  | TextMessage [text: "message"; mtype: PTS]   |  --->  |
|  <---  | TextMessage [text: "message"; mtype: PFS]   |  <---  |
*/

// =================================================================================

#endif//MSGTTYPES_H