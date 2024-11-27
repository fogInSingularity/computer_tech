//Задание похоже на 2, но использует отдельный процесс - 'арбитр очереди'

// Арбитр обслуживает очередь, часть сообщений он пропускает через себя и раздаёт потребителям.
// Арбитр очищает очередь после завершения работы одного из абонентов,
// регистрирует абонентов и даёт им идентификаторы.

// Каждый клиент при запуске представляется простым именем, и получает свой идентификатор.
// Клиент должен раз в 10 секунд дёргать арбитра(послать служебное сообщение),
//если он этого не сделает, то арбитр удалит его из списка доступных клиентов.

// При необходимости передать сообщение адресату,
// клиент сначала получает от арбитра номер принимающего абонента,
// а потом отправляет сообщение уже на этот номер

//  Работа в клиенте начинается с регистрации текущего пользователя
//  #<Имя>
//  Передача сообщения: <Имя>#<Сообщение>
//  Выход из имени - ## 

//  Сдесь изложены лишь наброски алгоритма, начинать необходимо с прорабтоки
//  алгоритмов работы клиента и арбитра.
//  Клиента удобнее реализовывать на потоках.  
