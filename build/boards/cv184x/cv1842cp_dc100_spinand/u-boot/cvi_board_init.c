int cvi_board_init(void)
{
#if 0 /* pinmux set in alios */
        PINMUX_CONFIG(IIC0_SCL, CV_SCL0__CR_4WTDI);
        PINMUX_CONFIG(IIC0_SDA, CV_SDA0__CR_4WTDO);

        PINMUX_CONFIG(PAD_MIPI_TXP1, IIC2_SCL);
        PINMUX_CONFIG(PAD_MIPI_TXM1, IIC2_SDA);
        PINMUX_CONFIG(PAD_MIPI_TXP0, XGPIOC_13);
        PINMUX_CONFIG(PAD_MIPI_TXM0, CAM_MCLK1);

#endif
        /* AI_LED & sensor */
        /* eth to gpio */
        mmio_write_32(0x03009804, (mmio_read_32(0x03009804)) | 0x1);
        mmio_write_32(0x03009808, (mmio_read_32(0x03009808) & (~0x1f)) | 0x1);
        mmio_write_32(0x03009800, 0x0905);
        mdelay(1);
        mmio_write_32(0x0300907c, (mmio_read_32(0x0300907c) & (~0x1f00)) | 0x500);
        mmio_write_32(0x03009050, 0x4000);
        mmio_write_32(0x03009078, 0x0f00);
        mmio_write_32(0x03009074, 0x606);
        mmio_write_32(0x03009070, 0x606);

        /* AI_LED default choice */
        PINMUX_CONFIG(PAD_ETH_RXP, XGPIOB_27);
        /* CAM_LED */
        PINMUX_CONFIG(PWR_GPIO0, PWM_8);
        /* USB chrg */
        PINMUX_CONFIG(USB_VBUS_DET, XGPIOB_6);

        PINMUX_CONFIG(SPK_EN, XGPIOA_15);               /* 4G_RESET_N */

        /* tp */
        PINMUX_CONFIG(IIC0_SCL, CV_SCL0__CR_4WTDI);
        PINMUX_CONFIG(IIC0_SDA, CV_SDA0__CR_4WTDO);
        PINMUX_CONFIG(SD0_PWR_EN, XGPIOA_14);           /* INT */
        PINMUX_CONFIG(AUX0, XGPIOA_30);                 /* RST */
        /* change debug i2c to i2c4, so can use i2c0 */
        mmio_write_32(0x030000b4, 0x84);

        /* sensor */
        PINMUX_CONFIG(PAD_ETH_TXP, IIC1_SCL);
        PINMUX_CONFIG(PAD_ETH_TXM, IIC1_SDA);
        PINMUX_CONFIG(GPIO_RTX, CAM_MCLK0);

        /* wifi pin */
        PINMUX_CONFIG(JTAG_CPU_TMS, XGPIOA_19); /* WIFI_EN */
        PINMUX_CONFIG(SD1_CLK, PWR_SD1_CLK_VO37);
        PINMUX_CONFIG(SD1_CMD, PWR_SD1_CMD_VO36);
        PINMUX_CONFIG(SD1_D0, PWR_SD1_D0_VO35);
        PINMUX_CONFIG(SD1_D1, PWR_SD1_D1_VO34);
        PINMUX_CONFIG(SD1_D2, PWR_SD1_D2_VO33);
        PINMUX_CONFIG(SD1_D3, PWR_SD1_D3_VO32);

        return 0;
}
