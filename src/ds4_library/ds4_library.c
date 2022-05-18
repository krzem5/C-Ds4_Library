#include <ds4_library.h>
#include <stdlib.h>



#define ACCELERATION_FACTOR 0.0011733020388230986f
#define ANGULAR_VELOCITY_FACTOR 0.0010652969463144809f



_Bool ds4_connect_device(ds4_raw_device_t* p,ds4_device_t* o){
	void* fh=_ds4_init(p,o);
	if (!fh){
		return 0;
	}
	o->buttons=0;
	o->l2=0;
	o->r2=0;
	o->lx=0;
	o->ly=0;
	o->rx=0;
	o->ry=0;
	o->timestamp=0;
	o->battery=0;
	o->acc_x=0;
	o->acc_y=0;
	o->acc_z=0;
	o->avel_x=0;
	o->avel_y=0;
	o->avel_z=0;
	o->touch[0].x=0;
	o->touch[0].y=0;
	o->touch[0].id=0;
	o->touch[1].x=0;
	o->touch[1].y=0;
	o->touch[1].id=0;
	o->color=0;
	o->led_on=0;
	o->led_off=0;
	o->rumble_small=0;
	o->rumble_big=0;
	o->_fh=fh;
	return 1;
}



void ds4_disconnect_device(ds4_device_t* d){
	d->_out_bf[0]=0x05;
	d->_out_bf[1]=0x7f;
	d->_out_bf[2]=0x04;
	d->_out_bf[4]=0;
	d->_out_bf[5]=0;
	d->_out_bf[6]=0;
	d->_out_bf[7]=0;
	d->_out_bf[8]=0;
	d->_out_bf[9]=0;
	d->_out_bf[10]=0;
	_ds4_send_data(d);
	_ds4_deinit(d);
}



void ds4_free_enumeration(ds4_raw_device_t* p,ds4_raw_device_count_t l){
	while (l){
		l--;
		free(*(p+l));
	}
	free(p);
}



void ds4_send_config(ds4_device_t* d){
	uint64_t cfg=(d->color&0xffffff)|(((uint64_t)(d->led_on))<<24)|(((uint64_t)(d->led_off))<<32)|(((uint64_t)(d->rumble_small))<<40)|(((uint64_t)(d->rumble_big))<<48);
	if (cfg==d->_last_cfg){
		return;
	}
	d->_last_cfg=cfg;
	d->_out_bf[0]=0x05;
	d->_out_bf[1]=0x7f;
	d->_out_bf[2]=0x04;
	d->_out_bf[4]=d->rumble_small;
	d->_out_bf[5]=d->rumble_big;
	d->_out_bf[6]=d->color&0xff;
	d->_out_bf[7]=(d->color>>8)&0xff;
	d->_out_bf[8]=d->color>>16;
	d->_out_bf[9]=d->led_on;
	d->_out_bf[10]=d->led_off;
	_ds4_send_data(d);
}



void ds4_update(ds4_device_t* d){
	if (!_ds4_recv_data(d)){
		return;
	}
	d->buttons=0;
	uint8_t dp=d->_in_bf[5]&0x0f;
	if (dp==0||dp==1||dp==7){
		d->buttons|=DS4_BUTTON_UP;
	}
	if (dp>=3&&dp<=5){
		d->buttons|=DS4_BUTTON_DOWN;
	}
	if (dp>=5&&dp<=7){
		d->buttons|=DS4_BUTTON_LEFT;
	}
	if (dp>=1&&dp<=3){
		d->buttons|=DS4_BUTTON_RIGHT;
	}
	if (d->_in_bf[5]&32){
		d->buttons|=DS4_BUTTON_CROSS;
	}
	if (d->_in_bf[5]&64){
		d->buttons|=DS4_BUTTON_CIRCLE;
	}
	if (d->_in_bf[5]&16){
		d->buttons|=DS4_BUTTON_SQURARE;
	}
	if (d->_in_bf[5]&128){
		d->buttons|=DS4_BUTTON_TRIANGLE;
	}
	if (d->_in_bf[6]&1){
		d->buttons|=DS4_BUTTON_L1;
	}
	if (d->_in_bf[6]&2){
		d->buttons|=DS4_BUTTON_R1;
	}
	if (d->_in_bf[6]&4){
		d->buttons|=DS4_BUTTON_L2;
	}
	if (d->_in_bf[6]&8){
		d->buttons|=DS4_BUTTON_R2;
	}
	if (d->_in_bf[6]&16){
		d->buttons|=DS4_BUTTON_SHARE;
	}
	if (d->_in_bf[6]&32){
		d->buttons|=DS4_BUTTON_OPTIONS;
	}
	if (d->_in_bf[6]&64){
		d->buttons|=DS4_BUTTON_L3;
	}
	if (d->_in_bf[6]&128){
		d->buttons|=DS4_BUTTON_R3;
	}
	if (d->_in_bf[7]&1){
		d->buttons|=DS4_BUTTON_LOGO;
	}
	if (d->_in_bf[7]&2){
		d->buttons|=DS4_BUTTON_TOUCHPAD;
	}
	d->lx=-128+d->_in_bf[1];
	d->ly=127-d->_in_bf[2];
	d->rx=-128+d->_in_bf[3];
	d->ry=127-d->_in_bf[4];
	d->l2=d->_in_bf[8];
	d->r2=d->_in_bf[9];
	d->timestamp=d->_in_bf[7]>>2;
	d->battery=((d->_in_bf[30]&0xf)==11?0:d->_in_bf[30]<<5);
	d->acc_x=((float)(int16_t)(d->_in_bf[13]|(d->_in_bf[14]<<8)))*ACCELERATION_FACTOR;
	d->acc_y=((float)(int16_t)(d->_in_bf[15]|(d->_in_bf[16]<<8)))*ACCELERATION_FACTOR;
	d->acc_z=((float)(int16_t)(d->_in_bf[17]|(d->_in_bf[18]<<8)))*ACCELERATION_FACTOR;
	d->avel_x=((float)(int16_t)(d->_in_bf[19]|(d->_in_bf[20]<<8)))*-ANGULAR_VELOCITY_FACTOR;
	d->avel_y=((float)(int16_t)(d->_in_bf[21]|(d->_in_bf[22]<<8)))*ANGULAR_VELOCITY_FACTOR;
	d->avel_z=((float)(int16_t)(d->_in_bf[23]|(d->_in_bf[24]<<8)))*ANGULAR_VELOCITY_FACTOR;
	d->touch[0].x=d->_in_bf[36]|((d->_in_bf[37]&0x0f)<<8);
	d->touch[0].y=((d->_in_bf[37]&0xf0)>>4)|(d->_in_bf[38]<<4);
	d->touch[0].id=(d->_in_bf[35]>>7?DS4_NO_DATA:d->_in_bf[35]&0x7f);
	d->touch[1].x=d->_in_bf[40]|((d->_in_bf[41]&0x0f)<<8);
	d->touch[1].y=((d->_in_bf[41]&0xf0)>>4)|(d->_in_bf[42]<<4);
	d->touch[1].id=(d->_in_bf[39]>>7?DS4_NO_DATA:d->_in_bf[39]&0x7f);
}
