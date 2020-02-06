# **IP Blocker**
this is very simple IP blocker that have two modes :
1.BlackList:Accept every packets without those their pair of source_ip:source_port is in the config.txt.
for using this mode you have to write "BlackList" in first line of config.txt.
2.WhiteList:Accept just packets that their pair of source_ip:source_port is in the config.txt.
for using this mode you have to write "WhiteList" in first line of config.txt.

there are two structure . one for network hook and one for char device.
user program read config file every 5 seconeds and then send it for char device and char device update list and type of work. network hook get every packet and check ip an port of source and do the aproprate work.
only one user can connect to module every time.

## **Getting started**
you can use below instructions to run this IP blocker

### **Prerequisites**
you have to install linux headers

#### **Installing**
first for finding your linux headers version , open terminal and type this:

```shell
$ sudo apt-cache search linux-headers-$(uname -r)
```

 and as result you see some thing like this :


```shell
$ linux-headers-4.15.0-54-generic - Linux kernel headers for version 4.15.0 on 64 bit x86 SMP
```

and now you have toinstall it :


```shell
$ sudo apt-get install linux-headers-4.15.0-54-generic
```

## **Running**
first write in config file like example.
then for compiling kernel module :


```shell
$ make
```

then for adding to kernel and start of running:


```shell
$ sudo insmod ip_blocker.ko
```

then for compiling user code:


```shell
$ gcc user.c -o user.out
```

finaly for running user code :

```shell
$ sudo ./user.out
```

you can see status of module by this :


```shell
$ dmesg
```

shut down by this :


```shell
$ sudo rmmod ip_blocker
```

## **Support**
Reach out to me at one of the following places!
- Telegram at <a href="https://t.me/sajede_nick" target="_blank">@sajede_nick</a>
- Gmail at <a href="mailto:sajede.nicknadaf78@gmail.com" target="_blank">sajede.nicknadaf78@gmail.com</a>

## **LICENSE**

[![License](https://img.shields.io/:license-mit-blue.svg?style=flat-square)](http://badges.mit-license.org)
