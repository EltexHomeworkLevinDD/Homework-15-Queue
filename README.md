# Queue
## Задание
### Задание 1 Написать программу клиент-сервер.
Первый процесс создаёт очередь, записывает сообщение для второй программы. Вторая читает, посылает ответ. Первая считывает, потом завершается.на System V и на POSIX
git branch task-1 - Задание 1, system V
git branch task-2 - Задание 1, POSIX
### Задание 2 Написать чат с общей комнатой
Есть сервер, который создаёт очередь. Там будут храниться все сообщения. Клиенты подключаются к очереди и забирают все сообщения в комнате и могут располагать свои сообщения. надо и читать и писать в любой момент. Можно использовать для этого два потока. Один ждёт служебные сообщения от сервера. Второй ждёт ввода пользователя и может отправлять sms на сервер.
## Пометки
В src/submain.c указан относительный путь, запускать из главной директории проекта "{Project}/"