#include <ds4_library.h>
#include <fcntl.h>
#include <libudev.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>



static struct udev* udev_ctx;



void _ds4_deinit(ds4_device_t* d){
	close((int)(unsigned long long int)(d->_fh));
}



void* _ds4_init(ds4_raw_device_t* p,ds4_device_t* o){
	int h=open(*p,O_RDWR|O_NONBLOCK);
	return (h==-1?NULL:(void*)(unsigned long long int)h);
}



_Bool _ds4_recv_data(ds4_device_t* d){
	return (read((int)(unsigned long long int)(d->_fh),d->_in_bf,64)==64);
}



void _ds4_send_data(ds4_device_t* d){
	ssize_t sz=write((int)(unsigned long long int)(d->_fh),d->_out_bf,32);
	if (sz!=32){
		ASSERT(!"Device error");
	}
}



void ds4_deinit(void){
	udev_unref(udev_ctx);
}



ds4_raw_device_t* ds4_enumerate_usb(ds4_raw_device_count_t* l){
	ds4_raw_device_t* o=NULL;
	ds4_raw_device_count_t ol=0;
	struct udev_enumerate* dev_list=udev_enumerate_new(udev_ctx);
	udev_enumerate_add_match_subsystem(dev_list,"hidraw");
	udev_enumerate_scan_devices(dev_list);
	struct udev_list_entry* entry=udev_enumerate_get_list_entry(dev_list);
	while (entry){
		struct udev_device* dev=udev_device_new_from_syspath(udev_ctx,udev_list_entry_get_name(entry));
		struct udev_device* p=udev_device_get_parent(dev);
		if (!p||strcmp(udev_device_get_subsystem(p),"hid")){
			goto _cleanup_loop;
		}
		const char* nm=udev_device_get_property_value(p,"HID_NAME");
		if (!nm||(strcmp(nm,"Sony Interactive Entertainment Wireless Controller")&&strcmp(nm,"Sony Computer Entertainment Wireless Controller"))){
			goto _cleanup_loop;
		}
		const char* path=udev_device_get_devnode(dev);
		if (path){
			ol++;
			o=realloc(o,ol*sizeof(ds4_raw_device_t));
			size_t len=strlen(path)+1;
			*(o+ol-1)=malloc(len);
			memcpy(*(o+ol-1),path,len);
		}
_cleanup_loop:
		udev_device_unref(dev);
		entry=udev_list_entry_get_next(entry);
	}
	udev_enumerate_unref(dev_list);
	*l=ol;
	return o;
}



void ds4_init(void){
	udev_ctx=udev_new();
}
