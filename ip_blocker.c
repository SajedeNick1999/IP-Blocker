#include <linux/kernel.h>
#include<linux/module.h>
#include<linux/netfilter.h>
#include<linux/netfilter_ipv4.h>
#include<linux/skbuff.h>
#include<linux/udp.h>
#include<linux/tcp.h>
#include<linux/icmp.h>
#include<linux/ip.h>
#include<linux/inet.h>
#include<linux/init.h>
#include<linux/device.h>
#include<linux/fs.h>
#include<linux/uaccess.h>


#define DEVICE_NAME "config_reader"
#define CLASS_NAME "config_reader_class"
#define LIST_CAPACITY 5


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sajede Nicknadaf");
MODULE_DESCRIPTION("A simple module for blocking some IPs.");
MODULE_VERSION("0.1");

struct sk_buff *sock_buff;
struct iphdr *ip_header;
struct tcphdr *tcp_header;
struct udphdr *udp_header;
unsigned int ret;
char myip[16];
char list[LIST_CAPACITY][25];
unsigned int port;
char addr_pair[25];
          ///< A version number to inform users
static int block_type=0;//0 is black list and 1 is white list
static int next_id=0;
static int i=0;

static int    majorNumber;                  ///< Stores the device number -- determined automatically
static char   message[25] = {0};           ///< Memory for the string that is passed from userspace
//static short  size_of_message;              ///< Used to remember the size of the string stored
static int    numberOpens = 0;              ///< Counts the number of times the device is opened
static struct class*  ebbcharClass  = NULL; ///< The device-driver class struct pointer
static struct device* ebbcharDevice = NULL; ///< The device-driver device struct pointer


static int     dev_open2(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);


unsigned int packet_hook(unsigned int hooknum, struct sk_buff *skb,
                        const struct net_device *in, const struct net_device *out,
                        int(*okfn)(struct sk_buff *));

static struct file_operations fops =
{
   .open = dev_open2,
   .write = dev_write,
   .release = dev_release,
};

static struct nf_hook_ops packet_drop __read_mostly = {
  .pf = NFPROTO_IPV4,
  .priority = NF_IP_PRI_FIRST,
  .hooknum = NF_INET_LOCAL_IN,
  .hook = (nf_hookfn *) packet_hook
};

static int __init packet_drop_init(void){
  printk(KERN_INFO "IP blocker loaded\n");
  ret = nf_register_net_hook(&init_net,&packet_drop);

  //printk(KERN_INFO "EBBChar: Initializing the EBBChar LKM\n");

  // Try to dynamically allocate a major number for the device -- more difficult but worth it
  majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
  if (majorNumber<0){
     printk(KERN_ALERT "IP blocker: failed to register a major number\n");
     return majorNumber;
  }
  printk(KERN_INFO "IP blocker: registered correctly with major number %d\n", majorNumber);

  // Register the device class
  ebbcharClass = class_create(THIS_MODULE, CLASS_NAME);
  if (IS_ERR(ebbcharClass)){                // Check for error and clean up if there is
     unregister_chrdev(majorNumber, DEVICE_NAME);
     printk(KERN_ALERT "Failed to register device class\n");
     return PTR_ERR(ebbcharClass);          // Correct way to return an error on a pointer
  }
  printk(KERN_INFO "IP blocker: device class registered correctly\n");

  // Register the device driver
  ebbcharDevice = device_create(ebbcharClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
  if (IS_ERR(ebbcharDevice)){               // Clean up if there is an error
     class_destroy(ebbcharClass);           // Repeated code but the alternative is goto statements
     unregister_chrdev(majorNumber, DEVICE_NAME);
     printk(KERN_ALERT "Failed to create the device\n");
     return PTR_ERR(ebbcharDevice);
  }
  printk(KERN_INFO "IP blocker: device class created correctly\n"); // Made it! device was initialized
  //return 0;
  printk(KERN_INFO "IP blocker: type of work is BlackList\n");
  if(ret)
    printk(KERN_INFO "FAILED\n");

  return ret;


}

static void __exit packet_drop_exit(void){
  printk(KERN_INFO "IP bloker unloaded\n");
  nf_unregister_net_hook(&init_net,&packet_drop);
  device_destroy(ebbcharClass, MKDEV(majorNumber, 0));     // remove the device
   class_unregister(ebbcharClass);                          // unregister the device class
   class_destroy(ebbcharClass);                             // remove the device class
   unregister_chrdev(majorNumber, DEVICE_NAME);             // unregister the major number
   printk(KERN_INFO "IP blocker: Goodbye from the LKM!\n");
}


static int dev_open2(struct inode *inodep, struct file *filep){
   if(numberOpens>=1){
     dev_release(inodep,filep);
     return 1;
   }
   numberOpens++;
   printk(KERN_INFO "IP blocker: Device has been opened %d time(s)\n", numberOpens);
   return 0;
}


static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){

   copy_from_user(message, buffer, len);
     // appending received string with its length

   if(!strcmp(message,"BlackList")||!strcmp(message,"BlackList\n")||!strcmp(message,"\nBlackList")){
     if(block_type!=0){
        printk(KERN_INFO "IP blocker: type of work is %s\n", message);
        block_type=0;//black list
        next_id=0;
    }
  }
  else if(!strcmp(message,"WhiteList")||!strcmp(message,"WhiteList\n")||!strcmp(message,"\nWhiteList")){
    if(block_type!=1){
       printk(KERN_INFO "IP blocker: type of work is %s\n", message);
       block_type=1;//black list
       next_id=0;
   }
  }

  else{
    for(i=0;i<next_id;i++){
      if(!strcmp(list[i],message))
        return len;
    }
    //if(strcmp(list[next_id],message)){
      sprintf(list[next_id],message,len);
      next_id++;
      printk(KERN_INFO "IP blocker:new to list: %s\n",message);
    //}
  }

   return len;
}


static int dev_release(struct inode *inodep, struct file *filep){
   printk(KERN_INFO "IP blocker: Device successfully closed\n");
   numberOpens--;
   return 0;
}


unsigned int packet_hook(unsigned int hooknum, struct sk_buff *skb,
                        const struct net_device *in, const struct net_device *out,
                        int(*okfn)(struct sk_buff *)){

  sock_buff = skb;
  ip_header = (struct iphdr *)skb_network_header(sock_buff);
  if(!sock_buff)
    return NF_DROP;




  snprintf(myip,16,"%pI4",&ip_header->saddr);
  //printk(KERN_INFO "%s\n",myip);
  if(ip_header->protocol==IPPROTO_UDP)
  {

    udp_header=(struct udphdr *)skb_transport_header(sock_buff);
    port=(unsigned int)ntohs( udp_header->source);
    printk(KERN_INFO "UDP - %s:%d\n",myip,port);
  }
  else if(ip_header->protocol==IPPROTO_TCP)
  {

    tcp_header=(struct tcphdr *)skb_transport_header(sock_buff);
    port=(unsigned int)ntohs( tcp_header->source);
    printk(KERN_INFO "TCP - %s:%d\n",myip,port);
  }
  else{
    return NF_ACCEPT;
  }

    sprintf(addr_pair,"%s:%d\n",myip,port);
    if(block_type==0){//BlackList
      //printk(KERN_INFO "I am in BlackList\n");
      for(i=0;i<next_id;i++){
        //printk(KERN_INFO "addr_pair = %s  &  list = %s\n",addr_pair,list[i]);
        //printk(KERN_INFO "addr_pair = %d\n",strcmp(list[i],addr_pair));
        if(!strcmp(list[i],addr_pair)){
          printk(KERN_INFO "DROP : %s\n",addr_pair);
          return NF_DROP;
        }
      }
      return NF_ACCEPT;
    }
    else{
      //printk(KERN_INFO "I am in whiteList\n");
      for(i=0;i<next_id;i++){
        //printk(KERN_INFO "addr_pair = %s  &  list = %s\n",addr_pair,list[i]);
        if(!strcmp(list[i],addr_pair)){
          printk(KERN_INFO "ACCEPT : %s\n",addr_pair);
          return NF_ACCEPT;
        }
      }
      return NF_DROP;
    }

  //return NF_ACCEPT;
}

module_init(packet_drop_init);
module_exit(packet_drop_exit);
