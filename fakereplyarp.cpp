#include<iostream>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<netdb.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/ioctl.h>
#include<net/if.h>
#include<net/ethernet.h>
#include<ifaddrs.h>
#include<linux/if_packet.h>
#include<sys/time.h>
#include<string.h>
#include"Wang-C-C-lib-/socketlib/IOfile.cpp"

class arpreply{
	private:
		int send;
	
	public:
		arpreply(char *dstMac, char *dstIP ,char *srcMac ,char srcIP){
			
		}
	
};

int main(void ){
	IOfile f("scanLAN.txt");
//	f.fileread();

	//f.pre_order();	


	return 0;
}
