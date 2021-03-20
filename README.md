# Wificutter
There have two files in this project.The scanLan.cpp use to scan local LAN and it will save IP and Mac address in ArpList.txt.    
The other file can use it to crash the ARP table.    

* Install
````shell
./compile.sh
````

* Run Scan
````shell
    sudo ./scan -I wlo1
````

* Run out
```shell
    sudo ./out -I wlo1 -dstIP 192.168.43.161  -dstMac 58:a0:23:a3:be:25 -fakeIP 192.168.43.160 -fakeMac 58:a0:23:a3:be:25 -N 100
```

