/******************************************************************************
  * @project: LT9611
  * @file: lt9611.c
  * @author: xhguo
  * @company: LONTIUM COPYRIGHT and CONFIDENTIAL
  * @date: 2017.07.25
/******************************************************************************/
/*******************************************************************************
History:
V9.0 20200612
1. Add CEC interrupter.

V10 20200612
1. update txpll power on sequence.
2. update hpd source from hpd and rx dc det.

V11 20200806
1. change hpd source from hpd only(0x8258 = 0x0a).
2. pcr_m --;

V12 20201009
1. CEC compliance Test.

V13. 20201024
1. support U3.

********************************************************************************/

#include "lt9611.h"
#define code
#define P10 1
struct Lontium_IC_Mode lt9611 = {
    single_port_mipi,  // mipi_port_cnt; //single_port_mipi or dual_port_mipi
    lane_cnt_2,        // mipi_lane_cnt; //1 or 2 or 4
    dsi,               // mipi_mode;     //dsi or csi
    Burst_Mode,
    audio_i2s,     // audio_out            //audio_i2s or audio_spdif
    dc_mode,       // hdmi_coupling_mode   //ac_mode or dc_mode
    hdcp_disable,  // hdcp_encryption      //hdcp_enable or hdcp_disable
    HDMI,          // HDMI or DVI
    RGB888         // RGB888 or YUV422
};

// typedef struct video_timing {
//     u16 hfp;110
//     u16 hs;40
//     u16 hbp;220
//     u16 hact;1280
//     u16 htotal;1650
//     u16 vfp;5
//     u16 vs;5
//     u16 vbp;20
//     u16 vact;720
//     u16 vtotal;750
//     bool h_polarity;1
//     bool v_polarity;1
//     u16 vic;
//     u8 aspact_ratio;  // 0=no data, 1=4:3, 2=16:9, 3=no data.
//     u32 pclk_khz;
// }video_timing;
code struct video_timing video_640x480_60Hz = {16, 96, 48, 640, 800, 10, 2, 33, 480, 525, 0, 0, 1, AR_4_3, 25000};
code struct video_timing video_720x480_60Hz = {16, 62, 60, 720, 858, 9, 6, 30, 480, 525, 0, 0, 2, AR_4_3, 27000};
code struct video_timing video_720x576_50Hz = {12, 64, 68, 720, 864, 5, 5, 39, 576, 625, 0, 0, 17, AR_4_3, 27000};

code struct video_timing video_1280x720_60Hz = {110, 40, 220, 1280, 1650, 5, 5, 20, 720, 750, 1, 1, 4, AR_16_9, 74250};
// code struct video_timing video_1280x720_50Hz = {440, 40, 220, 1280, 1980, 5, 5, 20, 720, 750, 1, 1, 19, AR_16_9,
// 74250};
code struct video_timing video_1280x720_50Hz = {110, 40, 220, 1280, 1650, 5, 5, 20, 720, 750, 1, 1, 4, AR_16_9, 61875};
code struct video_timing video_1280x720_30Hz = {110, 40, 220, 1280, 1650, 5, 5, 20, 720, 750, 1, 1, 4, AR_16_9, 32768};
code struct video_timing video_1280x720_24Hz = {110, 40, 220, 1280, 1650, 5, 5, 20, 720, 750, 1, 1, 4, AR_16_9, 29700};

code struct video_timing video_1920x1080_60Hz = {88,   44,   148, 1920, 2200, 4,       5,     36,
                                                 1080, 1125, 1,   1,    16,   AR_16_9, 148500};
// code struct video_timing video_1920x1080_120Hz   =   {88,  44, 148, 1920,  2200,  4,  5,  36, 1080, 1125, 1,1,0,
// AR_16_9,297000}; code struct video_timing video_1920x1080_50Hz   =    {528, 44, 148, 1920,  2640,  4,  5,  36, 1080,
// 1125, 1,1,31,AR_16_9,148500}; code struct video_timing video_1920x1080_30Hz   =    {88,  44, 148, 1920,  2200,  4, 5,
// 36, 1080, 1125, 1,1,34,AR_16_9,74250}; code struct video_timing video_1920x1080_25Hz   =    {528, 44, 148, 1920,
// 2640,  4,  5,  36, 1080, 1125, 1,1,33,AR_16_9,74250}; code struct video_timing video_1920x1080_24Hz   =    {638, 44,
// 148, 1920,  2750,  4,  5,  36, 1080, 1125, 1,1,32,AR_16_9,74250};

code struct video_timing video_3840x2160_30Hz = {176,  88,   296, 3840, 4400, 8,       10,    72,
                                                 2160, 2250, 1,   1,    95,   AR_16_9, 297000};
// code struct video_timing video_3840x2160_25Hz   =    {1056, 88, 296, 3840, 5280,  8,  10, 72, 2160, 2250,
// 1,1,94,AR_16_9, 297000}; code struct video_timing video_3840x2160_24Hz   =    {1276, 88, 296, 3840, 5500,  8,  10,
// 72, 2160, 2250, 1,1,93,AR_16_9, 297000};

// VESA-DMT
// code struct video_timing video_1440x2560_70Hz   =    {50, 20,  50,  1440,  1560,  9,  3,  13, 2560,  2585,
// 1,1,0,AR_16_9,282300};
code struct video_timing video_1366x768_60Hz = {70, 143, 213, 1366, 1792, 3, 3, 24, 768, 798, 1, 1, 0, AR_16_9, 85500};
// code struct video_timing video_1400x1050_60Hz   =    {88, 144, 232, 1400,  1864,  3,  4,  32, 1050,  1089,
// 1,1,0,AR_16_9,121750};

code struct video_timing video_800x600_60Hz = {48, 128, 88, 800, 1056, 1, 4, 23, 600, 628, 1, 1, 0, AR_16_9, 40000};

// code struct video_timing video_1024x768_60Hz    =    {24, 136, 160, 1024, 1344,  3,  6,  29, 768, 806,
// 0,0,0,AR_16_9,65000};
code struct video_timing video_1280x768_60Hz = {48,  32,  80, 1280, 1440, 3,       7,    12,
                                                768, 790, 1,  0,    0,    AR_16_9, 68250};  // reduced blank
// code struct video_timing video_1280x768_60Hz    =    {64, 128,   192, 1280, 1664,  3,  7,  20, 768, 798,
// 0,1,0,AR_16_9,79500}; //Normal code struct video_timing video_2560x1080_60Hz   =    {248,44, 148,2560,2720,  4,  5,
// 11, 1080,1100, 1,1,0,AR_16_9,198000};
////20190328
// code struct video_timing video_1280x800_60Hz    =    {72, 128, 200, 1280, 1680,  3,  6,  22, 800, 831,
// 0,1,0,AR_16_9,83500};
code struct video_timing video_1280x800_60Hz = {28, 32, 100, 1280, 1440, 2, 6, 15, 800, 823, 0, 0, 0, AR_16_9, 71000};
// code struct video_timing video_1280x960_60Hz    =    {96, 112, 312, 1280, 1800,  1,  3,  36, 960, 1000,
// 1,1,0,AR_16_9,108000};

// code struct video_timing video_1280x800_30Hz    =    {72, 128, 200, 1280, 1680,  3,  6,  22, 800, 831,
// 0,1,0,AR_16_9,41750}; code struct video_timing video_1280x960_30Hz    =    {96, 112, 312, 1280, 1800,  1,  3,  36,
// 960, 1000, 1,1,0,AR_16_9,54000};

// code struct video_timing video_1280x1024_60Hz   =    {48, 112, 248, 1280, 1688,  1,  3,  38, 1024, 1066,
// 1,1,0,AR_16_9,108000}; code struct video_timing video_1600x1200_60Hz   =    {64, 192, 304, 1600, 2160,  1,  3,  46,
// 1200, 1250, 1,1,0,AR_16_9,162000}; code struct video_timing video_1680x1050_60Hz   =    {48, 32,   80, 1680, 1840, 3,
// 6,  21, 1050, 1080, 1,0,0,AR_16_9,119000}; //reduced blank code struct video_timing video_1680x1050_60Hz   =    {104,
// 176,   280, 1680, 2240,  3,  6,  30, 1050, 1089, 0,1,0,AR_16_9,146250};//Normal

// code struct video_timing video_1280x720_30Hz    =    {1760, 40, 220,1280,  3300,  5,  5,  20, 720, 750,   1,1,0,
// AR_16_9,74250}; code struct video_timing video_1024x600_60Hz    =    {60,60, 100,1024,  1154,  2,  5, 10, 600, 617,
// 1,1,0,AR_16_9,34000};

// code struct video_timing video_2560x1440_50Hz   =    {176,272, 448,2560,  3456,  3,  5, 36, 1440, 1484,  1,1,0,
// AR_16_9,256250}; code struct video_timing video_2560x1440_60Hz   =    {48,32, 80,2560,  2720,  3,  5, 33, 1440, 1481,
// 1,1,0, AR_16_9,241500};

////20180731								        //hfp, hs, hbp,hact,htotal,vfp, vs, vbp,vact,vtotal, pclk_khz
// code struct video_timing video_2560x1600_60Hz   =    {48,32, 80,2560,  2720,  3,  6, 37, 1600, 1646,
// 1,1,0,AR_16_9,268500};

////VR timing
// code struct video_timing video_3840x1080_60Hz   =    {176,88, 296,3840,  4400,  4,  5,  36, 1080, 1125,   1,1,0,
// AR_16_9,297000}; code struct video_timing video_2560x720_60Hz    =    {220,80, 440,2560,  3300,  5,  5,  20, 720,
// 750, 1, 1, 0, AR_16_9,148500};

////mipi panel resolution
// code struct video_timing video_1080x1920_60Hz   =    {4,5,36,1080,1125,88,44,148,1920,2200,1,1,0,AR_16_9,148500};
code struct video_timing video_720x1280_60Hz    =    {5,  5,  20, 720,   750, 110,40, 220,1280,  1650, 1,1,0,AR_16_9,74250};
code struct video_timing video_540x960_60Hz = {30, 10, 30, 540, 610, 10, 10, 10, 960, 990, 1, 1, 0, AR_16_9, 36250};

// others
// code struct video_timing video_1200x1920_60Hz    ={180,60, 160, 1200, 1600,  35, 10, 25, 1920, 1990, 1, 1, 0,
// AR_16_9,191040}; code struct video_timing video_1920x720_60Hz     ={88, 44, 148, 1920, 2200,  5,  5,  20, 720,  750,
// 1, 1, 0, AR_16_9,100000};

u8 Sink_EDID[256];

struct video_timing *video;
struct cec_msg lt9611_cec_msg = {0};

static u8 CEC_RxData_Buff[16];
static u8 CEC_TxData_Buff[18];

enum PCRFormat PCR_Format;

bool flag_cec_data_received = 0;

u8 Tx_HPD = 0;
u8 pcr_m;

bool delay_1ms(u8 cnt)
{
    static u16 i = 0;
    static u16 j = 0;
    i++;
    if (i > 250) {
        i = 0;
        j++;
        // printf("\n xxxxxxxxxx");
    }
    if (j > cnt) {
        i = 0;
        j = 0;
        return 1;
    } else {
        return 0;
    }
}

void LT9611_Chip_ID(void)
{
    HDMI_WriteI2C_Byte(0xFF, 0x80);
    HDMI_WriteI2C_Byte(0xee, 0x01);

    printf("\nLT9611 ring Chip ID = 0x%x, 0x%x", HDMI_ReadI2C_Byte(0x00), HDMI_ReadI2C_Byte(0x01));

    HDMI_WriteI2C_Byte(0xFF, 0x81);
    HDMI_WriteI2C_Byte(0x01, 0x18);  // sel xtal clock
    HDMI_WriteI2C_Byte(0xFF, 0x80);
}

void LT9611_RST_PD_Init(void)
{
    /* power consumption for standby */
    HDMI_WriteI2C_Byte(0xFF, 0x81);
    HDMI_WriteI2C_Byte(0x02, 0x48);
    HDMI_WriteI2C_Byte(0x23, 0x80);
    HDMI_WriteI2C_Byte(0x30, 0x00);
    HDMI_WriteI2C_Byte(0x01, 0x00); /* i2c stop work */
}

void LT9611_LowPower_mode(bool on)
{
    /* only hpd irq is working for low power consumption */
    /* 1.8V: 15 mA */
    if (on) {
        HDMI_WriteI2C_Byte(0xFF, 0x81);
        HDMI_WriteI2C_Byte(0x02, 0x49);
        HDMI_WriteI2C_Byte(0x23, 0x80);
        HDMI_WriteI2C_Byte(0x30,
                           0x00);  // 0x00 --> 0xc0, tx phy and clk can not power down, otherwise dc det don't work.

        HDMI_WriteI2C_Byte(0xff, 0x80);
        HDMI_WriteI2C_Byte(0x11, 0x0a);
        printf("\r\n LT9611_LowPower_mode: enter low power mode ");
    } else {
        HDMI_WriteI2C_Byte(0xFF, 0x81);
        HDMI_WriteI2C_Byte(0x02, 0x12);
        HDMI_WriteI2C_Byte(0x23, 0x40);
        HDMI_WriteI2C_Byte(0x30, 0xea);

        HDMI_WriteI2C_Byte(0xff, 0x80);
        HDMI_WriteI2C_Byte(0x11, 0xfa);
        printf("\r\n LT9611_LowPower_mode: exit low power mode ");
    }
}

void LT9611_System_Init(void)  // dsren
{
    HDMI_WriteI2C_Byte(0xFF, 0x82);
    //	  HDMI_WriteI2C_Byte(0x45,0x70);//RGB SWAP
    HDMI_WriteI2C_Byte(0x51, 0x11);
    // Timer for Frequency meter
    HDMI_WriteI2C_Byte(0x1b, 0x69);  // Timer 2
    HDMI_WriteI2C_Byte(0x1c, 0x78);
    HDMI_WriteI2C_Byte(0xcb, 0x69);  // Timer 1
    HDMI_WriteI2C_Byte(0xcc, 0x78);

    /*power consumption for work*/
    HDMI_WriteI2C_Byte(0xff, 0x80);
    HDMI_WriteI2C_Byte(0x04, 0xf0);
    HDMI_WriteI2C_Byte(0x06, 0xf0);
    HDMI_WriteI2C_Byte(0x0a, 0x80);
    HDMI_WriteI2C_Byte(0x0b, 0x46);  // csc clk//46
    HDMI_WriteI2C_Byte(0x0d, 0xef);
    HDMI_WriteI2C_Byte(0x11, 0xfa);
}

void LT9611_MIPI_Input_Analog(void)  // xuxi
{
    // mipi mode
    HDMI_WriteI2C_Byte(0xff, 0x81);
    //	HDMI_WriteI2C_Byte(0x03,0xF8); //Dp/Dn swap
    HDMI_WriteI2C_Byte(0x06, 0x60);  // port A rx current
    HDMI_WriteI2C_Byte(0x07, 0x3f);  // eq
    HDMI_WriteI2C_Byte(0x08, 0x3f);  // eq
    HDMI_WriteI2C_Byte(0x0a, 0xfe);  // port A ldo voltage set
    HDMI_WriteI2C_Byte(0x0b, 0xbf);  // enable port A lprx

    HDMI_WriteI2C_Byte(0x11, 0x60);  // port B rx current
    HDMI_WriteI2C_Byte(0x12, 0x3f);  // eq
    HDMI_WriteI2C_Byte(0x13, 0x3f);  // eq
    HDMI_WriteI2C_Byte(0x15, 0xfe);  // port B ldo voltage set
    HDMI_WriteI2C_Byte(0x16, 0xbf);  // enable port B lprx

    HDMI_WriteI2C_Byte(0x1c, 0x03);  // PortA clk lane no-LP mode.
    HDMI_WriteI2C_Byte(0x20, 0x03);  // PortB clk lane no-LP mode.
}

void LT9611_MIPI_Input_Digtal(void)  // weiguo
{
    u8 lanes;
    u8 settle_count;
    lanes = lt9611.mipi_lane_cnt;

    printf("\nLT9611_MIPI_Input_Digtal: lt9611 set mipi lanes = %d", lanes);

    HDMI_WriteI2C_Byte(0xff, 0x82);
    HDMI_WriteI2C_Byte(0x4f, 0x80);  //[7] = Select ad_txpll_d_clk.

    if (lt9611.mipi_port_cnt == single_port_mipi) {
        if (P10)  // portA input
        {
            HDMI_WriteI2C_Byte(0xff, 0x82);
            HDMI_WriteI2C_Byte(0x50, 0x10);
            HDMI_WriteI2C_Byte(0xff, 0x83);
            HDMI_WriteI2C_Byte(0x03, 0x00);
            printf("\nLT9611_MIPI_Input_Digtal: portA input");
            printf("\r\nport A lane 0 settle = 0x%x", HDMI_ReadI2C_Byte(0x98));
            printf("\r\nport A lane 1 settle = 0x%x", HDMI_ReadI2C_Byte(0x9a));
            printf("\r\nport A lane 2 settle = 0x%x", HDMI_ReadI2C_Byte(0x9c));
            printf("\r\nport A lane 3 settle = 0x%x", HDMI_ReadI2C_Byte(0x9e));
            settle_count = HDMI_ReadI2C_Byte(0x98);
        } else  // portB input
        {
            HDMI_WriteI2C_Byte(0xff, 0x82);
            HDMI_WriteI2C_Byte(0x50, 0x14);
            HDMI_WriteI2C_Byte(0xff, 0x83);
            HDMI_WriteI2C_Byte(0x03, 0x40);
            printf("\nLT9611_MIPI_Input_Digtal: portB input");
            printf("\r\nport B lane 0 settle = 0x%x", HDMI_ReadI2C_Byte(0xA0));
            printf("\r\nport B lane 1 settle = 0x%x", HDMI_ReadI2C_Byte(0xA2));
            printf("\r\nport B lane 2 settle = 0x%x", HDMI_ReadI2C_Byte(0xA4));
            printf("\r\nport B lane 3 settle = 0x%x", HDMI_ReadI2C_Byte(0xA6));
            settle_count = HDMI_ReadI2C_Byte(0xA0);
        }
    } else {
        HDMI_WriteI2C_Byte(0xff, 0x82);
        HDMI_WriteI2C_Byte(0x50, 0x10);
        HDMI_WriteI2C_Byte(0xff, 0x83);
        HDMI_WriteI2C_Byte(0x03, 0x00);
        printf("\nLT9611_MIPI_Input_Digtal: portA+B input");
        printf("\r\nport A lane 0 settle = 0x%x", HDMI_ReadI2C_Byte(0x98));
        printf("\r\nport B lane 0 settle = 0x%x", HDMI_ReadI2C_Byte(0xA0));
        settle_count = HDMI_ReadI2C_Byte(0x98);
    }

    if (settle_count > 0x40) {
        settle_count = 0x06;
    } else {
        settle_count = settle_count / 2;
    }
    printf("\r\nsettle = 0x%x", settle_count);

    HDMI_WriteI2C_Byte(0xff, 0x83);
    HDMI_WriteI2C_Byte(0x00, lanes);         // PortA lane cnts
    HDMI_WriteI2C_Byte(0x04, lanes);         // PortB lane cnts
    HDMI_WriteI2C_Byte(0x02, settle_count);  // settle
    HDMI_WriteI2C_Byte(0x06, settle_count);  // settle

    if (lt9611.mipi_port_cnt == dual_port_mipi)  // dual_port_mipi
    {
        HDMI_WriteI2C_Byte(0x0a, 0x03);  // 1=dual_lr, 0=dual_en
        printf("\nLT9611_MIPI_Input_Digtal: lt9611 set mipi port = 2");
    } else  // single_port_mipi
    {
        HDMI_WriteI2C_Byte(0x0a, 0x00);  // 1=dual_lr, 0=dual_en
        printf("\nLT9611_MIPI_Input_Digtal: lt9611 set mipi ports = 1");
    }

#if 1
    if (lt9611.mipi_mode == csi) {
        printf("\nLT9611_MIPI_Input_Digtal: LT9611.mipi_mode = csi");
        HDMI_WriteI2C_Byte(0xff, 0x83);
        HDMI_WriteI2C_Byte(0x08, 0x10);  // csi_en
        HDMI_WriteI2C_Byte(0x2c, 0x40);  // csi_sel

        if (lt9611.input_color_space == RGB888) {
            HDMI_WriteI2C_Byte(0xff, 0x83);
            HDMI_WriteI2C_Byte(0x1c, 0x01);
        }
    } else
        printf("\nLT9611_MIPI_Input_Digtal: LT9611.mipi_mode = dsi");
#endif
}

void LT8618SX_Print_Video_Inf()
{
    video_timing _lt86x;
    video_timing *lt86x = &_lt86x;
    u8 temp;

    HDMI_WriteI2C_Byte(0xff, 0x82);  // video check
    temp = HDMI_ReadI2C_Byte(0x70);  // hs vs polarity
    if (temp & 0x02) {
        printf("\r\n vs_pol is 1");
    } else {
        printf("\r\n vs_pol is 0");
    }
    if (temp & 0x01) {
        printf("\r\n hs_pol is 1");
    } else {
        printf("\r\n hs_pol is 0");
    }

    lt86x->vs = HDMI_ReadI2C_Byte(0x71);

    lt86x->hs = HDMI_ReadI2C_Byte(0x72);
    lt86x->hs = ((lt86x->hs & 0x0f) << 8) + HDMI_ReadI2C_Byte(0x73);

    lt86x->vbp = HDMI_ReadI2C_Byte(0x74);
    lt86x->vfp = HDMI_ReadI2C_Byte(0x75);

    lt86x->hbp = HDMI_ReadI2C_Byte(0x76);
    lt86x->hbp = ((lt86x->hbp & 0x0f) << 8) + HDMI_ReadI2C_Byte(0x77);

    lt86x->hfp = HDMI_ReadI2C_Byte(0x78);
    lt86x->hfp = ((lt86x->hfp & 0x0f) << 8) + HDMI_ReadI2C_Byte(0x79);

    lt86x->vtotal = HDMI_ReadI2C_Byte(0x7a);
    lt86x->vtotal = (lt86x->vtotal << 8) + HDMI_ReadI2C_Byte(0x7b);

    lt86x->htotal = HDMI_ReadI2C_Byte(0x7c);
    lt86x->htotal = (lt86x->htotal << 8) + HDMI_ReadI2C_Byte(0x7d);

    lt86x->vact = HDMI_ReadI2C_Byte(0x7e);
    lt86x->vact = (lt86x->vact << 8) + HDMI_ReadI2C_Byte(0x7f);

    lt86x->hact = HDMI_ReadI2C_Byte(0x80);
    lt86x->hact = (lt86x->hact << 8) + HDMI_ReadI2C_Byte(0x81);

    printf("%s\n", __FUNCTION__);
    printf("##########################LT8618SX Input Infor#####################\n");
    printf("h_tal = %d\n", lt86x->htotal);
    printf("h_act = %d\n", lt86x->hact);
    printf("hfp = %d\n", lt86x->hfp);
    printf("hs_width = %d\n", lt86x->hs);
    printf("hbp = %d\n", lt86x->hbp);

    printf("v_tal = %d\n", lt86x->vtotal);
    printf("v_act = %d\n", lt86x->vact);
    printf("vfp = %d\n", lt86x->vfp);
    printf("vs_width = %d\n", lt86x->vs);
    printf("vbp = %d\n", lt86x->vbp);

    printf("---------------------------------------------------------\n");
}

void LT9611_Video_Check(void)  // dsren
{
    // LT8618SX_Print_Video_Inf();

#if 1
    u8 mipi_video_format = 0x00;
    u16 h_act, h_act_a, h_act_b, v_act, v_tal;
    u16 h_total_sysclk;

    HDMI_WriteI2C_Byte(0xff, 0x82);  // top video check module
    h_total_sysclk = HDMI_ReadI2C_Byte(0x86);
    h_total_sysclk = (h_total_sysclk << 8) + HDMI_ReadI2C_Byte(0x87);
    printf("\33[32m");
    printf("\n-----------------------------------------------------------------------------");
    printf("\nLT9611_Video_Check: h_total_sysclk = %d", h_total_sysclk);

    v_act = HDMI_ReadI2C_Byte(0x82);
    v_act = (v_act << 8) + HDMI_ReadI2C_Byte(0x83);
    v_tal = HDMI_ReadI2C_Byte(0x6c);
    v_tal = (v_tal << 8) + HDMI_ReadI2C_Byte(0x6d);

    HDMI_WriteI2C_Byte(0xff, 0x83);
    h_act_a = HDMI_ReadI2C_Byte(0x82);
    h_act_a = (h_act_a << 8) + HDMI_ReadI2C_Byte(0x83);

    h_act_b = HDMI_ReadI2C_Byte(0x86);
    h_act_b = (h_act_b << 8) + HDMI_ReadI2C_Byte(0x87);

    if (lt9611.input_color_space == YUV422) {
        printf("\nLT9611_Video_Check: lt9611.input_color_space = YUV422");
        h_act_a /= 2;
        h_act_b /= 2;
    } else if (lt9611.input_color_space == RGB888) {
        printf("\nLT9611_Video_Check: lt9611.input_color_space = RGB888");
        h_act_a /= 3;
        h_act_b /= 3;
    }

    mipi_video_format = HDMI_ReadI2C_Byte(0x88);

    printf("\nLT9611_Video_Check: h_act_a, h_act_b, v_act, v_tal: %d, %d, %d, %d, ", h_act_a, h_act_b, v_act, v_tal);
    printf("\nLT9611_Video_Check: mipi_video_format: 0x%x", mipi_video_format);

    if (lt9611.mipi_port_cnt == dual_port_mipi)  // dual port.
        h_act = h_act_a + h_act_b;
    else
        h_act = h_act_a;

    printf("\r\nLT9611_Video_Check: PCR_Format =");
    ///////////////////////formate detect///////////////////////////////////

    // DTV
    if ((h_act == video_640x480_60Hz.hact) && (v_act == video_640x480_60Hz.vact)) {
        printf(" video_640x480_60Hz ");
        PCR_Format = PCR_640x480_60Hz;
        video      = &video_640x480_60Hz;
    } else if ((h_act == (video_720x480_60Hz.hact)) && (v_act == video_720x480_60Hz.vact)) {
        printf(" video_720x480_60Hz ");
        PCR_Format = PCR_Standard;
        video      = &video_720x480_60Hz;
    }

    else if ((h_act == (video_720x576_50Hz.hact)) && (v_act == video_720x576_50Hz.vact)) {
        printf(" video_720x576_50Hz ");
        PCR_Format = PCR_Standard;
        video      = &video_720x576_50Hz;
    }

    else if ((h_act == video_1280x720_60Hz.hact) && (v_act == video_1280x720_60Hz.vact)) {
        printf(" video_1280x720 match: \n");
        if (h_total_sysclk < 630) {
            printf(" video_1280x720_60Hz ");
            PCR_Format = PCR_Standard;
            video      = &video_1280x720_60Hz;
        } else if (h_total_sysclk < 750) {
            printf(" video_1280x720_50Hz ");
            PCR_Format = PCR_Standard;
            video      = &video_1280x720_50Hz;
        } else if (h_total_sysclk < 1230) {
            printf(" video_1280x720_30Hz ");
            PCR_Format = PCR_Standard;
            video      = &video_1280x720_30Hz;
        } else {
            printf(" video_1280x720_24Hz ");
            PCR_Format = PCR_Standard;
            video      = &video_1280x720_24Hz;
        }
    }

    else if ((h_act == video_1920x1080_60Hz.hact) && (v_act == video_1920x1080_60Hz.vact))  // 1080P
    {
        if (h_total_sysclk < 430) {
            printf(" video_1920x1080_60Hz ");
            PCR_Format = PCR_Standard;
            video      = &video_1920x1080_60Hz;
        }
        //			else if(h_total_sysclk < 230)
        //			{
        //				printf(" video_1920x1080_120Hz ");
        //				PCR_Format=PCR_Standard;
        //				video = &video_1920x1080_120Hz;
        //			}
        //		  else if(h_total_sysclk < 510)
        //			{
        //				printf(" video_1920x1080_50Hz ");
        //				PCR_Format=PCR_Standard;
        //				video = &video_1920x1080_50Hz;
        //			}

        //		  else if(h_total_sysclk < 830)
        //			{
        //				printf(" video_1920x1080_30Hz ");
        //				PCR_Format=PCR_Standard;
        //				video = &video_1920x1080_30Hz;
        //			}

        //		  else if(h_total_sysclk < 980)
        //			{
        //				printf(" video_1920x1080_25Hz ");
        //				PCR_Format=PCR_Standard;
        //				video = &video_1920x1080_25Hz;
        //			}

        //			else if(h_total_sysclk < 1030)
        //			{
        //				printf(" video_1920x1080_24Hz ");
        //				PCR_Format=PCR_Standard;
        //				video = &video_1920x1080_24Hz;
        //			}
    }

    else if ((h_act == video_3840x2160_30Hz.hact) && (v_act == video_3840x2160_30Hz.vact))  // 2160P
    {
        //			if(h_total_sysclk < 430)
        //			{
        printf(" video_3840x2160_30Hz ");
        PCR_Format = PCR_Standard;
        video      = &video_3840x2160_30Hz;
        //			}
        //			else if(h_total_sysclk < 490)
        //			{
        //				printf(" video_3840x2160_25Hz ");
        //				PCR_Format=PCR_Standard;
        //				video = &video_3840x2160_25Hz;
        //			}
        //			else if(h_total_sysclk < 520)
        //			{
        //				printf(" video_3840x2160_24Hz ");
        //				PCR_Format=PCR_Standard;
        //				video = &video_3840x2160_24Hz;
        //			}
    }

    //	else if((h_act==(video_1440x2560_70Hz.hact))&&(v_act==video_1440x2560_70Hz.vact))
    //	{
    //		printf(" video_1440x2560_70Hz ");
    //		PCR_Format = PCR_Standard;
    //		video = &video_1440x2560_70Hz;
    //	}
    else if ((h_act == video_1366x768_60Hz.hact) && (v_act == video_1366x768_60Hz.vact)) {
        printf(" video_1366x768_60Hz ");
        PCR_Format = PCR_Standard;
        video      = &video_1366x768_60Hz;
    }
    //  else if((h_act==video_1400x1050_60Hz.hact)&&(v_act==video_1400x1050_60Hz.vact))
    //	{
    //		printf(" video_1400x1050_60Hz ");
    //		PCR_Format=PCR_Standard;
    //		video = &video_1400x1050_60Hz;
    //	}

    else if ((h_act == (video_800x600_60Hz.hact)) && (v_act == video_800x600_60Hz.vact)) {
        printf(" video_800x600_60Hz ");
        PCR_Format = PCR_Standard;
        video      = &video_800x600_60Hz;
    }

    //	else if((h_act==(video_1024x768_60Hz.hact))&&(v_act==video_1024x768_60Hz.vact))
    //	{
    //		printf(" video_1024x768_60Hz ");
    //		PCR_Format = PCR_Standard;
    //		video = &video_1024x768_60Hz;
    //	}

    else if ((h_act == (video_1280x768_60Hz.hact)) && (v_act == video_1280x768_60Hz.vact)) {
        printf(" video_1280x768_60Hz ");
        PCR_Format = PCR_Standard;
        video      = &video_1280x768_60Hz;
    }

    else if ((h_act == (video_1280x800_60Hz.hact)) && (v_act == video_1280x800_60Hz.vact)) {
        //			if(h_total_sysclk < 560)
        //			{
        printf(" video_1280x800_60Hz ");
        PCR_Format = PCR_Standard;
        video      = &video_1280x800_60Hz;
        //			}
        //			else
        //			{
        //				printf(" video_1280x800_30Hz ");
        //				PCR_Format = PCR_Standard;
        //				video = &video_1280x800_30Hz;
        //			}
    }

    //	else if((h_act==(video_1280x960_60Hz.hact))&&(v_act==video_1280x960_60Hz.vact))
    //	{

    //			if(h_total_sysclk < 460)
    //			{
    //				printf(" video_1280x960_60Hz ");
    //				PCR_Format = PCR_Standard;
    //				video = &video_1280x960_60Hz;
    //			}
    //			else
    //			{
    //				printf(" video_1280x960_30Hz ");
    //				PCR_Format = PCR_Standard;
    //				video = &video_1280x960_30Hz;
    //			}
    //	}

    //	else if((h_act==(video_1280x1024_60Hz.hact))&&(v_act==video_1280x1024_60Hz.vact))
    //	{
    //			printf(" video_1280x1024_60Hz ");
    //			PCR_Format = PCR_Standard;
    //			video = &video_1280x1024_60Hz;
    //	}

    //	else if((h_act==(video_1600x1200_60Hz.hact))&&(v_act==video_1600x1200_60Hz.vact))
    //	{
    //			printf(" video_1600x1200_60Hz ");
    //			PCR_Format = PCR_Standard;
    //			video = &video_1600x1200_60Hz;
    //	}

    //	else if((h_act==(video_1680x1050_60Hz.hact))&&(v_act==video_1680x1050_60Hz.vact))
    //	{
    //			printf(" video_1680x1050_60Hz ");
    //			PCR_Format = PCR_Standard;
    //			video = &video_1680x1050_60Hz;
    //	}

    //	else if((h_act==video_2560x1440_60Hz.hact)&&(v_act==video_2560x1440_60Hz.vact))
    //	{
    //			if(h_total_sysclk < 310)
    //			{
    //			  printf(" video_2560x1440_60Hz ");
    //			  PCR_Format = PCR_Standard;
    //			  video = &video_2560x1440_60Hz;
    //			}

    //			else if(h_total_sysclk < 370)
    //			{
    //		   	printf(" video_2560x1440_50Hz ");
    //			  PCR_Format = PCR_Standard;
    //			  video = &video_2560x1440_50Hz;
    //			}
    //	}

    	else if((h_act== video_720x1280_60Hz.hact)&&(v_act== video_720x1280_60Hz.vact))
    	{
    		printf(" video_720x1080_60Hz ");
    		PCR_Format=PCR_Standard;
    		video = &video_720x1280_60Hz;
    	}

    //	else if((h_act==video_1080x1920_60Hz.hact)&&(v_act==video_1080x1920_60Hz.vact))//&&
    //	{
    //		printf(" video_1080x1920_60Hz ");
    //		PCR_Format=PCR_Standard;
    //		video = &video_1080x1920_60Hz;
    //	}
    //
    //	else if((h_act==video_3840x1080_60Hz.hact)&&(v_act==video_3840x1080_60Hz.vact))//&&
    //	{
    //		printf(" video_3840x1080_60Hz ");
    //		PCR_Format = PCR_Standard;
    //		video = &video_3840x1080_60Hz;
    //	}
    //	else if((h_act==video_1024x600_60Hz.hact)&&(v_act==video_1024x600_60Hz.vact))//&&
    //	{
    //		printf(" video_1024x600_60Hz ");
    //		PCR_Format = PCR_Standard;
    //		video = &video_1024x600_60Hz;
    //	}
    //	else if((h_act==video_1280x800_60Hz.hact)&&(v_act==video_1280x800_60Hz.vact))//&&
    //	{
    //		printf(" video_1280x800_60Hz ");
    //		PCR_Format = PCR_Standard;
    //		video = &video_1280x800_60Hz;
    //	}
    else if ((h_act == video_540x960_60Hz.hact) && (v_act == video_540x960_60Hz.vact))  //&&
    {
        printf(" video_540x960_60Hz ");
        PCR_Format = PCR_Standard;
        video      = &video_540x960_60Hz;
    }
    //
    //	else if((h_act==video_2560x1600_60Hz.hact)&&(v_act==video_2560x1600_60Hz.vact))//&&
    //	{
    //		printf(" video_2560x1600_60Hz ");
    //		PCR_Format = PCR_Standard;
    //		video = &video_2560x1600_60Hz;
    //	}

    //	else if((h_act==video_2560x1080_60Hz.hact)&&(v_act==video_2560x1080_60Hz.vact))//&&
    //	{
    //		printf(" video_2560x1080_60Hz ");
    //		PCR_Format = PCR_Standard;
    //		video = &video_2560x1080_60Hz;
    //	}
    //
    //	else if((h_act==video_2560x720_60Hz.hact)&&(v_act==video_2560x720_60Hz.vact))//&&
    //	{
    //		printf(" video_2560x720_60Hz ");
    //		PCR_Format = PCR_Standard;
    //		video = &video_2560x720_60Hz;
    //	}

    //	else if((h_act==video_1200x1920_60Hz.hact)&&(v_act==video_1200x1920_60Hz.vact))//&&
    //	{
    //		printf(" video_1200x1920_60Hz ");
    //		PCR_Format = PCR_Standard;
    //		video = &video_1200x1920_60Hz;
    //	}
    //	else if((h_act==video_1920x720_60Hz.hact)&&(v_act==video_1920x720_60Hz.vact))//&&
    //	{
    //		printf(" video_1920x720_60Hz ");
    //		PCR_Format = PCR_Standard;
    //		video = &video_1920x720_60Hz;
    //	}

    else {
        PCR_Format = PCR_None;
        printf(" unknown video format ");
    }
    printf("\n-----------------------------------------------------------------------------");
    printf("\033[37m\033[37m");
#endif
}

void LT9611_MIPI_Video_Timing(struct video_timing *video_format)  // weiguo
{
    HDMI_WriteI2C_Byte(0xff, 0x83);
    HDMI_WriteI2C_Byte(0x0d, (u8)(video_format->vtotal / 256));
    HDMI_WriteI2C_Byte(0x0e, (u8)(video_format->vtotal % 256));  // vtotal
    HDMI_WriteI2C_Byte(0x0f, (u8)(video_format->vact / 256));
    HDMI_WriteI2C_Byte(0x10, (u8)(video_format->vact % 256));  // vactive
    HDMI_WriteI2C_Byte(0x11, (u8)(video_format->htotal / 256));
    HDMI_WriteI2C_Byte(0x12, (u8)(video_format->htotal % 256));  // htotal
    HDMI_WriteI2C_Byte(0x13, (u8)(video_format->hact / 256));
    HDMI_WriteI2C_Byte(0x14, (u8)(video_format->hact % 256));                      // hactive
    HDMI_WriteI2C_Byte(0x15, (u8)(video_format->vs % 256));                        // vsa
    HDMI_WriteI2C_Byte(0x16, (u8)(video_format->hs % 256));                        // hsa
    HDMI_WriteI2C_Byte(0x17, (u8)(video_format->vfp % 256));                       // vfp
    HDMI_WriteI2C_Byte(0x18, (u8)((video_format->vs + video_format->vbp) % 256));  // vss
    HDMI_WriteI2C_Byte(0x19, (u8)(video_format->hfp % 256));                       // hfp
    HDMI_WriteI2C_Byte(
        0x1a, (u8)(((video_format->hfp / 256) << 4) + (video_format->hs + video_format->hbp) / 256));  // 20180901
    HDMI_WriteI2C_Byte(0x1b, (u8)((video_format->hs + video_format->hbp) % 256));                      // hss
}

void LT9611_MIPI_Pcr(struct video_timing *video_format)  // weiguo
{
    u8 POL;
    u16 hact;
    hact = video_format->hact;
    POL  = (video_format->h_polarity) * 0x02 + (video_format->v_polarity);
    POL  = ~POL;
    POL &= 0x03;

    HDMI_WriteI2C_Byte(0xff, 0x83);

    if (lt9611.mipi_port_cnt == dual_port_mipi)  // dual port
    {
        hact = (hact >> 2);
        hact += 0x50;

        hact = (0x3e0 > hact ? hact : 0x3e0);

        HDMI_WriteI2C_Byte(0x0b, (u8)(hact >> 8));  // vsync mode
        HDMI_WriteI2C_Byte(0x0c, (u8)hact);         //=1/4 hact
        // hact -=0x40;
        HDMI_WriteI2C_Byte(0x48, (u8)(hact >> 8));  // de mode delay
        HDMI_WriteI2C_Byte(0x49, (u8)(hact));       //
    }

    else {
        HDMI_WriteI2C_Byte(0x0b, 0x01);  // vsync read delay(reference value)
        HDMI_WriteI2C_Byte(0x0c, 0x10);  //

        HDMI_WriteI2C_Byte(0x48, 0x00);  // de mode delay
        HDMI_WriteI2C_Byte(0x49, 0x81);  //=1/4 hact
    }

    /* stage 1 */
    HDMI_WriteI2C_Byte(0x21, 0x4a);  // bit[3:0] step[11:8]
    // HDMI_WriteI2C_Byte(0x22,0x40);//step[7:0]

    HDMI_WriteI2C_Byte(0x24, 0x71);  // bit[7:4]v/h/de mode; line for clk stb[11:8]
    HDMI_WriteI2C_Byte(0x25, 0x30);  // line for clk stb[7:0]

    HDMI_WriteI2C_Byte(0x2a, 0x01);  // clk stable in

    /* stage 2 */
    HDMI_WriteI2C_Byte(0x4a, 0x40);          // offset //0x10
    HDMI_WriteI2C_Byte(0x1d, (0x10 | POL));  // PCR de mode step setting.

    /* MK limit */

    //	switch(PCR_Format)
    //	{
    //		case PCR_640x480_60Hz:
    //		HDMI_WriteI2C_Byte(0xff,0x83);
    //		HDMI_WriteI2C_Byte(0x0b,0x02);
    //		HDMI_WriteI2C_Byte(0x0c,0x40);
    //		HDMI_WriteI2C_Byte(0x48,0x01);
    //		HDMI_WriteI2C_Byte(0x49,0x10);
    //		HDMI_WriteI2C_Byte(0x24,0x70);
    //		HDMI_WriteI2C_Byte(0x25,0x80);
    //		HDMI_WriteI2C_Byte(0x2a,0x10);
    //		HDMI_WriteI2C_Byte(0x2b,0x80);
    //		HDMI_WriteI2C_Byte(0x23,0x28);
    //		HDMI_WriteI2C_Byte(0x4a,0x10);
    //		HDMI_WriteI2C_Byte(0x1d,0xf3);
    //		printf("\r\nLT9611_MIPI_Pcr: 640x480_60Hz");
    //		break;
    //
    //		case PCR_1024x600_60Hz:
    //		HDMI_WriteI2C_Byte(0x24,0x70); //bit[7:4]v/h/de mode; line for clk stb[11:8]
    //		HDMI_WriteI2C_Byte(0x25,0x80); //line for clk stb[7:0]
    //		HDMI_WriteI2C_Byte(0x2a,0x10); //clk stable in
    //		/* stage 2 */
    //		//HDMI_WriteI2C_Byte(0x23,0x04); //pcr h mode step
    //		//HDMI_WriteI2C_Byte(0x4a,0x10); //offset //0x10
    //		HDMI_WriteI2C_Byte(0x1d,0xf0); //PCR de mode step setting.
    //		break;
    //
    //		default: break;
    //	}

    LT9611_MIPI_Video_Timing(video);

    HDMI_WriteI2C_Byte(0xff, 0x83);
    HDMI_WriteI2C_Byte(0x26, pcr_m);

    HDMI_WriteI2C_Byte(0xff, 0x80);
    HDMI_WriteI2C_Byte(0x11, 0x5a);  // Pcr reset
    HDMI_WriteI2C_Byte(0x11, 0xfa);
}

void LT9611_PLL(struct video_timing *video_format)  // zhangzhichun
{
    u32 pclk;
    u8 pll_lock_flag, cal_done_flag, band_out;
    u8 hdmi_post_div;
    u32 i;
    pclk = video_format->pclk_khz;
    printf("\r\nLT9611_PLL: set rx pll = %ld", pclk);

    HDMI_WriteI2C_Byte(0xff, 0x81);
    HDMI_WriteI2C_Byte(0x23, 0x40);  // Enable LDO and disable PD
    HDMI_WriteI2C_Byte(0x24, 0x62);  // 0x62, LG25UM58 issue, 20180824
    HDMI_WriteI2C_Byte(0x25, 0x80);  // pre-divider
    HDMI_WriteI2C_Byte(0x26, 0x55);
    HDMI_WriteI2C_Byte(0x2c, 0x37);
    // HDMI_WriteI2C_Byte(0x2d,0x99); //txpll_divx_set&da_txpll_freq_set
    // HDMI_WriteI2C_Byte(0x2e,0x01);
    HDMI_WriteI2C_Byte(0x2f, 0x01);
    // HDMI_WriteI2C_Byte(0x26,0x55);
    HDMI_WriteI2C_Byte(0x27, 0x66);
    HDMI_WriteI2C_Byte(0x28, 0x88);

    HDMI_WriteI2C_Byte(0x2a, 0x20);  // for U3.

    if (pclk > 150000) {
        HDMI_WriteI2C_Byte(0x2d, 0x88);
        hdmi_post_div = 0x01;
    } else if (pclk > 80000) {
        HDMI_WriteI2C_Byte(0x2d, 0x99);
        hdmi_post_div = 0x02;
    } else {
        HDMI_WriteI2C_Byte(0x2d, 0xaa);  // 0xaa
        hdmi_post_div = 0x04;
    }

    pcr_m = (u8)((pclk * 5 * hdmi_post_div) / 27000);
    pcr_m--;
    printf("\r\nLT9611_PLL: pcr_m = 0x%x, hdmi_post_div = %d", pcr_m, hdmi_post_div);  // Hex

    HDMI_WriteI2C_Byte(0xff, 0x83);
    HDMI_WriteI2C_Byte(0x2d, 0x40);         // M up limit
    HDMI_WriteI2C_Byte(0x31, 0x08);         // M down limit
    HDMI_WriteI2C_Byte(0x26, 0x80 | pcr_m); /* fixed M is to let pll locked*/

    pclk = pclk / 2;
    HDMI_WriteI2C_Byte(0xff, 0x82);  // 13.5M
    HDMI_WriteI2C_Byte(0xe3, pclk / 65536);
    pclk = pclk % 65536;
    HDMI_WriteI2C_Byte(0xe4, pclk / 256);
    HDMI_WriteI2C_Byte(0xe5, pclk % 256);

    HDMI_WriteI2C_Byte(0xde, 0x20);  // pll cal en, start calibration
    HDMI_WriteI2C_Byte(0xde, 0xe0);

    HDMI_WriteI2C_Byte(0xff, 0x80);
    HDMI_WriteI2C_Byte(0x11, 0x5a); /* Pcr clk reset */
    HDMI_WriteI2C_Byte(0x11, 0xfa);
    HDMI_WriteI2C_Byte(0x16, 0xf2); /* pll cal digital reset */
    HDMI_WriteI2C_Byte(0x18, 0xdc); /* pll analog reset */
    HDMI_WriteI2C_Byte(0x18, 0xfc);
    HDMI_WriteI2C_Byte(0x16, 0xf3); /*start calibration*/

    /* pll lock status */
    for (i = 0; i < 120000; i++) {
        HDMI_WriteI2C_Byte(0xff, 0x80);
        HDMI_WriteI2C_Byte(0x16, 0xe3); /* pll lock logic reset */
        HDMI_WriteI2C_Byte(0x16, 0xf3);
        HDMI_WriteI2C_Byte(0xff, 0x82);
        cal_done_flag = HDMI_ReadI2C_Byte(0xe7);
        band_out      = HDMI_ReadI2C_Byte(0xe6);
        pll_lock_flag = HDMI_ReadI2C_Byte(0x15);

        if ((pll_lock_flag & 0x80) && (cal_done_flag & 0x80) && (band_out != 0xff)) {
            printf("\r\nLT9611_PLL: HDMI pll locked,band out: 0x%x", band_out);
            break;
        } else {
            HDMI_WriteI2C_Byte(0xff, 0x80);
            HDMI_WriteI2C_Byte(0x11, 0x5a); /* Pcr clk reset */
            HDMI_WriteI2C_Byte(0x11, 0xfa);
            HDMI_WriteI2C_Byte(0x16, 0xf2); /* pll cal digital reset */
            HDMI_WriteI2C_Byte(0x18, 0xdc); /* pll analog reset */
            HDMI_WriteI2C_Byte(0x18, 0xfc);
            HDMI_WriteI2C_Byte(0x16, 0xf3); /*start calibration*/
            printf("\r\nLT9611_PLL: HDMI pll unlocked, reset pll");
        }
    }
}

void LT9611_HDMI_TX_Phy(void)  // xyji
{
    HDMI_WriteI2C_Byte(0xff, 0x81);
    HDMI_WriteI2C_Byte(0x30, 0x6a);  // tap0/1
    if (lt9611.hdmi_coupling_mode == ac_mode) {
        HDMI_WriteI2C_Byte(0x31, 0x73);  // DC: 0x44, AC:0x73
        printf("\r\nLT9611_HDMI_TX_Phy: AC couple");
    } else  // lt9611.hdmi_coupling_mode==dc_mode
    {
        HDMI_WriteI2C_Byte(0x31, 0x44);  // DC: 0x44, AC:0x73
        printf("\r\nLT9611_HDMI_TX_Phy: DC couple");
    }
    HDMI_WriteI2C_Byte(0x32, 0x4a);
    HDMI_WriteI2C_Byte(0x33, 0x0b);
    HDMI_WriteI2C_Byte(0x34, 0x00);
    HDMI_WriteI2C_Byte(0x35, 0x00);
    HDMI_WriteI2C_Byte(0x36, 0x00);
    HDMI_WriteI2C_Byte(0x37, 0x44);
    HDMI_WriteI2C_Byte(0x3f, 0x0f);
    HDMI_WriteI2C_Byte(0x40, 0x98);  // clk swing
    HDMI_WriteI2C_Byte(0x41, 0x98);  // D0 swing
    HDMI_WriteI2C_Byte(0x42, 0x98);  // D1 swing
    HDMI_WriteI2C_Byte(0x43, 0x98);  // D2 swing
    HDMI_WriteI2C_Byte(0x44, 0x0a);
}

#if 0
void LT9611_HDMI_TX_Phy(void) //xyji
{
  // AC mode with VL pass
	HDMI_WriteI2C_Byte(0xff,0x81);
	HDMI_WriteI2C_Byte(0x30,0xff); //tap1
	HDMI_WriteI2C_Byte(0x31,0x44); //DC: 0x44, AC:0x73 
	HDMI_WriteI2C_Byte(0x32,0xfe);
	HDMI_WriteI2C_Byte(0x33,0x73);
	HDMI_WriteI2C_Byte(0x34,0x03);
	HDMI_WriteI2C_Byte(0x35,0x03);
	HDMI_WriteI2C_Byte(0x36,0x03);
	HDMI_WriteI2C_Byte(0x37,0x44);
	HDMI_WriteI2C_Byte(0x3f,0x0f);
	HDMI_WriteI2C_Byte(0x40,0x98); //clk swing
	HDMI_WriteI2C_Byte(0x41,0x98); //D0 swing
	HDMI_WriteI2C_Byte(0x42,0x98); //D1 swing
	HDMI_WriteI2C_Byte(0x43,0x98); //D2 swing
	HDMI_WriteI2C_Byte(0x44,0x0a);
}
#endif

// void LT9611_HDCP_Init(void) //luodexing
//{
//	HDMI_WriteI2C_Byte(0xff,0x85);
//	HDMI_WriteI2C_Byte(0x07,0x1f);
//	HDMI_WriteI2C_Byte(0x13,0xfe);// [7]=force_hpd, [6]=force_rsen, [5]=vsync_pol, [4]=hsync_pol,
//	                              // [3]=hdmi_mode, [2]=no_accs_when_rdy, [1]=skip_wt_hdmi
//	HDMI_WriteI2C_Byte(0x17,0x0f);// [7]=ri_short_read, [3]=sync_pol_mode, [2]=srm_chk_done,
//	                              // [1]=bksv_srm_pass, [0]=ksv_list_vld
//	HDMI_WriteI2C_Byte(0x15,0x05);
//	//HDMI_WriteI2C_Byte(0x15,0x65);// [7]=key_ddc_st_sel, [6]=tx_hdcp_en,[5]=tx_auth_en, [4]=tx_re_auth
// }

void LT9611_HDCP_Init(void)  // luodexing
{
    HDMI_WriteI2C_Byte(0xff, 0x85);
    HDMI_WriteI2C_Byte(0x07, 0x1f);

    if (lt9611.hdmi_mode == HDMI) {
        HDMI_WriteI2C_Byte(0x13, 0xfc);  // [7]=force_hpd, [6]=force_rsen, [5]=vsync_pol, [4]=hsync_pol,
                                         // [3]=hdmi_mode, [2]=no_accs_when_rdy, [1]=skip_wt_hdmi
    } else {
        HDMI_WriteI2C_Byte(0x13, 0xf4);  // [7]=force_hpd, [6]=force_rsen, [5]=vsync_pol, [4]=hsync_pol,
                                         // [3]=hdmi_mode, [2]=no_accs_when_rdy, [1]=skip_wt_hdmi
    }

    HDMI_WriteI2C_Byte(0x17, 0x0f);  // [7]=ri_short_read, [3]=sync_pol_mode, [2]=srm_chk_done,
                                     // [1]=bksv_srm_pass, [0]=ksv_list_vld
    HDMI_WriteI2C_Byte(0x15, 0x05);
    // HDMI_WriteI2C_Byte(0x15,0x65);// [7]=key_ddc_st_sel, [6]=tx_hdcp_en,[5]=tx_auth_en, [4]=tx_re_auth
}

// void LT9611_load_hdcp_key(void) //luodexing
//{
//	HDMI_WriteI2C_Byte(0xff,0x85);
//   HDMI_WriteI2C_Byte(0x00,0x85);
//   //HDMI_WriteI2C_Byte(0x02,0x0a); //I2C 100K
//	HDMI_WriteI2C_Byte(0x03,0xc1); //bit1: Software-controlled key-loading procedure //bit0: During load key operation,
// it should keep high
////  HDMI_WriteI2C_Byte(0x03,0xc3); //bit1: Automatic key-loading procedure //bit0: During load key operation, it
/// should keep high
//	HDMI_WriteI2C_Byte(0x04,0xa0); //0xA0 is eeprom device address
//	HDMI_WriteI2C_Byte(0x05,0x00); //0x00 is eeprom offset address
//	HDMI_WriteI2C_Byte(0x06,0x20); //length for read
//	HDMI_WriteI2C_Byte(0x14,0xff);

//	HDMI_WriteI2C_Byte(0x07,0x19); //bit[2-0]: Sequential byte read
//	Timer0_Delay1ms(50); // wait 5ms for loading key.
////	HDMI_WriteI2C_Byte(0x07,0x1F); //bit[2-0]: None

//  //printf("\r\nLT9611_load_hdcp_key: 0x%02x",HDMI_ReadI2C_Byte(0x40));
//
//	if((HDMI_ReadI2C_Byte(0x40)&0x01) == 0x01)//0x81
//	{
//    printf("\r\nLT9611_load_hdcp_key: external key valid!");
//  }
//
//  else
//  {
//    printf("\r\nLT9611_load_hdcp_key: external key unvalid!");
//  }
//
//  HDMI_WriteI2C_Byte(0x03,0xc0);
//  HDMI_WriteI2C_Byte(0x07,0x1f); //bit[2-0]: None
//}

void LT9611_load_hdcp_key(void)  // luodexing
{
    HDMI_WriteI2C_Byte(0xff, 0x85);
    HDMI_WriteI2C_Byte(0x00, 0x85);
    // HDMI_WriteI2C_Byte(0x02,0x0a); //I2C 100K
    HDMI_WriteI2C_Byte(0x03, 0xc0);
    HDMI_WriteI2C_Byte(0x03, 0xc3);
    HDMI_WriteI2C_Byte(0x04, 0xa0);  // 0xA0 is eeprom device address
    HDMI_WriteI2C_Byte(0x05, 0x00);  // 0x00 is eeprom offset address
    HDMI_WriteI2C_Byte(0x06, 0x20);  // length for read
    HDMI_WriteI2C_Byte(0x14, 0xff);

    HDMI_WriteI2C_Byte(0x07, 0x11);  // 0x31
    HDMI_WriteI2C_Byte(0x07, 0x17);  // 0x37
    Timer0_Delay1ms(50);             // wait 5ms for loading key.
                                     // printf("\r\nLT9611_load_hdcp_key: 0x%02x",HDMI_ReadI2C_Byte(0x40));

    if ((HDMI_ReadI2C_Byte(0x40) & 0x81) == 0x81) {
        printf("\r\nLT9611_load_hdcp_key: external key valid");
    } else {
        printf("\r\nLT9611_load_hdcp_key: external key unvalid, using internal test key!");
    }

    HDMI_WriteI2C_Byte(0x03, 0xc2);
    HDMI_WriteI2C_Byte(0x07, 0x1f);
}

void LT9611_HDCP_Enable(void)  // luodexing
{
    HDMI_WriteI2C_Byte(0xff, 0x80);
    HDMI_WriteI2C_Byte(0x14, 0x80);  // 0x7f
    HDMI_WriteI2C_Byte(0x14, 0xff);
    HDMI_WriteI2C_Byte(0xff, 0x85);
    HDMI_WriteI2C_Byte(0x15, 0x01);  // disable HDCP
    HDMI_WriteI2C_Byte(0x15, 0x71);  // enable HDCP
    HDMI_WriteI2C_Byte(0x15, 0x65);  // enable HDCP
    printf("\r\n LT9611_HDCP_Enable!");
}

void LT9611_HDCP_Disable(void)  // luodexing
{
    HDMI_WriteI2C_Byte(0xff, 0x85);
    HDMI_WriteI2C_Byte(0x15, 0x45);
    printf("\r\n LT9611_HDCP_Disable!");
}

void LT9611_HDMI_Out_Enable(void)  // dsren
{
    HDMI_WriteI2C_Byte(0xff, 0x81);
    HDMI_WriteI2C_Byte(0x23, 0x40);

    HDMI_WriteI2C_Byte(0xff, 0x82);
    HDMI_WriteI2C_Byte(0xde, 0x20);
    HDMI_WriteI2C_Byte(0xde, 0xe0);

    HDMI_WriteI2C_Byte(0xff, 0x80);
    HDMI_WriteI2C_Byte(0x18, 0xdc); /* txpll sw rst */
    HDMI_WriteI2C_Byte(0x18, 0xfc);
    HDMI_WriteI2C_Byte(0x16, 0xf1); /* txpll calibration rest */
    HDMI_WriteI2C_Byte(0x16, 0xf3);

    HDMI_WriteI2C_Byte(0x11, 0x5a);  // Pcr reset
    HDMI_WriteI2C_Byte(0x11, 0xfa);

    HDMI_WriteI2C_Byte(0xff, 0x81);
    HDMI_WriteI2C_Byte(0x30, 0xea);

    if (lt9611.hdcp_encryption == hdcp_enable) {
        LT9611_HDCP_Enable();
    } else {
        LT9611_HDCP_Disable();
    }

    printf("\r\n LT9611_HDMI_Out_Enable");
}

void LT9611_HDMI_Out_Disable(void)  // dsren
{
    HDMI_WriteI2C_Byte(0xff, 0x81);
    HDMI_WriteI2C_Byte(0x30, 0x00); /* Txphy PD */
    HDMI_WriteI2C_Byte(0x23, 0x80); /* Txpll PD */
    printf("\r\n LT9611_HDMI_Out_Disable");
    if (lt9611.hdcp_encryption == hdcp_enable) {
        LT9611_HDCP_Disable();
    }
}

// void LT9611_HDMI_TX_Digital(struct video_timing *video_format) //dsren
//{
//     //bool hdmi_mode = lt9611->hdmi_mode;
//		u8 VIC = video_format->vic;
//		u8 AR = video_format->aspact_ratio;
//		u8 pb0,pb1,pb2,pb3,pb4;
//		u8 infoFrame_en;
//		u8 sum = 0;

//	  //infoFrame_en = (AIF_PKT_EN|AVI_PKT_EN|SPD_PKT_EN);//9611 RXΪMIPI������Ҫ��SPD_PKT
//	  infoFrame_en = (AIF_PKT_EN|AVI_PKT_EN);
//		//MPEG_PKT_EN,AIF_PKT_EN,SPD_PKT_EN,AVI_PKT_EN,UD0_PKT_EN,UD1_PKT_EN
//		printf("\r\nLT9611_HDMI_TX_Digital: infoFrame_en = 0x%02x",infoFrame_en);

//		pb2 =  (AR<<4) + 0x08;

//	  if(VIC == 95)
//		{
//			pb4 = 0x00;
//		}
//		else
//		{
//			pb4 =  VIC;
//		}

//    //pb0 = (((pb2 + pb4) <= 0x5f)?(0x5f - pb2 - pb4):(0x15f - pb2 - pb4));////////////////////////////////////////

//		HDMI_WriteI2C_Byte(0xff,0x82);

//		if(lt9611.hdmi_mode == HDMI)
//		{
//			HDMI_WriteI2C_Byte(0xd6,0x8e); //sync polarity
//		  printf("\r\nLT9611_HDMI_TX_Digital: HMDI mode = HDMI");
//		}
//		else if(lt9611.hdmi_mode == DVI)
//		{
//		  HDMI_WriteI2C_Byte(0xd6,0x0e); //sync polarity
//			printf("\r\nLT9611_HDMI_TX_Digital: HMDI mode = DVI");
//		}
//
//		pb1 = 0x12;//0x12=rgb888
//		pb3 = 0x08;//0x08=full 0x04=limit
//
//		sum = 0x82 + 0x02 + 0x0d + pb1 + pb2 + pb3 + pb4;
//		pb0 = 0x100 - sum;
//
//	  //AVI
//		HDMI_WriteI2C_Byte(0xff,0x84);
//		HDMI_WriteI2C_Byte(0x40,0x82);
//		HDMI_WriteI2C_Byte(0x41,0x02);
//		HDMI_WriteI2C_Byte(0x42,0x0d);
//		HDMI_WriteI2C_Byte(0x43,pb0); //AVI_PB0
//		HDMI_WriteI2C_Byte(0x44,pb1); //AVI_PB1
//		HDMI_WriteI2C_Byte(0x45,pb2); //AVI_PB2
//		HDMI_WriteI2C_Byte(0x46,pb3); //AVI_PB2
//		HDMI_WriteI2C_Byte(0x47,pb4); //AVI_PB4
//		HDMI_WriteI2C_Byte(0xff,0x84);
//	  HDMI_WriteI2C_Byte(0x10,0x02); //data iland
//		HDMI_WriteI2C_Byte(0x12,0x64); //act_h_blank
//
////		//AIF
////		HDMI_WriteI2C_Byte(0xb5,0x57);
////		HDMI_WriteI2C_Byte(0xb6,0x07);
////		HDMI_WriteI2C_Byte(0xb7,0x00);
////		HDMI_WriteI2C_Byte(0xb8,0x00);
////		HDMI_WriteI2C_Byte(0xb9,0x13);
//
//		if(VIC == 95)//VS_IF, 4k 30hz need send VS_IF packet.
//		{
//		   HDMI_WriteI2C_Byte(0xff,0x84);
//	   	 HDMI_WriteI2C_Byte(0x3d,infoFrame_en|UD0_PKT_EN); //UD1 infoframe enable //revise on 20200715

//		   HDMI_WriteI2C_Byte(0x74,0x81);  //HB0
//		   HDMI_WriteI2C_Byte(0x75,0x01);  //HB1
//		   HDMI_WriteI2C_Byte(0x76,0x05);  //HB2
//		   HDMI_WriteI2C_Byte(0x77,0x49);  //PB0
//		   HDMI_WriteI2C_Byte(0x78,0x03);  //PB1
//		   HDMI_WriteI2C_Byte(0x79,0x0c);  //PB2
//		   HDMI_WriteI2C_Byte(0x7a,0x00);  //PB3
//		   HDMI_WriteI2C_Byte(0x7b,0x20);  //PB4
//		   HDMI_WriteI2C_Byte(0x7c,0x01);  //PB5
//		}
//		else
//		{
//		   HDMI_WriteI2C_Byte(0xff,0x84);
//	   	 HDMI_WriteI2C_Byte(0x3d,infoFrame_en); //UD1 infoframe enable
//		}
//
////////////////////////////////////////////////////9611 RX为MIPI，不需要发这个包
////		if(infoFrame_en&&SPD_PKT_EN)
////		{
////		   HDMI_WriteI2C_Byte(0xff,0x84);
////		   HDMI_WriteI2C_Byte(0xc0,0x00);  //HB0
////		   HDMI_WriteI2C_Byte(0xc1,0x00);  //HB1
////		   HDMI_WriteI2C_Byte(0xc2,0x00);  //HB2

////		   HDMI_WriteI2C_Byte(0xc3,0x00);  //PB0
////		   HDMI_WriteI2C_Byte(0xc4,0x00);  //PB1
////		   HDMI_WriteI2C_Byte(0xc5,0x00);  //PB2
////		   HDMI_WriteI2C_Byte(0xc6,0x00);  //PB3
////		   HDMI_WriteI2C_Byte(0xc7,0x00);  //PB4
////		   HDMI_WriteI2C_Byte(0xc8,0x00);  //PB5
////		}
////////////////////////////////////////////////////
//}

void LT9611_HDMI_TX_Digital(struct video_timing *video_format)  // dsren
{
    // bool hdmi_mode = lt9611->hdmi_mode;
    u8 VIC = video_format->vic;
    u8 AR  = video_format->aspact_ratio;
    u8 pb0, pb2, pb4;
    u8 infoFrame_en;

    infoFrame_en = (AIF_PKT_EN | AVI_PKT_EN | SPD_PKT_EN);
    // MPEG_PKT_EN,AIF_PKT_EN,SPD_PKT_EN,AVI_PKT_EN,UD0_PKT_EN,UD1_PKT_EN
    printf("\r\nLT9611_HDMI_TX_Digital: infoFrame_en = 0x%02x", infoFrame_en);

    pb2 = (AR << 4) + 0x08;
    pb4 = VIC;

    pb0 = ((pb2 + pb4) <= 0x5f) ? (0x5f - pb2 - pb4) : (0x15f - pb2 - pb4);

    HDMI_WriteI2C_Byte(0xff, 0x82);
    if (lt9611.hdmi_mode == HDMI) {
        HDMI_WriteI2C_Byte(0xd6, 0x8e);  // sync polarity
        printf("\r\nLT9611_HDMI_TX_Digital: HMDI mode = HDMI");
    } else if (lt9611.hdmi_mode == DVI) {
        HDMI_WriteI2C_Byte(0xd6, 0x0e);  // sync polarity
        printf("\r\nLT9611_HDMI_TX_Digital: HMDI mode = DVI");
    }

    if (lt9611.audio_out == audio_i2s) {
        HDMI_WriteI2C_Byte(0xd7, 0x04);
    }

    if (lt9611.audio_out == audio_spdif) {
        HDMI_WriteI2C_Byte(0xd7, 0x80);
    }

    // AVI
    HDMI_WriteI2C_Byte(0xff, 0x84);
    HDMI_WriteI2C_Byte(0x43, pb0);  // AVI_PB0

    // HDMI_WriteI2C_Byte(0x44,0x10);//AVI_PB1
    HDMI_WriteI2C_Byte(0x45, pb2);  // AVI_PB2
    HDMI_WriteI2C_Byte(0x47, pb4);  // AVI_PB4

    HDMI_WriteI2C_Byte(0xff, 0x84);
    HDMI_WriteI2C_Byte(0x10, 0x02);  // data iland
    HDMI_WriteI2C_Byte(0x12, 0x64);  // act_h_blank

    // VS_IF, 4k 30hz need send VS_IF packet.
    if (VIC == 95) {
        HDMI_WriteI2C_Byte(0xff, 0x84);
        HDMI_WriteI2C_Byte(0x3d, infoFrame_en | UD0_PKT_EN);  // UD1 infoframe enable //revise on 20200715

        HDMI_WriteI2C_Byte(0x74, 0x81);  // HB0
        HDMI_WriteI2C_Byte(0x75, 0x01);  // HB1
        HDMI_WriteI2C_Byte(0x76, 0x05);  // HB2
        HDMI_WriteI2C_Byte(0x77, 0x49);  // PB0
        HDMI_WriteI2C_Byte(0x78, 0x03);  // PB1
        HDMI_WriteI2C_Byte(0x79, 0x0c);  // PB2
        HDMI_WriteI2C_Byte(0x7a, 0x00);  // PB3
        HDMI_WriteI2C_Byte(0x7b, 0x20);  // PB4
        HDMI_WriteI2C_Byte(0x7c, 0x01);  // PB5
    } else {
        HDMI_WriteI2C_Byte(0xff, 0x84);
        HDMI_WriteI2C_Byte(0x3d, infoFrame_en);  // UD1 infoframe enable
    }

    if (infoFrame_en && SPD_PKT_EN) {
        HDMI_WriteI2C_Byte(0xff, 0x84);
        HDMI_WriteI2C_Byte(0xc0, 0x00);  // HB0
        HDMI_WriteI2C_Byte(0xc1, 0x00);  // HB1
        HDMI_WriteI2C_Byte(0xc2, 0x00);  // HB2

        HDMI_WriteI2C_Byte(0xc3, 0x00);  // PB0
        HDMI_WriteI2C_Byte(0xc4, 0x00);  // PB1
        HDMI_WriteI2C_Byte(0xc5, 0x00);  // PB2
        HDMI_WriteI2C_Byte(0xc6, 0x00);  // PB3
        HDMI_WriteI2C_Byte(0xc7, 0x00);  // PB4
        HDMI_WriteI2C_Byte(0xc8, 0x00);  // PB5
    }
}

void LT9611_CSC(void)
{
    if (lt9611.input_color_space == YUV422) {
        HDMI_WriteI2C_Byte(0xff, 0x82);
        HDMI_WriteI2C_Byte(0xb9, 0x18);  // 0x1C
        //		 HDMI_WriteI2C_Byte(0xba,0x0C);
        printf("\r\nLT9611_CSC: Ypbpr 422 to RGB888");
    }
}

void LT9611_Audio_Init(void)  // sujin
{
    if (lt9611.audio_out == audio_i2s) {
        printf("\r\nAudio inut = I2S 2ch");

        HDMI_WriteI2C_Byte(0xFF, 0x82);
        HDMI_WriteI2C_Byte(0xd7, 0x04);

        HDMI_WriteI2C_Byte(0xff, 0x84);
        HDMI_WriteI2C_Byte(0x06, 0x08);
        HDMI_WriteI2C_Byte(0x07, 0x10);

        HDMI_WriteI2C_Byte(0x0f, 0x2b);  // 48K sampling frequency
        HDMI_WriteI2C_Byte(0x34, 0xd4);  // CTS_N 20180823 0xd5: sclk = 32fs, 0xd4: sclk = 64fs

        HDMI_WriteI2C_Byte(0x35, 0x00);  // N value = 6144
        HDMI_WriteI2C_Byte(0x36, 0x18);
        HDMI_WriteI2C_Byte(0x37, 0x00);

        //		HDMI_WriteI2C_Byte(0x0f,0xab);//96K sampling frequency
        //		HDMI_WriteI2C_Byte(0x34,0xd4);//CTS_N 20180823 0xd5: sclk = 32fs, 0xd4: sclk = 64fs
        //
        //		HDMI_WriteI2C_Byte(0x35,0x00);// N value = 12288
        //		HDMI_WriteI2C_Byte(0x36,0x30);
        //		HDMI_WriteI2C_Byte(0x37,0x00);

        //		HDMI_WriteI2C_Byte(0x0f,0x0b);//44.1K sampling frequency
        //		HDMI_WriteI2C_Byte(0x34,0xd4);//CTS_N 20180823 0xd5: sclk = 32fs, 0xd4: sclk = 64fs
        //
        //		HDMI_WriteI2C_Byte(0x35,0x00);// N value = 6272
        //		HDMI_WriteI2C_Byte(0x36,0x18);
        //		HDMI_WriteI2C_Byte(0x37,0x80);
    }

    if (lt9611.audio_out == audio_spdif) {
        printf("\r\nAudio inut = SPDIF");

        HDMI_WriteI2C_Byte(0xFF, 0x82);
        HDMI_WriteI2C_Byte(0xd7, 0x80);

        HDMI_WriteI2C_Byte(0xff, 0x84);
        HDMI_WriteI2C_Byte(0x06, 0x0c);
        HDMI_WriteI2C_Byte(0x07, 0x10);

        HDMI_WriteI2C_Byte(0x34, 0xd4);  // CTS_N
        HDMI_WriteI2C_Byte(0x36, 0x20);
    }
}

void LT9611_Read_EDID(void)  // luodexing
{
#ifdef _enable_read_edid_

    u8 i, j, edid_data;
    u8 extended_flag = 00;

    memset(Sink_EDID, 0, sizeof Sink_EDID);

    HDMI_WriteI2C_Byte(0xff, 0x85);
    // HDMI_WriteI2C_Byte(0x02,0x0a); //I2C 100K
    HDMI_WriteI2C_Byte(0x03, 0xc9);
    HDMI_WriteI2C_Byte(0x04, 0xA0);  // 0xA0 is EDID device address
    HDMI_WriteI2C_Byte(0x05, 0x00);  // 0x00 is EDID offset address
    HDMI_WriteI2C_Byte(0x06, 0x20);  // length for read
    HDMI_WriteI2C_Byte(0x14, 0x7f);

    for (i = 0; i < 8; i++)  // block 0 & 1
    {
        HDMI_WriteI2C_Byte(0x05, i * 32);  // 0x00 is EDID offset address
        HDMI_WriteI2C_Byte(0x07, 0x36);
        HDMI_WriteI2C_Byte(0x07, 0x34);      // 0x31
        HDMI_WriteI2C_Byte(0x07, 0x37);      // 0x37
        Timer0_Delay1ms(5);                  // wait 5ms for reading edid data.
        if (HDMI_ReadI2C_Byte(0x40) & 0x02)  // KEY_DDC_ACCS_DONE=1
        {
            if (HDMI_ReadI2C_Byte(0x40) & 0x50)  // DDC No Ack or Abitration lost
            {
                printf("\r\nread edid failed: no ack");
                goto end;
            } else {
                printf("\r\n");
                for (j = 0; j < 32; j++) {
                    edid_data             = HDMI_ReadI2C_Byte(0x83);
                    Sink_EDID[i * 32 + j] = edid_data;  // write edid data to Sink_EDID[];
                    if ((i == 3) && (j == 30)) {
                        extended_flag = edid_data & 0x03;
                    }
                    printf("%02x,", edid_data);
                }
                if (i == 3) {
                    if (extended_flag < 1)  // no block 1, stop reading edid.
                    {
                        printf("\r\n no block 1, stop reading edid");
                        goto end;
                    }
                }
            }
        } else {
            printf("\r\nread edid failed: accs not done");
            goto end;
        }
    }

    if (extended_flag < 2)  // no block 2, stop reading edid.
    {
        printf("\r\n no block 2, stop reading edid");
        goto end;
    } else {
        for (i = 0; i < 8; i++)  //  // block 2 & 3
        {
            HDMI_WriteI2C_Byte(0x05, i * 32);    // 0x00 is EDID offset address
            HDMI_WriteI2C_Byte(0x07, 0x76);      // 0x31
            HDMI_WriteI2C_Byte(0x07, 0x74);      // 0x31
            HDMI_WriteI2C_Byte(0x07, 0x77);      // 0x37
            Timer0_Delay1ms(5);                  // wait 5ms for reading edid data.
            if (HDMI_ReadI2C_Byte(0x40) & 0x02)  // KEY_DDC_ACCS_DONE=1
            {
                if (HDMI_ReadI2C_Byte(0x40) & 0x50)  // DDC No Ack or Abitration lost
                {
                    printf("\r\nread edid failed: no ack");
                    goto end;
                } else {
                    printf("\r\n");
                    for (j = 0; j < 32; j++) {
                        edid_data = HDMI_ReadI2C_Byte(0x83);
                        // Sink_EDID[256+i*32+j]= edid_data; // write edid data to Sink_EDID[];
                        printf("%02x,", edid_data);
                    }
                    if (i == 3) {
                        if (extended_flag < 3)  // no block 3, stop reading edid.
                        {
                            printf("\r\n no block 3, stop reading edid");
                            goto end;
                        }
                    }
                }
            } else {
                printf("\r\nread edid failed: accs not done");
                goto end;
            }
        }
    }
end:
    HDMI_WriteI2C_Byte(0x03, 0xc2);
    HDMI_WriteI2C_Byte(0x07, 0x1f);
#endif
}

u8 LT9611_Get_HPD_Status(void)
{
    HDMI_WriteI2C_Byte(0xff, 0x82);

    if (((HDMI_ReadI2C_Byte(0x5e)) & 0x04) == 0x04)  // 20200727
    {
        Timer0_Delay1ms(10);
        if (((HDMI_ReadI2C_Byte(0x5e)) & 0x04) == 0x04) {
            return 1;
        } else {
            return 0;
        }
    } else {
        return 0;
    }
}

/******************************************************************************************************************/
////////////////////////////////////////////////////CEC: start//////////////////////////////////////////////////////
void LT9611_HDMI_CEC_ON(bool enable)
{
    if (enable) {
        /* cec init */
        HDMI_WriteI2C_Byte(0xff, 0x80);
        HDMI_WriteI2C_Byte(0x0d, 0xff);
        HDMI_WriteI2C_Byte(0x15, 0xf1);  // reset cec logic
        HDMI_WriteI2C_Byte(0x15, 0xf9);
        HDMI_WriteI2C_Byte(0xff, 0x86);
        HDMI_WriteI2C_Byte(0xfe, 0xa5);  // clk div
    } else {
        HDMI_WriteI2C_Byte(0xff, 0x80);
        HDMI_WriteI2C_Byte(0x15, 0xf1);
    }
}

void lt9611_cec_logical_reset(void)
{
    HDMI_WriteI2C_Byte(0xff, 0x80);
    HDMI_WriteI2C_Byte(0x15, 0xf1);  // reset cec logic
    HDMI_WriteI2C_Byte(0x15, 0xf9);
}

void lt9611_cec_msg_set_logical_address(struct cec_msg *cec_msg)
{
    u8 logical_address;
    /*
        0xf8, 0xf7   //Register
        0x00, 0x01,  //LA 0
        0x00, 0x02,  //LA 1
        0x00, 0x03,  //LA 2
        0x00, 0x04,  //LA 3
        0x00, 0x10,  //LA 4
        0x00, 0x20,  //LA 5
        0x00, 0x30,  //LA 6
        0x00, 0x40,  //LA 7
        0x01, 0x00,  //LA 8
        0x02, 0x00,  //LA 9
        0x03, 0x00,  //LA 10
        0x04, 0x00,  //LA 11
        0x10, 0x00,  //LA 12
        0x20, 0x00,  //LA 13
        0x30, 0x00,  //LA 14
        0x40, 0x00	 //LA 15
    */

    if (!cec_msg->la_allocation_done) {
        logical_address = 15;
    } else {
        logical_address = cec_msg->logical_address;
    }

    if (logical_address > 15) {
        printf("\n LA error!");
        return;
    }

    HDMI_WriteI2C_Byte(0xff, 0x86);

    switch (logical_address) {
        case 0:
            HDMI_WriteI2C_Byte(0xf7, 0x01);
            HDMI_WriteI2C_Byte(0xf8, 0x00);
            break;

        case 1:
            HDMI_WriteI2C_Byte(0xf7, 0x02);
            HDMI_WriteI2C_Byte(0xf8, 0x00);
            break;

        case 2:
            HDMI_WriteI2C_Byte(0xf7, 0x03);
            HDMI_WriteI2C_Byte(0xf8, 0x00);
            break;

        case 3:
            HDMI_WriteI2C_Byte(0xf7, 0x04);
            HDMI_WriteI2C_Byte(0xf8, 0x00);
            break;

        case 4:
            HDMI_WriteI2C_Byte(0xf7, 0x10);
            HDMI_WriteI2C_Byte(0xf8, 0x00);
            break;

        case 5:
            HDMI_WriteI2C_Byte(0xf7, 0x20);
            HDMI_WriteI2C_Byte(0xf8, 0x00);
            break;

        case 6:
            HDMI_WriteI2C_Byte(0xf7, 0x30);
            HDMI_WriteI2C_Byte(0xf8, 0x00);
            break;

        case 7:
            HDMI_WriteI2C_Byte(0xf7, 0x40);
            HDMI_WriteI2C_Byte(0xf8, 0x00);
            break;

        case 8:
            HDMI_WriteI2C_Byte(0xf7, 0x00);
            HDMI_WriteI2C_Byte(0xf8, 0x01);
            break;

        case 9:
            HDMI_WriteI2C_Byte(0xf7, 0x00);
            HDMI_WriteI2C_Byte(0xf8, 0x02);
            break;

        case 10:
            HDMI_WriteI2C_Byte(0xf7, 0x00);
            HDMI_WriteI2C_Byte(0xf8, 0x03);
            break;

        case 11:
            HDMI_WriteI2C_Byte(0xf7, 0x00);
            HDMI_WriteI2C_Byte(0xf8, 0x04);
            break;

        case 12:
            HDMI_WriteI2C_Byte(0xf7, 0x00);
            HDMI_WriteI2C_Byte(0xf8, 0x10);
            break;

        case 13:
            HDMI_WriteI2C_Byte(0xf7, 0x00);
            HDMI_WriteI2C_Byte(0xf8, 0x20);
            break;

        case 14:
            HDMI_WriteI2C_Byte(0xf7, 0x00);
            HDMI_WriteI2C_Byte(0xf8, 0x30);
            break;

        case 15:
            HDMI_WriteI2C_Byte(0xf7, 0x00);
            HDMI_WriteI2C_Byte(0xf8, 0x40);
            break;

        default:
            break;
    }
}

static int do_checksum(const unsigned char *x, u8 len)
{
    unsigned char check = x[len];
    unsigned char sum   = 0;
    int i;

    printf("\nChecksum: 0x%x", check);

    for (i = 0; i < len; i++) sum += x[i];

    if ((unsigned char)(check + sum) != 0) {
        printf(" (should be 0x%x)\n", -sum & 0xff);
        return 0;
    }

    printf(" (valid)\n");
    return 1;
}

int lt9611_parse_physical_address(struct cec_msg *cec_msg, u8 *edid)  // parse edid data from edid.
{
    int ret = 0;
    int version;
    int offset   = 0;
    int offset_d = 0;
    int tag_code;
    u16 physical_address;

    version  = edid[0x81];
    offset_d = edid[0x82];

    if (!do_checksum(edid, 255)) return 0;  // prase_physical_address fail.

    if (version < 3) return 0;  // prase_physical_address fail.

    if (offset_d < 5) return 0;  // prase_physical_address fail.

    tag_code = (edid[0x84 + offset] & 0xe0) >> 5;

    while (tag_code != 0x03) {
        if ((edid[0x84 + offset] & 0x1f) == 0) return 0;

        offset += edid[0x84 + offset] & 0x1f;
        offset++;

        if (offset > (offset_d - 4)) return 0;

        tag_code = (edid[0x84 + offset] & 0xe0) >> 5;
    }

    printf("\nvsdb: 0x%x,0x%x,0x%x", edid[0x84 + offset], edid[0x85 + offset], edid[0x86 + offset]);

    if ((edid[0x84 + offset + 1] == 0x03) && (edid[0x84 + offset + 2] == 0x0c) && (edid[0x84 + offset + 3] == 0x00)) {
        physical_address = edid[0x84 + offset + 4];
        physical_address = (physical_address << 8) + edid[0x84 + offset + 5];

        cec_msg->physical_address = physical_address;
        printf("\nprase physical address success! %x", physical_address);
        return 1;
    }
    return 0;
}

void lt9611_hdmi_cec_read(struct cec_msg *cec_msg)  // transfer cec msg from LT9611 regisrer to rx_buffer.
{
    u8 size, i;
    HDMI_WriteI2C_Byte(0xff, 0x86);
    HDMI_WriteI2C_Byte(0xf5, 0x01);  // lock rx data buff
    size                     = HDMI_ReadI2C_Byte(0xd3);
    cec_msg->rx_data_buff[0] = size;
    // printf("\r\ncec rec: ");
    for (i = 1; i <= size; i++) {
        cec_msg->rx_data_buff[i] = HDMI_ReadI2C_Byte(0xd3 + i);
        // printf("0x%02x, ",cec_msg->rx_data_buff[i]);
    }
    HDMI_WriteI2C_Byte(0xf5, 0x00);  // unlock rx data buff
}

void lt9611_hdmi_cec_write(struct cec_msg *cec_msg)  // send cec msg which is in tx_buffer.
{
    u8 size, i;
    size                   = cec_msg->tx_data_buff[0];
    cec_msg->retries_times = 0;

    HDMI_WriteI2C_Byte(0xff, 0x86);
    HDMI_WriteI2C_Byte(0xf5, 0x01);  // lock rx data buff
    HDMI_WriteI2C_Byte(0xf4, size);
    for (i = 0; i <= size; i++) {
        HDMI_WriteI2C_Byte(0xe4 + i, cec_msg->tx_data_buff[1 + i]);
    }
    HDMI_WriteI2C_Byte(0xf9, 0x03);  // start send msg
    Timer0_Delay1ms(25 * i);         // wait HDMI
    HDMI_WriteI2C_Byte(0xf5, 0x00);  // unlock rx data buff
    HDMI_WriteI2C_Byte(0xf9, 0x02);
}

void lt9611_cec_msg_write_demo(void)  // for debug
{
    CEC_TxData_Buff[0] = 0x05;  // data counter to be send
    CEC_TxData_Buff[1] = 0x40;  // first cec data(in spec, sender id = 0x05,
                                // receiver id = 0x00
    CEC_TxData_Buff[2] = 0x84;  // second cec data(in spec, it is opcode =0x84)
    CEC_TxData_Buff[3] = 0x10;  // parameter of opcode
    CEC_TxData_Buff[4] = 0x00;  // parameter of opcode
    CEC_TxData_Buff[5] = 0x05;  // parameter of opcode

    // lt9611_hdmi_cec_write(CEC_TxData_Buff);
}

void lt9611_broad_cast_demo(void)  // for debug
{
    CEC_TxData_Buff[0] = 0x05;  // data counter to be send
    CEC_TxData_Buff[1] = 0x40;  // first cec data(in spec, sender id = 0x05,
                                // receiver id = 0x00
    CEC_TxData_Buff[2] = 0x84;  // second cec data(in spec, it is opcode =0x84)
    CEC_TxData_Buff[3] = 0x10;  // parameter of opcode
    CEC_TxData_Buff[4] = 0x00;  // parameter of opcode
    CEC_TxData_Buff[5] = 0x05;  // parameter of opcode

    // lt9611_hdmi_cec_write(CEC_TxData_Buff);
}

void lt9611_cec_la_allocation(struct cec_msg *cec_msg)  // polling  logical address.
{
    u8 logical_address;

    logical_address          = cec_msg->logical_address;
    cec_msg->tx_data_buff[0] = 0x01;  // data counter to be send
    cec_msg->tx_data_buff[1] = (logical_address << 4) | logical_address;
    // first cec data(in spec, sender id = 0x04,
    // receiver id = 0x04;
    lt9611_hdmi_cec_write(cec_msg);
}

void lt9611_cec_report_physical_address(struct cec_msg *cec_msg)  // report physical address.
{
    cec_msg->tx_data_buff[0] = 0x05;  // data counter to be send
    cec_msg->tx_data_buff[1] = (cec_msg->logical_address << 4) | 0x0f;
    // first cec data([7:4]=initiator ;[7:4]= destintion)
    cec_msg->tx_data_buff[2] = 0x84;                                  // opcode
    cec_msg->tx_data_buff[3] = (u8)(cec_msg->physical_address >> 8);  // parameter of opcode
    cec_msg->tx_data_buff[4] = (u8)(cec_msg->physical_address);       // parameter of opcode
    cec_msg->tx_data_buff[5] = 0x04;                                  // device type = playback device

    // printf("\nPA:%x, %x",cec_msg->tx_data_buff[3],cec_msg->tx_data_buff[4]);

    lt9611_hdmi_cec_write(cec_msg);
}

void lt9611_cec_menu_activate(struct cec_msg *cec_msg)  // report physical address.
{
    cec_msg->tx_data_buff[0] = 0x04;  // data counter to be send
    cec_msg->tx_data_buff[1] = (cec_msg->logical_address << 4) | cec_msg->destintion;
    // first cec data([7:4]=initiator ;[7:4]= destintion)
    cec_msg->tx_data_buff[2] = 0x8e;  // opcode
    cec_msg->tx_data_buff[3] = 0x00;  // parameter of opcode

    // printf("\nPA:%x, %x",cec_msg->tx_data_buff[3],cec_msg->tx_data_buff[4]);

    lt9611_hdmi_cec_write(cec_msg);
}

void lt9611_cec_feature_abort(struct cec_msg *cec_msg, u8 reason)  // report feature abort
{
    cec_msg->tx_data_buff[0] = 0x03;  // data counter to be send
    cec_msg->tx_data_buff[1] = (cec_msg->logical_address << 4) | cec_msg->destintion;
    // first cec data([7:4]=initiator ;[7:4]= destintion)
    cec_msg->tx_data_buff[2] = 0x00;    // opcode
    cec_msg->tx_data_buff[3] = reason;  // parameter1 of opcode

    lt9611_hdmi_cec_write(cec_msg);
}

void lt9611_cec_frame_retransmission(struct cec_msg *cec_msg)
{
    if (cec_msg->retries_times < 5) {
        HDMI_WriteI2C_Byte(0xff, 0x86);
        HDMI_WriteI2C_Byte(0xf9, 0x02);
        HDMI_WriteI2C_Byte(0xf9, 0x03);  // start send msg
    }
    cec_msg->retries_times++;
}

void lt9611_cec_device_polling(struct cec_msg *cec_msg)
{
    static u8 i;
    if (!cec_msg->la_allocation_done) {
        cec_msg->tx_data_buff[0] = 0x01;  // data counter to be send
        cec_msg->tx_data_buff[1] = i;     // first cec data(in spec, sender id = 0x04,
                                          // receiver id = 0x04;
        lt9611_hdmi_cec_write(cec_msg);
        if (i > 13) cec_msg->la_allocation_done = 1;
        (i > 13) ? (i = 0) : (i++);
    }
}

void lt9611_cec_msg_tx_handle(struct cec_msg *cec_msg)
{
    u8 cec_status;
    u8 header;
    u8 opcode;
    u8 i;
    cec_status = cec_msg->cec_status;

    //    if( cec_msg ->send_msg_done) //There is no tx msg to be handled
    //        return;

    if (cec_status & CEC_ERROR_INITIATOR) {
        printf("\nCEC_ERROR_INITIATOR.");
        lt9611_cec_logical_reset();
        return;
    }

    if (cec_status & CEC_ARB_LOST) {
        printf("\nCEC_ARB_LOST.");  // lost arbitration

        return;
    }

    if (cec_status & (CEC_SEND_DONE | CEC_NACK | CEC_ERROR_FOLLOWER)) do {
            printf("\ntx_date: ");
            for (i = 0; i < cec_msg->tx_data_buff[0]; i++) printf("0x%02x, ", cec_msg->tx_data_buff[i + 1]);

            if (cec_status & CEC_SEND_DONE) printf("CEC_SEND_DONE >>");

            if (cec_status & CEC_NACK) printf("NACK >>");

            header = cec_msg->tx_data_buff[1];

            if ((header == 0x44) || (header == 0x88) || (header == 0xbb))  // logical address allocation
            {
                if (cec_status & CEC_NACK) {
                    cec_msg->logical_address = header & 0x0f;
                    printf("la_allocation_done.");
                    lt9611_cec_msg_set_logical_address(cec_msg);
                    lt9611_cec_report_physical_address(cec_msg);
                }

                if (cec_status & CEC_SEND_DONE) {
                    if (cec_msg->logical_address == 4)  // go to next la.
                        cec_msg->logical_address = 8;
                    else if (cec_msg->logical_address == 8)  // go to next la.
                        cec_msg->logical_address = 11;
                    else if (cec_msg->logical_address == 11)  // go to next la.
                        cec_msg->logical_address = 4;

                    lt9611_cec_la_allocation(cec_msg);
                }
                break;
            }
            if (cec_status & (CEC_NACK | CEC_ERROR_FOLLOWER)) {
                lt9611_cec_frame_retransmission(cec_msg);
            }
            if (cec_msg->tx_data_buff[0] < 2)  // check tx data length
                break;
            opcode = cec_msg->tx_data_buff[2];
            if (opcode == 0x84) {
                cec_msg->report_physical_address_done = 1;
                printf("report_physical_address.");
            }

            if (opcode == 0x00) {
                printf("feature abort");
            }
        } while (0);
}

void lt9611_cec_msg_rx_parse(struct cec_msg *cec_msg)
{
    u8 cec_status;
    u8 header;
    u8 opcode;
    u8 initiator;
    u8 destintion;
    u8 i;
    cec_status = cec_msg->cec_status;

    //    if( cec_msg ->parse_msg_done) //There is no Rx msg to be parsed
    //        return;

    if (cec_status & CEC_ERROR_FOLLOWER) {
        printf("\nCEC_ERROR_FOLLOWER.");

        return;
    }

    if (!(cec_status & CEC_REC_DATA)) {
        return;
    }

    lt9611_hdmi_cec_read(&lt9611_cec_msg);

    if (cec_msg->rx_data_buff[0] < 1)  // check rx data length
    {
        return;
    }

    printf("\nrx_date: ");
    for (i = 0; i < cec_msg->rx_data_buff[0]; i++) printf("0x%02x, ", cec_msg->rx_data_buff[i + 1]);

    printf("parse <<");
    header     = cec_msg->rx_data_buff[1];
    destintion = header & 0x0f;
    initiator  = (header & 0xf0) >> 4;
    // cec_msg ->parse_msg_done = 1;

    if (header == 0x4f) {
        printf("lt9611 broadcast msg.");
    }

    if (cec_msg->rx_data_buff[0] < 2)  // check rx data length
        return;

    opcode = cec_msg->rx_data_buff[2];

    // CECT 12 Invalid Msg Tests
    if ((header & 0x0f) == 0x0f) {
        if ((opcode == 0x00) || (opcode == 0x83) || (opcode == 0x8e) || (opcode == 0x90) || (opcode == 0xff)) {
            printf("Invalid msg, destination address error");  // these msg should not be broadcast msg, but they do.
            return;
        }
    } else {
        if ((opcode == 0x84) || (opcode == 0x84) || (opcode == 0x84)) {
            printf("Invalid msg, destination address error");  // these msg should be broadcast msg, but they not.
            return;
        }
    }

    if (opcode == 0xff)  // abort
    {
        printf("abort.");
        if (destintion == 0x0f)  // ignor broadcast abort msg.
            return;
        cec_msg->destintion = initiator;
        lt9611_cec_feature_abort(cec_msg, CEC_ABORT_REASON_0);
    }

    if (opcode == 0x83)  // give physical address
    {
        printf("give physical address.");
        lt9611_cec_report_physical_address(cec_msg);
    }

    if (opcode == 0x90)  // report power status
    {
        printf("report power status.");
        if (cec_msg->rx_data_buff[0] < 3) {
            printf("<error:parameters missing");
            return;  // parameters missing, ignor this msg.
        }
    }

    if (opcode == 0x8e)  // menu status
    {
        printf("menu status.");
        if (cec_msg->rx_data_buff[0] < 3) {
            printf("<error:parameters missing");
            return;  // parameters missing, ignor this msg.
        }
    }

    if (opcode == 0x00)  // feature abort
    {
        printf("feature abort.");
        if (cec_msg->rx_data_buff[0] < 3) {
            printf("<error:parameters missing");
            return;  // parameters missing, ignor this msg.
        }
    }

    if (opcode == 0x9e)  // cec version
    {
        printf("cec version.");
        if (cec_msg->rx_data_buff[0] < 3) {
            printf("<error:parameters missing");
            return;  // parameters missing, ignor this msg.
        }
    }

    if (opcode == 0x84)  // report physical address
    {
        printf("report physical address.");
        if (cec_msg->rx_data_buff[0] < 5) {
            printf("<error:parameters missing");
            return;  // parameters missing, ignor this msg.
        }
    }

    if (opcode == 0x86)  // set stream path
    {
        printf("set stream path.");
        if (cec_msg->rx_data_buff[0] < 4) {
            printf("<error:parameters missing");
            return;  // parameters missing, ignor this msg.
        }
        lt9611_cec_report_physical_address(cec_msg);
        Timer0_Delay1ms(120);
        lt9611_cec_menu_activate(cec_msg);
    }
}

void lt9611_cec_msg_init(struct cec_msg *cec_msg)
{
    LT9611_HDMI_CEC_ON(1);
    cec_msg->physical_address             = 0x2000;
    cec_msg->logical_address              = 4;
    cec_msg->report_physical_address_done = 0;
    cec_msg->la_allocation_done           = 0;
    lt9611_cec_msg_set_logical_address(cec_msg);
}
/////////////////////////////////////////////////CEC: end///////////////////////////////////////////////////////////
/******************************************************************************************************************/

/////////////////////////////////////////////////////////////
// These function for debug: start
/////////////////////////////////////////////////////////////
void LT9611_Frequency_Meter_Byte_Clk(void)
{
#ifdef _Frequency_Meter_Byte_Clk_
    u8 temp;
    u32 reg = 0x00;

    /* port A byte clk meter */
    HDMI_WriteI2C_Byte(0xff, 0x82);
    HDMI_WriteI2C_Byte(0xc7, 0x03);  // PortA
    Timer0_Delay1ms(50);
    temp = HDMI_ReadI2C_Byte(0xcd);
    if ((temp & 0x60) == 0x60) /* clk stable */
    {
        reg  = (u32)(temp & 0x0f) * 65536;
        temp = HDMI_ReadI2C_Byte(0xce);
        reg  = reg + (u16)temp * 256;
        temp = HDMI_ReadI2C_Byte(0xcf);
        reg  = reg + temp;
        printf("\r\nport A byte clk = %ld", reg);
    } else /* clk unstable */
        printf("\r\nport A byte clk unstable");

    /* port B byte clk meter */
    HDMI_WriteI2C_Byte(0xff, 0x82);
    HDMI_WriteI2C_Byte(0xc7, 0x04);
    Timer0_Delay1ms(50);
    temp = HDMI_ReadI2C_Byte(0xcd);
    if ((temp & 0x60) == 0x60) /* clk stable */
    {
        reg  = (u32)(temp & 0x0f) * 65536;
        temp = HDMI_ReadI2C_Byte(0xce);
        reg  = reg + (u16)temp * 256;
        temp = HDMI_ReadI2C_Byte(0xcf);
        reg  = reg + temp;
        printf("\r\nport B byte clk = %ld", reg);
    } else /* clk unstable */
        printf("\r\nport B byte clk unstable");
#endif
}

void LT9611_Htotal_Sysclk(void)
{
#ifdef _htotal_stable_check_
    u16 reg;
    u8 loopx;
    for (loopx = 0; loopx < 10; loopx++) {
        HDMI_WriteI2C_Byte(0xff, 0x82);
        reg = HDMI_ReadI2C_Byte(0x86);
        reg = reg * 256 + HDMI_ReadI2C_Byte(0x87);
        printf("\r\nHtotal_Sysclk = %d", reg);
        Timer0_Delay1ms(50);
        // printdec_u32(reg);
    }
#endif
}

void LT9611_Pcr_MK_Print(void)
{
#ifdef _pcr_mk_printf_
    u8 loopx;

    HDMI_WriteI2C_Byte(0xff, 0x83);
    for (loopx = 0; loopx < 10; loopx++) {
        printf("\r\nPCRstable_bit4:0x%x", HDMI_ReadI2C_Byte(0x97));
        printf("\r\n 0x%x", HDMI_ReadI2C_Byte(0xb4));
        printf(" 0x%x", HDMI_ReadI2C_Byte(0xb5));
        printf(" 0x%x", HDMI_ReadI2C_Byte(0xb6));
        printf(" 0x%x", HDMI_ReadI2C_Byte(0xb7));
        Timer0_Delay1ms(500);
    }
#endif
}

void LT9611_Dphy_debug(void)
{
#ifdef _mipi_Dphy_debug_
    u8 temp;

    HDMI_WriteI2C_Byte(0xff, 0x83);
    temp = HDMI_ReadI2C_Byte(0xbc);
    if (temp == 0x55)
        printf("\r\nport A lane PN is right");
    else
        printf("\r\nport A lane PN error 0x83bc = 0x%x", temp);

    temp = HDMI_ReadI2C_Byte(0x99);
    if (temp == 0xb8)
        printf("\r\nport A lane 0 sot right ");
    else
        printf("\r\nport A lane 0 sot error = 0x%x", temp);

    temp = HDMI_ReadI2C_Byte(0x9b);
    if (temp == 0xb8)
        printf("\r\nport A lane 1 sot right ");
    else
        printf("\r\nport A lane 1 sot error = 0x%x", temp);

    temp = HDMI_ReadI2C_Byte(0x9d);
    if (temp == 0xb8)
        printf("\r\nport A lane 2 sot right ");
    else
        printf("\r\nport A lane 2 sot error = 0x%x", temp);

    temp = HDMI_ReadI2C_Byte(0x9f);
    if (temp == 0xb8)
        printf("\r\nport A lane 3 sot right ");
    else
        printf("\r\nport A lane 3 sot error = 0x%x", temp);

#endif
}

/////////////////////////////////////////////////////////////
// These function for debug: end
/////////////////////////////////////////////////////////////

void LT9611_IRQ_Init(void)  // dsren
{
    // int hpd interrupt
    HDMI_WriteI2C_Byte(0xff, 0x82);
    // HDMI_WriteI2C_Byte(0x10,0x00); //Output low level active;
    HDMI_WriteI2C_Byte(0x58, 0x0a);  // Det HPD 0x0a --> 0x08 20200727
    HDMI_WriteI2C_Byte(0x59, 0x00);  // HPD debounce width

    // intial vid change interrupt
    HDMI_WriteI2C_Byte(0x9e, 0xf7);
}

void LT9611_Globe_Interrupts(bool on)
{
    if (on) {
        HDMI_WriteI2C_Byte(0xff, 0x81);
        HDMI_WriteI2C_Byte(0x51, 0x10);  // hardware mode irq pin out
    } else {
        HDMI_WriteI2C_Byte(0xff, 0x81);  // software mode irq pin out = 1;
        HDMI_WriteI2C_Byte(0x51, 0x30);
    }
}

void LT9611_Enable_Interrupts(u8 interrupts, bool on)
{
    if (interrupts == HPD_INTERRUPT_ENABLE) {
        if (on) {
            HDMI_WriteI2C_Byte(0xff, 0x82);
            HDMI_WriteI2C_Byte(0x07, 0xff);  // clear3
            HDMI_WriteI2C_Byte(0x07, 0x3f);  // clear3
            HDMI_WriteI2C_Byte(0x03, 0x3f);  // mask3  //Tx_det
            printf("\r\nLT9611_Enable_Interrupts: hpd_irq_enable");
        } else {
            HDMI_WriteI2C_Byte(0xff, 0x82);
            HDMI_WriteI2C_Byte(0x07, 0xff);  // clear3
            HDMI_WriteI2C_Byte(0x03, 0xff);  // mask3  //Tx_det
            printf("\r\nLT9611_Enable_Interrupts: hpd_irq_disable");
        }
    }

    if (interrupts == VID_CHG_INTERRUPT_ENABLE) {
        if (on) {
            HDMI_WriteI2C_Byte(0xff, 0x82);
            HDMI_WriteI2C_Byte(0x9e, 0xff);  // clear vid chk irq
            HDMI_WriteI2C_Byte(0x9e, 0xf7);
            HDMI_WriteI2C_Byte(0x04, 0xff);  // clear0
            HDMI_WriteI2C_Byte(0x04, 0xfe);  // clear0
            HDMI_WriteI2C_Byte(0x00, 0xfe);  // mask0 vid_chk_IRQ
            printf("\r\nLT9611_Enable_Interrupts: vid_chg_irq_enable");
        } else {
            HDMI_WriteI2C_Byte(0xff, 0x82);
            HDMI_WriteI2C_Byte(0x04, 0xff);  // clear0
            HDMI_WriteI2C_Byte(0x00, 0xff);  // mask0 vid_chk_IRQ
            printf("\r\nLT9611_Enable_Interrupts: vid_chg_irq_disable");
        }
    }
    if (interrupts == CEC_INTERRUPT_ENABLE) {
        if (on) {
            HDMI_WriteI2C_Byte(0xff, 0x86);
            HDMI_WriteI2C_Byte(0xfa, 0x00);  // cec interrup mask
            HDMI_WriteI2C_Byte(0xfc, 0x7f);  // cec irq clr
            HDMI_WriteI2C_Byte(0xfc, 0x00);

            /* cec irq init */
            HDMI_WriteI2C_Byte(0xff, 0x82);
            HDMI_WriteI2C_Byte(0x01, 0x7f);  // mask bit[7]
            HDMI_WriteI2C_Byte(0x05, 0xff);  // clr bit[7]
            HDMI_WriteI2C_Byte(0x05, 0x7f);
        } else {
            HDMI_WriteI2C_Byte(0xff, 0x86);
            HDMI_WriteI2C_Byte(0xfa, 0xff);  // cec interrup mask
            HDMI_WriteI2C_Byte(0xfc, 0x7f);  // cec irq clr

            /* cec irq init */
            HDMI_WriteI2C_Byte(0xff, 0x82);
            HDMI_WriteI2C_Byte(0x01, 0xff);  // mask bit[7]
            HDMI_WriteI2C_Byte(0x05, 0xff);  // clr bit[7]
        }
    }
}

void LT9611_HDP_Interrupt_Handle(void)
{
    HDMI_WriteI2C_Byte(0xff, 0x82);
    HDMI_WriteI2C_Byte(0x07, 0xff);  // clear3
    HDMI_WriteI2C_Byte(0x07, 0x3f);  // clear3

    ////////////////////////////////////////////////////////////////////	HDCP_CTS
    printf("\r\nLT9611_HDP_Interrupt_Handle: HPD acted!");
    LT9611_HDMI_Out_Disable();
    ////////////////////////////////////////////////////////////////////

    if (LT9611_Get_HPD_Status()) {
        printf("\r\nLT9611_HDP_Interrupt_Handle: HDMI connected.");
        LT9611_LowPower_mode(0);
        LT9611_Enable_Interrupts(VID_CHG_INTERRUPT_ENABLE, 1);
        Timer0_Delay1ms(100);
        LT9611_Read_EDID();

#ifdef cec_on
        lt9611_parse_physical_address(&lt9611_cec_msg, Sink_EDID);
        lt9611_cec_la_allocation(&lt9611_cec_msg);
#endif

        LT9611_Video_Check();
        if (PCR_Format != PCR_None) {
            LT9611_PLL(video);
            LT9611_MIPI_Pcr(video);
            LT9611_HDMI_TX_Digital(video);
            LT9611_Pcr_MK_Print();
            LT9611_HDMI_Out_Enable();
        } else {
            LT9611_HDMI_Out_Disable();
            printf("\r\nLT9611_HDP_Interrupt_Handle: no mipi video, disable hdmi output");
        }
    } else {
        printf("\r\nLT9611_HDP_Interrupt_Handle: HDMI disconnected");
        LT9611_Enable_Interrupts(VID_CHG_INTERRUPT_ENABLE, 0);
        LT9611_LowPower_mode(1);

#ifdef cec_on
        lt9611_cec_msg_init(&lt9611_cec_msg);
#endif
    }
}

void LT9611_Vid_Chg_Interrupt_Handle(void)
{
    u8 settle_count;

    printf("\r\nLT9611_Vid_Chg_Interrupt_Handle: ");
#if 1
    HDMI_WriteI2C_Byte(0xff, 0x82);
    HDMI_WriteI2C_Byte(0x9e, 0xff);  // clear vid chk irq
    HDMI_WriteI2C_Byte(0x9e, 0xf7);

    HDMI_WriteI2C_Byte(0x04, 0xff);  // clear0 irq
    HDMI_WriteI2C_Byte(0x04, 0xfe);
#endif
    Timer0_Delay1ms(500);
    HDMI_WriteI2C_Byte(0xff, 0x83);
    if (P10)  // portA input
    {
        printf("\r\nport A lane 0 settle = 0x%x", HDMI_ReadI2C_Byte(0x98));
        printf("\r\nport A lane 1 settle = 0x%x", HDMI_ReadI2C_Byte(0x9a));
        printf("\r\nport A lane 2 settle = 0x%x", HDMI_ReadI2C_Byte(0x9c));
        printf("\r\nport A lane 3 settle = 0x%x", HDMI_ReadI2C_Byte(0x9e));
        settle_count = HDMI_ReadI2C_Byte(0x98);
    } else  // portB input
    {
        printf("\r\nport B lane 0 settle = 0x%x", HDMI_ReadI2C_Byte(0xA0));
        printf("\r\nport B lane 1 settle = 0x%x", HDMI_ReadI2C_Byte(0xA2));
        printf("\r\nport B lane 2 settle = 0x%x", HDMI_ReadI2C_Byte(0xA4));
        printf("\r\nport B lane 3 settle = 0x%x", HDMI_ReadI2C_Byte(0xA6));
        settle_count = HDMI_ReadI2C_Byte(0xA0);
    }

    if (settle_count > 0x40) {
        settle_count = 0x06;
    } else {
        settle_count = settle_count / 2;
    }
    settle_count = settle_count / 2;
    HDMI_WriteI2C_Byte(0x02, settle_count);  // settle
    HDMI_WriteI2C_Byte(0x06, settle_count);  // settle

    LT9611_Video_Check();

    if (PCR_Format != PCR_None) {
        HDMI_WriteI2C_Byte(0xff, 0x80);
        HDMI_WriteI2C_Byte(0x11, 0x5A);
        Timer0_Delay1ms(10);
        HDMI_WriteI2C_Byte(0x11, 0xFA);

        LT9611_PLL(video);
        LT9611_MIPI_Pcr(video);
        LT9611_HDMI_TX_Digital(video);
        LT9611_Pcr_MK_Print();
        LT9611_HDMI_Out_Enable();
    } else {
        // printf("\r\nLT9611_Vid_Chg_Interrupt_Handle: no mipi video");
        LT9611_HDMI_Out_Disable();
    }
}

void lt9611_cec_msg_Interrupt_Handle(struct cec_msg *cec_msg)
{
    u8 cec_status;

    HDMI_WriteI2C_Byte(0xff, 0x86);
    cec_status = HDMI_ReadI2C_Byte(0xd2);

    cec_msg->cec_status = cec_status;
    printf("\nIRQ cec_status: 0x%02x", cec_status);

    HDMI_WriteI2C_Byte(0xff, 0x86);
    HDMI_WriteI2C_Byte(0xfc, 0x7f);  // cec irq clr
    HDMI_WriteI2C_Byte(0xfc, 0x00);

    HDMI_WriteI2C_Byte(0xff, 0x82);
    HDMI_WriteI2C_Byte(0x05, 0xff);  // clear3
    HDMI_WriteI2C_Byte(0x05, 0x7f);  // clear3

    lt9611_cec_msg_tx_handle(cec_msg);
    lt9611_cec_msg_rx_parse(cec_msg);
}

/////////////////////////////////////////////////////////////
// These function for Pattern output: start
/////////////////////////////////////////////////////////////
void LT9611_pattern_gcm(struct video_timing *video_format)
{
    u8 POL;
    POL = (video_format->h_polarity) * 0x10 + (video_format->v_polarity) * 0x20;
    POL = ~POL;
    POL &= 0x30;

    HDMI_WriteI2C_Byte(0xff, 0x82);
    HDMI_WriteI2C_Byte(0xa3, (u8)((video_format->hs + video_format->hbp) / 256));  // de_delay
    HDMI_WriteI2C_Byte(0xa4, (u8)((video_format->hs + video_format->hbp) % 256));
    HDMI_WriteI2C_Byte(0xa5, (u8)((video_format->vs + video_format->vbp) % 256));  // de_top
    HDMI_WriteI2C_Byte(0xa6, (u8)(video_format->hact / 256));
    HDMI_WriteI2C_Byte(0xa7, (u8)(video_format->hact % 256));  // de_cnt
    HDMI_WriteI2C_Byte(0xa8, (u8)(video_format->vact / 256));
    HDMI_WriteI2C_Byte(0xa9, (u8)(video_format->vact % 256));  // de_line
    HDMI_WriteI2C_Byte(0xaa, (u8)(video_format->htotal / 256));
    HDMI_WriteI2C_Byte(0xab, (u8)(video_format->htotal % 256));  // htotal
    HDMI_WriteI2C_Byte(0xac, (u8)(video_format->vtotal / 256));
    HDMI_WriteI2C_Byte(0xad, (u8)(video_format->vtotal % 256));  // vtotal
    HDMI_WriteI2C_Byte(0xae, (u8)(video_format->hs / 256));
    HDMI_WriteI2C_Byte(0xaf, (u8)(video_format->hs % 256));  // hvsa
    HDMI_WriteI2C_Byte(0xb0, (u8)(video_format->vs % 256));  // vsa

    HDMI_WriteI2C_Byte(0x47, (u8)(POL | 0x07));  // sync polarity
}

void LT9611_pattern_pixel_clk(struct video_timing *video_format)
{
    u32 pclk;
    pclk = video_format->pclk_khz;
    printf("\r\nset pixel clk = %ld", pclk);
    // printdec_u32(pclk); //Dec

    HDMI_WriteI2C_Byte(0xff, 0x83);
    HDMI_WriteI2C_Byte(0x2d, 0x50);

    if (pclk == 297000) {
        HDMI_WriteI2C_Byte(0x26, 0xb6);
        HDMI_WriteI2C_Byte(0x27, 0xf0);
    }
    if (pclk == 148500) {
        HDMI_WriteI2C_Byte(0x26, 0xb7);
    }
    if (pclk == 74250) {
        HDMI_WriteI2C_Byte(0x26, 0x9c);
    }
    HDMI_WriteI2C_Byte(0xff, 0x80);
    HDMI_WriteI2C_Byte(0x11, 0x5a);  // Pcr reset
    HDMI_WriteI2C_Byte(0x11, 0xfa);
}

void LT9611_pattern_en(void)
{
    HDMI_WriteI2C_Byte(0xff, 0x82);
    HDMI_WriteI2C_Byte(0x4f, 0x80);  //[7] = Select ad_txpll_d_clk.
    HDMI_WriteI2C_Byte(0x50, 0x20);
}

void LT9611_pattern(void)
{
    // DTV
    // video = &video_640x480_60Hz;
    // video = &video_720x480_60Hz;
    video = &video_1280x720_60Hz;
    // video = &video_1920x1080_60Hz;
    // video = &video_3840x2160_30Hz;

    // DMT
    // video = &video_1024x600_60Hz;
    // video = &video_1024x600_60Hz;
    // video = &video_1280x800_60Hz;

    // video = &video_1920x1080_30Hz;
	
    LT9611_Chip_ID();
    LT9611_System_Init();
    LT9611_pattern_en();
    LT9611_PLL(video);
    LT9611_pattern_gcm(video);

    LT9611_HDMI_TX_Digital(video);
    LT9611_HDMI_TX_Phy();

#if 0	
	//Audio pattern
	HDMI_WriteI2C_Byte(0xff,0x82);
	HDMI_WriteI2C_Byte(0xd6,0x8c);
	HDMI_WriteI2C_Byte(0xd7,0x06); //sync polarity
		
	HDMI_WriteI2C_Byte(0xff,0x84);
	HDMI_WriteI2C_Byte(0x06,0x0c);
	HDMI_WriteI2C_Byte(0x07,0x10);
	HDMI_WriteI2C_Byte(0x16,0x01);
	
	HDMI_WriteI2C_Byte(0x34,0xd4); //CTS_N
	
  LT9611_Audio_Init();
#endif

    //	LT9611_HDCP_Init();
    //	LT9611_load_hdcp_key();
    //  LT9611_Read_EDID();

    LT9611_HDMI_Out_Enable();
	printf("LT9611_pattern\n");
    //  LT9611_HDMI_CEC_ON(1);
    //	lt9611_cec_msg_set_logical_address();

    //	while(1)
    //	{
    //		lt9611_cec_msg_write_demo();
    //	}
}

/////////////////////////////////////////////////////////////
// These function for Pattern output: end
/////////////////////////////////////////////////////////////

// struct reg_sequence {
// 	uint16_t reg;
// 	uint8_t val;
// };
// const struct reg_sequence lt9611_power_on_seq[] = {
// 	/* LT9611_System_Init */
// 	{ 0x8101, 0x18 }, /* sel xtal clock */

// 	/* timer for frequency meter */
// 	{ 0x821b, 0x69 }, /* timer 2 */
// 	{ 0x821c, 0x78 },
// 	{ 0x82cb, 0x69 }, /* timer 1 */
// 	{ 0x82cc, 0x78 },

// 	/* irq init */
// 	{ 0x8251, 0x01 },
// 	{ 0x8258, 0x0a }, /* hpd irq */
// 	{ 0x8259, 0x80 }, /* hpd debounce width */
// 	{ 0x829e, 0xf7 }, /* video check irq */

// 	/* power consumption for work */
// 	{ 0x8004, 0xf0 },
// 	{ 0x8006, 0xf0 },
// 	{ 0x800a, 0x80 },
// 	{ 0x800b, 0x40 },
// 	{ 0x800d, 0xef },
// 	{ 0x8011, 0xfa },
// };
// const struct reg_sequence lt9611_power_off_seq[] = {
// 	{ 0x8130, 0x6a }
// };
// const struct reg_sequence lt9611_mipi_input_analog_reg_cfg[] = {
// 	{ 0x8106, 0x40 }, /* port A rx current */
// 	{ 0x810a, 0xfe }, /* port A ldo voltage set */
// 	{ 0x810b, 0xbf }, /* enable port A lprx */
// 	{ 0x8111, 0x40 }, /* port B rx current */
// 	{ 0x8115, 0xfe }, /* port B ldo voltage set */
// 	{ 0x8116, 0xbf }, /* enable port B lprx */

// 	{ 0x811c, 0x03 }, /* PortA clk lane no-LP mode */
// 	{ 0x8120, 0x03 }, /* PortB clk lane with-LP mode */
// };
// #define LT9611_2LANES 2
// struct reg_sequence lt9611_mipi_input_digital_reg_cfg[] = {
// 	{ 0x8300, LT9611_2LANES },
// 	{ 0x830a, 0x00 },
// 	{ 0x824f, 0x80 },
// 	{ 0x8250, 0x10 },
// 	{ 0x8302, 0x0a },
// 	{ 0x8306, 0x0a },
// };

// struct reg_sequence lt9611_mipi_video_setup_reg_cfg[] = {
// 	{ 0x830d, (u8)(v_total / 256) },
// 	{ 0x830e, (u8)(v_total % 256) },
// 	{ 0x830f, (u8)(vactive / 256) },
// 	{ 0x8310, (u8)(vactive % 256) },
// 	{ 0x8311, (u8)(h_total / 256) },
// 	{ 0x8312, (u8)(h_total % 256) },
// 	{ 0x8313, (u8)(hactive / 256) },
// 	{ 0x8314, (u8)(hactive % 256) },
// 	{ 0x8315, (u8)(vsync_len % 256) },
// 	{ 0x8316, (u8)(hsync_len % 256) },
// 	{ 0x8317, (u8)(vfront_porch % 256) },
// 	{ 0x8318, (u8)(vsync_porch % 256) },
// 	{ 0x8319, (u8)(hfront_porch % 256) },
// 	{ 0x831a, (u8)(hsync_porch / 256) },
// 	{ 0x831b, (u8)(hsync_porch % 256) },
// };
// const struct reg_sequence lt9611_pcr_setup_reg_cfg[] = {
// 	{ 0x830b, 0x01 },
// 	{ 0x830c, 0x10 },
// 	{ 0x8348, 0x00 },
// 	{ 0x8349, 0x81 },

// 	/* stage 1 */
// 	{ 0x8321, 0x4a },
// 	{ 0x8324, 0x71 },
// 	{ 0x8325, 0x30 },
// 	{ 0x832a, 0x01 },

// 	/* stage 2 */
// 	{ 0x834a, 0x40 },
// 	{ 0x831d, 0x10 },

// 	/* MK limit */
// 	{ 0x832d, 0x38 },
// 	{ 0x8331, 0x08 },
// };
// // regmap_multi_reg_write(lt9611->regmap, reg_cfg, ARRAY_SIZE(reg_cfg));
// const struct reg_sequence lt9611_pcr_setup_reg_cfg2[] = {
// 	{ 0x830b, 0x03 },
// 	{ 0x830c, 0xd0 },
// 	{ 0x8348, 0x03 },
// 	{ 0x8349, 0xe0 },
// 	{ 0x8324, 0x72 },
// 	{ 0x8325, 0x00 },
// 	{ 0x832a, 0x01 },
// 	{ 0x834a, 0x10 },
// 	{ 0x831d, 0x10 },
// 	{ 0x8326, 0x37 },
// };
// const struct reg_sequence lt9611_pcr_setup_reg_cfg3[] = {
// 	{ 0x8011, 0x5a },
// 	{ 0x8011, 0xfa }
// };
// const struct reg_sequence lt9611_pll_setup_reg_cfg[] = {
// 	/* txpll init */
// 	{ 0x8123, 0x40 },
// 	{ 0x8124, 0x64 },
// 	{ 0x8125, 0x80 },
// 	{ 0x8126, 0x55 },
// 	{ 0x812c, 0x37 },
// 	{ 0x812f, 0x01 },
// 	{ 0x8126, 0x55 },
// 	{ 0x8127, 0x66 },
// 	{ 0x8128, 0x88 },
// 	{ 0x812d, 0xaa },

// 	{ 0x82e3, pclk >> 17 },
// 	{ 0x82e4, pclk >> 9 },
// 	{ 0x82e5, pclk >> 1 },
// 	{ 0x8016, 0xf1 },
// 	{ 0x8016, 0xf3 },
// };
// const struct reg_sequence lt9611_hdmi_tx_digital_reg_cfg3[] = {
// 	{ 0x8443, 0x46 - vic },
// 	{ 0x8447, vic },
// 	{ 0x843d, 0x0a },
// 	{ 0x82d6, 0x8c },
// 	{ 0x82d7, 0x04 },
// };
// struct reg_sequence lt9611_hdmi_tx_phy_reg_cfg[] = {
// 	{ 0x8130, 0x6a },
// 	{ 0x8131, 0x44 }, /* HDMI DC mode */
// 	{ 0x8132, 0x4a },
// 	{ 0x8133, 0x0b },
// 	{ 0x8134, 0x00 },
// 	{ 0x8135, 0x00 },
// 	{ 0x8136, 0x00 },
// 	{ 0x8137, 0x44 },
// 	{ 0x813f, 0x0f },
// 	{ 0x8140, 0xa0 },
// 	{ 0x8141, 0xa0 },
// 	{ 0x8142, 0xa0 },
// 	{ 0x8143, 0xa0 },
// 	{ 0x8144, 0x0a },
// };

// const struct reg_sequence lt9611_hdmi_tx_digital_reg_cfg3[] = {
// 	{ 0x8203, val },
// 	{ 0x8207, 0xff },
// 	{ 0x8207, 0x3f }
// };

// const struct reg_sequence sleep_setup[] = {
// 	{ 0x8024, 0x76 },
// 	{ 0x8023, 0x01 },
// 	{ 0x8157, 0x03 }, /* set addr pin as output */
// 	{ 0x8149, 0x0b },
// 	{ 0x8151, 0x30 }, /* disable IRQ */
// 	{ 0x8102, 0x48 }, /* MIPI Rx power down */
// 	{ 0x8123, 0x80 },
// 	{ 0x8130, 0x00 },
// 	{ 0x8100, 0x01 }, /* bandgap power down */
// 	{ 0x8101, 0x00 }, /* system clk power down */
// };

// const struct reg_sequence lt9611_power_off_reg_cfg3[] = {
// 	{ 0x8130, 0x6a }
// };

// const struct reg_sequence HDMI_off_reg_cfg3[] = {
// 	{ 0x8130, 0x6a }
// };

// void my_LT9611_Init(void)
// {
// 	LT9611_HDMI_Out_Enable(0);
// 	LT9611_TTL_Input_Analog();
// 	LT9611_RST_PD_Init();
// 	LT9611_TTL_Input_Digital(lt86x);
// 	LT9611_Audio_Init(lt86x);
// 	LT9611_PLL(lt86x);
// 	LT9611_Video_Check(lt86x);

// }

void LT9611_Init(void)
{
    video = &video_1280x720_60Hz;

    u32 cnt = 0;
    // RESET_LT9611();
    LT9611_Chip_ID();
    LT9611_System_Init();

    // LT9611_RST_PD_Init();
    LT9611_MIPI_Input_Analog();
    LT9611_MIPI_Input_Digtal();

    //	Timer0_Delay1ms(500);/////////////////////////////////////
    //	LT9611_Video_Check();/////////////////////////////////////

    //	LT9611_PLL(video);////////////////////////////////////////
    //	LT9611_MIPI_Pcr(video); //////////////////////////////////

    LT9611_Audio_Init();
    LT9611_CSC();

    	if(lt9611.hdcp_encryption == hdcp_enable)
    	{
    LT9611_HDCP_Init();
    LT9611_load_hdcp_key();
    	}

    	// LT9611_HDMI_TX_Digital(video);////////////////////////////

    LT9611_HDMI_TX_Phy();

    LT9611_IRQ_Init();

    ////////////////////////////////////////////////////////////CEC
    // LT9611_HDMI_CEC_ON(1);
    // lt9611_cec_msg_set_logical_address();
    lt9611_cec_msg_init(&lt9611_cec_msg);
    ////////////////////////////////////////////////////////////

    LT9611_Enable_Interrupts(HPD_INTERRUPT_ENABLE, 1);
    LT9611_Enable_Interrupts(VID_CHG_INTERRUPT_ENABLE, 0);
    LT9611_Enable_Interrupts(CEC_INTERRUPT_ENABLE, 1);

    LT9611_Frequency_Meter_Byte_Clk();
    LT9611_Dphy_debug();
    LT9611_Htotal_Sysclk();

    Timer0_Delay1ms(200);  // HPD have debounce, wait HPD irq.
    LT9611_HDP_Interrupt_Handle();
    // lt9611_cec_msg_write_demo();
    printf("\r\n==========================LT9611 Initial End===============================");

    // while (1) {
    //     if (irq_task_flag) {
    //         // print("\r\nirq task...");
    LT9611_IRQ_Task();
    // sleep(1);
    // LT9611_IRQ_Task();
    // sleep(1);
    // LT9611_IRQ_Task();
    // sleep(1);
    // LT9611_IRQ_Task();
    // sleep(1);
    //         irq_task_flag = 0;
    //         set_EPI;
    //     }
    //     if (lt9611.hdcp_encryption == hdcp_enable) {
    //         if (LT9611_Get_HPD_Status()) {
    //             HDMI_WriteI2C_Byte(0xff, 0x85);
    //             if ((HDMI_ReadI2C_Byte(0x44) & 0x06) == 0x06) {
    //                 LT9611_HDCP_Disable();
    //                 LT9611_HDCP_Enable();
    //             }
    //         }
    //     }
    // }
}

// void IntbInterruptFun(void) interrupt 7
// {
//     if (PIF == 0x08) {
//         PIF = 0x00;  // clear interrupt flag
//         clr_IE0;
//     }
//     irq_task_flag = 1;
//     clr_EPI;
// }

void LT9611_IRQ_Task(void)
{
    u8 irq_flag3;
    u8 irq_flag0;
    u8 irq_flag1;

    // printf("\r\nLT9611_IRQ_Task :IRQ Task");

    HDMI_WriteI2C_Byte(0xff, 0x82);

    irq_flag0 = HDMI_ReadI2C_Byte(0x0c);
    irq_flag1 = HDMI_ReadI2C_Byte(0x0d);
    irq_flag3 = HDMI_ReadI2C_Byte(0x0f);

    if (irq_flag0 & 0x01)  // vedio_change_interrupt
    {
        LT9611_Vid_Chg_Interrupt_Handle();
    }

    if ((irq_flag1 & 0x80) == 0x80) {
        lt9611_cec_msg_Interrupt_Handle(&lt9611_cec_msg);
    }

    if (irq_flag3 & 0xc0)  // HPD_interrupt
    {
        LT9611_HDP_Interrupt_Handle();
    }
}
