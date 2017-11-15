#include<stdio.h> //printf
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include<arpa/inet.h>
#include<sys/socket.h>
#include<stdlib.h> 
#include<wiringPi.h>
#include<stdio.h>

#define BUFLEN 512  //Max length of buffer
#define PORT 8888   //The port on which to listen for incoming data
 
void die(char *s)
{
    perror(s);
    exit(1);
}
 
int main(void)
{
    struct sockaddr_in si_me, si_other;
     
    int s, i, slen = sizeof(si_other) , recv_len;
    char buf[BUFLEN];
    char passwordDownstairs[] = "Putt-password-here\n";
    char passwordUpstrairs[] = "Putt-other-password-here\n";

    wiringPiSetup () ;
    pinMode (2, OUTPUT) ;
    digitalWrite (2,  HIGH) ;
    pinMode (3, OUTPUT) ;
    digitalWrite (3,  HIGH) ;
    pinMode (4, OUTPUT) ;
    digitalWrite (4,  HIGH) ;


    //create a UDP socket
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
     
    // zero out the structure
    memset((char *) &si_me, 0, sizeof(si_me));
     
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(PORT);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
     
    //bind socket to port
    if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
    {
        die("bind");
    }
     
    //keep listening for data
    while(1)
    {
        printf("Waiting for data...");
        fflush(stdout);
	memset(buf,0,strlen(buf)); 

        //try to receive some data, this is a blocking call
        if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1)
        {
            die("recvfrom()");
        }
         
        //print details of the client/peer and the data received
        printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        printf("Data: %s\n" , buf);

	if(strstr(buf,passwordDownstairs) != 0) {
		printf("Opening downstairs\n");
		digitalWrite (2, LOW) ;
		delay (5000) ;
		digitalWrite (2,  HIGH) ;
	}

	if(strstr(buf,passwordUpstrairs) != 0) {
                printf("Opening upstairs\n");

		//switch direction
		digitalWrite (4, LOW) ;

		//power motor
                digitalWrite (3, LOW) ;
                delay (3550) ;
                digitalWrite (3,  HIGH) ;

		//switch direction back
		digitalWrite (4,  HIGH) ;
		delay (1000) ;

		//power motor again
                digitalWrite (3, LOW) ;
                delay (1950) ;
                digitalWrite (3,  HIGH) ;
        }
    }
 
    close(s);
    return 0;
}
