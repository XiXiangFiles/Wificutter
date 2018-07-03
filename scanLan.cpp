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

class infoHost{
	private:
		char src_mac[6];
		char srcIP[4];
		char srcIP6[16];

		void setHostmac(char **interface){
			struct ifreq ifr;
			memset(&ifr,0,sizeof(ifr));
			memcpy(ifr.ifr_name,*interface,strlen(*interface));	
			if(ioctl(this->send,SIOCGIFHWADDR ,&ifr)<0){
				perror("Failed to get scr_mac");
				exit(0);
			}
			memcpy(this->src_mac,ifr.ifr_hwaddr.sa_data,sizeof(ifr.ifr_hwaddr.sa_data));
		}	
		void setHostip( char *interface){
			struct 	ifaddrs *ifaddr,*ifaptr;
			getifaddrs(&ifaddr);
			ifaptr=ifaddr;
			while(ifaddr->ifa_next != NULL){
			
				if(ifaddr->ifa_addr->sa_family == AF_INET && strcmp(ifaddr->ifa_name,interface)==0){
				
					void *ptr;
					ptr=&((struct sockaddr_in *)ifaddr->ifa_addr)->sin_addr;
					memcpy(srcIP,ptr,4);
				
				}else if(ifaddr->ifa_addr->sa_family == AF_INET6 && strcmp(ifaddr->ifa_name,interface)==0){
				
					void *ptr;
					ptr=&((struct sockaddr_in6 *)ifaddr->ifa_addr)->sin6_addr;
					memcpy(srcIP6,(char* )ptr,16);
				
				}
				ifaddr=ifaddr->ifa_next;
			};
			freeifaddrs(ifaptr);
		}

	public : 	
		int send;
		infoHost(char *interface){
			if((send=socket(PF_PACKET,SOCK_RAW,htons(ETH_P_ALL)))<0){
				perror("Failed to open socket");
				exit(0);
			}
			memset(srcIP,0,4);
			memset(srcIP6,0,16);
			setHostmac(&interface);
			setHostip(interface);

		}
		~infoHost(){
			close(send);
		}
		char *getMac(){
			return src_mac;
		}
		char *getIP(){
			return srcIP;
		}
		char *getIP6(){
			return srcIP6;
		}
	
};
class packetARP:public infoHost{
	private:
		struct sockaddr_ll device;

		uint8_t ethernet2[1514];
	public:
		packetARP(char *interface,char *dstIP):infoHost(interface){
			memset(ethernet2,0,1514);
			char targetIP[4];
			for(int i=0 ; i<6 ;i++){
				ethernet2[i]=0xff;
			}
			memcpy(ethernet2+6,getMac(),6);
			ethernet2[12]=0x80;
			ethernet2[13]=0x06;
		//==================================== Hardware type
			ethernet2[14]=0x00;
			ethernet2[15]=0x01;
		//==================================== Protocol Type
			ethernet2[16]=0x08;
			ethernet2[17]=0x00;
		//==================================== hardware address length
			ethernet2[18]=0x06;
		//==================================== protocol address length
			ethernet2[19]=0x04;
		//==================================== opcode
			ethernet2[20]=0x01;
		//==================================== sender mac addr + ip & target mac + ip
			memcpy(ethernet2+20,getMac(),6);
			memcpy(ethernet2+26,getIP(), 4);
			if(inet_pton(AF_INET,dstIP,targetIP)!=1){
				perror("fail to convert ip");
				exit(1);
			}
			for(int i=0;i<6;i++){
				ethernet2[31+i]=0x00;
			}
			memcpy(ethernet2+36,targetIP,4);
			


			device.sll_family=AF_PACKET;
			device.sll_ifindex=if_nametoindex(interface);
			memcpy (device.sll_addr, getMac(), 6 * sizeof (uint8_t));
			device.sll_halen = htons (6);

		}
		~packetARP(){
		
		}
		uint8_t *sendARPreq(){
			

			return 0;
		}
	
};

int main(void ){
	infoHost h("wlan0");
	h.getIP();
	h.getIP6();
	h.getMac();
	return 0;
}
