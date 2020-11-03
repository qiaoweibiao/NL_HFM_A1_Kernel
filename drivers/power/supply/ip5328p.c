/*
 * Driver for IP5328P Micro/Mini USB IC with integrated charger
 *
 *			Copyright (C) 2011 Texas Instruments
 *			Copyright (C) 2011 National Semiconductor
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/i2c.h>
#include <linux/power_supply.h>
#include <linux/platform_data/ip5328p.h>
#include <linux/of.h>

#define IP5328P_NUM_INTREGS	2
#define DEFAULT_DEBOUNCE_MSEC	270




#if 0

/* Registers */
#define IP5328P_CTRL1  0x01//boost 和 和 charger  使能寄存器

#define IP5328P_CTRL2  0x03//按键控制寄存器
#define SYS_CTL3  0X04//芯片内部温度和 NTC  温度控制寄存器
#define SYS_CTL4  0X05//照明灯控制寄存器
#define SYS_CTL5  0X07//轻载关机时间设置
#define SYS_CTL6  0X0A//电量灯配置寄存器
#define SYS_CTL7  0X0B//输出口自动检测负载寄存器
#define SYS_CTL8  0X0C//按键控制输出口寄存器
#define SYS_CTL9  0X0D//边充边放功能寄存器
#define SYS_CTL10  0X0E//按键控制输出口寄存器

#define VBAT_LO  0X10 //低电关机阈值设置
#define VINOV  0X11	//VIN  输入过压设置
#define VBUSOV  0X12	//VBUS	输入过压设置
#define BOOST_LINC  0X13	//输出线补设置寄存器

#define TYPEC_CTRL0  0X1A //typec CC	上拉设置 寄存器
#define TYPEC_CTRL1  0X1B//typec CC  模式 配置寄存
#define TYPEC_CTRL2  0X1C//typec PD  协议使能寄存器
#define TYPEC_CTRL3  0X1E//输入请求最高电压设置

#define CHG_CTL1  0X21//充电停充电压设置寄存器
#define CHG_CTL2  0X22//充电充满电压设置寄存器
#define CHG_CTL3  0X23//充电欠压环设置寄存器 5V 7V 
#define CHG_CTL4  0X24//充电欠压环设置寄存器 9V 12V

#define CHG_ISET_9V  0x26//
#define CHG_ISET_12V  0x27//
#define CHG_ISET_5V_VBUS  0x29
#define CHG_ISET_5V_VIN  0x2A
#define CHG_ISET_7V  0x2B
#define CHG_TIMER_EN  0x2C
#define CHG_TIMER_SET  0x2D

#define DCDC_FREQ  0x31//DC-DC  开关频率设置寄存器

#define QC_EN  0x3E//输入输出口 DCP	快充协议使能寄存器

#define IC_TEMP  0X42//IC  内部过温寄存器
#define CHG_NTC_TEMP  0X43//充电 NTC  阈值寄存器


#define BST_NTC_TEMP  0x54//放电 NTC  阈值寄存器
#define PMOS_REG_CTL0  0x59//输入输出口控制寄存器
#define PMOS_REG_CTL1  0x5A//输入输出口控制寄存器
#define FORCE_EN  0x5B//寄存器关机和复位控制寄存器

#define FLAG0  0x7E//异常标志位
#define FLAG1  0x7F//按键和过压标志
#define BST_POWERLOW  0x81//轻载关机 功率 阈值设置寄存器
#define RSET  0x82//电池内阻补偿寄存器
#define IPMOSLOW  0x86//多口转单口 MOS  关电流阈值设置寄存器
#define BATOCV_LOW  0x88//低电退出主动退出快充设置寄存器
#define IPMOSLOW_TIME  0x90//多口转单口时间设置寄存器
#define QC_VMAX  0x96// （设置 QC	协议最大输出电压
#define BATOCV_LOW_DN  0x9F//

#define DCP_DIG_CTL0  0xA0
#define DCP_DIG_CTL1  0xA1
#define DCP_DIG_CTL2  0xA2
#define BOOST_5V_ISET  0xA8
#define BOOST_VSET  0x4C

#define DCP_DIG_CTL10  0xAA
#define LED_STATUS  0XDB//电量信息寄存器

/*以下为只读寄存器*/
#define BATVADC_DAT0  0x64//BAT  真实电压 寄存器
#define BATVADC_DAT1  0x65//BAT  真实电压 寄存器
#define BATIADC_DAT0  0x66//BAT  端电流 寄存器 
#define BATIADC_DAT1  0x67//BAT  端电流 寄存器 
#define SYSVADC_DAT0  0x68//VSYS	端电压值 寄存器
#define SYSVADC_DAT1  0x69//VSYS	端电压值 寄存器
#define SYSIADC_DAT0  0x6A//10  毫欧 采样 电阻 流过的电流 寄存器 
#define SYSIADC_DAT1  0x6B//10  毫欧 采样 电阻 流过的电流 寄存器 
#define VINIADC_DAT0  0x6C//VIN  通路 MOS  流过的电流 寄存器 
#define VINIADC_DAT1  0x6D//VIN  通路 MOS  流过的电流 寄存器
#define VBUSIADC_DAT0  0x6E//VBUS  通路 MOS  流过的电流 寄存器
#define VBUSIADC_DAT1  0x6F//VBUS  通路 MOS  流过的电流 寄存器
#define VOUT1IADC_DAT0  0x70//VOUT1  通路 MOS  流过的电流
#define VOUT1IADC_DAT1  0x71//VOUT1  通路 MOS  流过的电流
#define VOUT2IADC_DAT0  0x72//VOUT2  通路 MOS  流过的电流
#define VOUT2IADC_DAT1  0x73//VOUT2  通路 MOS  流过的电流
#define RSETADC_DAT0  0x74//
#define RSETADC_DAT0  0x75//
#define GPIADC_DAT0  0x78//GPIO ADC  电压值寄存器
#define GPIADC_DAT1  0x79//GPIO ADC  电压值寄存器
#define BATOCV_DAT0  0x7A//BATOCV  电压 寄存器
#define BATOCV_DAT1  0x7B//BATOCV  电压 寄存器
#define POWER_DAT0  0x7C//输入输出功率寄存器
#define POWER_DAT1  0x7D//输入输出功率寄存器
#define SYS_STATUS  0xD1//系统状态指示寄存器
#define KEY_IN  0xD2//系统状态指示寄存器
#define OV_FLAG  0xD3//系统过压/ 欠压寄存器
#define VIN_VBUS_STATE	 0xD5//VIN/VBUS  充电电压寄存器
#define CHG_STATUS 0xD7//充电状态指示寄存器
#define LOW_STATUS	 0xD9//系统轻载标志位
#define NTC_FLAG   0xDA//NTC	状态指示寄存器
#define LOWCUR_FLAG   0xDE//常开 N  小时标准位
#define MOS_ON	0xE5// MOS  开启状态寄存器 
#define BST_V_FLAG	 0xFB//BOOST	电压范围寄存器
#define TYPEC_OK  0XB8//  TYPEC	连接状态寄存器
#define TYPEC_FLAG	 0xFC//TYPEC	上拉状态寄存器



#endif

/*以下是原来代码中的寄存器设置*/
#define IP5328P_CTRL1		0x01//boost 和 charger  使能寄存器
#define IP5328P_CTRL2		0x03//按键控制寄存器
#define IP5328P_CTRL4		0x05//LED 控制
#define IP5328P_INT1		0x7E//异常标志位
#define IP5328P_INT2		0x7F//按键和过压标志
#define IP5328P_STATUS1		0xD1//系统状态指示寄存器
#define IP5328P_STATUS2		0XDB//电量信息寄存器
#define IP5328P_CHGCTRL2	0X1C//typec PD  协议使能寄存器
#define IP5328P_CHGCTRL2	0X1C//typec PD  协议使能寄存器
#define IP5328P_SWCTRL		0XDB//typec PD  协议使能寄存器

#define BATVADC_DAT0  		0x64//BAT  真实电压 寄存器
#define BATVADC_DAT1  		0x65//BAT  真实电压 寄存器


/* CTRL1 register */
#define IP5328P_EN_CHARGER		BIT(1)
#define IP5328P_EN_BOOST		BIT(2)//
#define IP5328P_EN_LED			BIT(0)//
#define IP5328P_EN_KEY			BIT(3)//









#define IP5328P_ID200_EN		BIT(4)

/* CTRL2 register */
#define IP5328P_CHGDET_EN	BIT(1)
#define IP5328P_INT_EN		BIT(6)

/* SWCTRL register */
#define IP5328P_SW_DM1_DM	(0x0 << 0)
#define IP5328P_SW_DM1_HiZ	(0x7 << 0)
#define IP5328P_SW_DP2_DP	(0x0 << 3)
#define IP5328P_SW_DP2_HiZ	(0x7 << 3)

/* INT1 register */
#define IP5328P_IDNO		(0xF << 0)
#define IP5328P_VBUS		BIT(4)

/* STATUS1 register */
#define IP5328P_CHGSTAT		(3 << 4)
#define IP5328P_CHPORT		BIT(6)
#define IP5328P_DCPORT		BIT(7)
#define IP5328P_STAT_EOC		0x30

/* STATUS2 register */
#define IP5328P_TEMP_STAT	(3 << 5)
#define IP5328P_TEMP_SHIFT	5

/* CHGCTRL2 register */
#define IP5328P_ICHG_SHIFT	4

enum IP5328P_dev_id {
	IP5328P_ID_NONE,
	IP5328P_ID_TA,
	IP5328P_ID_DEDICATED_CHG,
	IP5328P_ID_USB_CHG,
	IP5328P_ID_USB_DS,
	IP5328P_ID_MAX,
};

enum IP5328P_die_temp {
	IP5328P_TEMP_75C,
	IP5328P_TEMP_95C,
	IP5328P_TEMP_115C,
	IP5328P_TEMP_135C,
};

struct IP5328P_psy {
	struct power_supply *ac;
	struct power_supply *usb;
	struct power_supply *batt;
};

struct IP5328P_chg {
	struct device *dev;
	struct i2c_client *client;
	struct mutex xfer_lock;
	struct IP5328P_psy *psy;
	struct IP5328P_platform_data *pdata;

	/* Charger Data */
	enum IP5328P_dev_id devid;
	struct IP5328P_chg_param *chg_param;

	/* Interrupt Handling */
	int irq;
	struct delayed_work work;
	unsigned long debounce_jiffies;
};

static int IP5328P_read_bytes(struct IP5328P_chg *pchg, u8 reg, u8 *data, u8 len)
{
	s32 ret;

	mutex_lock(&pchg->xfer_lock);
	ret = i2c_smbus_read_i2c_block_data(pchg->client, reg, len, data);
	mutex_unlock(&pchg->xfer_lock);

	return (ret != len) ? -EIO : 0;//判断语句 return  -EIO : 0
}

static inline int IP5328P_read_byte(struct IP5328P_chg *pchg, u8 reg, u8 *data)
{
	return IP5328P_read_bytes(pchg, reg, data, 1);
}

static int IP5328P_write_byte(struct IP5328P_chg *pchg, u8 reg, u8 data)
{
	int ret;

	mutex_lock(&pchg->xfer_lock);
	ret = i2c_smbus_write_byte_data(pchg->client, reg, data);
	mutex_unlock(&pchg->xfer_lock);

	return ret;
}

static bool IP5328P_is_charger_attached(const char *name, int id)
{
	if (!strcmp(name, "ac"))
		return id == IP5328P_ID_TA || id == IP5328P_ID_DEDICATED_CHG;
	else if (!strcmp(name, "usb"))
		return id == IP5328P_ID_USB_CHG;

	return id >= IP5328P_ID_TA && id <= IP5328P_ID_USB_CHG;
}

static int IP5328P_init_device(struct IP5328P_chg *pchg)
{
	u8 val;
	u8 val_LED;
	u8 val_KEY;
	u8 BAT_H_8bit;
	u8 BAT_L_8bit;
	int ret;
	u8 intstat[IP5328P_NUM_INTREGS];
	u8 EN_CHARGER;
	u8 EN_BOOST;

	
#if 0
	IP5328P_read_byte(pchg, IP5328P_CTRL1, &val);
	
	val = val & IP5328P_EN_CHARGER;
	printk("the value of IP5328P_EN_CHARGER = %d\n",val);

	IP5328P_read_byte(pchg, IP5328P_CTRL4, &val_LED);
	
	val_LED = val_LED & IP5328P_EN_LED;
	printk("the value of IP5328P_EN_LED = %d\n",val_LED);

	IP5328P_read_byte(pchg, IP5328P_CTRL4, &val_KEY);


	val_KEY = val_KEY & IP5328P_EN_KEY;
	printk("the value of IP5328P_EN_KEY = %d\n",val_KEY);

#endif

	IP5328P_read_byte(pchg, BATVADC_DAT0, &BAT_H_8bit);
	BAT_H_8bit = BAT_H_8bit & 0xFF;
	printk("the value of IP5328P_EN_KEY = %d\n",BAT_H_8bit);
	IP5328P_read_byte(pchg, BATVADC_DAT1, &BAT_L_8bit);
	BAT_L_8bit = BAT_L_8bit & 0xFF;
	printk("the value of IP5328P_EN_KEY = %d\n",BAT_L_8bit);

}

static int IP5328P_is_dedicated_charger(struct IP5328P_chg *pchg)
{
	u8 val;

	IP5328P_read_byte(pchg, IP5328P_STATUS1, &val);
	return val & IP5328P_DCPORT;
}

static int IP5328P_is_usb_charger(struct IP5328P_chg *pchg)
{
	u8 val;

	IP5328P_read_byte(pchg, IP5328P_STATUS1, &val);
	return val & IP5328P_CHPORT;
}

static inline void IP5328P_ctrl_switch(struct IP5328P_chg *pchg, u8 sw)
{
	IP5328P_write_byte(pchg, IP5328P_SWCTRL, sw);
}

static void IP5328P_id_detection(struct IP5328P_chg *pchg, u8 id, int vbusin)
{
	struct IP5328P_platform_data *pdata = pchg->pdata;
	u8 devid = IP5328P_ID_NONE;
	u8 swctrl = IP5328P_SW_DM1_HiZ | IP5328P_SW_DP2_HiZ;

	switch (id) {
	case 0x5:
		devid = IP5328P_ID_TA;
		pchg->chg_param = pdata ? pdata->ac : NULL;
		break;
	case 0xB:
		if (IP5328P_is_dedicated_charger(pchg)) {
			pchg->chg_param = pdata ? pdata->ac : NULL;
			devid = IP5328P_ID_DEDICATED_CHG;
		} else if (IP5328P_is_usb_charger(pchg)) {
			pchg->chg_param = pdata ? pdata->usb : NULL;
			devid = IP5328P_ID_USB_CHG;
			swctrl = IP5328P_SW_DM1_DM | IP5328P_SW_DP2_DP;
		} else if (vbusin) {
			devid = IP5328P_ID_USB_DS;
			swctrl = IP5328P_SW_DM1_DM | IP5328P_SW_DP2_DP;
		}
		break;
	default:
		devid = IP5328P_ID_NONE;
		pchg->chg_param = NULL;
		break;
	}

	pchg->devid = devid;
	IP5328P_ctrl_switch(pchg, swctrl);
}

static void IP5328P_enable_chgdet(struct IP5328P_chg *pchg)
{
	u8 val;

	IP5328P_read_byte(pchg, IP5328P_CTRL2, &val);
	val |= IP5328P_CHGDET_EN;
	IP5328P_write_byte(pchg, IP5328P_CTRL2, val);
}

static void IP5328P_delayed_func(struct work_struct *_work)
{
	struct IP5328P_chg *pchg = container_of(_work, struct IP5328P_chg,
						work.work);
	u8 intstat[IP5328P_NUM_INTREGS];
	u8 idno;
	u8 vbus;

	if (IP5328P_read_bytes(pchg, IP5328P_INT1, intstat, IP5328P_NUM_INTREGS)) {
		dev_err(pchg->dev, "can not read INT registers\n");
		return;
	}

	idno = intstat[0] & IP5328P_IDNO;
	vbus = intstat[0] & IP5328P_VBUS;

	IP5328P_id_detection(pchg, idno, vbus);
	IP5328P_enable_chgdet(pchg);

	power_supply_changed(pchg->psy->ac);
	power_supply_changed(pchg->psy->usb);
	power_supply_changed(pchg->psy->batt);
}

static irqreturn_t IP5328P_isr_func(int irq, void *ptr)
{
	struct IP5328P_chg *pchg = ptr;

	schedule_delayed_work(&pchg->work, pchg->debounce_jiffies);
	return IRQ_HANDLED;
}

static int IP5328P_setup_irq(struct IP5328P_chg *pchg)
{
	int ret;
	int irq = pchg->client->irq;
	unsigned delay_msec = pchg->pdata ? pchg->pdata->debounce_msec :
						DEFAULT_DEBOUNCE_MSEC;

	INIT_DELAYED_WORK(&pchg->work, IP5328P_delayed_func);

	if (irq <= 0) {
		dev_warn(pchg->dev, "invalid irq number: %d\n", irq);
		return 0;
	}

	ret = request_threaded_irq(irq,	NULL, IP5328P_isr_func,
				IRQF_TRIGGER_FALLING | IRQF_ONESHOT,
				"IP5328P_irq", pchg);

	if (ret)
		return ret;

	pchg->irq = irq;
	pchg->debounce_jiffies = msecs_to_jiffies(delay_msec);

	return 0;
}

static void IP5328P_release_irq(struct IP5328P_chg *pchg)
{
	cancel_delayed_work_sync(&pchg->work);

	if (pchg->irq)
		free_irq(pchg->irq, pchg);
}

static enum power_supply_property IP5328P_charger_prop[] = {
	POWER_SUPPLY_PROP_ONLINE,
};

static enum power_supply_property IP5328P_battery_prop[] = {
	POWER_SUPPLY_PROP_STATUS,
	POWER_SUPPLY_PROP_HEALTH,
	POWER_SUPPLY_PROP_PRESENT,
	POWER_SUPPLY_PROP_VOLTAGE_NOW,
	POWER_SUPPLY_PROP_CAPACITY,
	POWER_SUPPLY_PROP_TEMP,
};

static char *battery_supplied_to[] = {
	"main_batt",
};

static int IP5328P_charger_get_property(struct power_supply *psy,
				       enum power_supply_property psp,
				       union power_supply_propval *val)
{
	struct IP5328P_chg *pchg = dev_get_drvdata(psy->dev.parent);

	if (psp != POWER_SUPPLY_PROP_ONLINE)
		return -EINVAL;

	val->intval = IP5328P_is_charger_attached(psy->desc->name, pchg->devid);

	return 0;
}

static bool IP5328P_is_high_temperature(enum IP5328P_die_temp temp)
{
	switch (temp) {
	case IP5328P_TEMP_95C:
	case IP5328P_TEMP_115C:
	case IP5328P_TEMP_135C:
		return true;
	default:
		return false;
	}
}

static int IP5328P_battery_get_property(struct power_supply *psy,
				       enum power_supply_property psp,
				       union power_supply_propval *val)
{
	struct IP5328P_chg *pchg = dev_get_drvdata(psy->dev.parent);
	struct IP5328P_platform_data *pdata = pchg->pdata;
	enum IP5328P_die_temp temp;
	u8 read;

	switch (psp) {
	case POWER_SUPPLY_PROP_STATUS:
		if (!IP5328P_is_charger_attached(psy->desc->name, pchg->devid)) {
			val->intval = POWER_SUPPLY_STATUS_DISCHARGING;
			return 0;
		}

		IP5328P_read_byte(pchg, IP5328P_STATUS1, &read);

		val->intval = (read & IP5328P_CHGSTAT) == IP5328P_STAT_EOC ?
				POWER_SUPPLY_STATUS_FULL :
				POWER_SUPPLY_STATUS_CHARGING;
		break;
	case POWER_SUPPLY_PROP_HEALTH:
		IP5328P_read_byte(pchg, IP5328P_STATUS2, &read);
		temp = (read & IP5328P_TEMP_STAT) >> IP5328P_TEMP_SHIFT;

		val->intval = IP5328P_is_high_temperature(temp) ?
			POWER_SUPPLY_HEALTH_OVERHEAT :
			POWER_SUPPLY_HEALTH_GOOD;
		break;
	case POWER_SUPPLY_PROP_PRESENT:
		if (!pdata)
			return -EINVAL;

		if (pdata->get_batt_present)
			val->intval = pdata->get_batt_present();
		break;
	case POWER_SUPPLY_PROP_VOLTAGE_NOW:
		if (!pdata)
			return -EINVAL;

		if (pdata->get_batt_level)
			val->intval = pdata->get_batt_level();
		break;
	case POWER_SUPPLY_PROP_CAPACITY:
		if (!pdata)
			return -EINVAL;

		if (pdata->get_batt_capacity)
			val->intval = pdata->get_batt_capacity();
		break;
	case POWER_SUPPLY_PROP_TEMP:
		if (!pdata)
			return -EINVAL;

		if (pdata->get_batt_temp)
			val->intval = pdata->get_batt_temp();
		break;
	default:
		break;
	}

	return 0;
}

static void IP5328P_charger_changed(struct power_supply *psy)
{
	struct IP5328P_chg *pchg = dev_get_drvdata(psy->dev.parent);
	u8 eoc_level;
	u8 ichg;
	u8 val;

	/* skip if no charger exists */
	if (!IP5328P_is_charger_attached(psy->desc->name, pchg->devid))
		return;

	/* update charging parameters */
	if (pchg->chg_param) {
		eoc_level = pchg->chg_param->eoc_level;
		ichg = pchg->chg_param->ichg;
		val = (ichg << IP5328P_ICHG_SHIFT) | eoc_level;
		IP5328P_write_byte(pchg, IP5328P_CHGCTRL2, val);
	}
}



static const struct power_supply_desc IP5328P_usb_desc = {
	.name			= "usb",
	.type			= POWER_SUPPLY_TYPE_USB,
	.properties		= IP5328P_charger_prop,
	.num_properties		= ARRAY_SIZE(IP5328P_charger_prop),
	.get_property		= IP5328P_charger_get_property,
};


static int IP5328P_register_psy(struct IP5328P_chg *pchg)
{
	struct power_supply_config psy_cfg = {}; /* Only for ac and usb */
	struct IP5328P_psy *psy;

	psy = devm_kzalloc(pchg->dev, sizeof(*psy), GFP_KERNEL);
	if (!psy)
		return -ENOMEM;

	pchg->psy = psy;

	psy_cfg.supplied_to = battery_supplied_to;
	psy_cfg.num_supplicants = ARRAY_SIZE(battery_supplied_to);


	psy->usb = power_supply_register(pchg->dev, &IP5328P_usb_desc,
					 &psy_cfg);


	return -EPERM;
}

static void IP5328P_unregister_psy(struct IP5328P_chg *pchg)
{
	struct IP5328P_psy *psy = pchg->psy;

	if (!psy)
		return;

	power_supply_unregister(psy->ac);
	power_supply_unregister(psy->usb);
	power_supply_unregister(psy->batt);
}

#ifdef CONFIG_OF
static struct IP5328P_chg_param
*IP5328P_parse_charge_pdata(struct device *dev, struct device_node *np)
{
	struct IP5328P_chg_param *param;

	param = devm_kzalloc(dev, sizeof(*param), GFP_KERNEL);
	if (!param)
		goto out;

	of_property_read_u8(np, "eoc-level", (u8 *)&param->eoc_level);
	of_property_read_u8(np, "charging-current", (u8 *)&param->ichg);
out:
	return param;
}

static struct IP5328P_platform_data *IP5328P_parse_dt(struct device *dev)
{
	struct device_node *np = dev->of_node;
	struct device_node *child;
	struct IP5328P_platform_data *pdata;
	const char *type;

	pdata = devm_kzalloc(dev, sizeof(*pdata), GFP_KERNEL);
	if (!pdata)
		return ERR_PTR(-ENOMEM);

	of_property_read_u32(np, "debounce-ms", &pdata->debounce_msec);

	/* If charging parameter is not defined, just skip parsing the dt */
	if (of_get_child_count(np) == 0)
		return pdata;

	for_each_child_of_node(np, child) {
		of_property_read_string(child, "charger-type", &type);

		if (!strcmp(type, "ac"))
			pdata->ac = IP5328P_parse_charge_pdata(dev, child);

		if (!strcmp(type, "usb"))
			pdata->usb = IP5328P_parse_charge_pdata(dev, child);
	}

	return pdata;
}
#else
static struct IP5328P_platform_data *IP5328P_parse_dt(struct device *dev)
{
	return NULL;
}
#endif

static int IP5328P_probe(struct i2c_client *cl, const struct i2c_device_id *id)
{
	struct IP5328P_chg *pchg;
	struct IP5328P_platform_data *pdata;
	int ret;
	printk("11111111111111111111111111111   IP5328P_probe ");
	if (!i2c_check_functionality(cl->adapter, I2C_FUNC_SMBUS_I2C_BLOCK))
		return -EIO;

	if (cl->dev.of_node) {//dts 参数传入
		pdata = IP5328P_parse_dt(&cl->dev);
		if (IS_ERR(pdata))
			return PTR_ERR(pdata);
	} else {
		pdata = dev_get_platdata(&cl->dev);
	}

	pchg = devm_kzalloc(&cl->dev, sizeof(*pchg), GFP_KERNEL);
	if (!pchg)
		return -ENOMEM;

	pchg->client = cl;//来源于结构体中
	pchg->dev = &cl->dev;
	pchg->pdata = pdata;
	i2c_set_clientdata(cl, pchg);

	mutex_init(&pchg->xfer_lock);

	ret = IP5328P_init_device(pchg);//初始化设备
	if (ret) {
		dev_err(pchg->dev, "i2c communication err: %d", ret);
		return ret;
	}

	ret = IP5328P_register_psy(pchg);//设备寄存器
	if (ret) {
		dev_err(pchg->dev, "power supplies register err: %d", ret);
		return ret;
	}

	ret = IP5328P_setup_irq(pchg);
	if (ret) {
		dev_err(pchg->dev, "irq handler err: %d", ret);
		IP5328P_unregister_psy(pchg);
		return ret;
	}

	return 0;
}

static int IP5328P_remove(struct i2c_client *cl)
{
	struct IP5328P_chg *pchg = i2c_get_clientdata(cl);

	IP5328P_release_irq(pchg);
	IP5328P_unregister_psy(pchg);
	return 0;
}

static const struct of_device_id IP5328P_dt_ids[] = {
	{ .compatible = "injoinic,ip5328p", },
	{ }
};
MODULE_DEVICE_TABLE(of, IP5328P_dt_ids);

static const struct i2c_device_id IP5328P_ids[] = {
	{"IP5328P", 0},
	{ }
};
MODULE_DEVICE_TABLE(i2c, IP5328P_ids);

static struct i2c_driver IP5328P_driver = {
	.driver = {
		   .name = "IP5328P",
		   .of_match_table = of_match_ptr(IP5328P_dt_ids),
		   },
	.probe = IP5328P_probe,
	.remove = IP5328P_remove,
	.id_table = IP5328P_ids,
};
module_i2c_driver(IP5328P_driver);

MODULE_DESCRIPTION("TI/National Semiconductor IP5328P charger driver");
MODULE_AUTHOR("Milo Kim <milo.kim@ti.com>, Daniel Jeong <daniel.jeong@ti.com>");
MODULE_LICENSE("GPL");
