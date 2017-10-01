#include <stdio.h>
#include <stdlib.h>

#include <libusb-1.0/libusb.h>

void callbackUSBTransferComplete(struct libusb_transfer *xfr) {
    switch(xfr->status) {
        case LIBUSB_TRANSFER_COMPLETED:
            // Success here, data transfered are inside 
            // xfr->buffer
            // and the length is
            // xfr->actual_length
            printf("Data transfer succeeded!...\n");
            break;
        case LIBUSB_TRANSFER_CANCELLED:
        case LIBUSB_TRANSFER_NO_DEVICE:
        case LIBUSB_TRANSFER_TIMED_OUT:
        case LIBUSB_TRANSFER_ERROR:
        case LIBUSB_TRANSFER_STALL:
        case LIBUSB_TRANSFER_OVERFLOW:
            // Various type of errors here
            printf("ERROR %d in USBTransferComplete...\n", xfr->status);
            break;
    }
}

int main(int argc, char **argv) {

    // INITIALIZE LIBRARY & GET DEVICE HANDLE

    libusb_device_handle *dev_handle = NULL;
    libusb_device **list;
    size_t count, i;

    // Initialize library
    libusb_init(NULL);

    // Get list of USB devices currently connected
    count = libusb_get_device_list(NULL, &list);

    for(i = 0; i < count; i++) {
       struct libusb_device_descriptor desc;

       libusb_get_device_descriptor(list[i], &desc);

       // Is our device?
       if(desc.idVendor == 0x04b4 && desc.idProduct == 0x00f1) {
          // Open USB device and get handle
          libusb_open(list[i], &dev_handle);
          break;
       }
    }

    libusb_free_device_list(list, 1);

    if(dev_handle == NULL) {
        printf("Couldn't find a device with matching vend/prod...\n");
        return 0;
    }

    // ISSUE TRANSFER REQUEST

    unsigned char *data;
    const unsigned int size = 4096;
    unsigned int received = 0;

    data = malloc(size);

    while(1) {
        if( libusb_bulk_transfer(dev_handle,0x81,data,size,&received,1) == 0) {
            printf("Got %d bytes, data[0] is %x\n", received, ((unsigned int *)data)[0]);
        } else {
            printf("FailWhale\n");
        }
    }

    return 0;
}
