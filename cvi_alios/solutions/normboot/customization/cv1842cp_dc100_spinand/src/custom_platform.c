#include "platform.h"
#include <drv/pin.h>
#include <pinctrl-mars.h>
#include "cvi_type.h"
#include "iic_recovery.h"
#include <drv/adc.h>

#define GPIO_PIN_MASK(_gpio_num) (1 << _gpio_num)

void PWR_VBATCheck(void);

int PLATFORM_ReadPwrAdc3(uint32_t *raw, uint32_t *mv);
void _GPIOSetValue(u8 gpio_grp, u8 gpio_num, u8 level)
{
	csi_error_t ret;
	csi_gpio_t gpio = {0};

	ret = csi_gpio_init(&gpio, gpio_grp);
	if(ret != CSI_OK) {
		printf("csi_gpio_init failed\r\n");
		return;
	}
	// gpio write
	ret = csi_gpio_dir(&gpio , GPIO_PIN_MASK(gpio_num), GPIO_DIRECTION_OUTPUT);

	if(ret != CSI_OK) {
		printf("csi_gpio_dir failed\r\n");
		return;
	}
	csi_gpio_write(&gpio , GPIO_PIN_MASK(gpio_num), level);
	//printf("test pin end and success.\r\n");
}
void PLATFORM_SpkMute(int value)
{
	static int run_once = 0;
	u8 gpio_spken_r_grp = 4;
	u8 gpio_spken_r_num = 2;
#if defined(CONFIG_CHIP_cv1842hp) || defined(CONFIG_CHIP_cv1843hp) || defined(CONFIG_CHIP_cv1841h)
	u8 gpio_spken_l_grp;
	u8 gpio_spken_l_num;
#endif

	if(run_once) {
		return;
	}
	run_once = 1;

#if defined(CONFIG_CHIP_cv1841c) || defined(CONFIG_CHIP_cv1842cp)
	gpio_spken_r_grp = 4;
	gpio_spken_r_num = 2;
#endif

#if defined(CONFIG_CHIP_cv1842hp) || defined(CONFIG_CHIP_cv1843hp) || defined(CONFIG_CHIP_cv1841h)
	gpio_spken_r_grp = 0;
	gpio_spken_r_num = 30;
	gpio_spken_l_grp = 0;
	gpio_spken_l_num = 15;
#endif
//0静音 ，1非静音
    if(value){
        _GPIOSetValue(gpio_spken_r_grp, gpio_spken_r_num, 1);
#if defined(CONFIG_CHIP_cv1842hp) || defined(CONFIG_CHIP_cv1843hp) || defined(CONFIG_CHIP_cv1841h)
        _GPIOSetValue(gpio_spken_l_grp, gpio_spken_l_num, 1);
#endif
    }else{
        _GPIOSetValue(gpio_spken_r_grp, gpio_spken_r_grp, 0);
#if defined(CONFIG_CHIP_cv1842hp) || defined(CONFIG_CHIP_cv1843hp) || defined(CONFIG_CHIP_cv1841h)
        _GPIOSetValue(gpio_spken_l_grp, gpio_spken_l_num, 0);
#endif
    }
}
static void _AudioPinmux(void)
{

#if defined(CONFIG_CHIP_cv1841c) || defined(CONFIG_CHIP_cv1842cp)
    PINMUX_CONFIG(PWR_GPIO2, PWR_GPIO_2);
#endif
#if defined(CONFIG_CHIP_cv1842hp) || defined(CONFIG_CHIP_cv1843hp) || defined(CONFIG_CHIP_cv1841h)
    PINMUX_CONFIG(SPK_EN, XGPIOA_15);
    PINMUX_CONFIG(AUX0, XGPIOA_30);
#endif
	PLATFORM_SpkMute(0);
}
static void _UartPinmux()
{
    // uart2 pinmux
    PINMUX_CONFIG(UART2_TX, UART2_TX);
    PINMUX_CONFIG(UART2_RX, UART2_RX);

    // uart1 pinmux
    // PINMUX_CONFIG(IIC0_SCL, UART1_TX);
    // PINMUX_CONFIG(IIC0_SDA, UART1_RX);

    // uart0 pinmux
    // PINMUX_CONFIG(UART0_TX, UART0_TX);
    // PINMUX_CONFIG(UART0_RX, UART0_RX);
}

static void _SensorPinmux()
{
	//Sensor Pinmux
#if defined (CONFIG_CHIP_cv1811c) || defined (CONFIG_CHIP_cv1801c) || defined (CONFIG_CHIP_cv1812cp)
	PINMUX_CONFIG(PAD_MIPI_TXP1, IIC2_SCL);
	PINMUX_CONFIG(PAD_MIPI_TXM1, IIC2_SDA);
	PINMUX_CONFIG(PAD_MIPI_TXM0, CAM_MCLK1);
#endif
#if defined (CONFIG_CHIP_cv1811h) || defined (CONFIG_CHIP_cv1812h) || defined (CONFIG_CHIP_cv1811ha)|| defined (CONFIG_CHIP_cv1812ha) || defined (CONFIG_CHIP_cv1813h)
	// PINMUX_CONFIG(IIC2_SCL, IIC2_SCL);
	// PINMUX_CONFIG(IIC2_SDA, IIC2_SDA);

	// PINMUX_CONFIG(IIC3_SCL, IIC3_SCL);
	// PINMUX_CONFIG(IIC3_SDA, IIC3_SDA);
	PINMUX_CONFIG(CAM_MCLK0, CAM_MCLK0);

	// PINMUX_CONFIG(IIC2_SCL, IIC2_SCL);
	// PINMUX_CONFIG(IIC2_SDA, IIC2_SDA);
	PINMUX_CONFIG(CAM_RST0, XGPIOA_2);
	PINMUX_CONFIG(IIC3_SCL, IIC3_SCL);
	PINMUX_CONFIG(IIC3_SDA, IIC3_SDA);
#endif
#if defined (CONFIG_CHIP_cv1801b) || defined (CONFIG_CHIP_cv180zb)
	PINMUX_CONFIG(PAD_MIPIRX0N, IIC1_SCL);
	PINMUX_CONFIG(PAD_MIPIRX1P, IIC1_SDA);

#if 0 //evb mipi switch
	PINMUX_CONFIG(PAD_MIPIRX1N, XGPIOC_8);
#endif

	PINMUX_CONFIG(PAD_MIPIRX0P, CAM_MCLK0);
#endif
}

static void _MipiRxPinmux(void)
{
//mipi rx pinmux
#if 0 //need porting for cv180x
    PINMUX_CONFIG(PAD_MIPIRX4P, XGPIOC_3);
    PINMUX_CONFIG(PAD_MIPIRX4N, XGPIOC_2);
#endif
}

static void _MipiTxPinmux(void)
{
//mipi tx pinmux
#if CONFIG_PANEL_ILI9488
	PINMUX_CONFIG(PAD_MIPI_TXM1, XGPIOC_14);
	PINMUX_CONFIG(PAD_MIPI_TXP1, XGPIOC_15);
	PINMUX_CONFIG(PAD_MIPI_TXM2, XGPIOC_16);
	PINMUX_CONFIG(PAD_MIPI_TXP2, XGPIOC_17);
	PINMUX_CONFIG(IIC0_SCL, XGPIOA_28);
#elif (CONFIG_PANEL_HX8394)
#if CONFIG_BOARD_CV181XC || CONFIG_BOARD_CV184X
	PINMUX_CONFIG(PAD_MIPI_TXM0, XGPIOC_12);
	PINMUX_CONFIG(PAD_MIPI_TXP0, XGPIOC_13);
	PINMUX_CONFIG(PAD_MIPI_TXM1, XGPIOC_14);
	PINMUX_CONFIG(PAD_MIPI_TXP1, XGPIOC_15);
	PINMUX_CONFIG(PAD_MIPI_TXM2, XGPIOC_16);
	PINMUX_CONFIG(PAD_MIPI_TXP2, XGPIOC_17);
	PINMUX_CONFIG(JTAG_CPU_TCK, XGPIOA_18);
	PINMUX_CONFIG(JTAG_CPU_TMS, XGPIOA_19);
	PINMUX_CONFIG(SPK_EN, XGPIOA_15);
#elif defined(__CV181X__)
	PINMUX_CONFIG(PAD_MIPI_TXM0, XGPIOC_12);
	PINMUX_CONFIG(PAD_MIPI_TXP0, XGPIOC_13);
	PINMUX_CONFIG(PAD_MIPI_TXM1, XGPIOC_14);
	PINMUX_CONFIG(PAD_MIPI_TXP1, XGPIOC_15);
	PINMUX_CONFIG(PAD_MIPI_TXM2, XGPIOC_16);
	PINMUX_CONFIG(PAD_MIPI_TXP2, XGPIOC_17);
	PINMUX_CONFIG(PAD_MIPI_TXM3, XGPIOC_20);
	PINMUX_CONFIG(PAD_MIPI_TXP3, XGPIOC_21);
	PINMUX_CONFIG(PAD_MIPI_TXM4, XGPIOC_18);
	PINMUX_CONFIG(PAD_MIPI_TXP4, XGPIOC_19);
#endif
#endif
}

#if (CONFIG_APP_DEBUG_JTAG == 1)
void JTAG_PinmuxIn()
{
    PINMUX_CONFIG(IIC0_SDA, CV_SDA0__CR_4WTDO);
    PINMUX_CONFIG(IIC0_SCL, CV_SCL0__CR_4WTDI);
}
#endif

static void i2c1_pinmux_config_gpio(void)
{
	PINMUX_CONFIG(PAD_ETH_TXP, XGPIOB_25); // IIC1_SCL
	PINMUX_CONFIG(PAD_ETH_TXM, XGPIOB_24); // IIC1_SDA
}

static void i2c1_pinmux_config_iic(void)
{
	PINMUX_CONFIG(PAD_ETH_TXP, IIC1_SCL); // GPIOB[25]
	PINMUX_CONFIG(PAD_ETH_TXM, IIC1_SDA); // GPIOB[24]
}

static void i2c1_recovery_config(void)
{
	PINMUX_CONFIG(PAD_ETH_TXP, IIC1_SCL); // GPIOB[25]
	PINMUX_CONFIG(PAD_ETH_TXM, IIC1_SDA); // GPIOB[24]

	iic_recovery_config_t i2c1_recovery = {
		.enable = 1,
		.scl_gpio_grp = 1,
		.scl_gpio_num = 25,
		.sda_gpio_grp = 1,
		.sda_gpio_num = 24,
		.config_gpio_mode = i2c1_pinmux_config_gpio,
		.config_iic_mode = i2c1_pinmux_config_iic,
	};

	csi_iic_set_recovery_config(1, &i2c1_recovery);
}

/* RTC 唤醒源、强制重启配置
 * DC100 项目中，RTC唤醒源为 PWR_WAKEUP0，
 * PWR_BUTTON1 作为电源键，短按 PWR_BUTTON1 可强制重启。
 */
void rtc_wakeup_config(void)
{
	/* pinmux 设置 */
	mmio_write_32(0x05027084, 1); // 解除 pinmux 锁定，PWR_BUTTON1
	mmio_write_32(0x0502708c, 1); // 解除 pinmux 锁定，PWR_WAKEUP0
	mmio_write_32(0x03001098, 0); // 切pinmux为 PWR_BUTTON1
	mmio_write_32(0x03001090, 0); // 切pinmux为 PWR_WAKEUP0
	mmio_write_32(0x05027084, 0); // 锁定 pinmux，PWR_BUTTON1
	mmio_write_32(0x0502708c, 0); // 锁定 pinmux，PWR_WAKEUP0

	mmio_write_32(0x050250ac, 0x2); // 设定 poweroff 时 rtc 不复位
	// mmio_write_32(0x050260d0, 0x3); // 不自动开机
	mmio_write_32(0x050260bc, 0x100); // RTC_EN_PWR_WAKEUP 设定唤醒源为 PWR_WAKEUP0
	// PWR_WAKEUP0 为上升沿触发（默认是高电平触发，会导致poweroff下去，立马又开机）
	mmio_write_32(0x0502606c, 0x16);

	/* 强制重启功能 */
	mmio_write_32(0x05026004, 0x800000); // 这个默认值就是0x800000，但不设定一下，又起不来。
	mmio_write_32(0x05026050, 0xdc780001); // 设定长按 PWR_BUTTON1 reset 去抖动时间，1s
	mmio_write_32(0x050260b8, 0xdc78000b); // 使能强制重启功能
}

void PLATFORM_IoInit(void)
{
	//pinmux 切换接口
	u_int32_t raw, mv;
	if (PLATFORM_ReadPwrAdc3(&raw, &mv) == 0){
		if (mv < 1042){
			PWR_VBATCheck();
		}
	}

	_UartPinmux();
	_MipiRxPinmux();
	_MipiTxPinmux();
	_SensorPinmux();
	_AudioPinmux();
	rtc_wakeup_config();
	#if (CONFIG_APP_DEBUG_JTAG == 1)
	JTAG_PinmuxIn();
	#endif
	i2c1_recovery_config();

	PINMUX_CONFIG(CAM_MCLK0, CAM_MCLK0);
	PINMUX_CONFIG(IIC3_SCL, IIC3_SCL);
	PINMUX_CONFIG(IIC3_SDA, IIC3_SDA);

#if 0 //evb mipi switch
	PINMUX_CONFIG(SD1_CMD, IIC3_SCL);
	PINMUX_CONFIG(SD1_CLK, IIC3_SDA);
	PINMUX_CONFIG(ADC1, PWM_3);
#endif
}

void PLATFORM_PowerOff(void)
{
//下电休眠前调用接口
}

void _PanelPinmux(void)
{
	// PWR_SEQ1 pinmux unlock
	// printf("PWR_SEQ1 pinmux unlock\n");
	// mmio_write_32(0x05027078, 0x11);
	// PINMUX_CONFIG(PWR_SEQ1, PWR_GPIO_3); // LCD_RST
	PINMUX_CONFIG(SPK_EN, XGPIOA_15); // LCD_RST
	PINMUX_CONFIG(JTAG_CPU_TCK, PWM_6); // LCD_BL
}

// 检测是否是看门狗或reboot触发的开机
static bool _IsRebootOrWatchdogWakeup(void)
{
    // bit26: REBOOT flag, bit27: WATCHDOG flag
    return ((mmio_read_32(0x050260f8) >> 26) & 0x3) != 0;
}

// 检测PWR_BUTTON1是否按下
// 返回: true-按键按下, false-按键未按下
static bool _IsPowerButtonPressed(void)
{
	// PWR_GPIO6 INPUT MODE
	mmio_write_32(0x05021004, mmio_read_32(0x05021004) & 0xFFFFFFBF);
	// DETECT PWR_WAKEUP0 LEVEL
	uint32_t key_value = mmio_read_32(0x05021050) & 0x40;
	return key_value != 0;
}

static void PowerKeyCheck(void)
{
	if (!_IsPowerButtonPressed()) {
		// 未按下电源键，走 poweroff 流程
		mmio_write_32(0x050260c0, 0x1); // 使能软件请求下电
		while (mmio_read_32(0x050260c0) != 0x1)
			;
		mmio_write_32(0x05025004, 0xab18); // 解锁对 RTC_CTRL0(0x05025008) 的读写
		while (1) {
			mmio_write_32(0x05025008, 0x10001); // 请求下电
		}
	}
}

int PLATFORM_PanelInit(void)
{
	// 看门狗或reboot触发的开机，不检测按键，直接继续启动
	if (!_IsRebootOrWatchdogWakeup()) {
		PowerKeyCheck();
	}
	_PanelPinmux();
#if (!defined(CONFIG_SUPPORT_VO) || (CONFIG_SUPPORT_VO))
#if CONFIG_PANEL_HX8394
	u8 pw_port, pw_pin, bl_port, bl_pin, rst_port, rst_pin;
	pw_port = 4;
	pw_pin = 1;
	bl_port = 4;
	bl_pin = 0;
	rst_port = 4;
	rst_pin = 2;
	_GPIOSetValue(pw_port, pw_pin, 1);
	_GPIOSetValue(bl_port, bl_pin, 1);
	_GPIOSetValue(rst_port, rst_pin, 1);
	udelay(20 * 1000);
	_GPIOSetValue(rst_port, rst_pin, 0);
	udelay(20 * 1000);
	_GPIOSetValue(rst_port, rst_pin, 1);
	udelay(20 * 1000);
#elif CONFIG_PANEL_OTA7290B
	u8 pw_port, pw_pin, bl_port, bl_pin, rst_port, rst_pin;
	pw_port = 0;
	pw_pin = 30;
	bl_port = 4;
	bl_pin = 0;
	rst_port = 0;
	rst_pin = 20;
	_GPIOSetValue(pw_port, pw_pin, 1);
	_GPIOSetValue(bl_port, bl_pin, 1);
	_GPIOSetValue(rst_port, rst_pin, 1);
	udelay(20 * 1000);
	_GPIOSetValue(rst_port, rst_pin, 0);
	udelay(20 * 1000);
	_GPIOSetValue(rst_port, rst_pin, 1);
	udelay(20 * 1000);
#elif CONFIG_DSI_LY030BXMS_480X640_NULL_2LANE_60FPS
	u8 rst_port = 0, rst_pin = 15;
	_GPIOSetValue(rst_port, rst_pin, 1);
	udelay(20 * 1000);
	_GPIOSetValue(rst_port, rst_pin, 0);
	udelay(20 * 1000);
	_GPIOSetValue(rst_port, rst_pin, 1);
	udelay(20 * 1000);
#endif
#endif

    return CVI_SUCCESS;
}

void PLATFORM_PanelBacklightCtl(int level)
{

}

int PLATFORM_IrCutCtl(int duty)
{
    return 0;
}

void PWR_VBATCheck(void)
{
	// 未按下电源键，走 poweroff 流程
	mmio_write_32(0x050260c0, 0x1); // 使能软件请求下电
	while (mmio_read_32(0x050260c0) != 0x1)
		;
	mmio_write_32(0x05025004, 0xab18); // 解锁对 RTC_CTRL0(0x05025008) 的读写
	while (1)
	{
		mmio_write_32(0x05025008, 0x10001); // 请求下电
	}
}

/*
 * 读取 PWR_ADC3 (PWR_VBAT_DET)
 *
 * PWR_ADC3 挂在 RTC 域 SARADC0 (chip_id=3, ch_id=3) 上。
 * 参考电压 VDD1.8A (1.8V)，12bit 分辨率。
 *
 * @param[out] raw   原始 ADC 值 (0~4095)
 * @param[out] mv    转换后的电压值 (mV)，可为 NULL
 * @return 0 成功，非 0 失败
 */
int PLATFORM_ReadPwrAdc3(uint32_t *raw, uint32_t *mv)
{
	csi_adc_t adc;
	int32_t value;
	csi_error_t ret;

	if (!raw)
		return -1;

	/* 1. Init ADC: chip 3 = rtc_adc0 */
	ret = csi_adc_init(&adc, 3);
	if (ret != CSI_OK) {
		printf("[FAIL] csi_adc_init(rtc_adc0) failed: %d\n", ret);
		return -1;
	}

	/* 2. Enable channel 2 (PWR_ADC3) */
	ret = csi_adc_channel_enable(&adc, 3, true);
	if (ret != CSI_OK) {
		printf("[FAIL] csi_adc_channel_enable ch2 failed: %d\n", ret);
		goto err_uninit;
	}

	/* 3. Start conversion */
	ret = csi_adc_start(&adc);
	if (ret != CSI_OK) {
		printf("[FAIL] csi_adc_start failed: %d\n", ret);
		goto err_uninit;
	}

	/* 4. Read value */
	value = csi_adc_read(&adc);
	if (value < 0) {
		printf("[FAIL] csi_adc_read failed: %d\n", value);
		csi_adc_stop(&adc);
		goto err_uninit;
	}

	/* 5. Stop */
	csi_adc_stop(&adc);

	*raw = (uint32_t)value;

	if (mv)
		*mv = (value * 1500) / 4096;   /* VDD1.8A, 12bit */

	csi_adc_uninit(&adc);
	return 0;

err_uninit:
	csi_adc_uninit(&adc);
	return -1;
}