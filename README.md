# Wificutter
There have two files in this project.The scanLan.cpp use to scan local LAN and it will save IP and Mac address in ArpList.txt.    
The other file can use it to crash the ARP table.    

* Install
````shell
cmake . 
make
````

* ScanLan.cpp
    * modify the code    
````cpp
infoHost h("wlan0");  // at line 239, you can modify the Network interface
packetARP s("wlan0",scanIP); // at line 239, you can modify the Network interface
````

* fakereplyarp.cpp
    * ***modify the code***      
```cpp
arpreply s("wlan0",dstIP,dstMac,routerIP,routerMac);  // at line 151, you can modify the Network interface at first parameter.
```

