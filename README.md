# UDP File Transfer
This project is the first programming assignment for CSCI-4273 Network Systems (Fall 2020) at CU Boulder.
The premise is to do unreliable file transfers over the User Datagram Protocol (UDP). Future work may include making it
reliable while still using UDP.

## Usage
In the root directory of the project is a Makefile. There are multiple make targets, but running the default using
`make` will compile the `utilities.c`, `udp_server.c`, and `udp_client.c`. This produces the `udpclient` and `udpserver`
binaries. The usage for these two binaries is as follows:

```shell script
server/udpserver <port>
client/udpclient <hostname> <port>
```

The `udpclient` will prompt you to enter a command. Your options are:
* `get [FILE]`: retrieve a file from the server.
* `put [FILE]`: send a file to the server.
* `delete [FILE]`: delete a file from the server.
* `ls`: list the files in the directory of the server.
* `exit`: tell the server to shutdown.

If you are requesting a file, you do not need the `[]` around the name of the file. For example, to retrieve `foo.txt`,
you need only say `get foo.txt`.

**Note**: Entering a command not listed above will result in the server repeating the command to the client.

## Files of Importance
The following are explanations of the code files and their significance.

### utilities.h & utilities.c
These two files contain a few, important functions. The two of most interest are `sendFile` and `receiveFile` which are
responsible for the code behind the client's `get` and `put` commands. When the client uses the `get` command, the
server calls the `sendFile` function while the client calls the `receiveFile` function. Similarly, when the client uses
the `put` command, the server calls the `receiveFile` function while the server calls the client calls the `sendFile`
function.

The file also contains the `error` and `trimSpace` functions. The `error` function prints an error message to the
console and exits. The `trimSpace` function removes whitespace from the end of a string that the user may have entered.

All of these functions were placed into the `utilities.c` file to reduce code duplication.

### server/udp_server.c
This file contains the server code for this project. It listens on the specified UDP port on all available interfaces
and waits for an incoming connection. Upon receiving a message, there is a series of statements to determine what
command was sent and what to do about it. If the message sent to the server does not match any registered command, it is
repeated to the client.

### client/udp_client.c
This file contains the client logic. It is very similar to the server code but has less logic around the commands that
are inputted to it.

### Makefile
The Makefile includes all of the targets to compile the `udpserver`, `udpclient`, and `utilities.o`. By default, it will
compile both the `udpserver` and `udpclient`. It also includes the `clean` target which will remove all object files
(`.o`).

## Credits
This project was designed by Jacob Malcy. While much of the socket code was provided to us, I wrote the majority of the
code myself.