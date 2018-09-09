# Wificutter
There have two files in this project.The scanLan.cpp use to scan local LAN and it will save IP and Mac address in ArpList.txt.    
The other file can use it to send cut package to router.    

* ScanLan.cpp
````
g++ -lpthread scanLan.cpp -o out
sudo ./out
````

* fakereplyarp.cpp
````
g++ fakereplyarp.cpp socketlib/HostInfo.cpp -o out
sudo ./out
````
