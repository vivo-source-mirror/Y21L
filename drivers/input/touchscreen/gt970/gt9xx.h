/* drivers/input/touchscreen/gt9xx.h
 * 
 * 2010 - 2013 Goodix Technology.
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be a reference 
 * to you, when you are integrating the GOODiX's CTP IC into your system, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
 * General Public License for more details.
 * 
 */

#ifndef _GOODIX_GT9XX_H_
#define _GOODIX_GT9XX_H_

#include <linux/kernel.h>
#include <linux/hrtimer.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
//#include <asm/uaccess.h>
#include <linux/uaccess.h>

#include <linux/vmalloc.h>
#include <linux/interrupt.h>
#include <linux/io.h>
//#include <mach/gpio.h>
#include <linux/gpio.h>

#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/input/mt.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/regulator/consumer.h>
#include <linux/firmware.h>
#include <linux/debugfs.h>
#include <linux/mutex.h>
#include <linux/wakelock.h>

#if defined(CONFIG_FB)
#include <linux/notifier.h>
#include <linux/fb.h>
#elif defined(CONFIG_HAS_EARLYSUSPEND)
#include <linux/earlysuspend.h>
#define GOODIX_SUSPEND_LEVEL 1
#endif


#define DRIVER_NAME "GTP"

//***************************PART1:ON/OFF define*******************************
#define GTP_CUSTOM_CFG        0
#define GTP_CHANGE_X2Y        0
#define GTP_DRIVER_SEND_CFG   1
#define GTP_HAVE_TOUCH_KEY    0
#define GTP_POWER_CTRL_SLEEP  0
#define GTP_ICS_SLOT_REPORT   1

#define GTP_AUTO_UPDATE       1   // auto update fw by .bin file as default
#define GTP_HEADER_FW_UPDATE  1    // auto update fw by gtp_default_FW in gt9xx_firmware.h, function together with GTP_AUTO_UPDATE
#define GTP_AUTO_UPDATE_CFG   0    // auto update config by .cfg file, function together with GTP_AUTO_UPDATE

#define GTP_COMPATIBLE_MODE   0    // compatible with GT9XXF

#define GTP_CREATE_WR_NODE    1
#define GTP_ESD_PROTECT       1    // esd protection with a cycle of 2 seconds

#define GTP_WITH_PEN          0
#define GTP_PEN_HAVE_BUTTON   0    // active pen has buttons, function together with GTP_WITH_PEN

#define GTP_GESTURE_WAKEUP    0    // gesture wakeup

#define GTP_DEBUG_ON          1
#define GTP_DEBUG_ARRAY_ON    0
#define GTP_DEBUG_FUNC_ON     0
#define KEY_GESTURE		KEY_CUSTOM_GESTURE	//KEY_F24
typedef enum
{
    DOZE_DISABLED = 0,
    DOZE_ENABLED = 1,
    DOZE_WAKEUP = 2,
}DOZE_T;

#define GTP_RW_ATTR (S_IRUGO | S_IWUGO)

#define GTP_I2C_ADDRESS_HIGH	0x14
#define RESET_DELAY_T3_US	200	/* T3: > 100us */
#define RESET_DELAY_T4		20	/* T4: > 5ms */
///////////////////////////////////////////////////////////////
#define GOODIX_MAX_CFG_GROUP 6
#define MAX_BUTTONS 4
#define GTP_FW_VERSION_BUFFER_MAXSIZE	4
#define GTP_REG_FW_VERSION	0x8144
#define GTP_PRODUCT_ID_MAXSIZE	5
#define GTP_PRODUCT_ID_BUFFER_MAXSIZE	6
#define GTP_REG_PRODUCT_ID	0x8140
#define GTP_REG_RAWDATA 		0x8B98
#define GTP_REG_DELTA			0xBB10
#define GTP_REG_BASELINE		0x81C0

#define CFG_LOC_DRVA_NUM		29
#define CFG_LOC_DRVB_NUM		30
#define CFG_LOC_SENS_NUM		31

///////////////////////////////////////////////////////////////
struct goodix_ts_platform_data {
	int irq_gpio;
	u32 irq_gpio_flags;
	int reset_gpio;
	u32 reset_gpio_flags;
	int vdd_gpio;
	u32 vdd_gpio_flags;
	const char *product_id;
	const char *fw_name;
	u32 x_max;
	u32 y_max;
	u32 x_min;
	u32 y_min;
	u32 panel_minx;
	u32 panel_miny;
	u32 panel_maxx;
	u32 panel_maxy;
	bool force_update;
	bool i2c_pull_up;
	bool enable_power_off;
	int config_data_len[GOODIX_MAX_CFG_GROUP];
	u8 *config_data[GOODIX_MAX_CFG_GROUP];
	u32 button_map[MAX_BUTTONS];
	u8 num_button;
	bool have_touch_key;
	bool driver_send_cfg;
	bool change_x2y;
	bool with_pen;
	bool slide_wakeup;
	bool dbl_clk_wakeup;

	int  suspend_resume_methods;
	int  fixed_key_type;
	const char *virtual_key_string;
};

struct goodix_ts_data {
    spinlock_t irq_lock;
    struct i2c_client *client;
    struct input_dev  *input_dev;
    struct goodix_ts_platform_data *pdata;
    struct hrtimer timer;
    struct work_struct  work;
	//add for i2c timeout start 2014.11.11
	struct work_struct irq_err_work;
	struct workqueue_struct *irq_err_workqueue;
	//add for i2c timeout end 2014.11.11
#if defined(CONFIG_FB)
	struct notifier_block fb_notif;
	struct work_struct fb_notifier_resume_work;
	struct work_struct fb_notifier_suspend_work;
	struct workqueue_struct *fb_notifier_workqueue;

#elif defined(CONFIG_HAS_EARLYSUSPEND)
	struct early_suspend early_suspend;
#endif
	struct regulator *vdd;
	struct regulator *vcc_i2c;
    s32 irq_is_disable;
    s32 use_irq;
    u16 abs_x_max;
    u16 abs_y_max;
    u8  max_touch_num;
    u8  int_trigger_type;
    u8  green_wake_mode;
    u8  enter_update;
    u8  gtp_is_suspend;
    u8  gtp_rawdiff_mode;
    u8  gtp_cfg_len;
    u8  fixed_cfg;
    u8  fw_error;
    u8  pnl_init_error;
    bool power_on;
#if GTP_WITH_PEN
    struct input_dev *pen_dev;
#endif

//#if GTP_ESD_PROTECT
    spinlock_t esd_lock;
    u8  esd_running;
    s32 clk_tick_cnt;
//#endif

	bool log_switch;
	bool is_calling;
	bool largetouch_flag;
	struct mutex suspend_mutex;
	struct wake_lock suspend_wakelock;

	struct mutex gesture_mutex;
	bool gesture_state;
	bool has_lcd_shutoff;
	bool need_change_to_dclick;
	int ts_ipod_flag;
	int edge_suppress_switch;
	int ts_dclick_switch;
	int dclick_proximity_switch;
	int ts_dclick_simulate_switch;
	atomic_t ts_state;
	u8 gesture_switch;
	u8 gesture_switch_export;
	u8 swipe_down_switch;
	u8 gesture_coordinate[250];/*gesture coordinate */
	u16 buf_gesture[130];/*gesture coordinate */
	int finger_state;
	int first_press_x;
	int first_press_y;
	int second_press_x;
	int second_press_y;
	int pressed_count;
	int release_count;
	bool is_dclick_valide;
	bool has_dclick_timer_start;
	int pre_state;
	int num_fingers;
	struct timer_list dclick_timer;
	struct work_struct dclick_timer_work;
	int dclick_dimension_x_min;
	int dclick_dimension_x_max;
	int dclick_dimension_y_min;
	int dclick_dimension_y_max;

};

extern u16 show_len;
extern u16 total_len;

//*************************** PART2:TODO define **********************************
// STEP_1(REQUIRED): Define Configuration Information Group(s)
// Sensor_ID Map:
/* sensor_opt1 sensor_opt2 Sensor_ID
    GND	GND		0 
    VDDIO	GND		1 
    NC		GND		2 
    GND	NC/300K		3 
    VDDIO	NC/300K		4 
    NC		NC/300K		5 
*/
// TODO: define your own default or for Sensor_ID == 0 config here. 
// The predefined one is just a sample config, which is not suitable for your tp in most cases.
#define CTP_CFG_GROUP1 {\
	0x43,0xD0,0x02,0x69,0x05,0x0A,0x35,0x00,0x01,0x08,\
	0x23,0x08,0x5A,0x32,0x33,0x35,0x00,0x00,0x37,0x13,\
	0x00,0x00,0x08,0x14,0x14,0x28,0x14,0x8B,0x2B,0x0C,\
	0x4B,0x4D,0xD3,0x07,0x00,0x00,0x00,0x9A,0x32,0x11,\
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x0A,0x60,\
	0x22,0x46,0x78,0x94,0xD0,0x02,0x8C,0x0B,0x85,0x04,\
	0xAC,0x49,0x00,0x9D,0x52,0x19,0x91,0x5B,0x00,0x86,\
	0x66,0x00,0x7D,0x71,0x00,0x7D,0x00,0x00,0x00,0x00,\
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
	0x00,0x00,0x00,0x00,0xFF,0x01,0x00,0x00,0x00,0x00,\
	0x00,0x00,0x00,0x00,0x00,0x14,0x50,0x00,0x00,0x00,\
	0x00,0x00,0x18,0x0E,0x10,0x16,0x14,0x12,0x08,0x06,\
	0x04,0x0A,0x0C,0x02,0xFF,0xFF,0x00,0x00,0x00,0x00,\
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
	0x04,0x41,0x00,0x0C,0x02,0x0F,0x04,0x10,0x06,0x12,\
	0x08,0x13,0x0A,0x16,0x20,0x18,0x21,0x1C,0x22,0x1D,\
	0x24,0x1E,0x26,0x1F,0xFF,0xFF,0xFF,0xFF,0x00,0x00,\
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
	0x00,0x00,0x00,0x00,0x39,0x01\
	}

// TODO: define your config for Sensor_ID == 1 here, if needed
#define CTP_CFG_GROUP2 {\
	}

// TODO: define your config for Sensor_ID == 2 here, if needed
#define CTP_CFG_GROUP3 {\
	}

// TODO: define your config for Sensor_ID == 3 here, if needed
#define CTP_CFG_GROUP4 {\
	0x43,0xD0,0x02,0x69,0x05,0x0A,0x35,0x00,0x01,0x08,\
	0x23,0x08,0x5A,0x32,0x33,0x35,0x00,0x00,0x37,0x13,\
	0x00,0x00,0x08,0x14,0x14,0x28,0x14,0x8B,0x2B,0x0C,\
	0x4B,0x4D,0xD3,0x07,0x00,0x00,0x00,0x9A,0x32,0x11,\
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x0A,0x60,\
	0x22,0x46,0x78,0x94,0xD0,0x02,0x8C,0x0B,0x85,0x04,\
	0xAB,0x49,0x00,0x9D,0x52,0x19,0x91,0x5B,0x00,0x86,\
	0x66,0x00,0x7D,0x71,0x00,0x7D,0x00,0x00,0x00,0x00,\
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
	0x00,0x00,0x00,0x00,0xFF,0x01,0x00,0x00,0x00,0x00,\
	0x00,0x00,0x00,0x00,0x00,0x14,0x50,0x00,0x00,0x00,\
	0x00,0x00,0x18,0x0E,0x10,0x16,0x14,0x12,0x08,0x06,\
	0x04,0x0A,0x0C,0x02,0xFF,0xFF,0x00,0x00,0x00,0x00,\
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
	0x04,0x41,0x00,0x0C,0x02,0x0F,0x04,0x10,0x06,0x12,\
	0x08,0x13,0x0A,0x16,0x20,0x18,0x21,0x1C,0x22,0x1D,\
	0x24,0x1E,0x26,0x1F,0xFF,0xFF,0xFF,0xFF,0x00,0x00,\
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
	0x00,0x00,0x00,0x00,0x3A,0x01\
}

// TODO: define your config for Sensor_ID == 4 here, if needed
#define CTP_CFG_GROUP5 {\
}

// TODO: define your config for Sensor_ID == 5 here, if needed
#define CTP_CFG_GROUP6 {\
}

// STEP_2(REQUIRED): Customize your I/O ports & I/O operations

#if 1
#define GTP_GPIO_AS_INT(pin)          do{\
                                            gpio_direction_input(pin);\
                                        }while(0)
#define GTP_GPIO_OUTPUT(pin,level)      gpio_direction_output(pin,level)
#define GTP_GPIO_FREE(pin)              gpio_free(pin)
#define GTP_GPIO_AS_INPUT(pin)          do{\
                                            gpio_direction_input(pin);\
                                        }while(0)
#endif

#if 0
#define GTP_RST_PORT 12
#define GTP_INT_PORT 13
#define GTP_INT_IRQ     gpio_to_irq(GTP_INT_PORT)
#define GTP_INT_CFG     S3C_GPIO_SFN(0xF)

#define GTP_GPIO_AS_INPUT(pin)          do{\
                                            gpio_direction_input(pin);\
                                        }while(0)
#define GTP_GPIO_AS_INT(pin)            do{\
                                            GTP_GPIO_AS_INPUT(pin);\
                                            s3c_gpio_cfgpin(pin, GTP_INT_CFG);\
                                        }while(0)
#define GTP_GPIO_GET_VALUE(pin)         gpio_get_value(pin)
#define GTP_GPIO_OUTPUT(pin,level)      gpio_direction_output(pin,level)
#define GTP_GPIO_REQUEST(pin, label)    gpio_request(pin, label)
#define GTP_GPIO_FREE(pin)              gpio_free(pin)
#endif
#define GTP_IRQ_TAB                     {IRQ_TYPE_EDGE_RISING, IRQ_TYPE_EDGE_FALLING, IRQ_TYPE_LEVEL_LOW, IRQ_TYPE_LEVEL_HIGH}

// STEP_3(optional): Specify your special config info if needed
#if GTP_CUSTOM_CFG
  #define GTP_MAX_HEIGHT   800
  #define GTP_MAX_WIDTH    480
  #define GTP_INT_TRIGGER  0            // 0: Rising 1: Falling
#else
  #define GTP_MAX_HEIGHT   4096
  #define GTP_MAX_WIDTH    4096
  #define GTP_INT_TRIGGER  1
#endif
#define GTP_MAX_TOUCH        10//5

// STEP_4(optional): If keys are available and reported as keys, config your key info here                             
#if GTP_HAVE_TOUCH_KEY
    #define GTP_KEY_TAB  {KEY_BACK, KEY_HOMEPAGE, KEY_MENU}
#endif

//***************************PART3:OTHER define*********************************
#define GTP_DRIVER_VERSION          "V2.2<2014/01/14>"
#define GTP_I2C_NAME                "Goodix-TS"
#define GT91XX_CONFIG_PROC_FILE     "gt9xx_config"
#define GTP_POLL_TIME         10    
#define GTP_ADDR_LENGTH       2
#define GTP_CONFIG_MIN_LENGTH 186
#define GTP_CONFIG_MAX_LENGTH 240
#define FAIL                  0
#define SUCCESS               1
#define SWITCH_OFF            0
#define SWITCH_ON             1

// Registers define
#define GTP_READ_COOR_ADDR    0x814E
#define GTP_REG_SLEEP         0x8040
#define GTP_REG_SENSOR_ID     0x814A
#define GTP_REG_CONFIG_DATA   0x8047
#define GTP_REG_VERSION       0x8140

#define RESOLUTION_LOC        3
#define TRIGGER_LOC           8

#define CFG_GROUP_LEN(p_cfg_grp)  (sizeof(p_cfg_grp) / sizeof(p_cfg_grp[0]))
// Log define
#define GTP_INFO(fmt,arg...)           printk("<<-GTP-INFO->> "fmt"\n",##arg)
#define GTP_ERROR(fmt,arg...)          printk("<<-GTP-ERROR->> "fmt"\n",##arg)
#define GTP_DEBUG(fmt,arg...)          do{\
                                         if(GTP_DEBUG_ON)\
                                         printk("<<-GTP-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
                                       }while(0)
#define GTP_DEBUG_ARRAY(array, num)    do{\
                                         s32 i;\
                                         u8* a = array;\
                                         if(GTP_DEBUG_ARRAY_ON)\
                                         {\
                                            printk("<<-GTP-DEBUG-ARRAY->>\n");\
                                            for (i = 0; i < (num); i++)\
                                            {\
                                                printk("%02x   ", (a)[i]);\
                                                if ((i + 1 ) %10 == 0)\
                                                {\
                                                    printk("\n");\
                                                }\
                                            }\
                                            printk("\n");\
                                        }\
                                       }while(0)
#define GTP_DEBUG_FUNC()               do{\
                                         if(GTP_DEBUG_FUNC_ON)\
                                         printk("<<-GTP-FUNC->> Func:%s@Line:%d\n",__func__,__LINE__);\
                                       }while(0)
#define GTP_SWAP(x, y)                 do{\
                                         typeof(x) z = x;\
                                         x = y;\
                                         y = z;\
                                       }while (0)

//*****************************End of Part III********************************
extern int usb_charger_flag;
void gtp_charger_switch(char on_or_off);

#endif /* _GOODIX_GT9XX_H_ */
