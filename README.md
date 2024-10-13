The goal of this exam is to create a mini server IRC in C language.

So we can confirm understanding of this function:
  * socket() to take the fd of the socket.
  * select() ou poll() for the managment of multiple clients.
  * bind() and listen() for prepare the server to welcome some clients.
  * accept() to add some new client in our server.
  * send() and recv() for send or receive some message from connected clients and diffuse it to other clients.

You just have to do:
````sh
make
````
and
````sh
./aout "port"
````

Now you can use nc to simulate a cient on others terminal to start a conversation:
````sh
nc localhost "port"
````
