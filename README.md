# projects

Offline dictionary attack

A simple java code which reads the Linux style shadow file and uses some common passwords to brute force.
firstly, The program reads shadow.txt,common-passwords.txt file.
Then takes all the words in common-passwords file concatinate with respective salt of the user.
Convert them to the hash based on the algorithm(MD5).
Finally compares them with the salted hash of the user and prints them.

passwords are stored in common-passwords.txt
shadow.txt consists of usernames,salt,saltedhash,etc.






DNS Server

A simple dns server written in c language which accept requests from the clients and once after establishing a connection with the client.When the client requests the ip address of a particular domain this dns server serves client request by sending reply with the number of ip address associated to that domain along with the preferred ip.






