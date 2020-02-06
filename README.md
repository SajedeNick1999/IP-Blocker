#IP Blocker
this is very simple IP blocker that have two modes :
1.BlackList:Accept every packets without those their pair of source_ip:source_port is in the config.txt.
for using this mode you have to write "BlackList" in first line of config.txt.
2.WhiteList:Accept just packets that their pair of source_ip:source_port is in the config.txt.
for using this mode you have to write "WhiteList" in first line of config.txt.

there are two structure . one for network hook and one for char device.
user program read config file every 5 seconeds and then send it for char device and char device update list and type of work. network hook get every packet and check ip an port of source and do the aproprate work.
only one user can connect to module every time.

##Getting started
you can use below instructions to run this IP blocker

###Prerequisites
you have to install linux headers

####Installing
first for finding your linux headers version , open terminal and type this:

>sudo apt-cache search linux-headers-$(uname -r)

 and as result you see some thing like this :

>linux-headers-4.15.0-54-generic - Linux kernel headers for version 4.15.0 on 64 bit x86 SMP


and now you have toinstall it :

>sudo apt-get install linux-headers-4.15.0-54-generic

##Running
first write in config file like example.
then for compiling kernel module :

>make

then for adding to kernel and start of running:

>sudo insmod ip_blocker.ko

then for compiling user code:

>gcc user.c -o user.out

finaly for running user code :

>sudo ./user.out

you can see status of module by this :

>dmesg

shut down by this :

>sudo rmmod ip_blocker


##Author
**Sajede Nicknadaf 9637453**
