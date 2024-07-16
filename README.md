# chatroom-c
chatroom-c is a multi-threaded CLI-based chat application in a mesh network design using plain C
with pthreads + sockets.

# Building
* GNU Make:
`make`

# Usage
Example:
`./chatroom-c`

Must have a `chatnode.properties` file matching the format shown in the repository. Default port is
`51966`, aka `0xCAFE`. Comment or remove the `ip` key in the properties file to host the chat room
on the desired port, otherwise, the IP + port combo is used by clients to connect to an existing
chat room by filling in the properties file correctly and running `/join` once the client is
started.

# Authors
* Mahafuj Alam
* Nicholas Botticelli

# Demo
An example video showing cross-network chatting using an older version with a couple visual bugs
(not affecting internal logic) can be seen in the YouTube video below.

[![Video demo](https://img.youtube.com/vi/PTEXrJTe-WI/0.jpg)](https://www.youtube.com/watch?v=PTEXrJTe-WI) 
