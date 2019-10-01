#include <iostream>
#include <libusb.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <linux/uinput.h>
using namespace std;
#define BULK_EP_IN	0X81

void emit(int fd, int type, int code, int value){
   struct input_event ie;
   ie.type =type;
   ie.code= code;
   ie.value =value;
cout<<"here in emit "<<fd <<" " <<"type "<<type<<"code "<<code <<" value "<<value<<endl;
   /* timestamp values below are ignored */
   ie.time.tv_sec = 0;
   ie.time.tv_usec = 0;
   write(fd, &ie, sizeof(ie));
}

int main() {
 //pointer to pointer of device, used to retrieve a list of devices
    libusb_device_handle *dev_handle; //a device handle
    libusb_context *ctx = NULL; //a libusb session
    int r; //for return values
    ssize_t cnt; //holding number of devices in list
    r = libusb_init(&ctx); //initialize the library for the session we just declared
    if(r < 0) {
        cout<<"Init Error "<<r<<endl; //there was an error
        return 1;
    }

    libusb_set_debug(ctx, 3); //set verbosity level to 3, as suggested in the documentation
//    cnt = libusb_get_device_list(ctx, &devs); //get the list of devices
    if(cnt < 0) {
        cout<<"Get Device Error"<<endl; //there was an error
        return 1;

    }

   // cout<<cnt<<" Devices in list."<<endl;
   //find out if this needs to be a variable 
    dev_handle = libusb_open_device_with_vid_pid(ctx, 0x9A7A, 0xBA17); //these are vendorID and productID I found for my usb device

    if(dev_handle == NULL)

        cout<<"Cannot open device"<<endl;

    else

        cout<<"Device Opened"<<endl;


//    int actual; //used to find out how many bytes were written	
//	cout<<"actual number of bytes written: "<<actual<<endl;
    if(libusb_kernel_driver_active(dev_handle, 0) == 1) { //find out if kernel driver is attached
        cout<<"Kernel Driver Active"<<endl;
}
    r = libusb_claim_interface(dev_handle, 0); //claim interface 0 (the first) of device (mine had jsut 1)

    if(r < 0) {
     cout<<"Cannot Claim Interface"<<endl;
        return 1;
    }

    cout<<"Claimed Interface"<<endl;
unsigned char * data= new unsigned char [2];

data[1]='\0';
int actual_length;
//int rr = libusb_bulk_transfer(dev_handle, LIBUSB_ENDPOINT_IN, data, sizeof(data), &actual_length, 0);
//if (rr == 0 && actual_length == sizeof(data)) {
    // results of the transaction can now be found in the data buffer
    // parse them here and report button press
	//cout<<"here: "<<endl;	
//}
//unsigned char endp =0x51;
int bytestoRead =1;
int running =1;
int x;
int y;
int button =0;
struct uinput_setup usetup;
//char * device="/dev/input/js0";
int fd =open("/dev/uinput/js0", O_WRONLY | O_NONBLOCK); 
cout<<fd<<endl;
 ioctl(fd, UI_SET_EVBIT, EV_KEY);
 ioctl(fd, UI_SET_KEYBIT, KEY_SPACE);
 ioctl(fd, UI_SET_KEYBIT, BTN_B);
 ioctl(fd, UI_SET_KEYBIT, BTN_A);
   memset(&usetup, 0, sizeof(usetup));
   usetup.id.bustype = BUS_USB;
   usetup.id.vendor = 0x9A7A; /* sample vendor */
   usetup.id.product = 0xBA17; /* sample product */
   strcpy(usetup.name, "js0");

ioctl(fd, UI_DEV_SETUP, &usetup);
ioctl(fd, UI_DEV_CREATE);
sleep(1);
while (running) {
cout<<"here"<<endl;
int a =libusb_bulk_transfer(dev_handle,BULK_EP_IN,data,bytestoRead,&bytestoRead,0 );
//cout<<"a is "<<a<<endl;
printf("%i", data[0]);
cout<<"\n"<<endl;
//printf("\nvalue:%i", (*data)[0]);
button =data[0] >>4;
x = data[0]>>2;

if(x==1) {
x= x*-1;
cout<<"here"<<endl;
}
if (button==1 && x==6) {
x=2;
}
if(button==1 && x==7){
x=3;
}

if(button==1 &&x==5){
x=1;
}
y=(data[0]&3);
if(y==3){
//y=y*-1;
//cout<<" here y "<<endl;
}
cout<<"button is "<<button<<endl;
cout<<"y is "<<y<<endl;
cout<<"x is "<<x<<endl;
cout<<&data<<endl;
//int val =atoi(data[0]);
emit(fd,EV_KEY,KEY_SPACE,1);
emit(fd, EV_SYN, SYN_REPORT,0);
emit(fd, EV_KEY,ABS_X,1);
emit(fd,EV_SYN, SYN_REPORT,0);
emit(fd,EV_KEY, ABS_Y,1);
emit(fd,EV_SYN,SYN_REPORT,0);
if(a!=0){
running=0;
}

} 
//get x y and button
   //just to see the data we want to write : abcd
    //r = libusb_bulk_transfer(dev_handle, (2 | LIBUSB_ENDPOINT_OUT), data, 4, &actual, 0); //my device's out endpoint was 2, found with trial- the device had 2 endpoints: 2 and 129
    if(r == 0) //we wrote the 4 bytes successfully
       cout<<"Writing Successful!"<<endl;

//    else

       // cout<<"Write Error"<<endl;
   // r = libusb_release_interface(dev_handle, 0); //release the claimed interface
    if(r!=0) {
        cout<<"Cannot Release Interface"<<endl;
        return 1;
    }
    cout<<"Released Interface"<<endl;
    libusb_close(dev_handle); //close the device we opened
    libusb_exit(ctx); //needs to be called to end the
//    delete[] data; //delete the allocated memory for data

    return 0;

}

