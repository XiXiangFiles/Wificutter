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

#define ARP 0x0806

class infoHost{
	private:
		int send;
		char src_mac[6];
		char src_IPv4[4];
		

	public : 
		infoHost(char *interface){
			if((send=socket(PF_PACKET,SOCK_RAW,htons(ETH_P_ALL)))<0){
				perror("Failed to open socket");
				exit(0);
			}
			getHostmac(&interface);
			getHostip();

		}
		~infoHost(){
		
		}
		infoHost(infoHost &host){
		
		}
	
		void getHostmac(char **interface){
			struct ifreq ifr;
			memset(&ifr,0,sizeof(ifr));
			memcpy(ifr.ifr_name,*interface,strlen(*interface));	
			if(ioctl(this->send,SIOCGIFHWADDR ,&ifr)<0){
				perror("Failed to get scr_mac");
				exit(0);
			}
			memcpy(this->src_mac,ifr.ifr_hwaddr.sa_data,6);
		}	
		void getHostip(){
			struct 	ifaddrs *ifaddr;
			getifaddrs(&ifaddr);
			while(ifaddr->ifa_next != NULL){
				struct 	ifaddrs *ptr;
				ptr=ifaddr;
				if(ifaddr->ifa_addr->sa_family == AF_INET){
					void *ptr;
					ptr=&((struct sockaddr_in *)ifaddr->ifa_addr)->sin_addr;
					
					printf("%s\n",ptr);
					//ifaddr->ifa_addr->sin_addr;
				}else if(ifaddr->ifa_addr->sa_family == AF_INET6){
				

				}
				ifaddr=ifaddr->ifa_next;
				freeifaddrs(ptr);
			};
		}
};


int main(void ){
	infoHost h("wlan0");
//	h.show();

	return 0;
}
