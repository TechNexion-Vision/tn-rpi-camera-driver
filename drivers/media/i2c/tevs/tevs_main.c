#include "asm-generic/errno-base.h"
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/pm_runtime.h>
#include <linux/regmap.h>
#include <linux/regulator/consumer.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-device.h>
#include <media/v4l2-event.h>
#include <media/v4l2-fwnode.h>
#include <media/v4l2-mediabus.h>
#include <asm/unaligned.h>
#include "tevs_tbls.h"

/* Define host command register of TEVS information page */
#define HOST_COMMAND_TEVS_INFO_VERSION_MSB                      (0x3000)
#define HOST_COMMAND_TEVS_INFO_VERSION_LSB                      (0x3002)
#define HOST_COMMAND_TEVS_BOOT_STATE                            (0x3004)

/* Define host command register of ISP control page */
#define HOST_COMMAND_ISP_CTRL_PREVIEW_WIDTH                     (0x3100)
#define HOST_COMMAND_ISP_CTRL_PREVIEW_HEIGHT                    (0x3102)
#define HOST_COMMAND_ISP_CTRL_PREVIEW_FORMAT                    (0x3104)
#define HOST_COMMAND_ISP_CTRL_PREVIEW_SENSOR_MODE               (0x3106)
#define HOST_COMMAND_ISP_CTRL_PREVIEW_THROUGHPUT                (0x3108)
#define HOST_COMMAND_ISP_CTRL_PREVIEW_MAX_FPS                   (0x310A)
#define HOST_COMMAND_ISP_CTRL_PREVIEW_EXP_TIME_UPPER_MSB        (0x310C)
#define HOST_COMMAND_ISP_CTRL_PREVIEW_EXP_TIME_UPPER_LSB        (0x310E)
#define HOST_COMMAND_ISP_CTRL_PREVIEW_EXP_TIME_MAX_MSB          (0x3110)
#define HOST_COMMAND_ISP_CTRL_PREVIEW_EXP_TIME_MAX_LSB          (0x3112)
#define HOST_COMMAND_ISP_CTRL_PREVIEW_HINF_CTRL                 (0x3114)
#define HOST_COMMAND_ISP_CTRL_AE_MODE                           (0x3116)
#define HOST_COMMAND_ISP_CTRL_EXP_TIME_MSB                      (0x3118)
#define HOST_COMMAND_ISP_CTRL_EXP_TIME_LSB                      (0x311A)
#define HOST_COMMAND_ISP_CTRL_EXP_TIME_MAX_MSB                  (0x311C)
#define HOST_COMMAND_ISP_CTRL_EXP_TIME_MAX_LSB                  (0x311E)
#define HOST_COMMAND_ISP_CTRL_EXP_TIME_MIN_MSB                  (0x3120)
#define HOST_COMMAND_ISP_CTRL_EXP_TIME_MIN_LSB                  (0x3122)
#define HOST_COMMAND_ISP_CTRL_EXP_GAIN                          (0x3124)
#define HOST_COMMAND_ISP_CTRL_EXP_GAIN_MAX                      (0x3126)
#define HOST_COMMAND_ISP_CTRL_EXP_GAIN_MIN                      (0x3128)
#define HOST_COMMAND_ISP_CTRL_CURRENT_EXP_TIME_MSB              (0x312A)
#define HOST_COMMAND_ISP_CTRL_CURRENT_EXP_TIME_LSB              (0x312C)
#define HOST_COMMAND_ISP_CTRL_CURRENT_EXP_GAIN                  (0x312E)
#define HOST_COMMAND_ISP_CTRL_BACKLIGHT_COMPENSATION            (0x3130)
#define HOST_COMMAND_ISP_CTRL_BACKLIGHT_COMPENSATION_MAX        (0x3132)
#define HOST_COMMAND_ISP_CTRL_BACKLIGHT_COMPENSATION_MIN        (0x3134)
#define HOST_COMMAND_ISP_CTRL_AWB_MODE                          (0x3136)
#define HOST_COMMAND_ISP_CTRL_AWB_TEMP                          (0x3138)
#define HOST_COMMAND_ISP_CTRL_AWB_TEMP_MAX                      (0x313A)
#define HOST_COMMAND_ISP_CTRL_AWB_TEMP_MIN                      (0x313C)
#define HOST_COMMAND_ISP_CTRL_BRIGHTNESS                        (0x313E)
#define HOST_COMMAND_ISP_CTRL_BRIGHTNESS_MAX                    (0x3140)
#define HOST_COMMAND_ISP_CTRL_BRIGHTNESS_MIN                    (0x3142)
#define HOST_COMMAND_ISP_CTRL_CONTRAST                          (0x3144)
#define HOST_COMMAND_ISP_CTRL_CONTRAST_MAX                      (0x3146)
#define HOST_COMMAND_ISP_CTRL_CONTRAST_MIN                      (0x3148)
#define HOST_COMMAND_ISP_CTRL_SATURATION                        (0x314A)
#define HOST_COMMAND_ISP_CTRL_SATURATION_MAX                    (0x314C)
#define HOST_COMMAND_ISP_CTRL_SATURATION_MIN                    (0x314E)
#define HOST_COMMAND_ISP_CTRL_GAMMA                             (0x3150)
#define HOST_COMMAND_ISP_CTRL_GAMMA_MAX                         (0x3152)
#define HOST_COMMAND_ISP_CTRL_GAMMA_MIN                         (0x3154)
#define HOST_COMMAND_ISP_CTRL_DENOISE                           (0x3156)
#define HOST_COMMAND_ISP_CTRL_DENOISE_MAX                       (0x3158)
#define HOST_COMMAND_ISP_CTRL_DENOISE_MIN                       (0x315A)
#define HOST_COMMAND_ISP_CTRL_SHARPEN                           (0x315C)
#define HOST_COMMAND_ISP_CTRL_SHARPEN_MAX                       (0x315E)
#define HOST_COMMAND_ISP_CTRL_SHARPEN_MIN                       (0x3160)
#define HOST_COMMAND_ISP_CTRL_FLIP                              (0x3162)
#define HOST_COMMAND_ISP_CTRL_EFFECT                            (0x3164)
#define HOST_COMMAND_ISP_CTRL_ZOOM_TYPE                         (0x3166)
#define HOST_COMMAND_ISP_CTRL_ZOOM_TIMES                        (0x3168)
#define HOST_COMMAND_ISP_CTRL_ZOOM_TIMES_MAX                    (0x316A)
#define HOST_COMMAND_ISP_CTRL_ZOOM_TIMES_MIN                    (0x316C)
#define HOST_COMMAND_ISP_CTRL_CT_X                              (0x316E)
#define HOST_COMMAND_ISP_CTRL_CT_Y                              (0x3170)
#define HOST_COMMAND_ISP_CTRL_CT_MAX                            (0x3172)
#define HOST_COMMAND_ISP_CTRL_CT_MIN                            (0x3174)
#define HOST_COMMAND_ISP_CTRL_SYSTEM_START                      (0x3176)
#define HOST_COMMAND_ISP_CTRL_ISP_RESET                         (0x3178)
#define HOST_COMMAND_ISP_CTRL_TRIGGER_MODE                      (0x317A)
#define HOST_COMMAND_ISP_CTRL_FLICK_CTRL                        (0x317C)
#define HOST_COMMAND_ISP_CTRL_MIPI_FREQ                         (0x317E)

/* Define host command register of ISP bootdata page */
#define HOST_COMMAND_ISP_BOOTDATA_1                             (0x4000)
#define HOST_COMMAND_ISP_BOOTDATA_2                             (0x4002)
#define HOST_COMMAND_ISP_BOOTDATA_3                             (0x4004)
#define HOST_COMMAND_ISP_BOOTDATA_4                             (0x4006)
#define HOST_COMMAND_ISP_BOOTDATA_5                             (0x4008)
#define HOST_COMMAND_ISP_BOOTDATA_6                             (0x400A)
#define HOST_COMMAND_ISP_BOOTDATA_7                             (0x400C)
#define HOST_COMMAND_ISP_BOOTDATA_8                             (0x400E)
#define HOST_COMMAND_ISP_BOOTDATA_9                             (0x4010)
#define HOST_COMMAND_ISP_BOOTDATA_10                            (0x4012)
#define HOST_COMMAND_ISP_BOOTDATA_11                            (0x4014)
#define HOST_COMMAND_ISP_BOOTDATA_12                            (0x4016)
#define HOST_COMMAND_ISP_BOOTDATA_13                            (0x4018)
#define HOST_COMMAND_ISP_BOOTDATA_14                            (0x401A)
#define HOST_COMMAND_ISP_BOOTDATA_15                            (0x401C)
#define HOST_COMMAND_ISP_BOOTDATA_16                            (0x401E)
#define HOST_COMMAND_ISP_BOOTDATA_17                            (0x4020)
#define HOST_COMMAND_ISP_BOOTDATA_18                            (0x4022)
#define HOST_COMMAND_ISP_BOOTDATA_19                            (0x4024)
#define HOST_COMMAND_ISP_BOOTDATA_20                            (0x4026)
#define HOST_COMMAND_ISP_BOOTDATA_21                            (0x4028)
#define HOST_COMMAND_ISP_BOOTDATA_22                            (0x402A)
#define HOST_COMMAND_ISP_BOOTDATA_23                            (0x402C)
#define HOST_COMMAND_ISP_BOOTDATA_24                            (0x402E)
#define HOST_COMMAND_ISP_BOOTDATA_25                            (0x4030)
#define HOST_COMMAND_ISP_BOOTDATA_26                            (0x4032)
#define HOST_COMMAND_ISP_BOOTDATA_27                            (0x4034)
#define HOST_COMMAND_ISP_BOOTDATA_28                            (0x4036)
#define HOST_COMMAND_ISP_BOOTDATA_29                            (0x4038)
#define HOST_COMMAND_ISP_BOOTDATA_30                            (0x403A)
#define HOST_COMMAND_ISP_BOOTDATA_31                            (0x403C)
#define HOST_COMMAND_ISP_BOOTDATA_32                            (0x403E)
#define HOST_COMMAND_ISP_BOOTDATA_33                            (0x4040)
#define HOST_COMMAND_ISP_BOOTDATA_34                            (0x4042)
#define HOST_COMMAND_ISP_BOOTDATA_35                            (0x4044)
#define HOST_COMMAND_ISP_BOOTDATA_36                            (0x4046)
#define HOST_COMMAND_ISP_BOOTDATA_37                            (0x4048)
#define HOST_COMMAND_ISP_BOOTDATA_38                            (0x404A)
#define HOST_COMMAND_ISP_BOOTDATA_39                            (0x404C)
#define HOST_COMMAND_ISP_BOOTDATA_40                            (0x404E)
#define HOST_COMMAND_ISP_BOOTDATA_41                            (0x4050)
#define HOST_COMMAND_ISP_BOOTDATA_42                            (0x4052)
#define HOST_COMMAND_ISP_BOOTDATA_43                            (0x4054)
#define HOST_COMMAND_ISP_BOOTDATA_44                            (0x4056)
#define HOST_COMMAND_ISP_BOOTDATA_45                            (0x4058)
#define HOST_COMMAND_ISP_BOOTDATA_46                            (0x405A)
#define HOST_COMMAND_ISP_BOOTDATA_47                            (0x405C)
#define HOST_COMMAND_ISP_BOOTDATA_48                            (0x405E)
#define HOST_COMMAND_ISP_BOOTDATA_49                            (0x4060)
#define HOST_COMMAND_ISP_BOOTDATA_50                            (0x4062)
#define HOST_COMMAND_ISP_BOOTDATA_51                            (0x4064)
#define HOST_COMMAND_ISP_BOOTDATA_52                            (0x4066)
#define HOST_COMMAND_ISP_BOOTDATA_53                            (0x4068)
#define HOST_COMMAND_ISP_BOOTDATA_54                            (0x406A)
#define HOST_COMMAND_ISP_BOOTDATA_55                            (0x406C)
#define HOST_COMMAND_ISP_BOOTDATA_56                            (0x406E)
#define HOST_COMMAND_ISP_BOOTDATA_57                            (0x4070)
#define HOST_COMMAND_ISP_BOOTDATA_58                            (0x4072)
#define HOST_COMMAND_ISP_BOOTDATA_59                            (0x4074)
#define HOST_COMMAND_ISP_BOOTDATA_60                            (0x4076)
#define HOST_COMMAND_ISP_BOOTDATA_61                            (0x4078)
#define HOST_COMMAND_ISP_BOOTDATA_62                            (0x407A)
#define HOST_COMMAND_ISP_BOOTDATA_63                            (0x407C)

/* Define special method for controlling ISP with I2C */
#define HOST_COMMAND_ISP_CTRL_I2C_ADDR                          (0xF000)
#define HOST_COMMAND_ISP_CTRL_I2C_DATA                          (0xF002)

#define TEVS_TRIGGER_CTRL                   	HOST_COMMAND_ISP_CTRL_TRIGGER_MODE

#define TEVS_BRIGHTNESS 						HOST_COMMAND_ISP_CTRL_BRIGHTNESS
#define TEVS_BRIGHTNESS_MAX 					HOST_COMMAND_ISP_CTRL_BRIGHTNESS_MAX
#define TEVS_BRIGHTNESS_MIN 					HOST_COMMAND_ISP_CTRL_BRIGHTNESS_MIN
#define TEVS_BRIGHTNESS_MASK 					(0xFFFF)
#define TEVS_CONTRAST 							HOST_COMMAND_ISP_CTRL_CONTRAST
#define TEVS_CONTRAST_MAX 						HOST_COMMAND_ISP_CTRL_CONTRAST_MAX
#define TEVS_CONTRAST_MIN 						HOST_COMMAND_ISP_CTRL_CONTRAST_MIN
#define TEVS_CONTRAST_MASK 						(0xFFFF)
#define TEVS_SATURATION 						HOST_COMMAND_ISP_CTRL_SATURATION
#define TEVS_SATURATION_MAX 					HOST_COMMAND_ISP_CTRL_SATURATION_MAX
#define TEVS_SATURATION_MIN 					HOST_COMMAND_ISP_CTRL_SATURATION_MIN
#define TEVS_SATURATION_MASK 					(0xFFFF)
#define TEVS_AWB_CTRL_MODE 						HOST_COMMAND_ISP_CTRL_AWB_MODE
#define TEVS_AWB_CTRL_MODE_MASK 				(0x00FF)
#define TEVS_AWB_CTRL_MODE_MANUAL_TEMP 			(7U << 0)
#define TEVS_AWB_CTRL_MODE_AUTO 				(15U << 0)
#define TEVS_AWB_CTRL_MODE_MANUAL_TEMP_IDX 		(0U << 0)
#define TEVS_AWB_CTRL_MODE_AUTO_IDX 			(1U << 0)
#define TEVS_GAMMA 								HOST_COMMAND_ISP_CTRL_GAMMA
#define TEVS_GAMMA_MAX 							HOST_COMMAND_ISP_CTRL_GAMMA_MAX
#define TEVS_GAMMA_MIN 							HOST_COMMAND_ISP_CTRL_GAMMA_MIN
#define TEVS_GAMMA_MASK 						(0xFFFF)
#define TEVS_AE_MANUAL_EXP_TIME 				HOST_COMMAND_ISP_CTRL_EXP_TIME_MSB
#define TEVS_AE_MANUAL_EXP_TIME_MAX 			HOST_COMMAND_ISP_CTRL_EXP_TIME_MAX_MSB
#define TEVS_AE_MANUAL_EXP_TIME_MIN 			HOST_COMMAND_ISP_CTRL_EXP_TIME_MIN_MSB
#define TEVS_AE_MANUAL_EXP_TIME_MASK 			(0xFFFFFFFF)
#define TEVS_AE_MANUAL_GAIN 					HOST_COMMAND_ISP_CTRL_EXP_GAIN
#define TEVS_AE_MANUAL_GAIN_MAX 				HOST_COMMAND_ISP_CTRL_EXP_GAIN_MAX
#define TEVS_AE_MANUAL_GAIN_MIN 				HOST_COMMAND_ISP_CTRL_EXP_GAIN_MIN
#define TEVS_AE_MANUAL_GAIN_MASK 				(0x00FF)
#define TEVS_ORIENTATION 						HOST_COMMAND_ISP_CTRL_FLIP
#define TEVS_ORIENTATION_HFLIP 					(1U << 0)
#define TEVS_ORIENTATION_VFLIP 					(1U << 1)
#define TEVS_FLICK_CTRL    						HOST_COMMAND_ISP_CTRL_FLICK_CTRL
#define TEVS_FLICK_CTRL_MASK					(0xFFFF) // TEVS_REG_16BIT(0x5440)
#define TEVS_FLICK_CTRL_FREQ(n)					((n) << 8)
#define TEVS_FLICK_CTRL_ETC_IHDR_UP				BIT(6)
#define TEVS_FLICK_CTRL_ETC_DIS					BIT(5)
#define TEVS_FLICK_CTRL_FRC_OVERRIDE_MAX_ET		BIT(4)
#define TEVS_FLICK_CTRL_FRC_OVERRIDE_UPPER_ET	BIT(3)
#define TEVS_FLICK_CTRL_FRC_EN					BIT(2)
#define TEVS_FLICK_CTRL_MODE_MASK				(3U << 0)
#define TEVS_FLICK_CTRL_MODE_DISABLED			(0U << 0)
#define TEVS_FLICK_CTRL_MODE_MANUAL				(1U << 0)
#define TEVS_FLICK_CTRL_MODE_AUTO				(2U << 0)
#define TEVS_FLICK_CTRL_FREQ_MASK			    (0xFF00)
#define TEVS_FLICK_CTRL_MODE_50HZ             	(TEVS_FLICK_CTRL_FREQ(50) | TEVS_FLICK_CTRL_MODE_MANUAL)
#define TEVS_FLICK_CTRL_MODE_60HZ             	(TEVS_FLICK_CTRL_FREQ(60) | TEVS_FLICK_CTRL_MODE_MANUAL)
#define TEVS_FLICK_MODE_DISABLED_IDX			(0U << 0)
#define TEVS_FLICK_MODE_ENABLED_IDX				(3U << 0)
#define TEVS_AWB_MANUAL_TEMP 					HOST_COMMAND_ISP_CTRL_AWB_TEMP
#define TEVS_AWB_MANUAL_TEMP_MAX 				HOST_COMMAND_ISP_CTRL_AWB_TEMP_MAX
#define TEVS_AWB_MANUAL_TEMP_MIN 				HOST_COMMAND_ISP_CTRL_AWB_TEMP_MIN
#define TEVS_AWB_MANUAL_TEMP_MASK 				(0xFFFF)
#define TEVS_SHARPEN 							HOST_COMMAND_ISP_CTRL_SHARPEN
#define TEVS_SHARPEN_MAX 						HOST_COMMAND_ISP_CTRL_SHARPEN_MAX
#define TEVS_SHARPEN_MIN 						HOST_COMMAND_ISP_CTRL_SHARPEN_MIN
#define TEVS_SHARPEN_MASK 						(0xFFFF)
#define TEVS_BACKLIGHT_COMPENSATION 			HOST_COMMAND_ISP_CTRL_BACKLIGHT_COMPENSATION
#define TEVS_BACKLIGHT_COMPENSATION_MAX 		HOST_COMMAND_ISP_CTRL_BACKLIGHT_COMPENSATION_MAX
#define TEVS_BACKLIGHT_COMPENSATION_MIN 		HOST_COMMAND_ISP_CTRL_BACKLIGHT_COMPENSATION_MIN
#define TEVS_BACKLIGHT_COMPENSATION_MASK 		(0xFFFF)
#define TEVS_DZ_TGT_FCT 						HOST_COMMAND_ISP_CTRL_ZOOM_TIMES
#define TEVS_DZ_TGT_FCT_MAX 					HOST_COMMAND_ISP_CTRL_ZOOM_TIMES_MAX
#define TEVS_DZ_TGT_FCT_MIN 					HOST_COMMAND_ISP_CTRL_ZOOM_TIMES_MIN
#define TEVS_DZ_TGT_FCT_MASK 					(0xFFFF)
#define TEVS_SFX_MODE 							HOST_COMMAND_ISP_CTRL_EFFECT
#define TEVS_SFX_MODE_SFX_MASK 					(0x00FF)
#define TEVS_SFX_MODE_SFX_NORMAL 				(0U << 0)
#define TEVS_SFX_MODE_SFX_BW 					(3U << 0)
#define TEVS_SFX_MODE_SFX_GRAYSCALE 			(6U << 0)
#define TEVS_SFX_MODE_SFX_NEGATIVE 				(7U << 0)
#define TEVS_SFX_MODE_SFX_SKETCH 				(15U << 0)
#define TEVS_SFX_MODE_SFX_NORMAL_IDX 			(0U << 0)
#define TEVS_SFX_MODE_SFX_BW_IDX 				(1U << 0)
#define TEVS_SFX_MODE_SFX_GRAYSCALE_IDX 		(2U << 0)
#define TEVS_SFX_MODE_SFX_NEGATIVE_IDX 			(3U << 0)
#define TEVS_SFX_MODE_SFX_SKETCH_IDX 			(4U << 0)
#define TEVS_AE_CTRL_MODE 						HOST_COMMAND_ISP_CTRL_AE_MODE
#define TEVS_AE_CTRL_MODE_MASK 					(0x00FF)
#define TEVS_AE_CTRL_MANUAL_EXP_TIME_GAIN 		(0U << 0)
#define TEVS_AE_CTRL_FULL_AUTO 					(12U << 0)
#define TEVS_AE_CTRL_MANUAL_EXP_TIME_GAIN_IDX 	(0U << 0)
#define TEVS_AE_CTRL_FULL_AUTO_IDX 				(1U << 0)
#define TEVS_DZ_CT_X 							HOST_COMMAND_ISP_CTRL_CT_X
#define TEVS_DZ_CT_X_MASK 						(0xFFFF)
#define TEVS_DZ_CT_Y 							HOST_COMMAND_ISP_CTRL_CT_Y
#define TEVS_DZ_CT_Y_MASK 						(0xFFFF)
#define TEVS_DZ_CT_MAX 							HOST_COMMAND_ISP_CTRL_CT_MAX
#define TEVS_DZ_CT_MIN 							HOST_COMMAND_ISP_CTRL_CT_MIN

#define V4L2_CID_TEVS_BSL_MODE            (V4L2_CID_USER_BASE + 44)
#define TEVS_TRIGGER_CTRL_MODE_MASK 		(0x0001)
#define TEVS_BSL_MODE_NORMAL_IDX 		    (0U << 0)
#define TEVS_BSL_MODE_FLASH_IDX 			(1U << 0)

#define DEFAULT_HEADER_VERSION 3
#define TEVS_BOOT_TIME						(250)

#define to_tevs(d) container_of(d, struct tevs, v4l2_subdev)

/* regulator supplies */
static const char * const tevs_supply_name[] = {
	/* Supplies can be enabled in any order */
	"VANA",  /* Analog (2.8V) supply */
	"VDIG",  /* Digital Core (1.8V) supply */
	"VDDL",  /* IF (1.2V) supply */
};

#define TEVS_NUM_SUPPLIES ARRAY_SIZE(tevs_supply_name)

struct header_info {
	u8 header_version;
	u16 content_offset;
	u16 sensor_type;
	u8 sensor_fuseid[16];
	u8 product_name[64];
	u8 lens_id[16];
	u16 fix_checksum;
	u8 tn_fw_version[2];
	u16 vendor_fw_version;
	u16 custom_number;
	u8 build_year;
	u8 build_month;
	u8 build_day;
	u8 build_hour;
	u8 build_minute;
	u8 build_second;
	u16 mipi_datarate;
	u32 content_len;
	u16 content_checksum;
	u16 total_checksum;
} __attribute__((packed));

struct tevs {
	struct v4l2_subdev v4l2_subdev;
	struct media_pad pad;
    
	struct v4l2_mbus_framefmt fmt;

	struct regmap *regmap;

	struct header_info *header_info;
	struct gpio_desc *reset_gpio;
	struct gpio_desc *standby_gpio;

	struct regulator_bulk_data supplies[TEVS_NUM_SUPPLIES];

	/* V4L2 Controls */
	struct v4l2_ctrl_handler ctrls;

	int data_lanes;
	int continuous_clock;
	int data_frequency;
	bool hw_reset_mode;
	bool trigger_mode;
	char *sensor_name;
	u8 selected_mode;
	u8 selected_sensor;

	/*
	 * Mutex for serialized access:
	 * Protect sensor module set pad format and start/stop streaming safely.
	 */
	struct mutex mutex;

	/* Streaming on/off */
	bool streaming;
};

static const struct regmap_config tevs_regmap_config = {
	.reg_bits = 16,
	.val_bits = 8,
	.cache_type = REGCACHE_NONE,
};

int tevs_i2c_read(struct tevs *tevs, u16 reg, u8 *val, u16 size)
{
	struct i2c_client *client = v4l2_get_subdevdata(&tevs->v4l2_subdev);
	int ret;

	ret = regmap_bulk_read(tevs->regmap, reg, val, size);
	if (ret < 0) {
		dev_err(&client->dev, "Failed to read from register: ret=%d, reg=0x%x\n", ret, reg);
		return ret;
	}

	return 0;
}

int tevs_i2c_read_16b(struct tevs *tevs, u16 reg, u16 *value)
{
	struct i2c_client *client = v4l2_get_subdevdata(&tevs->v4l2_subdev);
	u8 v[2] = { 0 };
	int ret;

	ret = tevs_i2c_read(tevs, reg, v, 2);
	if (ret < 0) {
		dev_err(&client->dev, 
			"Failed to read from register: ret=%d, reg=0x%x\n", ret, reg);
		return ret;
	}

	*value = (v[0] << 8) | v[1];
	dev_dbg(&client->dev, 
		"%s() read reg 0x%x, value 0x%x\n", 
		__func__, reg, *value);

	return 0;
}

int tevs_i2c_write(struct tevs *tevs, u16 reg, u8 *val, u16 size)
{
	struct i2c_client *client = v4l2_get_subdevdata(&tevs->v4l2_subdev);
	int ret;

	ret = regmap_bulk_write(tevs->regmap, reg, val, size);
	if (ret < 0) {
		dev_err(&client->dev, "Failed to write to register: ret=%d reg=0x%x\n", ret, reg);
		return ret;
	}

	return 0;
}

int tevs_i2c_write_16b(struct tevs *tevs, u16 reg, u16 val)
{
	struct i2c_client *client = v4l2_get_subdevdata(&tevs->v4l2_subdev);
	int ret;
	u8 data[2];
	data[0] = val >> 8;
	data[1] = val & 0xFF;

	ret = regmap_bulk_write(tevs->regmap, reg, data, 2);
	if (ret < 0) {
		dev_err(&client->dev, 
			"Failed to write to register: ret=%d reg=0x%x\n", ret, reg);
		return ret;
	}
	dev_dbg(&client->dev, 
		"%s() write reg 0x%x, value 0x%x\n", 
		__func__, reg, val);

	return 0;
}

int tevs_enable_trigger_mode(struct tevs *tevs, int enable)
{
	struct i2c_client *client = v4l2_get_subdevdata(&tevs->v4l2_subdev);
	int ret = 0;
	int count = 0;
	u16 val, trigger_data;
	dev_dbg(&client->dev, "%s(): enable:%d\n", __func__, enable);
	trigger_data = (0x300 | ( (enable > 0) ? 0x82 : 0x80));

	if((ret = tevs_i2c_write_16b(tevs, TEVS_TRIGGER_CTRL, trigger_data)) < 0)
		return ret;

	do {
		if((ret = tevs_i2c_read_16b(tevs, TEVS_TRIGGER_CTRL, &val)) < 0)
				return ret;
		if((val & 0x300) == 0)
			break;

	} while(count++ < 10);

	usleep_range(90000, 100000);

	return ret;
}

int tevs_check_version(struct tevs *tevs)
{
	struct i2c_client *client = v4l2_get_subdevdata(&tevs->v4l2_subdev);
	u8 version[4] = { 0 };
	int ret = 0;

	ret = tevs_i2c_read(tevs, HOST_COMMAND_TEVS_INFO_VERSION_MSB, &version[0], 4);
	if(ret < 0) {
		dev_err(&client->dev, "can't check version\n");
		return ret;
	}
	dev_info(&client->dev, "Version:%d.%d.%d.%d\n",
		    version[0], version[1], version[2], version[3]);

	return 0;
}

int tevs_load_header_info(struct tevs *tevs)
{
	struct i2c_client *client = v4l2_get_subdevdata(&tevs->v4l2_subdev);
	struct header_info *header = tevs->header_info;
	u8 header_ver;
	int ret = 0;

	ret = tevs_i2c_read(tevs, HOST_COMMAND_ISP_BOOTDATA_1, &header_ver, 1);

	if(ret < 0) {
		dev_err(&client->dev, "can't recognize header info\n");
		return ret;
	}

	if (header_ver == DEFAULT_HEADER_VERSION) {
		tevs_i2c_read(tevs, HOST_COMMAND_ISP_BOOTDATA_1,
				(u8*)header,
				sizeof(struct header_info));

		dev_info(
			&client->dev,
			"Product:%s, HeaderVer:%d, MIPI_Rate:%d\n",
			header->product_name, header->header_version,
			header->mipi_datarate);

		dev_dbg(&client->dev, "content checksum: %x, content length: %d\n",
			header->content_checksum, header->content_len);

		return 0;
	} else {
		dev_err(&client->dev, "can't recognize header version number '0x%X'\n",
			header_ver);
		return -EINVAL;
	}
}

int tevs_init_setting(struct tevs *tevs)
{
	struct i2c_client *client = v4l2_get_subdevdata(&tevs->v4l2_subdev);
	int ret = 0;

	if(tevs->trigger_mode) {
		ret = tevs_enable_trigger_mode(tevs, 1);
		if (ret != 0) {
			dev_err(&client->dev, "set trigger mode failed\n");
			return ret;
		}
	}

	ret += tevs_i2c_write_16b(tevs,
				HOST_COMMAND_ISP_CTRL_PREVIEW_FORMAT,
				0x50);
	ret += tevs_i2c_write_16b(tevs,
				HOST_COMMAND_ISP_CTRL_PREVIEW_HINF_CTRL,
				0x10 | (tevs->continuous_clock << 5) | (tevs->data_lanes));
	return ret;
}

static int tevs_standby(struct tevs *tevs, int enable)
{
	struct i2c_client *client = v4l2_get_subdevdata(&tevs->v4l2_subdev);
	u16 v = 0;
	int timeout = 0;
	dev_dbg(&client->dev, "%s():enable=%d\n", __func__, enable);

	if (enable == 1) {
		tevs_i2c_write_16b(tevs, HOST_COMMAND_ISP_CTRL_SYSTEM_START,
				     0x0000);
		while (timeout < 100) {
			tevs_i2c_read_16b(
				tevs, HOST_COMMAND_ISP_CTRL_SYSTEM_START, &v);
			if ((v & 0x100) == 0)
				break;
			if (++timeout >= 100) {
				dev_err(&client->dev, "timeout: line[%d]v=%x\n",
					__LINE__, v);
				return -EINVAL;
			}
			usleep_range(9000, 10000);
		}
		dev_dbg(&client->dev, "sensor standby\n");
	} else {
		tevs_i2c_write_16b(tevs, HOST_COMMAND_ISP_CTRL_SYSTEM_START,
				     0x0001);
		while (timeout < 100) {
			tevs_i2c_read_16b(
				tevs, HOST_COMMAND_ISP_CTRL_SYSTEM_START, &v);
			if ((v & 0x100) == 0x100)
				break;
			if (++timeout >= 100) {
				dev_err(&client->dev, "timeout: line[%d]v=%x\n",
					__LINE__, v);
				return -EINVAL;
			}
			usleep_range(9000, 10000);
		}
		dev_dbg(&client->dev, "sensor wakeup\n");
	}

	return 0;
}

static int tevs_check_boot_state(struct tevs *tevs)
{
	struct i2c_client *client = v4l2_get_subdevdata(&tevs->v4l2_subdev);
	u16 boot_state;
	u8 timeout = 0;
	int ret = 0;

	while (timeout < 20) {
		tevs_i2c_read_16b(tevs,
				HOST_COMMAND_TEVS_BOOT_STATE, &boot_state);
		if (boot_state == 0x08)
			break;
		dev_dbg(&client->dev, "tevs bootup state: %d\n", boot_state);
		if (++timeout >= 20) {
			dev_err(&client->dev, "tevs bootup timeout: state: 0x%02X\n", boot_state);
			ret = -EINVAL;
		}
		msleep(20);
	}

	return ret;
}

static int tevs_start_streaming(struct tevs *tevs)
{
	struct i2c_client *client = v4l2_get_subdevdata(&tevs->v4l2_subdev);
	int ret = 0;

	ret = pm_runtime_get_sync(&client->dev);
	if (ret < 0) {
		pm_runtime_put_noidle(&client->dev);
		return ret;
	}

	if(!(tevs->hw_reset_mode | tevs->trigger_mode))
        ret = tevs_standby(tevs, 0);
    if (ret == 0) {
        int fps = tevs_sensor_table[tevs->selected_sensor]
                    .res_list[tevs->selected_mode]
                    .framerates;
        dev_dbg(&client->dev, "%s() width=%d, height=%d\n",
            __func__,
            tevs_sensor_table[tevs->selected_sensor]
                .res_list[tevs->selected_mode]
                .width,
            tevs_sensor_table[tevs->selected_sensor]
                .res_list[tevs->selected_mode]
                .height);
        tevs_i2c_write_16b(
            tevs, HOST_COMMAND_ISP_CTRL_PREVIEW_SENSOR_MODE,
            tevs_sensor_table[tevs->selected_sensor]
                .res_list[tevs->selected_mode]
                .mode);
        tevs_i2c_write_16b(
            tevs, HOST_COMMAND_ISP_CTRL_PREVIEW_WIDTH,
            tevs_sensor_table[tevs->selected_sensor]
                .res_list[tevs->selected_mode]
                .width);
        tevs_i2c_write_16b(
            tevs, HOST_COMMAND_ISP_CTRL_PREVIEW_HEIGHT,
            tevs_sensor_table[tevs->selected_sensor]
                .res_list[tevs->selected_mode]
                .height);
        tevs_i2c_write_16b(
            tevs, HOST_COMMAND_ISP_CTRL_PREVIEW_MAX_FPS,
            fps);
    }
	/* Apply customized values from user */
	ret =  __v4l2_ctrl_handler_setup(tevs->v4l2_subdev.ctrl_handler);
	if (ret)
		goto err_rpm_put;

	return 0;

err_rpm_put:
	pm_runtime_put(&client->dev);
	return ret;
}

static void tevs_stop_streaming(struct tevs *tevs)
{
	struct i2c_client *client = v4l2_get_subdevdata(&tevs->v4l2_subdev);
	int ret = 0;

	/* set stream off register */
    if (!(tevs->hw_reset_mode | tevs->trigger_mode)) {
        ret = tevs_standby(tevs, 1);
        if (ret)
            dev_err(&client->dev, "%s failed to set stream\n", __func__);
    }

	pm_runtime_put(&client->dev);
}

static int tevs_set_stream(struct v4l2_subdev *sub_dev, int enable)
{
	struct tevs *tevs = to_tevs(sub_dev);
	int ret = 0;

	dev_dbg(sub_dev->dev, "%s() enable [%x]\n", __func__, enable);

	mutex_lock(&tevs->mutex);
	if (tevs->streaming == enable) {
		mutex_unlock(&tevs->mutex);
		return 0;
	}

	if (tevs->selected_mode >=
	    tevs_sensor_table[tevs->selected_sensor].res_list_size)
		return -EINVAL;

	if (enable == 0) {
        tevs_stop_streaming(tevs);
	} else {		
		ret = tevs_start_streaming(tevs);
		if (ret)
			goto err_unlock;
	}
	tevs->streaming = enable;

err_unlock:
	mutex_unlock(&tevs->mutex);

	return ret;
}

static int tevs_power_on(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct v4l2_subdev *sub_dev = i2c_get_clientdata(client);
	struct tevs *tevs = to_tevs(sub_dev);
	int ret = 0;

	dev_dbg(&client->dev, "%s()\n", __func__);

	ret = regulator_bulk_enable(TEVS_NUM_SUPPLIES, tevs->supplies);
	if (ret) {
		dev_err(&client->dev, "%s: failed to enable regulators\n",
			__func__);
		return ret;
	}

	// gpiod_set_value_cansleep(tevs->standby_gpio, 0);
	gpiod_set_value_cansleep(tevs->reset_gpio, 1);
	msleep(TEVS_BOOT_TIME);

	ret = tevs_check_boot_state(tevs);
	if(ret != 0) {
		goto error;
	}

    ret = tevs_init_setting(tevs);
    if (ret != 0) {
        dev_err(&client->dev, "init setting failed\n");
        goto error;
    }

	return ret;

error:
	gpiod_set_value_cansleep(tevs->reset_gpio, 0);
	regulator_bulk_disable(TEVS_NUM_SUPPLIES, tevs->supplies);
	return ret;
}

static int tevs_power_off(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct v4l2_subdev *sub_dev = i2c_get_clientdata(client);
	struct tevs *tevs = to_tevs(sub_dev);

	dev_dbg(&client->dev, "%s()\n", __func__);

    gpiod_set_value_cansleep(tevs->reset_gpio, 0);
    regulator_bulk_disable(TEVS_NUM_SUPPLIES, tevs->supplies);

	return 0;
}

static int __maybe_unused tevs_suspend(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct v4l2_subdev *sub_dev = i2c_get_clientdata(client);
	struct tevs *tevs = to_tevs(sub_dev);

	dev_dbg(&client->dev, "%s()\n", __func__);

	if (tevs->streaming)
		tevs_stop_streaming(tevs);

	return 0;
}

static int __maybe_unused tevs_resume(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct v4l2_subdev *sub_dev = i2c_get_clientdata(client);
	struct tevs *tevs = to_tevs(sub_dev);
	int ret;

	dev_dbg(&client->dev, "%s()\n", __func__);

	if (tevs->streaming) {
		ret = tevs_start_streaming(tevs);
		if (ret)
			goto error;
	}

	return 0;

error:
	tevs_stop_streaming(tevs);
	tevs->streaming = false;

	return ret;
}

static int tevs_get_regulators(struct tevs *tevs)
{
	struct i2c_client *client = v4l2_get_subdevdata(&tevs->v4l2_subdev);
	unsigned int i;

	for (i = 0; i < TEVS_NUM_SUPPLIES; i++)
		tevs->supplies[i].supply = tevs_supply_name[i];

	return devm_regulator_bulk_get(&client->dev,
				       TEVS_NUM_SUPPLIES,
				       tevs->supplies);
}

static int tevs_get_frame_interval(struct v4l2_subdev *sub_dev,
				  struct v4l2_subdev_frame_interval *fi)
{
	dev_dbg(sub_dev->dev, "%s()\n", __func__);

	if (fi->pad != 0)
		return -EINVAL;

	fi->interval.numerator = 1;
	fi->interval.denominator = 30;

	return 0;
}

static int tevs_set_frame_interval(struct v4l2_subdev *sub_dev,
				  struct v4l2_subdev_frame_interval *fi)
{
	dev_dbg(sub_dev->dev, "%s()\n", __func__);

	if (fi->pad != 0)
		return -EINVAL;

	fi->interval.numerator = 1;
	fi->interval.denominator = 30;

	return 0;
}

static int tevs_enum_mbus_code(struct v4l2_subdev *sub_dev,
			   	  struct v4l2_subdev_state *sub_state,
			      struct v4l2_subdev_mbus_code_enum *code)
{
	dev_dbg(sub_dev->dev, "%s()\n", __func__);
	if (code->pad || code->index > 0)
		return -EINVAL;

	code->code = MEDIA_BUS_FMT_UYVY8_1X16;

	return 0;
}

static int tevs_get_fmt(struct v4l2_subdev *sub_dev,
			   struct v4l2_subdev_state *sub_state,
		       struct v4l2_subdev_format *format)
{
	struct v4l2_mbus_framefmt *fmt;
	struct v4l2_mbus_framefmt *mbus_fmt = &format->format;
	struct tevs *tevs = to_tevs(sub_dev);

	mutex_lock(&tevs->mutex);

	if (format->pad != 0)
		return -EINVAL;
    
    dev_dbg(sub_dev->dev, "%s() which [%d]\n", __func__, format->which);
	if (format->which == V4L2_SUBDEV_FORMAT_TRY)
		fmt = v4l2_subdev_get_try_format(sub_dev, sub_state, format->pad);
	else
		fmt = &tevs->fmt;

	memmove(mbus_fmt, fmt, sizeof(struct v4l2_mbus_framefmt));
	mutex_unlock(&tevs->mutex);

	return 0;
}

static int tevs_set_fmt(struct v4l2_subdev *sub_dev,
			   struct v4l2_subdev_state *sub_state,
		       struct v4l2_subdev_format *format)
{
	struct v4l2_mbus_framefmt *fmt;
	struct v4l2_mbus_framefmt *mbus_fmt = &format->format;
	struct tevs *tevs = to_tevs(sub_dev);
	int i;

	dev_dbg(sub_dev->dev, "%s()\n", __func__);

	if (format->pad != 0)
		return -EINVAL;
    
	mutex_lock(&tevs->mutex);

	for (i = 0;
	     i < tevs_sensor_table[tevs->selected_sensor].res_list_size;
	     i++) {
		if (mbus_fmt->width ==
			    tevs_sensor_table[tevs->selected_sensor]
				    .res_list[i]
				    .width &&
		    mbus_fmt->height ==
			    tevs_sensor_table[tevs->selected_sensor]
				    .res_list[i]
				    .height)
			break;
	}

	if (i >= tevs_sensor_table[tevs->selected_sensor].res_list_size) {
		return -EINVAL;
	}
	tevs->selected_mode = i;
	dev_dbg(sub_dev->dev, "%s() selected mode index [%d]\n", __func__,
		tevs->selected_mode);

	mbus_fmt->width =
		tevs_sensor_table[tevs->selected_sensor].res_list[i].width;
	mbus_fmt->height = 
        tevs_sensor_table[tevs->selected_sensor].res_list[i].height;
	mbus_fmt->code = MEDIA_BUS_FMT_UYVY8_1X16;
	mbus_fmt->colorspace = V4L2_COLORSPACE_SRGB;
	mbus_fmt->ycbcr_enc = V4L2_MAP_YCBCR_ENC_DEFAULT(mbus_fmt->colorspace);
	mbus_fmt->quantization = V4L2_QUANTIZATION_FULL_RANGE;
	mbus_fmt->xfer_func = V4L2_MAP_XFER_FUNC_DEFAULT(mbus_fmt->colorspace);
	// memset(mbus_fmt->reserved, 0, sizeof(mbus_fmt->reserved));

	if (format->which == V4L2_SUBDEV_FORMAT_TRY)
		fmt = v4l2_subdev_get_try_format(sub_dev, sub_state, format->pad);
	else
		fmt = &tevs->fmt;

	memmove(fmt, mbus_fmt, sizeof(struct v4l2_mbus_framefmt));

	mutex_unlock(&tevs->mutex);

	return 0;
}

static int tevs_get_selection(struct v4l2_subdev *sub_dev,
			    struct v4l2_subdev_state *sub_state,
				struct v4l2_subdev_selection *sel)
{
	struct tevs *tevs = to_tevs(sub_dev);
	switch (sel->target) {
	case V4L2_SEL_TGT_CROP:
	case V4L2_SEL_TGT_NATIVE_SIZE:
	case V4L2_SEL_TGT_CROP_DEFAULT:
	case V4L2_SEL_TGT_CROP_BOUNDS:
		sel->r.top = 0;
		sel->r.left = 0;
		sel->r.width = tevs->fmt.width;
		sel->r.height = tevs->fmt.height;
		
		dev_dbg(sub_dev->dev, "%s() selection [%d, %d, %d, %d]\n", __func__,
			sel->r.top, sel->r.left, sel->r.width, sel->r.height);
		return 0;
	}

	return -EINVAL;
}

static int tevs_enum_frame_size(struct v4l2_subdev *sub_dev,
			       struct v4l2_subdev_state *sub_state,
			       struct v4l2_subdev_frame_size_enum *fse)
{
	struct tevs *tevs = to_tevs(sub_dev);

	dev_dbg(sub_dev->dev, "%s()\n", __func__);

	if ((fse->pad != 0) ||
	    (fse->index >=
	     tevs_sensor_table[tevs->selected_sensor].res_list_size))
		return -EINVAL;

	fse->min_width = fse->max_width =
		tevs_sensor_table[tevs->selected_sensor]
			.res_list[fse->index]
			.width;
	fse->min_height = fse->max_height =
		tevs_sensor_table[tevs->selected_sensor]
			.res_list[fse->index]
			.height;

	return 0;
}

static int tevs_enum_frame_interval(struct v4l2_subdev *sub_dev,
			       struct v4l2_subdev_state *sub_state,
				   struct v4l2_subdev_frame_interval_enum *fie)
{
	struct tevs *tevs = to_tevs(sub_dev);
	int i;
    
	dev_dbg(sub_dev->dev, "%s()\n", __func__);

	if ((fie->pad != 0) || (fie->index != 0))
		return -EINVAL;

	fie->interval.numerator = 1;

	for (i = 0;
	     i < tevs_sensor_table[tevs->selected_sensor].res_list_size;
	     i++) {
		if (fie->width == tevs_sensor_table[tevs->selected_sensor]
					  .res_list[i].width &&
		    fie->height == tevs_sensor_table[tevs->selected_sensor]
				    .res_list[i].height) {
			fie->interval.denominator =
				tevs_sensor_table[tevs->selected_sensor]
					.res_list[i].framerates;
			break;
		}
	}

	return 0;
}

static int tevs_open(struct v4l2_subdev *sub_dev, struct v4l2_subdev_fh *fh)
{
	struct tevs *tevs = to_tevs(sub_dev);
	struct v4l2_mbus_framefmt *try_fmt_img =
		v4l2_subdev_get_try_format(sub_dev, fh->state, 0);
	struct v4l2_rect *try_crop = 
        v4l2_subdev_get_try_crop(sub_dev, fh->state, 0);

	dev_dbg(sub_dev->dev, "%s()\n", __func__);
    
	mutex_lock(&tevs->mutex);

	/* Initialize try_fmt for the image pad */
	try_fmt_img = &tevs->fmt;

	/* Initialize try_crop rectangle. */
	try_crop->top = 0;
	try_crop->left = 0;
	try_crop->width = tevs_sensor_table[tevs->selected_sensor].res_list[0].width;
	try_crop->height = tevs_sensor_table[tevs->selected_sensor].res_list[0].height;

	mutex_unlock(&tevs->mutex);

	return 0;
}

/* -----------------------------------------------------------------------------
 * V4L2 Controls
 */
static const char *const awb_mode_strings[] = {
	"Manual Temp Mode", // TEVS_AWB_CTRL_MODE_MANUAL_TEMP
	"Auto Mode", // TEVS_AWB_CTRL_MODE_AUTO
	NULL,
};

static const char * const flick_mode_strings[] = {
	"Disabled", 
	"50 Hz",
	"60 Hz",
	"Auto",
	NULL,
};

static const char *const sfx_mode_strings[] = {
	"Normal Mode", // TEVS_SFX_MODE_SFX_NORMAL
	"Black and White Mode", // TEVS_SFX_MODE_SFX_BW
	"Grayscale Mode", // TEVS_SFX_MODE_SFX_GRAYSCALE
	"Negative Mode", // TEVS_SFX_MODE_SFX_NEGATIVE
	"Sketch Mode", // TEVS_SFX_MODE_SFX_SKETCH
	NULL,
};

static const char *const ae_mode_strings[] = {
	"Manual Mode", // TEVS_AE_CTRL_MANUAL_EXP_TIME_GAIN
	"Auto Mode", // TEVS_AE_CTRL_FULL_AUTO
	NULL,
};

static const char *const bsl_mode_strings[] = {
	"Normal Mode",
	"Bootstrap Mode",
	NULL,
};

static int tevs_s_ctrl(struct v4l2_ctrl *ctrl)
{
	struct tevs *tevs = container_of(ctrl->handler, struct tevs, ctrls);
	struct i2c_client *client = v4l2_get_subdevdata(&tevs->v4l2_subdev);
	int ret;

	switch (ctrl->id) {
	case V4L2_CID_BRIGHTNESS:
        ret = tevs_i2c_write_16b(tevs, TEVS_BRIGHTNESS,
                        ctrl->val & TEVS_BRIGHTNESS_MASK);
        break;
	case V4L2_CID_CONTRAST:
        ret = tevs_i2c_write_16b(tevs, TEVS_CONTRAST,
                        ctrl->val & TEVS_CONTRAST_MASK);
        break;
	case V4L2_CID_SATURATION:
		ret = tevs_i2c_write_16b(tevs, TEVS_SATURATION,
                        ctrl->val & TEVS_SATURATION_MASK);
        break;
	case V4L2_CID_AUTO_WHITE_BALANCE: {
        u16 val;
        switch (ctrl->val & TEVS_AWB_CTRL_MODE_MASK) {
        case 0:
            val = TEVS_AWB_CTRL_MODE_MANUAL_TEMP;
            break;
        case 1:
            val = TEVS_AWB_CTRL_MODE_AUTO;
            break;
        default:
            val = TEVS_AWB_CTRL_MODE_AUTO;
            break;
        }
	    ret = tevs_i2c_write_16b(tevs, TEVS_AWB_CTRL_MODE, val);
        break;
    }
	case V4L2_CID_GAMMA:
		ret = tevs_i2c_write_16b(tevs, TEVS_GAMMA,
                        ctrl->val & TEVS_GAMMA_MASK);
        break;
	case V4L2_CID_EXPOSURE: {
        u8 exp[4];
        __be32 exp_temp;
	    exp_temp = cpu_to_be32(ctrl->val);
	    memcpy(exp, &exp_temp, 4);
	    ret = tevs_i2c_write(tevs, TEVS_AE_MANUAL_EXP_TIME,
                    exp, 4);
        break;
    }
	case V4L2_CID_GAIN:
        ret = tevs_i2c_write_16b(tevs, TEVS_AE_MANUAL_GAIN,
                        ctrl->val & TEVS_AE_MANUAL_GAIN_MASK);
        break;
	case V4L2_CID_HFLIP: {
        u16 val;
		ret = tevs_i2c_read_16b(tevs, TEVS_ORIENTATION,
				        &val);
        val &= ~TEVS_ORIENTATION_HFLIP;
        val |= ctrl->val ? TEVS_ORIENTATION_HFLIP : 0;

	    ret = tevs_i2c_write_16b(tevs, TEVS_ORIENTATION,
                            val);
        break;
    }
	case V4L2_CID_VFLIP: {
        u16 val;
        ret = tevs_i2c_read_16b(tevs, TEVS_ORIENTATION,
                        &val);
        val &= ~TEVS_ORIENTATION_VFLIP;
        val |= ctrl->val ? TEVS_ORIENTATION_VFLIP : 0;

        ret = tevs_i2c_write_16b(tevs, TEVS_ORIENTATION,
                            val);
        break;
    }
	case V4L2_CID_POWER_LINE_FREQUENCY: {
        u16 val;
        switch(ctrl->val) {
        case 0:
            val = TEVS_FLICK_CTRL_MODE_DISABLED;
            break;
        case 1:
            val = TEVS_FLICK_CTRL_MODE_50HZ;
            break;
        case 2:
            val = TEVS_FLICK_CTRL_MODE_60HZ;
            break;
        case 3:
            val = TEVS_FLICK_CTRL_MODE_AUTO | 
                    TEVS_FLICK_CTRL_FRC_OVERRIDE_UPPER_ET | 
                    TEVS_FLICK_CTRL_FRC_EN;
            break;
        default:
            val = TEVS_FLICK_CTRL_MODE_DISABLED;
            break;
        }
	    ret = tevs_i2c_write_16b(tevs, TEVS_FLICK_CTRL, val);
    }
        break;
	case V4L2_CID_WHITE_BALANCE_TEMPERATURE:
        ret = tevs_i2c_write_16b(tevs, TEVS_AWB_MANUAL_TEMP,
				        ctrl->val & TEVS_AWB_MANUAL_TEMP_MASK);
        break;
	case V4L2_CID_SHARPNESS:
        ret = tevs_i2c_write_16b(tevs, TEVS_SHARPEN,
				        ctrl->val & TEVS_SHARPEN_MASK);
        break;
	case V4L2_CID_BACKLIGHT_COMPENSATION:
        ret = tevs_i2c_write_16b(tevs, TEVS_BACKLIGHT_COMPENSATION,
				        ctrl->val & TEVS_BACKLIGHT_COMPENSATION_MASK);
        break;
	case V4L2_CID_COLORFX: {
        u16 val;
        switch (ctrl->val) {
        case 0:
            val = TEVS_SFX_MODE_SFX_NORMAL;
            break;
        case 1:
            val = TEVS_SFX_MODE_SFX_BW;
            break;
        case 2:
            val = TEVS_SFX_MODE_SFX_GRAYSCALE;
            break;
        case 3:
            val = TEVS_SFX_MODE_SFX_NEGATIVE;
            break;
        case 4:
            val = TEVS_SFX_MODE_SFX_SKETCH;
            break;
        default:
            val = TEVS_SFX_MODE_SFX_NORMAL;
            break;
        }
        ret = tevs_i2c_write_16b(tevs, TEVS_SFX_MODE, val);
        break;
    }
	case V4L2_CID_EXPOSURE_AUTO: {
        u16 val;
        switch (ctrl->val) {
        case 0:
            val = TEVS_AE_CTRL_MANUAL_EXP_TIME_GAIN;
            break;
        case 1:
            val = TEVS_AE_CTRL_FULL_AUTO;
            break;
        default:
            val = TEVS_AE_CTRL_FULL_AUTO;
            break;
        }
        ret = tevs_i2c_write_16b(tevs, TEVS_AE_CTRL_MODE, val);
        break;
    }
	case V4L2_CID_PAN_ABSOLUTE:
        ret = tevs_i2c_write_16b(tevs, TEVS_DZ_CT_X,
				        ctrl->val & TEVS_DZ_CT_X_MASK);
        break;
	case V4L2_CID_TILT_ABSOLUTE:
        ret = tevs_i2c_write_16b(tevs, TEVS_DZ_CT_Y,
				        ctrl->val & TEVS_DZ_CT_Y_MASK);
        break;
	case V4L2_CID_ZOOM_ABSOLUTE:
        ret = tevs_i2c_write_16b(tevs, TEVS_DZ_TGT_FCT,
				        ctrl->val & TEVS_DZ_TGT_FCT_MASK);
        break;
	case V4L2_CID_TEVS_BSL_MODE: {
    	u8 bootcmd[6] = {0x00, 0x12, 0x3A, 0x61, 0x44, 0xDE};
        u8 startup[6] = {0x00, 0x40, 0xE2, 0x51, 0x21, 0x5B};
        u8 tmp;
        dev_dbg(&client->dev, "%s(): set bls mode: %d", __func__, ctrl->val);

        switch (ctrl->val) {
        case 0:
            ret = tevs_i2c_write(tevs, 0x8001, startup, 6);
            ret = tevs_i2c_read(tevs, 0x8001, &tmp, 1);
            break;
        case 1:
			ret = regulator_bulk_disable(TEVS_NUM_SUPPLIES, tevs->supplies);
            gpiod_set_value_cansleep(tevs->reset_gpio, 0);
            usleep_range(9000, 10000);
            gpiod_set_value_cansleep(tevs->standby_gpio, 1);
            msleep(100);
			ret = regulator_bulk_enable(TEVS_NUM_SUPPLIES, tevs->supplies);
            gpiod_set_value_cansleep(tevs->reset_gpio, 1);
            usleep_range(9000, 10000);
            gpiod_set_value_cansleep(tevs->standby_gpio, 0);
            msleep(100);
            ret = tevs_i2c_write(tevs, 0x8001, bootcmd, 6);
            msleep(100);
            ret = tevs_i2c_read(tevs, 0x8001, &tmp, 1);
            break;
        default:
            dev_err(&client->dev, "%s(): set err bls mode: %d", __func__, ctrl->val);
            ret = -EINVAL;
            break;
        }
        break;
    }
	default:
		dev_dbg(&client->dev, "Unknown control 0x%x\n",
			ctrl->id);
		return -EINVAL;
	}

    return ret;
}

static int tevs_g_ctrl(struct v4l2_ctrl *ctrl)
{
	struct tevs *tevs = container_of(ctrl->handler, struct tevs, ctrls);
	struct i2c_client *client = v4l2_get_subdevdata(&tevs->v4l2_subdev);
	u16 val;
	int ret;

	switch (ctrl->id) {
	case V4L2_CID_BRIGHTNESS:
        ret = tevs_i2c_read_16b(tevs, TEVS_BRIGHTNESS, &val);
        if (ret)
            break;

        ctrl->val = val & TEVS_BRIGHTNESS_MASK;
        break;
	case V4L2_CID_CONTRAST:
        ret = tevs_i2c_read_16b(tevs, TEVS_CONTRAST, &val);
        if (ret)
            break;
        ctrl->val = val & TEVS_CONTRAST_MASK;
        break;
	case V4L2_CID_SATURATION:
        ret = tevs_i2c_read_16b(tevs, TEVS_SATURATION, &val);
        if (ret)
            break;

        ctrl->val = val & TEVS_SATURATION_MASK;
        break;
	case V4L2_CID_AUTO_WHITE_BALANCE:
        ret = tevs_i2c_read_16b(tevs, TEVS_AWB_CTRL_MODE, &val);
        if (ret)
            break;

        switch (val & TEVS_AWB_CTRL_MODE_MASK) {
        case TEVS_AWB_CTRL_MODE_MANUAL_TEMP:
            ctrl->val = 0;
            break;
        case TEVS_AWB_CTRL_MODE_AUTO:
            ctrl->val = 1;
            break;
        default:
            ctrl->val = 1;
            break;
        }
        break;
	case V4L2_CID_GAMMA:
        ret = tevs_i2c_read_16b(tevs, TEVS_GAMMA, &val);
        if (ret)
            break;

        ctrl->val = val & TEVS_GAMMA_MASK;
        break;
	case V4L2_CID_EXPOSURE: {
        u8 exp[4] = { 0 };
        ret = tevs_i2c_read(tevs, TEVS_AE_MANUAL_EXP_TIME, exp, 4);
        if (ret)
            break;

        ctrl->val = be32_to_cpup((__be32*)exp);
        break;
    }
	case V4L2_CID_GAIN:
        ret = tevs_i2c_read_16b(tevs, TEVS_AE_MANUAL_GAIN, &val);
        if (ret)
            break;

        ctrl->val = val & TEVS_AE_MANUAL_GAIN_MASK;
        break;
	case V4L2_CID_HFLIP:
        ret = tevs_i2c_read_16b(tevs, TEVS_ORIENTATION, &val);
        if (ret)
            break;

        ctrl->val = !!(val & TEVS_ORIENTATION_HFLIP);
        break;
	case V4L2_CID_VFLIP:
        ret = tevs_i2c_read_16b(tevs, TEVS_ORIENTATION, &val);
        if (ret)
            break;

        ctrl->val = !!(val & TEVS_ORIENTATION_VFLIP);
        break;
	case V4L2_CID_POWER_LINE_FREQUENCY:
        ret = tevs_i2c_read_16b(tevs, TEVS_FLICK_CTRL, &val);
        if (ret)
            break;

        switch (val & TEVS_FLICK_CTRL_MODE_MASK)
        {
        case TEVS_FLICK_CTRL_MODE_DISABLED:
            ctrl->val = 0;
            break;
        case TEVS_FLICK_CTRL_MODE_MANUAL:
            if((val & TEVS_FLICK_CTRL_FREQ_MASK) == TEVS_FLICK_CTRL_FREQ(50))
                ctrl->val = 1;
            else if((val & TEVS_FLICK_CTRL_FREQ_MASK)  == TEVS_FLICK_CTRL_FREQ(60))
                ctrl->val = 2;
            break;
        case TEVS_FLICK_CTRL_MODE_AUTO:
            ctrl->val = 3;
            break;
        default:
            ctrl->val = 0;
            break;
        }
        break;
	case V4L2_CID_WHITE_BALANCE_TEMPERATURE:
        ret = tevs_i2c_read_16b(tevs, TEVS_AWB_MANUAL_TEMP, &val);
        if (ret)
            break;

        ctrl->val = val & TEVS_AWB_MANUAL_TEMP_MASK;
        break;
	case V4L2_CID_SHARPNESS:
        ret = tevs_i2c_read_16b(tevs, TEVS_SHARPEN, &val);
        if (ret)
            break;

        ctrl->val = val & TEVS_SHARPEN_MASK;
        break;
	case V4L2_CID_BACKLIGHT_COMPENSATION:
        ret = tevs_i2c_read_16b(tevs, TEVS_BACKLIGHT_COMPENSATION, &val);
        if (ret)
            break;

        ctrl->val = val & TEVS_BACKLIGHT_COMPENSATION_MASK;
        break;
	case V4L2_CID_COLORFX:    
        ret = tevs_i2c_read_16b(tevs, TEVS_SFX_MODE, &val);
        if (ret)
            break;

        switch (val & TEVS_SFX_MODE_SFX_MASK) {
        case TEVS_SFX_MODE_SFX_NORMAL:
            ctrl->val = 0;
            break;
        case TEVS_SFX_MODE_SFX_BW:
            ctrl->val = 1;
            break;
        case TEVS_SFX_MODE_SFX_GRAYSCALE:
            ctrl->val = 2;
            break;
        case TEVS_SFX_MODE_SFX_NEGATIVE:
            ctrl->val = 3;
            break;
        case TEVS_SFX_MODE_SFX_SKETCH:
            ctrl->val = 4;
            break;
        default:
            ctrl->val = 0;
            break;
        }
        break;
	case V4L2_CID_EXPOSURE_AUTO:
        ret = tevs_i2c_read_16b(tevs, TEVS_AE_CTRL_MODE, &val);
        if (ret)
            break;

        switch (val & TEVS_AE_CTRL_MODE_MASK) {
        case TEVS_AE_CTRL_MANUAL_EXP_TIME_GAIN:
            ctrl->val = 0;
            break;
        case TEVS_AE_CTRL_FULL_AUTO:
            ctrl->val = 1;
            break;
        default:
            ctrl->val = 1;
            break;
        }
        break;
	case V4L2_CID_PAN_ABSOLUTE:
        ret = tevs_i2c_read_16b(tevs, TEVS_DZ_CT_X, &val);
        if (ret)
            break;

        ctrl->val = val & TEVS_DZ_CT_X_MASK;
        break;
	case V4L2_CID_TILT_ABSOLUTE:
        ret = tevs_i2c_read_16b(tevs, TEVS_DZ_CT_Y, &val);
        if (ret)
            break;

        ctrl->val = val & TEVS_DZ_CT_Y_MASK;
        break;
	case V4L2_CID_ZOOM_ABSOLUTE:
        ret = tevs_i2c_read_16b(tevs, TEVS_DZ_TGT_FCT, &val);
        if (ret)
            break;

        ctrl->val = val & TEVS_DZ_TGT_FCT_MASK;
        break;
	case V4L2_CID_TEVS_BSL_MODE:
		// ctrl->val = 0;
		// dev_info(tevs->dev, "get tevs flash mode: %d", ctrl->val);
		return 0;
	default:
		dev_dbg(&client->dev, "Unknown control 0x%x\n",
			ctrl->id);
		return -EINVAL;
	}

    return ret;
}

static const struct v4l2_subdev_core_ops tevs_v4l2_subdev_core_ops = {
	// .s_power = tevs_power,
	.subscribe_event = v4l2_ctrl_subdev_subscribe_event,
	.unsubscribe_event = v4l2_event_subdev_unsubscribe,
};

static const struct v4l2_subdev_video_ops tevs_v4l2_subdev_video_ops = {
	.g_frame_interval = tevs_get_frame_interval,
	.s_frame_interval = tevs_set_frame_interval,
	.s_stream = tevs_set_stream,
};

static const struct v4l2_subdev_pad_ops tevs_v4l2_subdev_pad_ops = {
	.enum_mbus_code = tevs_enum_mbus_code,
	.get_fmt = tevs_get_fmt,
	.set_fmt = tevs_set_fmt,
	.get_selection = tevs_get_selection,
	.enum_frame_size = tevs_enum_frame_size,
	.enum_frame_interval = tevs_enum_frame_interval,
};

static const struct v4l2_subdev_ops tevs_subdev_ops = {
	.core = &tevs_v4l2_subdev_core_ops,
	.video = &tevs_v4l2_subdev_video_ops,
	.pad = &tevs_v4l2_subdev_pad_ops,
};

static const struct v4l2_subdev_internal_ops tevs_internal_ops = {
	.open = tevs_open,
};

static const struct v4l2_ctrl_ops tevs_ctrl_ops = {
	.s_ctrl = tevs_s_ctrl,
};

static const struct v4l2_ctrl_config tevs_ctrls[] = {
	{
		.ops = &tevs_ctrl_ops,
		.id = V4L2_CID_BRIGHTNESS,
		.name = "Brightness",
		.type = V4L2_CTRL_TYPE_INTEGER,
		.min = 0x0,
		.max = 0xFFFF,
		.step = 0x1,
		.def = 0x0,
	},
	{
		.ops = &tevs_ctrl_ops,
		.id = V4L2_CID_CONTRAST,
		.name = "Contrast",
		.type = V4L2_CTRL_TYPE_INTEGER,
		.min = 0x0,
		.max = 0xFFFF,
		.step = 0x1,
		.def = 0x0,
	},
	{
		.ops = &tevs_ctrl_ops,
		.id = V4L2_CID_SATURATION,
		.name = "Saturation",
		.type = V4L2_CTRL_TYPE_INTEGER,
		.min = 0x0,
		.max = 0xFFFF,
		.step = 0x1,
		.def = 0x0,
	},
	{
		.ops = &tevs_ctrl_ops,
		.id = V4L2_CID_AUTO_WHITE_BALANCE,
		.name = "White_Balance_Mode",
		.type = V4L2_CTRL_TYPE_MENU,
		.max = TEVS_AWB_CTRL_MODE_AUTO_IDX,
		.def = TEVS_AWB_CTRL_MODE_AUTO_IDX,
		.qmenu = awb_mode_strings,
	},
	{
		.ops = &tevs_ctrl_ops,
		.id = V4L2_CID_GAMMA,
		.name = "Gamma",
		.type = V4L2_CTRL_TYPE_INTEGER,
		.min = 0x0,
		.max = 0xFFFF,
		.step = 0x1,
		.def = 0x2333, // 2.2
	},
	{
		.ops = &tevs_ctrl_ops,
		.id = V4L2_CID_EXPOSURE,
		.name = "Exposure",
		.type = V4L2_CTRL_TYPE_INTEGER,
		.min = 0x0,
		.max = 0xF4240,
		.step = 1,
		.def = 0x8235, // 33333 us
	},
	{
		.ops = &tevs_ctrl_ops,
		.id = V4L2_CID_GAIN,
		.name = "Gain",
		.type = V4L2_CTRL_TYPE_INTEGER,
		.min = 0x1,
		.max = 0x40,
		.step = 0x1,
		.def = 0x1,
	},
	{
		.ops = &tevs_ctrl_ops,
		.id = V4L2_CID_HFLIP,
		.name = "HFlip",
		.type = V4L2_CTRL_TYPE_BOOLEAN,
		.min = 0,
		.max = 1,
		.step = 1,
		.def = 0,
	},
	{
		.ops = &tevs_ctrl_ops,
		.id = V4L2_CID_VFLIP,
		.name = "VFlip",
		.type = V4L2_CTRL_TYPE_BOOLEAN,
		.min = 0,
		.max = 1,
		.step = 1,
		.def = 0,
	},
	{
		.ops = &tevs_ctrl_ops,
		.id = V4L2_CID_POWER_LINE_FREQUENCY,
		.name = "Power_Line_Frequency",
		.type = V4L2_CTRL_TYPE_MENU,
		.max = TEVS_FLICK_MODE_ENABLED_IDX,
		.def = TEVS_FLICK_MODE_DISABLED_IDX,
		.qmenu = flick_mode_strings,
	},
	{
		.ops = &tevs_ctrl_ops,
		.id = V4L2_CID_WHITE_BALANCE_TEMPERATURE,
		.name = "White_Balance_Temperature",
		.type = V4L2_CTRL_TYPE_INTEGER,
		.min = 0x8FC,
		.max = 0x3A98,
		.step = 0x1,
		.def = 0x1388,
	},
	{
		.ops = &tevs_ctrl_ops,
		.id = V4L2_CID_SHARPNESS,
		.name = "Sharpness",
		.type = V4L2_CTRL_TYPE_INTEGER,
		.min = 0x0,
		.max = 0xFFFF,
		.step = 0x1,
		.def = 0x0,
	},
	{
		.ops = &tevs_ctrl_ops,
		.id = V4L2_CID_BACKLIGHT_COMPENSATION,
		.name = "Backlight_Compensation",
		.type = V4L2_CTRL_TYPE_INTEGER,
		.min = 0x0,
		.max = 0xFFFF,
		.step = 0x1,
		.def = 0x0,
	},
	{
		.ops = &tevs_ctrl_ops,
		.id = V4L2_CID_COLORFX,
		.name = "Special_Effect",
		.type = V4L2_CTRL_TYPE_MENU,
		.max = TEVS_SFX_MODE_SFX_SKETCH_IDX,
		.def = TEVS_SFX_MODE_SFX_NORMAL_IDX,
		.qmenu = sfx_mode_strings,
	},
	{
		.ops = &tevs_ctrl_ops,
		.id = V4L2_CID_EXPOSURE_AUTO,
		.name = "Exposure_Mode",
		.type = V4L2_CTRL_TYPE_MENU,
		.max = TEVS_AE_CTRL_FULL_AUTO_IDX,
		.def = TEVS_AE_CTRL_FULL_AUTO_IDX,
		.qmenu = ae_mode_strings,
	},
	{
		.ops = &tevs_ctrl_ops,
		.id = V4L2_CID_PAN_ABSOLUTE,
		.name = "Pan_Target",
		.type = V4L2_CTRL_TYPE_INTEGER,
		.min = 0x0,
		.max = 0xFFFF,
		.step = 0x1,
		.def = 0x0,
	},
	{
		.ops = &tevs_ctrl_ops,
		.id = V4L2_CID_TILT_ABSOLUTE,
		.name = "Tilt_Target",
		.type = V4L2_CTRL_TYPE_INTEGER,
		.min = 0x0,
		.max = 0xFFFF,
		.step = 0x1,
		.def = 0x0,
	},
	{
		.ops = &tevs_ctrl_ops,
		.id = V4L2_CID_ZOOM_ABSOLUTE,
		.name = "Zoom_Target",
		.type = V4L2_CTRL_TYPE_INTEGER,
		.min = 0x0,
		.max = 0xFFFF,
		.step = 0x1,
		.def = 0x0,
	},
	{
		.ops = &tevs_ctrl_ops,
		.id = V4L2_CID_TEVS_BSL_MODE,
		.name = "BSL_Mode",
		.type = V4L2_CTRL_TYPE_MENU,
		.max = TEVS_BSL_MODE_FLASH_IDX,
		.def = TEVS_BSL_MODE_NORMAL_IDX,
		.qmenu = bsl_mode_strings,
	},
};

static int tevs_ctrls_init(struct tevs *tevs)
{
	struct i2c_client *client = v4l2_get_subdevdata(&tevs->v4l2_subdev);
	unsigned int i;
	u16 val;
	int ret;

	ret = v4l2_ctrl_handler_init(&tevs->ctrls, ARRAY_SIZE(tevs_ctrls));
	if (ret)
		return ret;

	for (i = 0; i < ARRAY_SIZE(tevs_ctrls); i++) {
		struct v4l2_ctrl *ctrl = v4l2_ctrl_new_custom(
			&tevs->ctrls, &tevs_ctrls[i], NULL);
		ret = tevs_g_ctrl(ctrl);
		if (!ret && ctrl->default_value != ctrl->val) {
			// Updating default value based on firmware values
			dev_dbg(
				&client->dev,
				"Ctrl '%s' default value updated from %lld to %d\n",
				ctrl->name, ctrl->default_value, ctrl->val);
			ctrl->default_value = ctrl->val;
			ctrl->cur.val = ctrl->val;
		}
		// Updating maximum and minimum value
		switch (ctrl->id) {
		case V4L2_CID_BRIGHTNESS:
            // get brightness maximum value
            ret = tevs_i2c_read_16b(tevs, TEVS_BRIGHTNESS_MAX, &val);
            if (ret)
			    break;

            ctrl->maximum = val & TEVS_BRIGHTNESS_MASK;
            // get brightness minimum value
            ret = tevs_i2c_read_16b(tevs, TEVS_BRIGHTNESS_MIN, &val);
            if (ret)
			    break;

            ctrl->minimum = val & TEVS_BRIGHTNESS_MASK;
			break;
		case V4L2_CID_CONTRAST:
            // get contrast maximum value
            ret = tevs_i2c_read_16b(tevs, TEVS_CONTRAST_MAX, &val);
            if (ret)
			    break;

            ctrl->maximum = val & TEVS_CONTRAST_MASK;
            // get contrast minimum value
            ret = tevs_i2c_read_16b(tevs, TEVS_CONTRAST_MIN, &val);
            if (ret)
			    break;

            ctrl->minimum = val & TEVS_CONTRAST_MASK;
			break;
		case V4L2_CID_SATURATION:
            // get saturation maximum value
            ret = tevs_i2c_read_16b(tevs, TEVS_SATURATION_MAX, &val);
            if (ret)
			    break;

            ctrl->maximum = val & TEVS_SATURATION_MASK;
            // get saturation minimum value
            ret = tevs_i2c_read_16b(tevs, TEVS_SATURATION_MIN, &val);
            if (ret)
			    break;

            ctrl->minimum = val & TEVS_SATURATION_MASK;
			break;
		case V4L2_CID_GAMMA:
            // get gamma maximum value
            ret = tevs_i2c_read_16b(tevs, TEVS_GAMMA_MAX, &val);
            if (ret)
			    break;

            ctrl->maximum = val & TEVS_GAMMA_MASK;
            // get gamma minimum value
            ret = tevs_i2c_read_16b(tevs, TEVS_GAMMA_MIN, &val);
            if (ret)
			    break;

            ctrl->minimum = val & TEVS_GAMMA_MASK;
			break;
		case V4L2_CID_EXPOSURE: {
            u8 exp[4] = { 0 };
            // get exposure time maximum value 
            ret = tevs_i2c_read(tevs,TEVS_AE_MANUAL_EXP_TIME_MAX, exp, 4);
            if (ret)
			    break;

            ctrl->maximum = be32_to_cpup((__be32*)exp);
            // get exposure time minimum value 
            ret = tevs_i2c_read(tevs,TEVS_AE_MANUAL_EXP_TIME_MIN, exp, 4);
            if (ret)
			    break;

            ctrl->minimum = be32_to_cpup((__be32*)exp);
            break;
        }
		case V4L2_CID_GAIN:
            // get exposure gain maximum value
            ret = tevs_i2c_read_16b(tevs, TEVS_AE_MANUAL_GAIN_MAX, &val);
            if (ret)
			    break;

            ctrl->maximum = val & TEVS_AE_MANUAL_GAIN_MASK;
            // get exposure gamma minimum value
            ret = tevs_i2c_read_16b(tevs, TEVS_AE_MANUAL_GAIN_MIN, &val);
            if (ret)
			    break;

            ctrl->minimum = val & TEVS_AE_MANUAL_GAIN_MASK;
			break;
		case V4L2_CID_WHITE_BALANCE_TEMPERATURE:
            // get white balance temperature maximum value
            ret = tevs_i2c_read_16b(tevs, TEVS_AWB_MANUAL_TEMP_MAX, &val);
            if (ret)
			    break;

            ctrl->maximum = val & TEVS_AWB_MANUAL_TEMP_MASK;
            // get white balance temperature minimum value
            ret = tevs_i2c_read_16b(tevs, TEVS_AWB_MANUAL_TEMP_MIN, &val);
            if (ret)
			    break;

            ctrl->minimum = val & TEVS_AWB_MANUAL_TEMP_MASK;
			break;
		case V4L2_CID_SHARPNESS:
            // get sharpness maximum value
            ret = tevs_i2c_read_16b(tevs, TEVS_SHARPEN_MAX, &val);
            if (ret)
			    break;

            ctrl->maximum = val & TEVS_SHARPEN_MASK;
            // get sharpness minimum value
            ret = tevs_i2c_read_16b(tevs, TEVS_SHARPEN_MIN, &val);
            if (ret)
			    break;

            ctrl->minimum = val & TEVS_SHARPEN_MASK;
			break;
		case V4L2_CID_BACKLIGHT_COMPENSATION:
            // get backlight compensation maximum value
            ret = tevs_i2c_read_16b(tevs, TEVS_BACKLIGHT_COMPENSATION_MAX,
                                &val);
            if (ret)
			    break;

            ctrl->maximum = val & TEVS_BACKLIGHT_COMPENSATION_MASK;
            // get backlight compensation minimum value
            ret = tevs_i2c_read_16b(tevs, TEVS_BACKLIGHT_COMPENSATION_MIN,
                                &val);
            if (ret)
			    break;

            ctrl->minimum = val & TEVS_BACKLIGHT_COMPENSATION_MASK;
			break;
		case V4L2_CID_PAN_ABSOLUTE:
		case V4L2_CID_TILT_ABSOLUTE:
            // get pan and tilt maximum value
            ret = tevs_i2c_read_16b(tevs, TEVS_DZ_CT_MAX, &val);
            if (ret)
			    break;

            ctrl->maximum = val & TEVS_DZ_CT_X_MASK;
            // get pan and tilt minimum value
            ret = tevs_i2c_read_16b(tevs, TEVS_DZ_CT_MIN, &val);
            if (ret)
			    break;

            ctrl->minimum = val & TEVS_DZ_CT_X_MASK;
			break;
		case V4L2_CID_ZOOM_ABSOLUTE:
            // get zoom absolute maximum value
            ret = tevs_i2c_read_16b(tevs, TEVS_DZ_TGT_FCT_MAX, &val);
            if (ret)
			    break;

            ctrl->maximum = val & TEVS_DZ_TGT_FCT_MASK;
            // get zoom absolute minimum value
            ret = tevs_i2c_read_16b(tevs, TEVS_DZ_TGT_FCT_MIN, &val);
            if (ret)
			    break;

            ctrl->minimum = val & TEVS_DZ_TGT_FCT_MASK;
			break;            
		default:
			break;
		}
	}

	if (tevs->ctrls.error) {
		dev_err(&client->dev, "ctrls error\n");
		ret = tevs->ctrls.error;
		v4l2_ctrl_handler_free(&tevs->ctrls);
		return ret;
	}

	/* Use same mutex for controls as for everything else. */
	tevs->ctrls.lock = &tevs->mutex;
	tevs->v4l2_subdev.ctrl_handler = &tevs->ctrls;

	return 0;
}

static void tevs_ctrls_free(struct tevs *tevs)
{
    v4l2_ctrl_handler_free(&tevs->ctrls);
	mutex_destroy(&tevs->mutex);
}

static int tevs_check_hwcfg(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct v4l2_subdev *sub_dev = i2c_get_clientdata(client);
	struct tevs *tevs = to_tevs(sub_dev);
    int ret = 0;
	tevs->reset_gpio =
		devm_gpiod_get_optional(dev, "VANA-supply", GPIOD_OUT_HIGH);
	if (IS_ERR(tevs->reset_gpio)) {
		ret = PTR_ERR(tevs->reset_gpio);
		if (ret != -EPROBE_DEFER)
			dev_err(dev, "Cannot get reset GPIO (%d)", ret);
		return ret;
	}

	tevs->standby_gpio =
		devm_gpiod_get_optional(dev, "standby", GPIOD_OUT_LOW);
	if (IS_ERR(tevs->standby_gpio)) {
		ret = PTR_ERR(tevs->standby_gpio);
		if (ret != -EPROBE_DEFER)
			dev_err(dev, "Cannot get standby GPIO (%d)", ret);
		return ret;
	}

	tevs->data_lanes = 0;
	if (of_property_read_u32(dev->of_node, "data-lanes", &tevs->data_lanes) ==
	    0) {
		if ((tevs->data_lanes < 1) || (tevs->data_lanes > 4)) {
			dev_err(dev,
				"value of 'data-lanes' property is invaild\n");
			tevs->data_lanes = 4;
		}
	}

	tevs->continuous_clock = 0;
	if (of_property_read_u32(dev->of_node, "continuous-clock",
				 &tevs->continuous_clock) == 0) {
		if (tevs->continuous_clock > 1) {
			dev_err(dev,
				"value of 'continuous-clock' property is invaild\n");
			tevs->continuous_clock = 0;
		}
	}

	tevs->data_frequency = 0;
	if (of_property_read_u32(dev->of_node, "data-frequency",
				 &tevs->data_frequency) == 0) {
		if ((tevs->data_frequency != 0) && ((tevs->data_frequency < 100) || (tevs->data_frequency > 1200))) {
			dev_err(dev,
				"value of 'data-frequency = <%d>' property is invaild\n", tevs->data_frequency);
			return -EINVAL;
		}
	}

	tevs->hw_reset_mode =
		of_property_read_bool(dev->of_node, "hw-reset");

	tevs->trigger_mode = 
		of_property_read_bool(dev->of_node, "trigger-mode");

	dev_dbg(dev,
		"data-lanes [%d], continuous-clock [%d], hw-reset [%d], "
        "trigger-mode [%d]\n",
		tevs->data_lanes, tevs->continuous_clock, tevs->hw_reset_mode, 
        tevs->trigger_mode);

    return ret;
}

static int tevs_probe(struct i2c_client *client)
{
	struct device *dev = &client->dev;
	struct tevs *tevs = NULL;
	struct v4l2_mbus_framefmt *fmt;
	int i = ARRAY_SIZE(tevs_sensor_table);
	int ret;

	dev_info(dev, "%s() device node: %s\n", __func__,
		    client->dev.of_node->full_name);

	tevs = devm_kzalloc(dev, sizeof(struct tevs), GFP_KERNEL);
	if (!tevs) {
		dev_err(dev, "allocate memory failed\n");
		return -ENOMEM;
	}

	v4l2_i2c_subdev_init(&tevs->v4l2_subdev, client, &tevs_subdev_ops);

	i2c_set_clientdata(client, tevs);
	tevs->regmap = devm_regmap_init_i2c(client, &tevs_regmap_config);
	if (IS_ERR(tevs->regmap)) {
		dev_err(dev, "Unable to initialize I2C\n");
		return -ENODEV;
	}

	/* Check the hardware configuration in device tree */
    if (tevs_check_hwcfg(dev))
		return -EINVAL;

	ret = tevs_get_regulators(tevs);
	if (ret) {
		dev_err(dev, "failed to get regulators\n");
		return ret;
	}

    ret = tevs_power_on(dev);
	if (ret != 0) {
		dev_err(dev, "cannot find tevs camera\n");
		return ret;
	}

	if (tevs->data_frequency != 0) {
		ret = tevs_i2c_write_16b(tevs,
					HOST_COMMAND_ISP_CTRL_MIPI_FREQ,
					tevs->data_frequency);
		msleep(TEVS_BOOT_TIME);
		if (tevs_check_boot_state(tevs) != 0) {
			dev_err(dev, "check tevs bootup status failed\n");
			return -EINVAL;
		}
		if (ret < 0) {
			dev_err(dev, "set mipi frequency failed\n");
			return -EINVAL;
		}
	}

	ret = tevs_check_version(tevs);
	if (ret < 0) {
		dev_err(dev, "dev init failed\n");
		goto error_power_off;
	}

	tevs->header_info = devm_kzalloc(
			dev, sizeof(struct header_info), GFP_KERNEL);
	if (tevs->header_info == NULL) {
		dev_err(dev, "allocate header_info failed\n");
		ret = -ENOMEM;
        goto error_power_off;
	}

	ret = tevs_load_header_info(tevs);
	if (ret < 0) {
		dev_err(dev, "otp flash init failed\n");
        goto error_power_off;
	} else {
		for (i = 0; i < ARRAY_SIZE(tevs_sensor_table); i++) {
			if (strcmp((const char *)tevs->header_info
					   ->product_name,
				   tevs_sensor_table[i].sensor_name) == 0)
				break;
		}
	}

	if (i >= ARRAY_SIZE(tevs_sensor_table)) {
		dev_err(dev, "cannot not support the product: %s\n",
			(const char *)tevs->header_info->product_name);
		ret = -EINVAL;
        goto error_power_off;
	}

	tevs->selected_sensor = i;
	dev_dbg(dev, "selected_sensor:%d, sensor_name:%s\n", i,
		tevs->header_info->product_name);

	/* Initialize default format */
	fmt = &tevs->fmt;
	fmt->width =
		tevs_sensor_table[tevs->selected_sensor].res_list[0].width;
	fmt->height = 
        tevs_sensor_table[tevs->selected_sensor].res_list[0].height;
	fmt->field = V4L2_FIELD_NONE;
	fmt->code = MEDIA_BUS_FMT_UYVY8_1X16;
	fmt->colorspace = V4L2_COLORSPACE_SRGB;
	fmt->ycbcr_enc = V4L2_MAP_YCBCR_ENC_DEFAULT(fmt->colorspace);
	fmt->quantization = V4L2_QUANTIZATION_FULL_RANGE;
	fmt->xfer_func = V4L2_MAP_XFER_FUNC_DEFAULT(fmt->colorspace);
	// memset(fmt->reserved, 0, sizeof(fmt->reserved));
    
	ret = tevs_ctrls_init(tevs);
	if (ret) {
		dev_err(&client->dev, "failed to init controls: %d", ret);
		goto error_power_off;
	}

    /* Initialize subdev */
	tevs->v4l2_subdev.internal_ops = &tevs_internal_ops;
	// tevs->v4l2_subdev.entity.ops = &tevs_media_entity_ops;
	tevs->v4l2_subdev.flags |=
		(V4L2_SUBDEV_FL_HAS_EVENTS | V4L2_SUBDEV_FL_HAS_DEVNODE);
	tevs->v4l2_subdev.entity.function = MEDIA_ENT_F_CAM_SENSOR;
	
	/* Initialize source pads */
	tevs->pad.flags = MEDIA_PAD_FL_SOURCE;

	ret = media_entity_pads_init(&tevs->v4l2_subdev.entity, 1, &tevs->pad);
	if (ret) {
		dev_err(dev, "failed to init entity pads: %d\n", ret);
		goto error_handler_free;
	}

	ret = v4l2_async_register_subdev_sensor(&tevs->v4l2_subdev);
	if (ret != 0) {
		dev_err(dev, "failed to register sensor sub-device: %d\n", ret);
		goto error_media_entity;
	}

    dev_info(dev, "probe success\n");

	/* Enable runtime PM and turn off the device */
	pm_runtime_set_active(dev);
	pm_runtime_enable(dev);
	pm_runtime_idle(dev);

	return 0;

error_media_entity:
	media_entity_cleanup(&tevs->v4l2_subdev.entity);

error_handler_free:
	tevs_ctrls_free(tevs);

error_power_off:
	tevs_power_off(dev);

	return ret;
}

static void tevs_remove(struct i2c_client *client)
{
	struct v4l2_subdev *sub_dev = i2c_get_clientdata(client);
	struct tevs *tevs = to_tevs(sub_dev);

	v4l2_async_unregister_subdev(sub_dev);
	media_entity_cleanup(&sub_dev->entity);
    tevs_ctrls_free(tevs);

	pm_runtime_disable(&client->dev);
	if (!pm_runtime_status_suspended(&client->dev))
		tevs_power_off(&client->dev);
	pm_runtime_set_suspended(&client->dev);
}

static const struct of_device_id tevs_dt_ids[] = { { .compatible =
							   "tn,tevs" },
						 { /* sentinel */ } };
MODULE_DEVICE_TABLE(of, tevs_dt_ids);

static const struct dev_pm_ops tevs_pm_ops = {
	SET_SYSTEM_SLEEP_PM_OPS(tevs_suspend, tevs_resume)
	SET_RUNTIME_PM_OPS(tevs_power_off, tevs_power_on, NULL)
};

static struct i2c_driver sensor_i2c_driver = {
	.driver = {
		.name  = "tevs",
		.of_match_table = tevs_dt_ids,
		.pm = &tevs_pm_ops,
	},
	.probe = tevs_probe,
	.remove = tevs_remove,
};

module_i2c_driver(sensor_i2c_driver);

MODULE_AUTHOR("TECHNEXION Inc.");
MODULE_DESCRIPTION("TechNexion driver for TEVS");
MODULE_LICENSE("GPL v2");
MODULE_VERSION("1.0");
MODULE_ALIAS("Camera");
MODULE_SOFTDEP("pre: gpio_pca953x");
