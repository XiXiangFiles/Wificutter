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
		uint8_t ethernet2[60];
		int send;
		char dstMAC[6];
		char srcMAC[6];
		char dstIP[4];
		char srcIP[4];

		uint8_t *convertMac(char *macaddr){
			uint8_t mac[6];
			
			return mac;	
		}
		uint8_t randomMac(){
			uint8_t mac[6];

			return mac;
		}
	
	public:
		arpreply(char *dstMac, char *dstIP ,char *srcMac ,char *srcIP){
			if(send=socket(PF_PACKET,SOCK_RAW,htons(ETH_P_ALL))<0){
				perror("failed to creat socket");
				exit(1);
			}
			if(inet_pton(AF_INET,dstIP,this->dstIP)!=-1){
				perror("failed to convery dstIP");
				exit(1);
			}
			if(inet_pton(AF_INET,srcIP,this->srcIP)!=-1){
				perror("failed to convert srcIP");
				exit(1);
			}

			memcpy(this->dstMAC,convertMac(dstMac),6);
			memcpy(this->srcMAC,convertMac(srcMac),6);
		}
		void sendreply(){
				
			memcpy(ethernet2,this->dstMAC,6);
			memcpy(ethernet2+6,this->srcMAC,6);
			ethernet2[12]=0x08;
			ethernet2[13]=0x06;

		}
	
	
};

int main(void ){
	


	return 0;
}
