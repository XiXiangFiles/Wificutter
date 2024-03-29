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
#include<string.h>
#include<fstream>
#include<pthread.h>


using namespace std;

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
		uint8_t ethernet2[43];
		char dstip[4];

	public:
		packetARP(char *interface,char *dstIP):infoHost(interface){
			memset(ethernet2,0,40);
			char targetIP[4];
			for(int i=0 ; i<6 ;i++){
				ethernet2[i]=0xff;
			}
			memcpy(ethernet2+6,getMac(),6);
			ethernet2[12]=0x08;
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
			ethernet2[20]=0x00;
			ethernet2[21]=0x01;
		//==================================== sender mac addr + ip & target mac + ip
			memcpy(ethernet2+22,getMac(),6);
			memcpy(ethernet2+28,getIP(), 4);
			
			if(inet_pton(AF_INET,dstIP,targetIP)!=1){
				perror("fail to convert ip");
				exit(1);
			}

			for(int i=0;i<6;i++){
				ethernet2[33+i]=0x00;
			}

			memcpy(ethernet2+38,targetIP,4);
			memcpy(this->dstip,targetIP,4);
		
			device.sll_family=AF_PACKET;
			device.sll_ifindex=if_nametoindex(interface);
			memcpy (device.sll_addr, getMac(), 6 * sizeof (uint8_t));
			device.sll_halen = htons (6);

		}

		~packetARP(){
			close(send);	
		}

		void sendARPreq(){
			int bytes;
			char str[16];

			if ((bytes = sendto (send, ethernet2,sizeof(ethernet2), 0, (struct sockaddr *) &device, sizeof (device))) <= 0) {
			       	perror ("sendto() failed");
			        exit (1);
			 }
			strcpy(str,this->dstip);
		}

		void printREPLY(uint8_t *buffer,int size){
			for(int i=0;i<size;i++){
				printf("%x",buffer[i]);
			}
			printf("\n");
		
		}
	
};
void *recvARPreply(void *data){
	char *dstip=(char *)data;
	
	struct sockaddr_ll from;
	unsigned long diff;
	char checkMac[6];
	char checkIP[4];
	char checkSrcIP[4];
	int receive;				
	uint8_t recvEther2[60];
	
	char *filename="ArpList.txt";
	ofstream fout(filename,ios::out| ios::trunc);
	
	if(!fout.is_open()){
		perror("failed to open the file");
		pthread_exit(NULL);
	}
	
	socklen_t fromlen=sizeof(from);
	if((receive=socket(PF_PACKET,SOCK_RAW,htons(ETH_P_ALL)))<0)
		perror("failed to creat recvSocket ");

	while(true){
	
		memset(recvEther2,0,60);
		if((recvfrom(receive,recvEther2,60,0,(struct sockaddr *)&from,&fromlen))<0)
			perror("failed on recvfrom");
				
		memcpy(checkSrcIP,recvEther2+28,4);
		memcpy(checkMac,recvEther2+33,6);
		memcpy(checkIP,recvEther2+38,4);
		
		if(strcmp(checkSrcIP ,dstip)&& recvEther2[12]==0x08 && recvEther2[13]==0x06 && recvEther2[21]==0x02 ){
				
			char senderMac[6];
			char senderIP[4];
			char senderIPASC[INET_ADDRSTRLEN];
			char senderMacASC[18];

			char *c="\t";
			char *e=";\n";
			memcpy(senderMac,recvEther2+22,6);
			memcpy(senderIP,recvEther2+28,4);
			inet_ntop(AF_INET,senderIP,senderIPASC,INET_ADDRSTRLEN);
			memset(senderMacASC,0,18);	
			for(int i=0;i<6;i++){
				sprintf(senderMacASC+strlen(senderMacASC),"%x",(u_int8_t)senderMac[i]);
				if(i!=5){
					senderMacASC[strlen(senderMacASC)]=':';
				}
			}
			
			printf("%s\n",senderIPASC);
			printf("%s\n",senderMacASC);
			
			fout << senderIPASC <<","<<senderMacASC<<"\n"; 
		}
		
	}		
	//printf("%s done \n",dstip);
	fout.close();
	close(receive);
	//pthread_exit(NULL);
}

int main(int argc, char* argv[]){
	char *interface;
	if (argc < 3) {
		printf("Please input interface. ex: -I enp3s0");
		return 0;
	} else {
		if (strcmp(argv[1], "-I")){
			printf("unknown '%s' operator \n", argv[1]);
			return 0;
		}
		interface =  argv[2];
	}
	char local[15];
	char local2[15];
	char *p;
	char scanIP[15];
	int temp=0;
	int err;
	int countstr=0;
	pthread_t t;
	
	infoHost h(interface);
	inet_ntop(AF_INET,h.getIP(),local,INET_ADDRSTRLEN);
	inet_ntop(AF_INET,h.getIP(),local2,INET_ADDRSTRLEN);
	p=strtok(local,".");
		for(int i=0 ; i<4 ;i++){
		temp+=strlen(p);
		//printf("strlen= %d\n", strlen(p));
		if(i==2){
			countstr=temp;
			countstr+=3;
		}
		p=strtok(NULL,".");
	}
		
	memcpy(scanIP,local2,countstr);
	if(err=pthread_create(&t,NULL,recvARPreply,(void * )scanIP)!= 0 )
		perror("failed to create thread");

	for(int i=1;i<256;i++){
		char str[3];
		void *res;
		int err;

		sprintf(scanIP+countstr,"%d",i);
		//printf("send to %s \n",scanIP);
		
		packetARP s(interface,scanIP);
		s.sendARPreq();		
	}

	sleep(1);
	pthread_cancel(t);

	return 0;
}
