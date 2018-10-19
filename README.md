# projects

Offline dictionary attack

A simple java code which reads the Linux style shadow file and uses some common passwords to brute force.
firstly, The program reads shadow.txt,common-passwords.txt file.
Then takes all the words in common-passwords file concatinate with respective salt of the user.
Convert them to the hash based on the algorithm(MD5).
Finally compares them with the salted hash of the user and prints them.

passwords are stored in common-passwords.txt
shadow.txt consists of usernames,salt,saltedhash,etc.





