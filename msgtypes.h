#ifndef MSGTTYPES_H
#define MSGTTYPES_H

// ================================= Control messages =================================

#define MAX_CONTENT_SIZE 64    // Max size of field "content"
#define MAX_HISTORY_SIZE 4096

struct ControlMessage {
    long mtype;
    int message;
    char content[MAX_CONTENT_SIZE];
};

struct ControlHistory {
    long mtype;
    char hist[MAX_HISTORY_SIZE];
};

// ===== Client --> Server
#define CM_SETUP_CONNECT_REQEST 10
#define CM_GET_HISTORY_REQEST   12

// ===== Server --> Client
#define CM_SETUP_CONNECT_RESPONSE   30
#define CM_GET_HISTORY_RESPONSE     31

// ===== Bidirectional =====
#define CM_BREAK_CONNECT 100

/*
У каждого клиента есть PTS (priority to server) для отправки сообщений 
    на сервер и PFS (priority from server) для получения сообщений с сервера.
    Если получен PTS, то PFS = PTS + 1

           Диаграмма установления и разрушения соединения (Control Qeueue)
==================================================================================
| Client |                              MSG                              | Server | 
==================================================================================
|  --->  | CM_SETUP_CONNECT_REQEST     [content: "Name";     mtype: 1]   |  --->  |
|  <---  | CM_SETUP_CONNECT_RESPONSE   [content: "Name|PTS"; mtype: 2]   |  <---  |
|  --->  | CM_GET_HISTORY_REQEST       [content: "";         mtype: PTS] |  --->  |
|  <---  | CM_GET_HISTORY_RESPONSE     [content: "";         mtype: PFS] |  <---  |

|  --->  | CM_BREAK_CONNECT            [content: "";         mtype: PTS] |  --->  |
                                ------- OR -------
|  <---  | CM_BREAK_CONNECT            [content: "";         mtype: PFS] |  <---  |
*/

// ================================= Text messages =================================

#define MAX_TEXT_SIZE 256

struct TextMsg {
    long mtype;
    char text[MAX_TEXT_SIZE];
};

/*
                  Диаграмма обмена сообщениями (Text Qeueue)
================================================================
| Client |                    MSG                     | Server | 
================================================================
|  --->  | TextMsg [content: "message"; mtype: PTS]   |  --->  |
|  <---  | TextMsg [content: "message"; mtype: PFS]   |  <---  |
*/

// =================================================================================

#endif//MSGTTYPES_H