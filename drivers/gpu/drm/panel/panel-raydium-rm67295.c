// SPDX-License-Identifier: GPL-2.0
/*
 * Raydium RM67295 MIPI-DSI panel driver
 *
 * Copyright 2019 NXP
 */

#include <linux/backlight.h>
#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/regulator/consumer.h>

#include <video/mipi_display.h>
#include <video/of_videomode.h>
#include <video/videomode.h>

#include <drm/drm_crtc.h>
#include <drm/drm_mipi_dsi.h>
#include <drm/drm_panel.h>
#include <drm/drm_print.h>
#include <drm/drmP.h>


/* Panel specific color-format bits */
#define COL_FMT_16BPP 0x0E
#define COL_FMT_18BBB 0x1E
#define COL_FMT_18BPP 0x2E
#define COL_FMT_24BPP 0x3E
/* Write Manufacture Command Set Control */
#define WRMAUCCTR 0xFE

#define REGFLAG_DELAY 0xFF
#define REGFLAG_END_OF_TABLE 0xFD


/* Manufacturer Command Set pages (CMD2) */
struct cmd_set_entry {
	u8 cmd;
	u8 size;
	u8 param;
};


/*
 * There is no description in the Reference Manual about these commands.
 * We received them from vendor, so just use them as is.
 */
static const struct cmd_set_entry manufacturer_cmd_set[] = {
	{0xFE, 1, {0x04}},//Manufacture Command Set Page3 ( CMD2 P3)
	{0x01, 1, {0x08}},//Soft reset  
	{0x0e, 1, {0x10}},//Get signal mode   
	{0x0A, 1, {0xE6}},//Get power mode 
	{0x0B, 1, {0x8C}},//Get address mode  
	{0x0D, 1, {0xA0}},//Get display mode  
	{0x0F, 1, {0x40}},//Get diagnostic result
	{0x16, 1, {0x19}},// 
	{0x29, 1, {0x93}},//Set display on  
	{0x2A, 1, {0x93}},//
	{0x2F, 1, {0x02}},//
	{0x31, 1, {0x02}},//
	{0x33, 1, {0x05}},//
	{0x37, 1, {0x2F}},//
	{0x38, 1, {0x2F}},//Exit idle mode 
	{0x3A, 1, {0x1E}},//Set pixel format  
	{0x3B, 1, {0x1E}},//
	{0x3D, 1, {0x1D}},//
	{0x3F, 1, {0x80}},//
	{0x40, 1, {0x48}},//
	{0x41, 1, {0xE0}},//
	{0x4F, 1, {0x2F}},//Set deep standby mode
	{0x50, 1, {0x1e}},//
	{0x51, 1, {0x0A}},//Write display brightness
	{0xE2, 1, {0x00}},//
	{0xFE, 1, {0x07}},//Write CMD page switch  0x01 ~ 0x05  no 0x07
	{0x03, 1, {0x40}},//
	{0x05, 1, {0x00}},//Get number of errors on DSI
	{0x07, 1, {0x1E}},//
	{0x5B, 1, {0x85}},//Read CE1  
	{0xFE, 1, {0x06}},//Write CMD page switch  0x01 ~ 0x05  no 0x06
	{0x00, 1, {0xEC}},// Nop  
	{0x05, 1, {0x08}},//Get number of errors on DSI 
	{0x07, 1, {0xE2}},//
	{0x08, 1, {0xEC}},// 
	{0x0D, 1, {0x02}},//Get display mode  
	{0x0F, 1, {0xE2}},//Get diagnostic result  
	{0x32, 1, {0xEC}},//
	{0x37, 1, {0x05}},//
	{0x39, 1, {0xA0}},//Enter idle mode  
	{0x3A, 1, {0xEC}},//Set pixel format   
	{0x41, 1, {0x04}},//
	{0x43, 1, {0xA0}},//
	{0x44, 1, {0xEC}},//Set tear scan line
	{0x49, 1, {0x08}},//
	{0x4B, 1, {0xE2}},//
	{0x4C, 1, {0xEC}},//  
	{0x51, 1, {0x02}},//Write display brightness
	{0x53, 1, {0xE2}},//Write control display  
	{0x75, 1, {0xEC}},//  
	{0x7A, 1, {0x05}},//
	{0x7C, 1, {0xA0}},//
	{0x7D, 1, {0xEC}},//
	{0x82, 1, {0x04}},//
	{0x84, 1, {0xA0}},//
	{0x85, 1, {0xEC}},//
	{0x86, 1, {0x0F}},//
	{0x87, 1, {0xFF}},//
	{0x88, 1, {0x00}},//
	{0x8A, 1, {0x02}},//
	{0x8C, 1, {0xA0}},//
	{0x8D, 1, {0xEA}},//
	{0x8E, 1, {0x0F}},//
	{0x8F, 1, {0xFF}},//
	{0x90, 1, {0x06}},//
	{0x91, 1, {0x00}},//       
	{0x92, 1, {0x05}},//
	{0x93, 1, {0xF0}},//      
	{0x94, 1, {0x28}},//
	{0x95, 1, {0xEC}},//
	{0x96, 1, {0x0F}},//
	{0x97, 1, {0xFF}},//
	{0x98, 1, {0x00}},//
	{0x9A, 1, {0x02}},//
	{0x9C, 1, {0xA0}},//
	{0xAC, 1, {0x04}},//
	{0xB1, 1, {0x12}},//
	 
	{0xB2, 1, {0x17}},//
	{0xB3, 1, {0x17}},//
	{0xB4, 1, {0x17}},//
	{0xB5, 1, {0x17}},//
	{0xB6, 1, {0x11}},//
	{0xB7, 1, {0x08}},//
	{0xB8, 1, {0x09}},//
	{0xB9, 1, {0x06}},//
	{0xBA, 1, {0x07}},//
	{0xBB, 1, {0x17}},//
	{0xBC, 1, {0x17}},//
	{0xBD, 1, {0x17}},//
	{0xBE, 1, {0x17}},//
	{0xBF, 1, {0x17}},//
	{0xC0, 1, {0x17}},//
	{0xC1, 1, {0x17}},//
	{0xC2, 1, {0x17}},//
	{0xC3, 1, {0x17}},//
	{0xC4, 1, {0x0F}},//
	{0xC5, 1, {0x0E}},//
	{0xC6, 1, {0x00}},//
	{0xC7, 1, {0x01}},//
	{0xC8, 1, {0x10}},//
	{0xfe, 1, {0x0e}},//
	{0x12, 1, {0x37}},//
	{0xFE, 1, {0x0A}}, //
	{0x25, 1, {0x66}},//
	{0xFE, 1, {0x0D}},//
	{0x72, 1, {0x09}},//
	/* shaohanyue, 20160818 modified for lcm pcd */
	{0xFE, 1, {0x0D}},//
	{0x03, 1, {0x20}},//
	/* shaohanyue, 20160818 modified for lcm pcd */
	/*shaohanyue.wt 2016.09.05 ESD fail  start*/
	{0x20, 1, {0x00}},//Exit invert mode  
	{0x21, 1, {0x00}},//Enter invert mode  
	{0x1D, 1, {0x89}},//
	/*shaohanyue.wt 2016.09.05 ESD fail  end*/
	{0xFE, 1, {0x00}},
	{0x51, 1, {0xff}},//Write display brightness  
	{0x35, 1, {0x00}},//Set tear on  
	
	//{0xFE, 1, {0x00}},//白屏测试 像素全开
	//{0x23, 1, {0x00}},//白屏测试 像素全开


	{0x11, 1, {0x00}},//Exit sleep mode  
	{REGFLAG_DELAY, 120, {0x00}},
	{0x29, 1, {0x00}},//
	{REGFLAG_DELAY, 20, {0x00}},
	{REGFLAG_END_OF_TABLE, 0x00, {0x00}},
};

static const u32 rad_bus_formats[] = {
	MEDIA_BUS_FMT_RGB888_1X24,
	MEDIA_BUS_FMT_RGB666_1X18,
	MEDIA_BUS_FMT_RGB565_1X16,
};

static const u32 rad_bus_flags = DRM_BUS_FLAG_DE_LOW |
				 DRM_BUS_FLAG_PIXDATA_NEGEDGE;

struct rad_panel {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;

	struct gpio_desc *reset;
	struct backlight_device *backlight;

	struct regulator_bulk_data *supplies;
	unsigned int num_supplies;

	bool prepared;
	bool enabled;
};

static const struct drm_display_mode default_mode = {
	.clock = 132000,
	.hdisplay = 1920,
	.hsync_start = 1920 + 2,
	.hsync_end = 1920 + 2 + 2,
	.htotal = 1920 + 2 + 2 + 2,
	.vdisplay = 1280,
	.vsync_start = 1280 + 4,
	.vsync_end = 1280 + 4 + 4,
	.vtotal = 1280 + 4 + 4 + 4,
	.vrefresh =60,//没起作用
	.width_mm = 61,
	.height_mm = 110,
	.flags = DRM_MODE_FLAG_NHSYNC |
		 DRM_MODE_FLAG_NVSYNC,

#if 0
	.clock = 90000,
	.hdisplay = 1920,
	.hsync_start = 1920 + 2,
	.hsync_end = 1920 + 2 + 2,
	.htotal = 1920 + 2 + 2 + 2,
	.vdisplay = 1280,
	.vsync_start = 1280 + 4,
	.vsync_end = 1280 + 4 + 4,
	.vtotal = 1280 + 4 + 4 + 4,
	.vrefresh =60,
	.width_mm = 61,
	.height_mm = 110,
	.flags = DRM_MODE_FLAG_NHSYNC |
		 DRM_MODE_FLAG_NVSYNC,

#endif



	
};

static inline struct rad_panel *to_rad_panel(struct drm_panel *panel)
{
	return container_of(panel, struct rad_panel, panel);
}

static int rad_panel_push_cmd_list(struct mipi_dsi_device *dsi)
{
	size_t i;
	size_t count = ARRAY_SIZE(manufacturer_cmd_set);
	int ret = 0;

	for (i = 0; i < count; i++) {

		const struct cmd_set_entry *entry = &manufacturer_cmd_set[i];
		u8 buffer[3] = { entry->cmd, entry->size,entry->param };
	
		switch (entry->cmd){

			case REGFLAG_DELAY :
				msleep(entry->param);//
				break;
			case REGFLAG_END_OF_TABLE :
				i = count;
                break;
            default:
            ret = 	mipi_dsi_dcs_write(dsi, entry->cmd, &entry->param, entry->size);
			if (ret < 0)
				return ret;
		}
	}

	return ret;
};


static int color_format_from_dsi_format(enum mipi_dsi_pixel_format format)
{
	switch (format) {
	case MIPI_DSI_FMT_RGB565:
		return COL_FMT_16BPP;
	
	case MIPI_DSI_FMT_RGB666:
		return COL_FMT_18BBB;
		
	case MIPI_DSI_FMT_RGB666_PACKED:
		return COL_FMT_18BPP;
		
	case MIPI_DSI_FMT_RGB888:
		return COL_FMT_24BPP;
		
	default:
		return COL_FMT_24BPP; /* for backward compatibility */
		
	}
};

static int rad_panel_prepare(struct drm_panel *panel)
{
	struct rad_panel *rad = to_rad_panel(panel);
	int ret;

	if (rad->prepared)
		return 0;

	ret = regulator_bulk_enable(rad->num_supplies, rad->supplies);
	if (ret)
		return ret;

	if (rad->reset) {
		gpiod_set_value_cansleep(rad->reset, 1);//0
		usleep_range(18000, 20000);
	}

	rad->prepared = true;

	return 0;
}

static int rad_panel_unprepare(struct drm_panel *panel)
{
	struct rad_panel *rad = to_rad_panel(panel);
	int ret;

	if (!rad->prepared)
		return 0;

	/*
	 * Right after asserting the reset, we need to release it, so that the
	 * touch driver can have an active connection with the touch controller
	 * even after the display is turned off.
	 */
	if (rad->reset) {
		gpiod_set_value_cansleep(rad->reset, 0);//1
		usleep_range(15000, 17000);
		gpiod_set_value_cansleep(rad->reset, 1);//0
	}

	ret = regulator_bulk_disable(rad->num_supplies, rad->supplies);
	if (ret)
		return ret;

	rad->prepared = false;

	return 0;
}

static int rad_panel_enable(struct drm_panel *panel)
{
	struct rad_panel *rad = to_rad_panel(panel);
	struct mipi_dsi_device *dsi = rad->dsi;
	struct device *dev = &dsi->dev;
	int color_format = color_format_from_dsi_format(dsi->format);
	int ret;

	if (rad->enabled)
		return 0;

	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	ret = rad_panel_push_cmd_list(dsi);
	if (ret < 0) {
		DRM_DEV_ERROR(dev, "Failed to send MCS (%d)\n", ret);
		goto fail;
	}
	
#if 1

	/* Select User Command Set table (CMD1) */
	ret = mipi_dsi_generic_write(dsi, (u8[]){ WRMAUCCTR, 0x00 }, 2);
	if (ret < 0)
		goto fail;

	/* Software reset */
	ret = mipi_dsi_dcs_soft_reset(dsi);
	if (ret < 0) {
		DRM_DEV_ERROR(dev, "Failed to do Software Reset (%d)\n", ret);
		goto fail;
	}

	usleep_range(15000, 17000);

	/* Set DSI mode */
	ret = mipi_dsi_generic_write(dsi, (u8[]){ 0xC2, 0x0B }, 2);
	if (ret < 0) {
		DRM_DEV_ERROR(dev, "Failed to set DSI mode (%d)\n", ret);
		goto fail;
	}
	/* Set tear ON */
	ret = mipi_dsi_dcs_set_tear_on(dsi, MIPI_DSI_DCS_TEAR_MODE_VHBLANK);//MIPI_DSI_DCS_TEAR_MODE_VHBLANK MIPI_DSI_DCS_TEAR_MODE_VBLANK
	if (ret < 0) {
		DRM_DEV_ERROR(dev, "Failed to set tear ON (%d)\n", ret);
		goto fail;
	}
	/* Set tear scanline */
	ret = mipi_dsi_dcs_set_tear_scanline(dsi, 0x380);
	if (ret < 0) {
		DRM_DEV_ERROR(dev, "Failed to set tear scanline (%d)\n", ret);
		goto fail;
	}
	/* Set pixel format */
	ret = mipi_dsi_dcs_set_pixel_format(dsi, color_format);
	DRM_DEV_DEBUG_DRIVER(dev, "Interface color format set to 0x%x\n",
			     color_format);
	if (ret < 0) {
		DRM_DEV_ERROR(dev, "Failed to set pixel format (%d)\n", ret);
		goto fail;
	}
	/* Exit sleep mode */
	ret = mipi_dsi_dcs_exit_sleep_mode(dsi);
	if (ret < 0) {
		DRM_DEV_ERROR(dev, "Failed to exit sleep mode (%d)\n", ret);
		goto fail;
	}
#endif
	usleep_range(5000, 7000);

	ret = mipi_dsi_dcs_set_display_on(dsi);
	if (ret < 0) {
		DRM_DEV_ERROR(dev, "Failed to set display ON (%d)\n", ret);
		goto fail;
	}

	rad->backlight->props.power = FB_BLANK_UNBLANK;
	backlight_update_status(rad->backlight);

	rad->enabled = true;


	return 0;

fail:
	gpiod_set_value_cansleep(rad->reset, 0);//1

	return ret;
}

static int rad_panel_disable(struct drm_panel *panel)
{
	struct rad_panel *rad = to_rad_panel(panel);
	struct mipi_dsi_device *dsi = rad->dsi;
	struct device *dev = &dsi->dev;
	int ret;

	if (!rad->enabled)
		return 0;

	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	ret = mipi_dsi_dcs_set_display_off(dsi);
	if (ret < 0) {
		DRM_DEV_ERROR(dev, "Failed to set display OFF (%d)\n", ret);
		return ret;
	}

	usleep_range(5000, 10000);

	ret = mipi_dsi_dcs_enter_sleep_mode(dsi);
	if (ret < 0) {
		DRM_DEV_ERROR(dev, "Failed to enter sleep mode (%d)\n", ret);
		return ret;
	}
	rad->backlight->props.power = FB_BLANK_POWERDOWN;
	backlight_update_status(rad->backlight);

	rad->enabled = false;

	return 0;
}

static int rad_panel_get_modes(struct drm_panel *panel)
{
	struct drm_connector *connector = panel->connector;
	struct drm_display_mode *mode;

	mode = drm_mode_duplicate(panel->drm, &default_mode);
	if (!mode) {
		DRM_DEV_ERROR(panel->dev, "failed to add mode %ux%ux@%u\n",
			      default_mode.hdisplay, default_mode.vdisplay,
			      default_mode.vrefresh);
		return -ENOMEM;
	}

	drm_mode_set_name(mode);
	mode->type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED;
	drm_mode_probed_add(panel->connector, mode);

	connector->display_info.width_mm = mode->width_mm;
	connector->display_info.height_mm = mode->height_mm;
	connector->display_info.bus_flags = rad_bus_flags;

	drm_display_info_set_bus_formats(&connector->display_info,
					 rad_bus_formats,
					 ARRAY_SIZE(rad_bus_formats));
	return 1;
}

static int rad_bl_get_brightness(struct backlight_device *bl)
{
	struct mipi_dsi_device *dsi = bl_get_data(bl);
	struct rad_panel *rad = mipi_dsi_get_drvdata(dsi);
	u16 brightness;
	int ret;

	if (!rad->prepared)
		return 0;

	dsi->mode_flags &= ~MIPI_DSI_MODE_LPM;

	ret = mipi_dsi_dcs_get_display_brightness(dsi, &brightness);
	if (ret < 0)
		return ret;

	bl->props.brightness = brightness;

	return brightness & 0xff;
}

static int rad_bl_update_status(struct backlight_device *bl)
{
	struct mipi_dsi_device *dsi = bl_get_data(bl);
	struct rad_panel *rad = mipi_dsi_get_drvdata(dsi);
	int ret = 0;

	if (!rad->prepared)
		return 0;

	dsi->mode_flags &= ~MIPI_DSI_MODE_LPM;

	ret = mipi_dsi_dcs_set_display_brightness(dsi, bl->props.brightness);
	if (ret < 0)
		return ret;

	return 0;
}

static const struct backlight_ops rad_bl_ops = {
	.update_status = rad_bl_update_status,
	.get_brightness = rad_bl_get_brightness,
};

static const struct drm_panel_funcs rad_panel_funcs = {
	.prepare = rad_panel_prepare,
	.unprepare = rad_panel_unprepare,
	.enable = rad_panel_enable,
	.disable = rad_panel_disable,
	.get_modes = rad_panel_get_modes,
};

static const char * const rad_supply_names[] = {
	"v3p3",
	"v1p8",
};

static int rad_init_regulators(struct rad_panel *rad)
{
	struct device *dev = &rad->dsi->dev;
	int i;

	rad->num_supplies = ARRAY_SIZE(rad_supply_names);
	rad->supplies = devm_kcalloc(dev, rad->num_supplies,
				     sizeof(*rad->supplies), GFP_KERNEL);
	if (!rad->supplies)
		return -ENOMEM;

	for (i = 0; i < rad->num_supplies; i++)
		rad->supplies[i].supply = rad_supply_names[i];

	return devm_regulator_bulk_get(dev, rad->num_supplies, rad->supplies);
};

static int rad_panel_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct device_node *np = dev->of_node;
	struct rad_panel *panel;
	struct backlight_properties bl_props;
	int ret;
	u32 video_mode;

	panel = devm_kzalloc(&dsi->dev, sizeof(*panel), GFP_KERNEL);
	if (!panel)
		return -ENOMEM;

	mipi_dsi_set_drvdata(dsi, panel);

	panel->dsi = dsi;

	dsi->format = MIPI_DSI_FMT_RGB888;
	dsi->mode_flags =  MIPI_DSI_MODE_VIDEO_HSE | MIPI_DSI_MODE_VIDEO;

	ret = of_property_read_u32(np, "video-mode", &video_mode);
	if (!ret) {
		switch (video_mode) {
		case 0:
			/* burst mode */
			dsi->mode_flags |= MIPI_DSI_MODE_VIDEO_BURST;
			break;
		case 1:
			/* non-burst mode with sync event */
			break;
		case 2:
			/* non-burst mode with sync pulse */
			dsi->mode_flags |= MIPI_DSI_MODE_VIDEO_SYNC_PULSE;
			break;
		default:
			dev_warn(dev, "invalid video mode %d\n", video_mode);
			break;
		}
	}

	ret = of_property_read_u32(np, "dsi-lanes", &dsi->lanes);
	if (ret) {
		dev_err(dev, "Failed to get dsi-lanes property (%d)\n", ret);
		return ret;
	}

	panel->reset = devm_gpiod_get(dev, "reset", GPIOD_OUT_LOW);

	if (IS_ERR(panel->reset)) {
		ret = PTR_ERR(panel->reset);
		dev_err(dev, "Failed to get reset gpio (%d)\n", ret);
		return ret;
	}
	gpiod_set_value_cansleep(panel->reset, 0);//1

	memset(&bl_props, 0, sizeof(bl_props));
	bl_props.type = BACKLIGHT_RAW;
	bl_props.brightness = 255;
	bl_props.max_brightness = 255;

	panel->backlight = devm_backlight_device_register(dev, dev_name(dev),
							  dev, dsi, &rad_bl_ops,
							  &bl_props);
	if (IS_ERR(panel->backlight)) {
		ret = PTR_ERR(panel->backlight);
		dev_err(dev, "Failed to register backlight (%d)\n", ret);
		return ret;
	}

	ret = rad_init_regulators(panel);
	if (ret)
		return ret;

	drm_panel_init(&panel->panel);
	panel->panel.funcs = &rad_panel_funcs;
	panel->panel.dev = dev;
	dev_set_drvdata(dev, panel);

	ret = drm_panel_add(&panel->panel);
	if (ret)
		return ret;

	ret = mipi_dsi_attach(dsi);
	if (ret)
		drm_panel_remove(&panel->panel);

	return ret;
}

static int rad_panel_remove(struct mipi_dsi_device *dsi)
{
	struct rad_panel *rad = mipi_dsi_get_drvdata(dsi);
	struct device *dev = &dsi->dev;
	int ret;

	ret = mipi_dsi_detach(dsi);
	if (ret)
		DRM_DEV_ERROR(dev, "Failed to detach from host (%d)\n",
			      ret);

	drm_panel_remove(&rad->panel);

	return 0;
}

static void rad_panel_shutdown(struct mipi_dsi_device *dsi)
{
	struct rad_panel *rad = mipi_dsi_get_drvdata(dsi);

	rad_panel_disable(&rad->panel);
	rad_panel_unprepare(&rad->panel);
}

static const struct of_device_id rad_of_match[] = {
	{ .compatible = "raydium,rm67295", },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, rad_of_match);

static struct mipi_dsi_driver rad_panel_driver = {
	.driver = {
		.name = "panel-raydium-rm67295",
		.of_match_table = rad_of_match,
	},
	.probe = rad_panel_probe,
	.remove = rad_panel_remove,
	.shutdown = rad_panel_shutdown,
};
module_mipi_dsi_driver(rad_panel_driver);

MODULE_AUTHOR("Robert Chiras <robert.chiras@nxp.com>");
MODULE_DESCRIPTION("DRM Driver for Raydium RM67295 MIPI DSI panel");
MODULE_LICENSE("GPL v2");

