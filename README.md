# Задание
 Написать 2 программы, первая сервер, вторая клиент. Сервер создает 
очередь сообщений для реализации чата с общей комнатой (если 
необходимо, то можно и больше очередей создать) и его задача 
уведомлять клиентов о появлении новых участников, о новых 
сообщениях. Клиент подключается к очереди, созданной сервером, 
сообщает ему свое имя и получает в ответ все сообщения в комнате. 
Далее может отправлять сообщения в общий чат. Получение служебных 
сообщений от сервера (имена новых клиентов, сообщения от других 
пользователей) и отправка сообщений в чат лучше реализовать в разных 
потоках. Интерфейс клиента реализуем с помощью библиотеки ncurses. 
# Заметки
- Запускать можно из любой директории
- Если выйти некорректно (не через "/exit"), серевер может начать работать некорректно
- После воода имени начинается процедура установления соединения, ввести команду выхода во время ввода имени нельзя
- Запускать можно только project_dir/Makefile, остальные вызываются рекурсивнои зависят от переменных главного project_dir/Makefile
- Для создания очереди сервером создаётся файл /tmp/queue_key_file, который создаётся заного при запуске сервера и удаляется при его завершении
- Сервер завершается корректно, если ввести любой символ в stdin, очередь удалится
- Перед удалением очереди на сервере используется простая задержка, а не правильная проверка состояния пользователей