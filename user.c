#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>

#define BUFFER_LENGTH 25               ///< The buffer length (crude but fine)
    ///< The receive buffer from the LKM

int main(){
   int ret, fd;
   char stringToSend[BUFFER_LENGTH];
   printf("Starting device test code example...\n");
   fd = open("/dev/config_reader", O_RDWR);             // Open the device with read/write access
   while (fd < 0){
      perror("Failed to open the device...");
      //return errno;
      fd = open("/dev/config_reader", O_RDWR);
   }
   FILE *fd2;

   while(1){
     sleep(5);
     fd2=fopen("config.txt","r");
     bzero(stringToSend,BUFFER_LENGTH);
     while(fgets(stringToSend,BUFFER_LENGTH,fd2)!=NULL){
       printf("%s\n",stringToSend);
       ret = write(fd, stringToSend, strlen(stringToSend)+1); // Send the string to the LKM
       if (ret < 0){
          perror("Failed to write the message to the device.");
          return errno;
       }
       bzero(stringToSend,BUFFER_LENGTH);
     }
     fclose(fd2);
   }
   // while(1){
   //   printf("Type in a short string to send to the kernel module:\n");
   //   bzero(stringToSend,BUFFER_LENGTH);
   //   scanf("%s", stringToSend);                // Read in a string (with spaces)
   //   printf("Writing message to the device [%s].\n", stringToSend);
   //   ret = write(fd, stringToSend, strlen(stringToSend)+1); // Send the string to the LKM
   //   if (ret < 0){
   //      perror("Failed to write the message to the device.");
   //      return errno;
   //   }
   // }


   return 0;
}
