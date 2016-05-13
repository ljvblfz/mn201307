/*
 * Copyright (C) 2012 Spreadtrum Communications Inc.
 *
 */

#include "sprdfb_chip_common.h"
#include "sprdfb.h"
#include "sprdfb_panel.h"

#if defined(CONFIG_I2C) && defined(CONFIG_SPI)
extern struct panel_if_ctrl sprdfb_mcu_ctrl;
#endif
extern struct panel_if_ctrl sprdfb_rgb_ctrl;
extern struct panel_if_ctrl sprdfb_mipi_ctrl;

extern struct panel_spec lcd_s6e8aa5x01_mipi_spec;
extern struct panel_spec lcd_nt35510_mipi_spec;
extern struct panel_spec lcd_nt35516_mipi_spec;
extern struct panel_spec lcd_nt35516_mcu_spec;
extern struct panel_spec lcd_nt35516_rgb_spi_spec;
extern struct panel_spec lcd_nt35521_mipi_spec;
extern struct panel_spec lcd_otm8018b_mipi_spec;
extern struct panel_spec lcd_hx8363_mcu_spec;
extern struct panel_spec lcd_panel_hx8363_rgb_spi_spec;
extern struct panel_spec lcd_panel_hx8363_rgb_spi_spec_viva;
extern struct panel_spec lcd_s6d0139_spec;
extern struct panel_spec lcd_otm1283a_mipi_spec;
extern struct panel_spec lcd_ssd2075_mipi_spec;
extern struct panel_spec lcd_panel_st7789v;
extern struct panel_spec lcd_panel_sc7798_rgb_spi;
extern struct panel_spec lcd_hx8369b_mipi_spec;
extern struct panel_spec lcd_sd7798d_mipi_spec;
extern struct panel_spec lcd_nt35502_mipi_spec;
extern struct panel_spec lcd_panel_ili9341;
extern struct panel_spec lcd_panel_ili9486;
extern struct panel_spec lcd_panel_ili9486_rgb_spi;
extern struct panel_spec lcd_ili9486s1_mipi_spec;
extern struct panel_spec lcd_nt51017_mipi_lvds_spec;
extern struct panel_spec lcd_t8861_mipi_spec;
extern struct panel_spec lcd_hx8379a_mipi_spec;
extern struct panel_spec lcd_hx8379c_mipi_spec;
extern struct panel_spec lcd_hx8389c_mipi_spec;
extern struct panel_spec ili6150_lvds_spec;
extern struct panel_spec lcd_rm68180_mipi_spec;
extern struct panel_spec lcd_rm68172_mipi_spec;
extern struct panel_spec lcd_ili9806e_mipi_spec;
extern struct panel_spec lcd_ili9806e_2_mipi_spec;
extern struct panel_spec lcd_ili9881c_mipi_spec;
extern struct panel_spec lcd_otm8019a_mipi_spec;
extern struct panel_spec lcd_fl10802_mipi_spec;
extern struct panel_spec lcd_fl11281_mipi_spec;
extern struct panel_spec lcd_jd9161_mipi_spec;
extern struct panel_spec lcd_hx8369b_mipi_vivaltoVE_spec;
extern struct panel_spec lcd_vx5b3d_mipi_spec;
extern struct panel_spec lcd_hx8369b_grandneo_mipi_spec;
extern struct panel_spec lcd_hx8369b_tshark2_j3_mipi_spec;
extern struct panel_spec lcd_sd7798d_mipi_spec;
extern struct panel_spec lcd_s6d77a1_mipi_spec;
extern struct panel_spec lcd_nt51017_mipi_spec;
extern struct panel_spec lcd_hx8394d_mipi_spec;
extern struct panel_spec lcd_hx8394f_mipi_spec;
extern struct panel_spec lcd_hx8394a_mipi_spec;
extern struct panel_spec lcd_nt35596h_mipi_spec;
extern struct panel_spec lcd_s6d7aa0x62_mipi_spec;
extern struct panel_spec lcd_s6d77ala_mipi_spec;
extern struct panel_spec lcd_ota7155a_mipi_spec;
extern struct panel_spec lcd_s6e88a0x02_mipi_spec;
extern struct panel_spec lcd_rm68200g_mipi_spec;
extern struct panel_spec lcd_ili9881c_mipi_spec;
extern struct panel_spec lcd_otm1287a_mipi_spec;

void sprdfb_panel_remove(struct sprdfb_device *dev);

static ushort colormap[256];

static struct panel_cfg panel_cfg[] = {
#if defined (CONFIG_FB_LCD_ILI9881C_MIPI)
	{
		.lcd_id = 0x9881,
		.panel = &lcd_ili9881c_mipi_spec,
	},
#endif
#if defined (CONFIG_FB_LCD_RM68200G_MIPI)
	{
		.lcd_id = 0x6820,
		.panel = &lcd_rm68200g_mipi_spec,
	},
#endif
#if defined (CONFIG_FB_LCD_S6E88A0X02_AMS430BF01)   // sharkls j1_3g lte  has used
	{
		.lcd_id = 0x88a2,
		.panel = &lcd_s6e88a0x02_mipi_spec,
	},
#endif	
#if defined (CONFIG_FB_LCD_OTA7155A_MIPI)
	{
		.lcd_id = 0x7155,
		.panel = &lcd_ota7155a_mipi_spec,
	},
#endif
#if defined (CONFIG_FB_LCD_S6D77ALA_MIPI_SHARKLSJ1MINI) 
		{
			.lcd_id = 0x6d77,
			.panel = &lcd_s6d77ala_mipi_spec,
		},
#endif
#if defined (CONFIG_FB_LCD_HX8379C_MIPI_SHARKL_J1POPLTE) || defined (CONFIG_FB_LCD_HX8379C_MIPI_SHARKLS_J1MINI)||defined (CONFIG_FB_LCD_HX8379C_MIPI_SHARKLS_J3LTE)||defined (CONFIG_FB_LCD_HX8379C_MIPI_SHARKLS_J3VLT) 
		{
			.lcd_id = 0x8379,
			.panel = &lcd_hx8379c_mipi_spec,
		},
#endif

#ifdef CONFIG_FB_LCD_S6E8AA5X01_MIPI
{
	.lcd_id = 0x400002,
	.panel = &lcd_s6e8aa5x01_mipi_spec,
},
#endif
#ifdef CONFIG_FB_LCD_NT35510_MIPI
	{
		.lcd_id = 0x10,
		.panel = &lcd_nt35510_mipi_spec,
	},
#endif

#ifdef CONFIG_FB_LCD_NT35521_MIPI
	{
		.lcd_id = 0x5521,
		.panel = &lcd_nt35521_mipi_spec,
	},
#endif

#ifdef CONFIG_FB_LCD_NT35516_MIPI
	{
		.lcd_id = 0x16,
		.panel = &lcd_nt35516_mipi_spec,
	},
#endif

#ifdef CONFIG_FB_LCD_HX8394D_MIPI
	{
		.lcd_id = 0x8394,
		.panel = &lcd_hx8394d_mipi_spec,
	},
#endif

#ifdef CONFIG_FB_LCD_HX8394F_MIPI
	{
		.lcd_id = 0x8394,
		.panel = &lcd_hx8394f_mipi_spec,
	},
#endif

#ifdef CONFIG_FB_LCD_NT35516_MCU
	{
		.lcd_id = 0x16,
		.panel = &lcd_nt35516_mcu_spec,
	},
#endif

#ifdef CONFIG_FB_LCD_NT35516_RGB_SPI
	{
		.lcd_id = 0x16,
		.panel = &lcd_nt35516_rgb_spi_spec,
	},
#endif

#ifdef CONFIG_FB_LCD_OTM8018B_MIPI
	{
		.lcd_id = 0x18,
		.panel = &lcd_otm8018b_mipi_spec,
	},
#endif

#ifdef CONFIG_FB_LCD_HX8363_MCU
	{
		.lcd_id = 0x18,
		.panel = &lcd_hx8363_mcu_spec,
	},
#endif

#ifdef CONFIG_FB_LCD_VIVA_RGB_SPI
	{
		.lcd_id = 0x63,
		.panel = &lcd_panel_hx8363_rgb_spi_spec_viva,
	},
#endif

#ifdef CONFIG_FB_LCD_RM68172_MIPI
	{
		.lcd_id = 0x8172,
		.panel = &lcd_rm68172_mipi_spec,
	},
#endif

#ifdef CONFIG_FB_LCD_RM68180_MIPI
	{
		.lcd_id = 0x80,
		.panel = &lcd_rm68180_mipi_spec,
	},
#endif

#ifdef CONFIG_FB_LCD_HX8363_RGB_SPI
	{
		.lcd_id = 0x84,
		.panel = &lcd_panel_hx8363_rgb_spi_spec,
	},
#endif

#ifdef CONFIG_FB_LCD_S6D0139
	{
		.lcd_id = 0x139,
		.panel = &lcd_s6d0139_spec,
	},
#endif

#ifdef CONFIG_FB_LCD_OTM1283A_MIPI
	{
		.lcd_id = 0x1283,
		.panel = &lcd_otm1283a_mipi_spec,
	},
#endif

#ifdef CONFIG_FB_LCD_OTM1287A_MIPI
	{
		.lcd_id = 0x1287,
		.panel = &lcd_otm1287a_mipi_spec,
	},
#endif

#ifdef CONFIG_FB_LCD_SSD2075_MIPI
	{
		.lcd_id = 0x2075,
		.panel = &lcd_ssd2075_mipi_spec,
	},
#endif

#ifdef CONFIG_FB_LCD_ST7789V_MCU
	{
		.lcd_id = 0x7789,
		.panel = &lcd_panel_st7789v,
	},
#endif

#ifdef CONFIG_FB_LCD_SC7798_RGB_SPI
	{
		.lcd_id = 0x7798,
		.panel = &lcd_panel_sc7798_rgb_spi,
	},
#endif

#if defined(CONFIG_FB_LCD_HX8369B_MIPI) || defined(CONFIG_FB_LCD_HX8369B_MIPI_COREPRIMELITE)|| defined(CONFIG_FB_LCD_HX8369B_MIPI_SHARKLS_Z3LTE) || defined(CONFIG_FB_LCD_HX8369B_MIPI_SHARKL_J1POPLTE) || defined(CONFIG_FB_LCD_HX8369B_MIPI_SHARKLS_J1MINI)|| defined(CONFIG_FB_LCD_HX8369B_MIPI_SHARKLS_J3LTE)|| defined(CONFIG_FB_LCD_HX8369B_MIPI_SHARKLS_J3VLT)
	{
		.lcd_id = 0x8369,
		.panel = &lcd_hx8369b_mipi_spec,
	},
#endif

#if defined (CONFIG_FB_LCD_HX8369B_MIPI) ||defined (CONFIG_FB_LCD_HX8369B_MIPI_KIRAN3G)
	{
		.lcd_id = 0x8369,
		.panel = &lcd_hx8369b_mipi_spec,
	},
#endif

#if defined(CONFIG_FB_LCD_SD7798D_MIPI_COREPRIMELITE) || defined(CONFIG_FB_LCD_SD7798D_MIPI_SHARKLS_Z3LTE)
	{
		.lcd_id = 0x55b8f0,
		.panel = &lcd_sd7798d_mipi_spec,
	},
#endif

#ifdef CONFIG_FB_LCD_NT35502_MIPI
	{
		.lcd_id = 0x8370,
		.panel = &lcd_nt35502_mipi_spec,
	},
#endif

#ifdef CONFIG_FB_LCD_NT51017_MIPI
	{
		.lcd_id = 0x51017,
		.panel = &lcd_nt51017_mipi_spec,
	},
#endif


#ifdef CONFIG_FB_LCD_ILI9341
	{
		.lcd_id = 0x9341,
		.panel = &lcd_panel_ili9341,
	},
#endif

#ifdef CONFIG_FB_LCD_ILI9486
	{
		.lcd_id = 0x9486,
		.panel = &lcd_panel_ili9486,
	},
#endif

#ifdef CONFIG_FB_LCD_ILI9486_RGB_SPI
	{
		.lcd_id = 0x9486,
		.panel = &lcd_panel_ili9486_rgb_spi,
	},
#endif

#ifdef CONFIG_FB_LCD_ILI9486S1_MIPI
	{
		.lcd_id = 0x8370,
		.panel = &lcd_ili9486s1_mipi_spec,
	},
#endif

#ifdef CONFIG_FB_LCD_HX8369B_MIPI_VIVALTO_VE
	{
		.lcd_id = 0x8369,
		.panel = &lcd_hx8369b_mipi_vivaltoVE_spec,
	},
#endif

#ifdef CONFIG_FB_LCD_VX5B3D_MIPI
	{
		.lcd_id = 0x8282,
		.panel = &lcd_vx5b3d_mipi_spec,
	},
#endif

#ifdef CONFIG_FB_LCD_GRANDNEO_MIPI
	{
		.lcd_id = 0x8369,
		.panel = &lcd_hx8369b_grandneo_mipi_spec,
	},
#endif

#ifdef CONFIG_FB_LCD_TSHARK2_J3_MIPI
	{
		.lcd_id = 0x8369,
		.panel = &lcd_hx8369b_tshark2_j3_mipi_spec,
	},
#endif

#ifdef CONFIG_FB_LCD_SD7798D_MIPI
	{
		.lcd_id = 0x55b8f0,
		.panel = &lcd_sd7798d_mipi_spec,
	},
#endif

#ifdef CONFIG_FB_LCD_S6D77A1_MIPI_PIKEA_J1
	{
		.lcd_id = 0x55b810,
		.panel = &lcd_s6d77a1_mipi_spec,
	},
#endif

#ifdef CONFIG_FB_LCD_NT51017_LVDS
	{
		.lcd_id = 0xC749,
		.panel = &lcd_nt51017_mipi_lvds_spec,
	},
#endif

#ifdef CONFIG_FB_LCD_T8861_MIPI
	{
		.lcd_id = 0x04,
		.panel = &lcd_t8861_mipi_spec,
	},
#endif

#ifdef CONFIG_FB_LCD_HX8379A_MIPI
	{
		.lcd_id = 0x8379,
		.panel = &lcd_hx8379a_mipi_spec,
	},
#endif

#ifdef CONFIG_FB_LCD_HX8389C_MIPI
	{
		.lcd_id = 0x8389,
		.panel = &lcd_hx8389c_mipi_spec,
	},
#endif

#ifdef CONFIG_FB_LCD_S6D7AA0X62_MIPI
	{
		.lcd_id = 0x6262,
		.panel = &lcd_s6d7aa0x62_mipi_spec,
	},
#endif

#ifdef CONFIG_FB_LCD_ILI6150_LVDS
	{
		.lcd_id = 0x1806,
		.panel = &ili6150_lvds_spec,
	},
#endif

#ifdef CONFIG_FB_LCD_ILI9806E_MIPI
{
    .lcd_id = 0x4,
    .panel = &lcd_ili9806e_mipi_spec,
},
#endif
#ifdef CONFIG_FB_LCD_ILI9806E_2_MIPI
{
    .lcd_id = 0x980602,
    .panel = &lcd_ili9806e_2_mipi_spec,
},
#endif

#ifdef CONFIG_FB_LCD_OTM8019A_MIPI
	{
		.lcd_id = 0x8019,
		.panel = &lcd_otm8019a_mipi_spec,
	},
#endif

#ifdef CONFIG_FB_LCD_FL11281_MIPI
	{
		.lcd_id = 0x1821,
		.panel = &lcd_fl11281_mipi_spec,
	},
#endif

#ifdef CONFIG_FB_LCD_FL10802_MIPI
	{
		.lcd_id = 0x1080,
		.panel = &lcd_fl10802_mipi_spec,
	},
#endif
#ifdef CONFIG_FB_LCD_JD9161_MIPI
	{
		.lcd_id = 0x916100,
		.panel = &lcd_jd9161_mipi_spec,
	},
#endif

#ifdef CONFIG_FB_LCD_HX8394D_MIPI
{
	.lcd_id = 0x8394,
	.panel = &lcd_hx8394d_mipi_spec,
},
#endif

#ifdef CONFIG_FB_LCD_HX8394A_MIPI
{
	.lcd_id = 0x8394,
	.panel = &lcd_hx8394a_mipi_spec,
},
#endif

#ifdef CONFIG_FB_LCD_NT35596H_MIPI
{
	.lcd_id = 0x96,
	.panel = &lcd_nt35596h_mipi_spec,
},
#endif

#ifdef CONFIG_FB_LCD_ILI9881C_MIPI
{
    .lcd_id = 0x9881,
    .panel = &lcd_ili9881c_mipi_spec,
},
#endif
};

vidinfo_t panel_info = {
#if defined(CONFIG_LCD_QVGA)
	.vl_col = 240,
	.vl_row = 320,
#elif defined(CONFIG_LCD_HVGA)
	.vl_col = 320,
	.vl_row = 480,
#elif defined(CONFIG_LCD_WVGA)
	.vl_col = 480,
	.vl_row = 800,
#elif defined(CONFIG_LCD_FWVGA)
	.vl_col = 480,
	.vl_row = 854,
#elif defined(CONFIG_LCD_QHD)
	.vl_col = 540,
	.vl_row = 960,
#elif defined(CONFIG_LCD_WSVGA)
	.vl_col = 1024,
	.vl_row = 600,
#elif defined(CONFIG_LCD_XGA)
	.vl_col = 768,
	.vl_row = 1024,
#elif defined(CONFIG_LCD_720P)
	.vl_col = 720,
	.vl_row = 1280,
#elif defined(CONFIG_LCD_1080P)
	.vl_col = 1080,
	.vl_row = 1920,
#endif
	.vl_bpix = 4,
	.cmap = colormap,
};

#ifdef CONFIG_LCD_COMPATIBLE_BY_READ_ID_VOLTAGE  //GST huafeizhou140324 add-s
#include "adc_drvapi.h"
uint16_t lcd_type;

unsigned int lcd_vol_value[][2]={
	{							     0, LCD_VOL_VALUE0+LCD_VOL_MARGIN},	//                         
	{LCD_VOL_VALUE1-LCD_VOL_MARGIN, LCD_VOL_VALUE1},	   //                        
	{LCD_VOL_VALUE2-LCD_VOL_MARGIN, LCD_VOL_VALUE2+LCD_VOL_MARGIN},   //     
	{LCD_VOL_VALUE3-LCD_VOL_MARGIN, LCD_VOL_VALUE3+LCD_VOL_MARGIN},   // 
	{0xff,0xff} //                           
};

#define _BUF_SIZE 5
uint32_t id_voltage_buf[_BUF_SIZE];

void put_id_voltage_value(uint32_t value)
{
    int i;
    for(i=0;i<_BUF_SIZE -1;i++)
    {
        id_voltage_buf[i] = id_voltage_buf[i+1];
    }

    id_voltage_buf[_BUF_SIZE-1] = value;
}

uint32_t get_id_voltage_value(void)
{
    unsigned long sum=0;
    int i;
    for(i=0; i < _BUF_SIZE; i++)
      sum += id_voltage_buf[i];

    return sum/_BUF_SIZE;
}

static uint32_t get_lcd_vol_type_by_adc(void)
{
	int i,adc_value;
	uint32_t id_vol,index;
	
	ADC_Init();
	udelay(10);

	//dummy read
	for(i=0;i<5;i++)
	{
		adc_value = ADC_GetValue(0, 0); //7715 a910 ADCI0 true:0-3.075v false:0-1.2v
	}
	
	for(i=0;i<_BUF_SIZE;i++)
	{
	retry_adc:
		adc_value = ADC_GetValue(0, 0); //7715 a910 ADCI0 true:0-3.075v false:0-1.2v
		printf("***get_lcd_vol_type(i=%d,adc_value=%d)\n",i,adc_value);
		if(adc_value < 0)
		{
		    printf("ADC read error\n");
		    udelay(10);
		    goto retry_adc;
		}
		else
		{
		    put_id_voltage_value(adc_value);
		}
	}
	
	adc_value = get_id_voltage_value();
	id_vol = (adc_value * 3075)/4096;
	printf("[sprdfb_panel]:adc_value:%d---id_vol:%d---\n",adc_value,id_vol);

	for(index = 0;((lcd_vol_value[index][0] != 0xff) && (lcd_vol_value[index][1] != 0xff));index++)
	{
		if((id_vol>=lcd_vol_value[index][0])&&(id_vol<=lcd_vol_value[index][1]))
		{
			return index;
		}
	}
	return 0;  //default value
}

#define GPIO_LCD_ID      134

static uint32_t get_lcd_vol_type_by_gpio(void)
{
	int i, id_vol;
	uint32_t index=0;

	sprd_gpio_request(NULL,GPIO_LCD_ID);
	sprd_gpio_direction_input(NULL, GPIO_LCD_ID);
	udelay(10);

retry_lcd_id:
	id_vol = sprd_gpio_get(NULL, GPIO_LCD_ID);
	printf("**zhangbei*get_lcd_vol_type(i=%d,id_vol=%d)\n",i,id_vol);
	id_vol = (id_vol>>6) & 1;
	printf("***get_lcd_vol_type(i=%d,id_vol=%d)\n",i,id_vol);
	if((id_vol != 0)&&(id_vol != 1)) {
		printf("GPIO read error\n");
		udelay(10);
		index++;
		if(index<10) { //try 10 times
			goto retry_lcd_id;
		} else {
			id_vol = 0;
		}	
	}

	if(0 == id_vol) {
		index = LCD_VOL_0MV;
	} else {//(0x01==id_vol)
		index = LCD_VOL_2800MV;
	}
	printf("[sprdfb_panel]:index:%d---\n",index);

	return index;
}
#endif

static int32_t panel_reset_dispc(struct panel_spec *self)
{
	dispc_write(1, DISPC_RSTN);
	mdelay(20);
	dispc_write(0, DISPC_RSTN);
	mdelay(20);
	dispc_write(1, DISPC_RSTN);
	/* wait 10ms util the lcd is stable */
	mdelay(120);
	return 0;
}

static void panel_reset(struct sprdfb_device *dev)
{
	FB_PRINT("sprdfb: [%s]\n",__FUNCTION__);

	//clk/data lane enter LP
	if(NULL != dev->if_ctrl->panel_if_before_panel_reset){
		dev->if_ctrl->panel_if_before_panel_reset(dev);
		mdelay(5);
	}

	//reset panel
	panel_reset_dispc(dev->panel);
}

static int panel_mount(struct sprdfb_device *dev, struct panel_spec *panel)
{
	uint16_t rval = 1;

	printf("sprdfb: [%s], type = %d\n",__FUNCTION__, panel->type);

	switch(panel->type){
#if defined(CONFIG_I2C) && defined(CONFIG_SPI)
	case SPRDFB_PANEL_TYPE_MCU:
		dev->if_ctrl = &sprdfb_mcu_ctrl;
		break;
#endif
	case SPRDFB_PANEL_TYPE_RGB:
	case SPRDFB_PANEL_TYPE_LVDS:
		dev->if_ctrl = &sprdfb_rgb_ctrl;
		break;
#if ((!defined(CONFIG_SC7710G2)) && (!defined(CONFIG_SPX15)))
	case SPRDFB_PANEL_TYPE_MIPI:
		dev->if_ctrl = &sprdfb_mipi_ctrl;
		break;
#endif
	default:
		printf("sprdfb: [%s]: erro panel type.(%d)",__FUNCTION__, panel->type);
		dev->if_ctrl = NULL;
		rval = 0 ;
		break;
	};

	if(NULL == dev->if_ctrl){
		return -1;
	}

	if(dev->if_ctrl->panel_if_check){
		rval = dev->if_ctrl->panel_if_check(panel);
	}

	if(0 == rval){
		printf("sprdfb: [%s] check panel fail!\n", __FUNCTION__);
		dev->if_ctrl = NULL;
		return -1;
	}

	dev->panel = panel;

	if(NULL == dev->panel->ops->panel_reset){
		dev->panel->ops->panel_reset = panel_reset_dispc;
	}

	dev->if_ctrl->panel_if_mount(dev);

	return 0;
}


int panel_init(struct sprdfb_device *dev)
{
	if((NULL == dev) || (NULL == dev->panel)){
		printf("sprdfb: [%s]: Invalid param\n", __FUNCTION__);
		return -1;
	}

	FB_PRINT("sprdfb: [%s], type = %d\n",__FUNCTION__, dev->panel->type);

	if(NULL != dev->if_ctrl->panel_if_init){
		dev->if_ctrl->panel_if_init(dev);
	}
	return 0;
}

int panel_ready(struct sprdfb_device *dev)
{
	if((NULL == dev) || (NULL == dev->panel)){
		printf("sprdfb: [%s]: Invalid param\n", __FUNCTION__);
		return -1;
	}

	FB_PRINT("sprdfb: [%s],  type = %d\n",__FUNCTION__, dev->panel->type);

	if(NULL != dev->if_ctrl->panel_if_ready){
		dev->if_ctrl->panel_if_ready(dev);
	}

	return 0;
}


static struct panel_spec *adapt_panel_from_readid(struct sprdfb_device *dev)
{
	int id, i, ret, b_panel_reset=0;
	#ifdef CONFIG_LCD_COMPATIBLE_BY_READ_ID_VOLTAGE  //GST huafeizhou140324 add-s
	uint32_t vol_type=0;
	#endif
	
	FB_PRINT("sprdfb: [%s]\n",__FUNCTION__);

#ifdef CONFIG_LCD_COMPATIBLE_BY_READ_ID_VOLTAGE  //GST huafeizhou140324 add-s
	#ifdef CONFIG_LCD_COMPATIBLE_READ_ID_VOLTAGE_BY_ADC
	vol_type=get_lcd_vol_type_by_adc();
	#endif
	#ifdef CONFIG_LCD_COMPATIBLE_READ_ID_VOLTAGE_BY_GPIO
	vol_type=get_lcd_vol_type_by_gpio();
	#endif
	save_lcd_vol_type_to_kernel(vol_type);
	printf("***adapt_panel_from_readid(vol_type=%d)\n",vol_type);
#endif

	for(i = 0;i<(sizeof(panel_cfg))/(sizeof(panel_cfg[0]));i++) {
		printf("sprdfb: [%s]: try panel 0x%x\n", __FUNCTION__, panel_cfg[i].lcd_id);
		ret = panel_mount(dev, panel_cfg[i].panel);
		if(ret < 0){
			printf("sprdfb: panel_mount failed!\n");
			continue;
		}
		dev->ctrl->update_clk(dev);
		panel_init(dev);
		if ((b_panel_reset==0) || (1 == dev->panel->is_need_reset))
		{
			panel_reset(dev);
			b_panel_reset=1;
		}
		id = dev->panel->ops->panel_readid(dev->panel);
		if(id == panel_cfg[i].lcd_id) {
			printf("sprdfb: [%s]: LCD Panel 0x%x is attached!\n", __FUNCTION__, panel_cfg[i].lcd_id);

		#ifdef CONFIG_LCD_COMPATIBLE_BY_READ_ID_VOLTAGE  //GST huafeizhou140324 add-s
			if(NULL !=dev->panel->ops->panel_get_vol_type){
				dev->panel->ops->panel_get_vol_type(dev->panel,vol_type);//vol_type to uboot
			}
		#endif
					
			if(NULL != dev->panel->ops->panel_init){
				dev->panel->ops->panel_init(dev->panel);		//zxdebug modify for LCD adaptor 
			}
			
			save_lcd_id_to_kernel(id);
			panel_ready(dev);
			return panel_cfg[i].panel;
		} else {							//zxdbg for LCD adaptor
			printf("sprdfb: [%s]: LCD Panel 0x%x attached fail!go next\n", __FUNCTION__, panel_cfg[i].lcd_id);
			sprdfb_panel_remove(dev);				//zxdebug modify for LCD adaptor 
		}
	}
	
	printf("sprdfb:  [%s]: final failed to attach LCD Panel!\n", __FUNCTION__);
	return NULL;
}

uint16_t sprdfb_panel_probe(struct sprdfb_device *dev)
{
	struct panel_spec *panel;

	if(NULL == dev){
		printf("sprdfb: [%s]: Invalid param\n", __FUNCTION__);
		return -1;
	}

	FB_PRINT("sprdfb: [%s]\n",__FUNCTION__);

	/* can not be here in normal; we should get correct device id from uboot */
	panel = adapt_panel_from_readid(dev);

	if (panel) {
		FB_PRINT("sprdfb: [%s] got panel\n", __FUNCTION__);
		return 0;
	}

	printf("sprdfb: [%s] can not got panel\n", __FUNCTION__);

	return -1;
}

void sprdfb_panel_invalidate_rect(struct panel_spec *self,
				uint16_t left, uint16_t top,
				uint16_t right, uint16_t bottom)
{
	FB_PRINT("sprdfb: [%s]\n, (%d, %d, %d,%d)",__FUNCTION__, left, top, right, bottom);

	if(NULL != self->ops->panel_invalidate_rect){
		self->ops->panel_invalidate_rect(self, left, top, right, bottom);
	}
}

void sprdfb_panel_invalidate(struct panel_spec *self)
{
	FB_PRINT("sprdfb: [%s]\n",__FUNCTION__);

	if(NULL != self->ops->panel_invalidate){
		self->ops->panel_invalidate(self);
	}
}

void sprdfb_panel_before_refresh(struct sprdfb_device *dev)
{
	FB_PRINT("sprdfb: [%s]\n",__FUNCTION__);

	if(NULL != dev->if_ctrl->panel_if_before_refresh)
		dev->if_ctrl->panel_if_before_refresh(dev);
}

void sprdfb_panel_after_refresh(struct sprdfb_device *dev)
{
	FB_PRINT("sprdfb: [%s]\n",__FUNCTION__);

	if(NULL != dev->if_ctrl->panel_if_after_refresh)
		dev->if_ctrl->panel_if_after_refresh(dev);
}

void sprdfb_panel_remove(struct sprdfb_device *dev)
{
	FB_PRINT("sprdfb: [%s]\n",__FUNCTION__);

	if((NULL != dev->if_ctrl) && (NULL != dev->if_ctrl->panel_if_uninit)){
		dev->if_ctrl->panel_if_uninit(dev);
	}
	dev->panel = NULL;
}

