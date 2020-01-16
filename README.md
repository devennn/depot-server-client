# depot-server

This is an assignment for Programming Course

Features
1. As a server - client
  - Receiving Connection from client
  - Connect to another server
2. Using multithread to process messages at the same time
3. Prevent connection to/from the same server/client
4. Sorting items in dictionary order
5. Minimal race conditions - Up to 20 client connections
6. Only receive messages from connection (not stdin)

How it works
1. Run program 
2. Program will print Port number (localhost)
3. Connect to port.
4. Perform Communication by sending Message

| Message                      | Parameters                                                                           | Action                                                                                                                                 |
|------------------------------|--------------------------------------------------------------------------------------|----------------------------------------------------------------------------------------------------------------------------------------|
| Connect : p                    | p : port to connect to                                                               | Connect to another depot                                                                                                               |
| IM : p : name                   | p : the command port of the depot sending the message name : name of the other depot | Another depot has connected to you and is introducing itself. Both parties should send one of these immediately after a new connection |
| Deliver : q : t                 | q : quantity of good t : type of good                                                | Add the quantity of the good to your stocks.                                                                                           |
| Withdraw : q : t                | q : quantity of good t : type of good                                                | Remove the quantity of the good from your stocks.                                                                                      |
| Transfer : q : t : dest          | k : key to refer to later                                                            | As with Delivery but deferred until later                                                                                              |
| Defer : k : Deliver : q : t        | k : key to refer to later                                                            | As with Delivery but deferred until later                                                                                              |
| Defer : k : Withdraw : q : t       | k : key to refer to later                                                            | As with Withdraw but deferred until later                                                                                              |
| Defer : k : Transfer : q : t : dest | k : key to refer to later                                                            | As with Transfer but deferred until later                                                                                              |
| Execute : k                    | k : key for deferred tasks to be executed                                            | Carry out all deferred orders with key k                                                                                               |
