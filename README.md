# simplechatroom
create a simple chat room in linux, include client and server.

`make test`<br>
create libtest.so, include socket libaray, epoll operation encapsulation and simple thread poll implementation<br>
`make server`<br>
build  executive file: server<br>
`make client`<br>
build executive file: client<br>

## thread poll
just a simple try and it's not applicable tolong connection or block call such as read,accept.
