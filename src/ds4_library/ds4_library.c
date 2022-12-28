#include <ds4_library.h>
#include <stdlib.h>



#define ACCELERATION_FACTOR 0.0011733020388230986f
#define ANGULAR_VELOCITY_FACTOR 0.0010652969463144809f



_Bool ds4_connect_device(const ds4_raw_device_t* raw,ds4_device_t* out){
	void* handle=_ds4_init(raw);
	if (!handle){
		return 0;
	}
	out->buttons=0;
	out->l2=0;
	out->r2=0;
	out->lx=0;
	out->ly=0;
	out->rx=0;
	out->ry=0;
	out->timestamp=0;
	out->battery=0;
	out->avel_x=0;
	out->avel_y=0;
	out->avel_z=0;
	out->acc_x=0;
	out->acc_y=0;
	out->acc_z=0;
	out->touch[0].x=0;
	out->touch[0].y=0;
	out->touch[0].id=0;
	out->touch[1].x=0;
	out->touch[1].y=0;
	out->touch[1].id=0;
	out->color=0;
	out->led_on=0;
	out->led_off=0;
	out->rumble_small=0;
	out->rumble_big=0;
	out->_last_config=0;
	out->_handle=handle;
	return 1;
}



void ds4_disconnect_device(ds4_device_t* device){
	device->_out_buffer[0]=0x05;
	device->_out_buffer[1]=0x7f;
	device->_out_buffer[2]=0x04;
	device->_out_buffer[4]=0;
	device->_out_buffer[5]=0;
	device->_out_buffer[6]=0;
	device->_out_buffer[7]=0;
	device->_out_buffer[8]=0;
	device->_out_buffer[9]=0;
	device->_out_buffer[10]=0;
	_ds4_send_data(device);
	_ds4_deinit(device);
}



void ds4_free_enumeration(ds4_raw_device_t* raw,ds4_raw_device_count_t count){
	while (count){
		count--;
		free((void*)(raw[count]));
	}
	free(raw);
}



void ds4_send_config(ds4_device_t* device){
	uint64_t config=(device->color&0xffffff)|(((uint64_t)(device->led_on))<<24)|(((uint64_t)(device->led_off))<<32)|(((uint64_t)(device->rumble_small))<<40)|(((uint64_t)(device->rumble_big))<<48);
	if (config==device->_last_config){
		return;
	}
	device->_last_config=config;
	device->_out_buffer[0]=0x05;
	device->_out_buffer[1]=0x7f;
	device->_out_buffer[2]=0x04;
	device->_out_buffer[4]=device->rumble_small;
	device->_out_buffer[5]=device->rumble_big;
	device->_out_buffer[6]=device->color&0xff;
	device->_out_buffer[7]=(device->color>>8)&0xff;
	device->_out_buffer[8]=device->color>>16;
	device->_out_buffer[9]=device->led_on;
	device->_out_buffer[10]=device->led_off;
	_ds4_send_data(device);
}



void ds4_update(ds4_device_t* device){
	if (!_ds4_recv_data(device)){
		return;
	}
	device->buttons=0;
	uint8_t dp=device->_in_buffer[5]&0x0f;
	if (dp==0||dp==1||dp==7){
		device->buttons|=DS4_BUTTON_UP;
	}
	if (dp>=3&&dp<=5){
		device->buttons|=DS4_BUTTON_DOWN;
	}
	if (dp>=5&&dp<=7){
		device->buttons|=DS4_BUTTON_LEFT;
	}
	if (dp>=1&&dp<=3){
		device->buttons|=DS4_BUTTON_RIGHT;
	}
	if (device->_in_buffer[5]&32){
		device->buttons|=DS4_BUTTON_CROSS;
	}
	if (device->_in_buffer[5]&64){
		device->buttons|=DS4_BUTTON_CIRCLE;
	}
	if (device->_in_buffer[5]&16){
		device->buttons|=DS4_BUTTON_SQURARE;
	}
	if (device->_in_buffer[5]&128){
		device->buttons|=DS4_BUTTON_TRIANGLE;
	}
	if (device->_in_buffer[6]&1){
		device->buttons|=DS4_BUTTON_L1;
	}
	if (device->_in_buffer[6]&2){
		device->buttons|=DS4_BUTTON_R1;
	}
	if (device->_in_buffer[6]&4){
		device->buttons|=DS4_BUTTON_L2;
	}
	if (device->_in_buffer[6]&8){
		device->buttons|=DS4_BUTTON_R2;
	}
	if (device->_in_buffer[6]&16){
		device->buttons|=DS4_BUTTON_SHARE;
	}
	if (device->_in_buffer[6]&32){
		device->buttons|=DS4_BUTTON_OPTIONS;
	}
	if (device->_in_buffer[6]&64){
		device->buttons|=DS4_BUTTON_L3;
	}
	if (device->_in_buffer[6]&128){
		device->buttons|=DS4_BUTTON_R3;
	}
	if (device->_in_buffer[7]&1){
		device->buttons|=DS4_BUTTON_LOGO;
	}
	if (device->_in_buffer[7]&2){
		device->buttons|=DS4_BUTTON_TOUCHPAD;
	}
	device->lx=-128+device->_in_buffer[1];
	device->ly=127-device->_in_buffer[2];
	device->rx=-128+device->_in_buffer[3];
	device->ry=127-device->_in_buffer[4];
	device->l2=device->_in_buffer[8];
	device->r2=device->_in_buffer[9];
	device->timestamp=device->_in_buffer[7]>>2;
	device->battery=((device->_in_buffer[30]&0xf)==11?0:device->_in_buffer[30]<<5);
	device->avel_x=((float)(int16_t)(device->_in_buffer[13]|(device->_in_buffer[14]<<8)))*-ANGULAR_VELOCITY_FACTOR;
	device->avel_y=((float)(int16_t)(device->_in_buffer[15]|(device->_in_buffer[16]<<8)))*ANGULAR_VELOCITY_FACTOR;
	device->avel_z=((float)(int16_t)(device->_in_buffer[17]|(device->_in_buffer[18]<<8)))*ANGULAR_VELOCITY_FACTOR;
	device->acc_x=((float)(int16_t)(device->_in_buffer[19]|(device->_in_buffer[20]<<8)))*ACCELERATION_FACTOR;
	device->acc_y=((float)(int16_t)(device->_in_buffer[21]|(device->_in_buffer[22]<<8)))*ACCELERATION_FACTOR;
	device->acc_z=((float)(int16_t)(device->_in_buffer[23]|(device->_in_buffer[24]<<8)))*ACCELERATION_FACTOR;
	device->touch[0].x=device->_in_buffer[36]|((device->_in_buffer[37]&0x0f)<<8);
	device->touch[0].y=((device->_in_buffer[37]&0xf0)>>4)|(device->_in_buffer[38]<<4);
	device->touch[0].id=(device->_in_buffer[35]>>7?DS4_NO_DATA:device->_in_buffer[35]&0x7f);
	device->touch[1].x=device->_in_buffer[40]|((device->_in_buffer[41]&0x0f)<<8);
	device->touch[1].y=((device->_in_buffer[41]&0xf0)>>4)|(device->_in_buffer[42]<<4);
	device->touch[1].id=(device->_in_buffer[39]>>7?DS4_NO_DATA:device->_in_buffer[39]&0x7f);
}
