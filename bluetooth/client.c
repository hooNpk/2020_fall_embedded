#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>>
#include <bluetooth/l2cap.h>
#include <bluetooth/rfcomm.h>

#define PACKET_LENGTH 0

int main(int argc, char** argv){
	
	/* this time we are not going to use BlueZ but instead use sockets
	   to establish connections in a point to point manner */

	struct sockaddr_rc addr = {0};
	int s, status;
	char* message = "hello, world!";
	char dest[10] = {0};

	/* require the server address to be specified explicitly on the command line */
	if(argc < 2){
		fprintf(stderr, "usage: %s <br-address>\n", argv[0]);
		return -1;
	}

	/*copy the bluetooth address string */
	strncpy(dest, argv[1], 10);

	/*allocate a socket */
	s = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);

	/* set connection parameters (who to connect to) */
	addr.rc_family = AF_BLUETOOTH;
	addr.rc_psm = htobs(0x1001);
	str2ba(dest, &addr.l2_bdaddr);

	/*now that we have a bluetooth address, try to connect*/



}
