#include <ds4_library.h>
#include <fcntl.h>
#include <libudev.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>



static struct udev* udev_ctx;



void _ds4_deinit(const ds4_device_t* device){
	close((int)(unsigned long long int)(device->_handle));
}



void* _ds4_init(const ds4_raw_device_t* raw){
	int handle=open(*raw,O_RDWR|O_NONBLOCK);
	return (handle==-1?NULL:(void*)(unsigned long long int)handle);
}



_Bool _ds4_recv_data(ds4_device_t* device){
	return (read((int)(unsigned long long int)(device->_handle),device->_in_buffer,64)==64);
}



void _ds4_send_data(const ds4_device_t* device){
	if (write((int)(unsigned long long int)(device->_handle),device->_out_buffer,32)!=32){
		ASSERT(!"Device error");
	}
}



void ds4_deinit(void){
	udev_unref(udev_ctx);
}



ds4_raw_device_t* ds4_enumerate_usb(ds4_raw_device_count_t* count){
	ds4_raw_device_t* out=NULL;
	ds4_raw_device_count_t length=0;
	struct udev_enumerate* dev_list=udev_enumerate_new(udev_ctx);
	udev_enumerate_add_match_subsystem(dev_list,"hidraw");
	udev_enumerate_scan_devices(dev_list);
	struct udev_list_entry* entry=udev_enumerate_get_list_entry(dev_list);
	while (entry){
		struct udev_device* dev=udev_device_new_from_syspath(udev_ctx,udev_list_entry_get_name(entry));
		struct udev_device* parent=udev_device_get_parent(dev);
		if (!parent||strcmp(udev_device_get_subsystem(parent),"hid")){
			goto _cleanup_loop;
		}
		const char* nm=udev_device_get_property_value(parent,"HID_NAME");
		if (!nm||(strcmp(nm,"Sony Interactive Entertainment Wireless Controller")&&strcmp(nm,"Sony Computer Entertainment Wireless Controller"))){
			goto _cleanup_loop;
		}
		const char* path=udev_device_get_devnode(dev);
		if (path){
			length++;
			out=realloc(out,length*sizeof(ds4_raw_device_t));
			size_t path_len=strlen(path)+1;
			*(out+length-1)=malloc(path_len*sizeof(char));
			memcpy((void*)out[length-1],path,path_len);
		}
_cleanup_loop:
		udev_device_unref(dev);
		entry=udev_list_entry_get_next(entry);
	}
	udev_enumerate_unref(dev_list);
	*count=length;
	return out;
}



void ds4_init(void){
	udev_ctx=udev_new();
}
