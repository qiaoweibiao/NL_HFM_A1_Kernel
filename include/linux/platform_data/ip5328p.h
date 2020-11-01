/*
 * IP5328P Micro/Mini USB IC with integrated charger
 *
 *			Copyright (C) 2011 Texas Instruments
 *			Copyright (C) 2011 National Semiconductor
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef _IP5328P_H
#define _IP5328P_H

enum IP5328P_eoc_level {
	IP5328P_EOC_5P,
	IP5328P_EOC_10P,
	IP5328P_EOC_16P,
	IP5328P_EOC_20P,
	IP5328P_EOC_25P,
	IP5328P_EOC_33P,
	IP5328P_EOC_50P,
};

enum IP5328P_ichg {
	IP5328P_ICHG_90mA,
	IP5328P_ICHG_100mA,
	IP5328P_ICHG_400mA,
	IP5328P_ICHG_450mA,
	IP5328P_ICHG_500mA,
	IP5328P_ICHG_600mA,
	IP5328P_ICHG_700mA,
	IP5328P_ICHG_800mA,
	IP5328P_ICHG_900mA,
	IP5328P_ICHG_1000mA,
};

/**
 * struct IP5328P_chg_param
 * @eoc_level : end of charge level setting
 * @ichg      : charging current
 */
struct IP5328P_chg_param {
	enum IP5328P_eoc_level eoc_level;
	enum IP5328P_ichg ichg;
};

/**
 * struct IP5328P_platform_data
 * @get_batt_present  : check battery status - exists or not
 * @get_batt_level    : get battery voltage (mV)
 * @get_batt_capacity : get battery capacity (%)
 * @get_batt_temp     : get battery temperature
 * @ac                : charging parameters for AC type charger
 * @usb               : charging parameters for USB type charger
 * @debounce_msec     : interrupt debounce time
 */
struct IP5328P_platform_data {
	u8 (*get_batt_present)(void);
	u16 (*get_batt_level)(void);
	u8 (*get_batt_capacity)(void);
	u8 (*get_batt_temp)(void);
	struct IP5328P_chg_param *ac;
	struct IP5328P_chg_param *usb;
	unsigned int debounce_msec;
};

#endif
