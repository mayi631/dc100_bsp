#include "platform.h"
#include <drv/pin.h>
#include <pinctrl-mars.h>
#include "cvi_type.h"
#include "iic_recovery.h"

#define GPIO_PIN_MASK(_gpio_num) (1 << _gpio_num)

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

void PLATFORM_IoInit(void)
{
	//pinmux 切换接口
	_UartPinmux();
	_MipiRxPinmux();
	_MipiTxPinmux();
	_SensorPinmux();
	_AudioPinmux();
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

void _PWRButtonPinmux(void)
{
	// PWR_BUTTON1 pinmux unlock
	// "IOBLK_GRTC_REG_PWR_BUTTON1 0x0502_7020"
	// "FMUX_GPIO_REG_IOCTRL_PWR_BUTTON1 0x0300_1098"
	// printf("PWR_BUTTON1 pinmux unlock\n");
	// PINMUX_CONFIG(PWR_BUTTON1, PWR_GPIO_8);
	// PWR_GPIO6 INPUT MODE
	mmio_write_32(0x05021004, mmio_read_32(0x05021004) & 0xFFFFFFBF);
	// DETECT PWR_WAKEUP0 LEVEL
	uint32_t key_value = mmio_read_32(0x05021050) & 0x40;
	if(!key_value) {
		// printf("PWR_BUTTON1 is not pressed\n");
		mmio_write_32(0x050260c0, 0x1);
		while (mmio_read_32(0x050260c0) != 0x1)
			;
		mmio_write_32(0x05025004, 0xab18);
		mmio_write_32(0x03001098, 0); // 切pinmux为 PWR_BUTTON1
		mmio_write_32(0x03001090, 0); // 切pinmux为 PWR_WAKEUP0

		mmio_write_32(0x05027084, 0); // 锁定pinmux为 PWR_BUTTON1
		mmio_write_32(0x0502708c, 0); // 锁定pinmux为 PWR_WAKEUP0，防止poweroff时被重置

		mmio_write_32(0x050250ac, 0x2); // 设定 poweroff 时 rtc 不复位
		mmio_write_32(0x050260d0, 0x3); // 不自动开机
		mmio_write_32(0x050260bc, 0x1100); // RTC_EN_PWR_WAKEUP 设定唤醒源为 PWR_BUTTON1、PWR_WAKEUP0
		// 设定触发模式，PWR_BUTTON1 为低电平触发，
		// PWR_WAKEUP0 为上升沿触发（默认是高电平触发，会导致poweroff下去，立马又开机）
		mmio_write_32(0x0502606c, 0x16);

		while (1){
			mmio_write_32(0x05025008, 0x10001);
		}
	}
}

int PLATFORM_PanelInit(void)
{
	_PWRButtonPinmux();
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
#elif CONFIG_PANEL_LY030BXMS
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
