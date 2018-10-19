/* CS656-00N Group M1 - Akin Raju (ar2286) / Aneri Oza (ao288) / Anjo Sam Thomas (at672)/ Molly Correia (mwc34) / Nikhil Lalkota (nl269)/ Ritisha Vijay Thakar (rt383)*/
#include <sys/socket.h>							//For socket functions
#include <netinet/in.h> 						//Defines INET_ADDRSTRLEN macro as 16 & INET6_ADDRSTRLEN as 46
#include <sys/types.h>							
#include <arpa/inet.h>							//For inet functions
#include <stdio.h>
#include <string.h> 							//For memset function
#include <unistd.h> 							//For close function
#include <stdlib.h> 							//For exit function
#include <netdb.h>							//For getaddrinfo()
#include <ctype.h>
#define QUEUE_SIZE 50

void print_err(char *message){//Handles error and exits program 
	perror(message);
	exit(1);
}

void dns(int conn){ //Receive, process and reply DNS requests
  	float value=0,smallest=0;					
	char recv_buf[4096]="",send_buf[4096]="REQ ",iplist[20][INET6_ADDRSTRLEN]={""},ip[INET6_ADDRSTRLEN],*ptr,pingBuff[100],valueBuff[100];
	struct addrinfo *address_list, hints;
	int recvlen=0,i=0,j=0,getaddr_ret,d=0,in=0,k=0,postioin=0;;
	if ( ( recvlen=recv(conn,recv_buf,sizeof(recv_buf)-1,0) ) <0 )  print_err("Receive error");  //Receives DNS request from client
	recv_buf[strlen(recv_buf)-2]='\0';							     //Terminates string properly
	snprintf(send_buf,sizeof(send_buf),"REQ: %s\n",recv_buf);
	printf("\t%s ",send_buf);
	if((send(conn,send_buf,strlen(send_buf)+1,0)) <0)	 print_err("Error sending data");			
	
	memset(&valueBuff,0,sizeof(valueBuff));
	memset(&hints, 0,sizeof(hints));
	hints.ai_family = AF_UNSPEC;  					//Get IPv4 and IPv6 addresses
	hints.ai_socktype = 0; 						//Get any socket type
	hints.ai_protocol = 0;						//Get any protocol type
	
	if ( (getaddr_ret=getaddrinfo(recv_buf, NULL, &hints, &address_list) ) !=0){ //function call to return the list of IP's
		getaddr_ret==EAI_NONAME ? snprintf(send_buf,sizeof(send_buf),"NO IP ADDRESSES FOUND\n") : snprintf(send_buf,sizeof(send_buf),"%s\n",gai_strerror(getaddr_ret));
		if((send(conn,send_buf,strlen(send_buf)+1,0)) <0)	 print_err("Error sending data");	
		return;
	}
	for( ; address_list!=NULL ; address_list=address_list->ai_next,i++){//Sorting IPv4 and IPv6 address
		if (address_list->ai_family==AF_INET){
			inet_ntop( address_list->ai_family, &((struct sockaddr_in *)address_list->ai_addr)->sin_addr,ip,INET6_ADDRSTRLEN);
		}
		else if (address_list->ai_family==AF_INET6){
			inet_ntop( address_list->ai_family, &((struct sockaddr_in6 *)address_list->ai_addr)->sin6_addr,ip,INET6_ADDRSTRLEN);
		}
		strcat(iplist[i],ip);
	}
	freeaddrinfo(address_list);
	for (j=0;j<i;j++){
		snprintf(send_buf,sizeof(send_buf)," IP = %s\n",iplist[j]);
		if((send(conn,send_buf,strlen(send_buf)+1,0)) <0)	 print_err("Error sending data");	
	}
	//Prefered IP logic--  Algorithm: IP with the shortest Round Trip time is taken as prefered IP. 3 Pings per IP is made.
	for(j=0;j<i;j++){
		char command[100]="ping -c 1 ";
		if(strlen(iplist[j])<=16){
			strcat(command,iplist[j]);                               //The command is formulated
			FILE *OutBuffer;
			(OutBuffer = popen(command, "r"))== NULL?exit(1):NULL;   //The command is executed to get rtt
			in=0;
 			 while(in<2){
  				fgets(pingBuff, sizeof(pingBuff)-1, OutBuffer);
  				ptr=strstr(pingBuff,"time=");
  				if(ptr!=NULL){
				  d=0;
					  for (k=0; ptr[k]!= '\0'; k++) { 
 						if (isdigit(ptr[k]) != 0){       //To extract Digits from the Buffer
							valueBuff[d]=ptr[k];
							d=d+1;
						}
						else if(ptr[k]=='.'){
							valueBuff[d]=ptr[k];	//Adding the decimal point if rrt <100ms
							d=d+1;
						}
	 				 }
				}
  				in=in+1;
  			}
 			value=atof(valueBuff);
 			pclose(OutBuffer);
			snprintf(send_buf,sizeof(send_buf),"RRT :%f\n",value); //this will display the RTT value on the client display for cross checking
			if((send(conn,send_buf,strlen(send_buf)+1,0)) <0)	 print_err("Error sending data");
			if(smallest==0){
				smallest=value;
			}
			if(value<=smallest){
				postioin=j;
				smallest=value;
			}	
		}
	}
	snprintf(send_buf,sizeof(send_buf)," PREFERRED IP = %s\n",iplist[postioin]);
	if((send(conn,send_buf,strlen(send_buf)+1,0)) <0)	 print_err("Error sending data");		
}

int main(int argc, char **argv){
	int listener, conn, serverport, clientport, conn_counter = 0,sport,sockoptval=1;
	socklen_t clength, slenght;
	char caddr_string[INET_ADDRSTRLEN],saddr_string[INET_ADDRSTRLEN];
	struct sockaddr_in serveraddr, clientaddr, sock;
	
	if (argc!=2){
		printf("Please pass a port number for the server as argument.\nUsage: ./dns <port number>\nExiting..\n");  exit(1);
	}
	if( (serverport=atoi(argv[1])) > 65535){
		printf("Invalid port entered.\nExiting\n");  exit(1);
	} 	
	//Server Socket is created. AF_INET is the address family IPV4, SOCK_STREAM is for TCP connection, Last field represents protocol (IP is 0)   
	if ( (listener=socket(AF_INET, SOCK_STREAM,0)) < 0 )   print_err("Listen error");
	
	//Setting SO_REUSEADDR option in case of restarting server on same port.
	if ( setsockopt(listener,SOL_SOCKET, SO_REUSEADDR, &sockoptval, sizeof(int)) <0 ) print_err("setsockopt() error");
	
	memset (&serveraddr , 0 , sizeof(serveraddr));			// Sets the structure with 0s.
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);			// Accepts connection through any of the server IPs 
	serveraddr.sin_port = htons(serverport);			// The port number passed as argument is used to listen for connection.
	
	if ( bind(listener, (struct sockaddr *)&serveraddr , sizeof(serveraddr)) <0 )   print_err("Bind error");// To assign the address and port to the socket.
	// Listens on the port for client to connect. QUEUE_SIZE is the max number of clients that can be queued before their connection gets refused.
	if ( listen(listener,QUEUE_SIZE)<0 )	   print_err("Listen error"); 
	printf("DNS Server Listening on socket %d\n",serverport);
	
	while(1){
		memset (&clientaddr , 0 , sizeof(clientaddr));
		clength=sizeof(clientaddr);
		if ( (conn=accept( listener, (struct sockaddr *) &clientaddr , &clength )) <0 )   print_err("Connect error"); 
		// The accept function is used to establish a connection from the client.
		conn_counter++;
		
		inet_ntop(AF_INET, &(clientaddr.sin_addr), caddr_string, INET_ADDRSTRLEN); 	//Converts binary client IP to dot decimal format. 
		clientport=ntohs(clientaddr.sin_port); 						//Port number of client is converted to host byte order for printing.
		
		memset (&sock , 0 , sizeof(sock));
		slenght=sizeof(sock);
		getsockname(conn,(struct sockaddr *) &sock, &slenght);				//Obtains the server IP and port used. 
		inet_ntop(AF_INET, &(sock.sin_addr), saddr_string, INET_ADDRSTRLEN); 		//Converts binary client IP to dot decimal format.
		sport=ntohs(sock.sin_port); 							//Port number of server is converted to host byte order for printing. 

		printf("(%d) Incoming client connection from [%s:%d] to me [%s:%d]\n",conn_counter,caddr_string,clientport,saddr_string,sport);
	    dns(conn);
		close(conn); 									//The client connection is closed 
	}
}
