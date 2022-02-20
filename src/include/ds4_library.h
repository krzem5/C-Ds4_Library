#ifndef __DS4_LIBRARY_H__
#define __DS4_LIBRARY_H__ 1
#ifdef _MSC_VER
#include <windows.h>
#endif
#include <signal.h>
#include <stdint.h>
#include <stdio.h>



#define ASSERT(x) \
	do{ \
		if (!(x)){ \
			printf("%s:%u (%s): %s: Assertion Failed\n",__FILE__,__LINE__,__func__,#x); \
			raise(SIGABRT); \
		} \
	}while (0)



#define DS4_BUTTON_UP 0x00001
#define DS4_BUTTON_DOWN 0x00002
#define DS4_BUTTON_LEFT 0x00004
#define DS4_BUTTON_RIGHT 0x00008
#define DS4_BUTTON_L1 0x00010
#define DS4_BUTTON_R1 0x00020
#define DS4_BUTTON_L2 0x00040
#define DS4_BUTTON_R2 0x00080
#define DS4_BUTTON_L3 0x00100
#define DS4_BUTTON_R3 0x00200
#define DS4_BUTTON_CROSS 0x00400
#define DS4_BUTTON_CIRCLE 0x00800
#define DS4_BUTTON_SQURARE 0x01000
#define DS4_BUTTON_TRIANGLE 0x02000
#define DS4_BUTTON_OPTIONS 0x04000
#define DS4_BUTTON_SHARE 0x08000
#define DS4_BUTTON_PS 0x10000
#define DS4_BUTTON_TOUCHPAD 0x20000

#define DS4_CONFIG_FORCE_UPDATE 0xffffffffffffffffull

#define DS4_MAKE_COLOR(r,g,b) (((r)&0xff)|(((g)&0xff)<<8)|(((b)&0xff)<<16))

#define DS4_NO_DATA 255



typedef uint32_t ds4_raw_device_count_t;



typedef struct __DS4_TOUCH_POINT{
	uint16_t x;
	uint16_t y;
	uint8_t id;
} ds4_touch_point_t;



typedef struct __DS4_DEVICE{
	uint32_t buttons;
	int8_t lx;
	int8_t ly;
	int8_t rx;
	int8_t ry;
	uint8_t l2;
	uint8_t r2;
	uint8_t timestamp;
	uint8_t battery;
	float acc_x;
	float acc_y;
	float acc_z;
	float avel_x;
	float avel_y;
	float avel_z;
	ds4_touch_point_t touch[2];
	uint32_t color;
	uint8_t led_on;
	uint8_t led_off;
	uint8_t rumble_small;
	uint8_t rumble_big;
	uint64_t _last_cfg;
	void* _fh;
	uint8_t _in_bf[64];
	uint8_t _out_bf[32];
} ds4_device_t;



typedef char* ds4_raw_device_t;



void _ds4_deinit(ds4_device_t* d);



void* _ds4_init(ds4_raw_device_t* p,ds4_device_t* d);



_Bool _ds4_recv_data(ds4_device_t* d);



void _ds4_send_data(ds4_device_t* d);



_Bool ds4_connect_device(ds4_raw_device_t* p,ds4_device_t* o);



void ds4_deinit(void);



void ds4_disconnect_device(ds4_device_t* o);



ds4_raw_device_t* ds4_enumerate_usb(ds4_raw_device_count_t* l);



void ds4_free_enumeration(ds4_raw_device_t* p,ds4_raw_device_count_t l);



void ds4_init(void);



void ds4_send_config(ds4_device_t* d);



void ds4_update(ds4_device_t* d);



#endif
