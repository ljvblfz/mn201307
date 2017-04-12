/*
 * sysfs support for HD-audio core device
 */

#include <linux/slab.h>
#include <linux/sysfs.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <sound/core.h>
#include <sound/hdaudio.h>
#include "local.h"


#define PAGE_4K_SIZE 0x1000
#define USE_DUMP_STACK

#ifdef USE_DUMP_STACK
#include <linux/kprobes.h>
#include <asm/traps.h>
#endif

extern struct hdac_stream *g_azx_dev;
extern int g_bdl_hack;
#define SIN_STEPS 1000
/*#define SIN_STEPS 50*/
void fill_dmaarea_with_16khz(u16 *v_dma_area, u32 length)
{
	const u16 sample_table[SIN_STEPS] = {
		0x0000, 0x0066, 0x00CD, 0x0134, 0x019B, 0x0202, 0x0269, 0x02D0,
		0x0336, 0x039D,
		0x0404, 0x046A, 0x04D1, 0x0538, 0x059E, 0x0605, 0x066B, 0x06D1,
		0x0738, 0x079E,
		0x0804, 0x086A, 0x08D0, 0x0936, 0x099C, 0x0A01, 0x0A67, 0x0ACC,
		0x0B32, 0x0B97,
		0x0BFC, 0x0C61, 0x0CC6, 0x0D2B, 0x0D8F, 0x0DF4, 0x0E58, 0x0EBC,
		0x0F20, 0x0F84,
		0x0FE8, 0x104C, 0x10AF, 0x1112, 0x1175, 0x11D8, 0x123B, 0x129D,
		0x1300, 0x1362,
		0x13C4, 0x1426, 0x1487, 0x14E9, 0x154A, 0x15AB, 0x160B, 0x166C,
		0x16CC, 0x172C,
		0x178C, 0x17EB, 0x184B, 0x18AA, 0x1909, 0x1967, 0x19C6, 0x1A24,
		0x1A81, 0x1ADF,
		0x1B3C, 0x1B99, 0x1BF6, 0x1C52, 0x1CAE, 0x1D0A, 0x1D66, 0x1DC1,
		0x1E1C, 0x1E77,
		0x1ED1, 0x1F2B, 0x1F85, 0x1FDE, 0x2037, 0x2090, 0x20E8, 0x2140,
		0x2198, 0x21EF,
		0x2247, 0x229D, 0x22F4, 0x234A, 0x239F, 0x23F5, 0x244A, 0x249E,
		0x24F2, 0x2546,
		0x259A, 0x25ED, 0x263F, 0x2692, 0x26E4, 0x2735, 0x2786, 0x27D7,
		0x2827, 0x2877,
		0x28C7, 0x2916, 0x2964, 0x29B3, 0x2A01, 0x2A4E, 0x2A9B, 0x2AE7,
		0x2B34, 0x2B7F,
		0x2BCB, 0x2C15, 0x2C60, 0x2CAA, 0x2CF3, 0x2D3C, 0x2D85, 0x2DCD,
		0x2E14, 0x2E5C,
		0x2EA2, 0x2EE9, 0x2F2E, 0x2F74, 0x2FB8, 0x2FFD, 0x3040, 0x3084,
		0x30C7, 0x3109,
		0x314B, 0x318C, 0x31CD, 0x320E, 0x324D, 0x328D, 0x32CC, 0x330A,
		0x3348, 0x3385,
		0x33C2, 0x33FE, 0x343A, 0x3475, 0x34B0, 0x34EA, 0x3523, 0x355D,
		0x3595, 0x35CD,
		0x3604, 0x363B, 0x3672, 0x36A8, 0x36DD, 0x3712, 0x3746, 0x3779,
		0x37AC, 0x37DF,
		0x3811, 0x3842, 0x3873, 0x38A3, 0x38D3, 0x3902, 0x3930, 0x395E,
		0x398B, 0x39B8,
		0x39E4, 0x3A10, 0x3A3B, 0x3A65, 0x3A8F, 0x3AB8, 0x3AE1, 0x3B09,
		0x3B30, 0x3B57,
		0x3B7D, 0x3BA3, 0x3BC8, 0x3BEC, 0x3C10, 0x3C33, 0x3C56, 0x3C78,
		0x3C99, 0x3CBA,
		0x3CDA, 0x3CFA, 0x3D19, 0x3D37, 0x3D55, 0x3D72, 0x3D8E, 0x3DAA,
		0x3DC5, 0x3DE0,
		0x3DFA, 0x3E13, 0x3E2C, 0x3E44, 0x3E5C, 0x3E72, 0x3E89, 0x3E9E,
		0x3EB3, 0x3EC7,
		0x3EDB, 0x3EEE, 0x3F01, 0x3F12, 0x3F23, 0x3F34, 0x3F44, 0x3F53,
		0x3F62, 0x3F70,
		0x3F7D, 0x3F89, 0x3F95, 0x3FA1, 0x3FAC, 0x3FB6, 0x3FBF, 0x3FC8,
		0x3FD0, 0x3FD8,
		0x3FDE, 0x3FE5, 0x3FEA, 0x3FEF, 0x3FF3, 0x3FF7, 0x3FFA, 0x3FFC,
		0x3FFE, 0x3FFF,
		0x3FFF, 0x3FFF, 0x3FFE, 0x3FFD, 0x3FFB, 0x3FF8, 0x3FF4, 0x3FF0,
		0x3FEB, 0x3FE6,
		0x3FE0, 0x3FD9, 0x3FD2, 0x3FCA, 0x3FC1, 0x3FB8, 0x3FAE, 0x3FA4,
		0x3F98, 0x3F8D,
		0x3F80, 0x3F73, 0x3F65, 0x3F57, 0x3F48, 0x3F38, 0x3F28, 0x3F17,
		0x3F05, 0x3EF3,
		0x3EE0, 0x3ECD, 0x3EB8, 0x3EA4, 0x3E8E, 0x3E78, 0x3E61, 0x3E4A,
		0x3E32, 0x3E1A,
		0x3E01, 0x3DE7, 0x3DCC, 0x3DB1, 0x3D96, 0x3D79, 0x3D5C, 0x3D3F,
		0x3D21, 0x3D02,
		0x3CE2, 0x3CC2, 0x3CA2, 0x3C81, 0x3C5F, 0x3C3C, 0x3C19, 0x3BF6,
		0x3BD1, 0x3BAC,
		0x3B87, 0x3B61, 0x3B3A, 0x3B13, 0x3AEB, 0x3AC3, 0x3A99, 0x3A70,
		0x3A46, 0x3A1B,
		0x39EF, 0x39C3, 0x3997, 0x396A, 0x393C, 0x390D, 0x38DF, 0x38AF,
		0x387F, 0x384E,
		0x381D, 0x37EB, 0x37B9, 0x3786, 0x3753, 0x371F, 0x36EA, 0x36B5,
		0x367F, 0x3649,
		0x3612, 0x35DB, 0x35A3, 0x356B, 0x3532, 0x34F9, 0x34BE, 0x3484,
		0x3449, 0x340D,
		0x33D1, 0x3395, 0x3357, 0x331A, 0x32DB, 0x329D, 0x325E, 0x321E,
		0x31DE, 0x319D,
		0x315C, 0x311A, 0x30D8, 0x3095, 0x3052, 0x300E, 0x2FCA, 0x2F85,
		0x2F40, 0x2EFA,
		0x2EB4, 0x2E6E, 0x2E27, 0x2DDF, 0x2D97, 0x2D4F, 0x2D06, 0x2CBC,
		0x2C73, 0x2C28,
		0x2BDE, 0x2B92, 0x2B47, 0x2AFB, 0x2AAE, 0x2A61, 0x2A14, 0x29C6,
		0x2978, 0x292A,
		0x28DB, 0x288B, 0x283C, 0x27EB, 0x279B, 0x274A, 0x26F8, 0x26A6,
		0x2654, 0x2602,
		0x25AF, 0x255B, 0x2508, 0x24B3, 0x245F, 0x240A, 0x23B5, 0x235F,
		0x2309, 0x22B3,
		0x225D, 0x2206, 0x21AE, 0x2157, 0x20FF, 0x20A6, 0x204E, 0x1FF5,
		0x1F9B, 0x1F42,
		0x1EE8, 0x1E8E, 0x1E33, 0x1DD8, 0x1D7D, 0x1D21, 0x1CC6, 0x1C6A,
		0x1C0D, 0x1BB1,
		0x1B54, 0x1AF7, 0x1A99, 0x1A3B, 0x19DD, 0x197F, 0x1921, 0x18C2,
		0x1863, 0x1804,
		0x17A4, 0x1744, 0x16E4, 0x1684, 0x1624, 0x15C3, 0x1562, 0x1501,
		0x14A0, 0x143E,
		0x13DD, 0x137B, 0x1319, 0x12B6, 0x1254, 0x11F1, 0x118E, 0x112B,
		0x10C8, 0x1065,
		0x1001, 0x0F9E, 0x0F3A, 0x0ED6, 0x0E72, 0x0E0D, 0x0DA9, 0x0D44,
		0x0CDF, 0x0C7B,
		0x0C16, 0x0BB1, 0x0B4B, 0x0AE6, 0x0A81, 0x0A1B, 0x09B5, 0x0950,
		0x08EA, 0x0884,
		0x081E, 0x07B8, 0x0752, 0x06EB, 0x0685, 0x061F, 0x05B8, 0x0552,
		0x04EB, 0x0484,
		0x041E, 0x03B7, 0x0350, 0x02EA, 0x0283, 0x021C, 0x01B5, 0x014E,
		0x00E7, 0x0080,
		0x001A, 0xFFB4, 0xFF4D, 0xFEE6, 0xFE7F, 0xFE18, 0xFDB1, 0xFD4B,
		0xFCE4, 0xFC7D,
		0xFC16, 0xFBB0, 0xFB49, 0xFAE2, 0xFA7C, 0xFA15, 0xF9AF, 0xF949,
		0xF8E2, 0xF87C,
		0xF816, 0xF7B0, 0xF74A, 0xF6E4, 0xF67E, 0xF619, 0xF5B3, 0xF54D,
		0xF4E8, 0xF483,
		0xF41E, 0xF3B9, 0xF354, 0xF2EF, 0xF28A, 0xF226, 0xF1C1, 0xF15D,
		0xF0F9, 0xF095,
		0xF031, 0xEFCE, 0xEF6A, 0xEF07, 0xEEA4, 0xEE41, 0xEDDE, 0xED7B,
		0xED19, 0xECB7,
		0xEC55, 0xEBF3, 0xEB91, 0xEB30, 0xEACF, 0xEA6E, 0xEA0D, 0xE9AD,
		0xE94C, 0xE8EC,
		0xE88C, 0xE82D, 0xE7CD, 0xE76E, 0xE70F, 0xE6B1, 0xE652, 0xE5F4,
		0xE596, 0xE539,
		0xE4DB, 0xE47E, 0xE422, 0xE3C5, 0xE369, 0xE30D, 0xE2B1, 0xE256,
		0xE1FB, 0xE1A0,
		0xE146, 0xE0EC, 0xE092, 0xE039, 0xDFDF, 0xDF87, 0xDF2E, 0xDED6,
		0xDE7E, 0xDE27,
		0xDDD0, 0xDD79, 0xDD22, 0xDCCC, 0xDC76, 0xDC21, 0xDBCC, 0xDB77,
		0xDB23, 0xDACF,
		0xDA7C, 0xDA28, 0xD9D6, 0xD983, 0xD931, 0xD8E0, 0xD88E, 0xD83E,
		0xD7ED, 0xD79D,
		0xD74D, 0xD6FE, 0xD6B0, 0xD661, 0xD613, 0xD5C6, 0xD579, 0xD52C,
		0xD4E0, 0xD494,
		0xD448, 0xD3FE, 0xD3B3, 0xD369, 0xD31F, 0xD2D6, 0xD28E, 0xD245,
		0xD1FE, 0xD1B6,
		0xD170, 0xD129, 0xD0E3, 0xD09E, 0xD059, 0xD015, 0xCFD1, 0xCF8D,
		0xCF4A, 0xCF08,
		0xCEC6, 0xCE84, 0xCE43, 0xCE03, 0xCDC3, 0xCD83, 0xCD44, 0xCD06,
		0xCCC8, 0xCC8A,
		0xCC4E, 0xCC11, 0xCBD5, 0xCB9A, 0xCB5F, 0xCB25, 0xCAEB, 0xCAB2,
		0xCA79, 0xCA41,
		0xCA0A, 0xC9D2, 0xC99C, 0xC966, 0xC931, 0xC8FC, 0xC8C8, 0xC894,
		0xC861, 0xC82E,
		0xC7FC, 0xC7CA, 0xC79A, 0xC769, 0xC739, 0xC70A, 0xC6DC, 0xC6AE,
		0xC680, 0xC653,
		0xC627, 0xC5FB, 0xC5D0, 0xC5A6, 0xC57C, 0xC552, 0xC529, 0xC501,
		0xC4DA, 0xC4B3,
		0xC48C, 0xC467, 0xC441, 0xC41D, 0xC3F9, 0xC3D5, 0xC3B3, 0xC391,
		0xC36F, 0xC34E,
		0xC32E, 0xC30E, 0xC2EF, 0xC2D0, 0xC2B3, 0xC295, 0xC279, 0xC25D,
		0xC241, 0xC227,
		0xC20C, 0xC1F3, 0xC1DA, 0xC1C2, 0xC1AA, 0xC193, 0xC17D, 0xC167,
		0xC152, 0xC13E,
		0xC12A, 0xC117, 0xC104, 0xC0F2, 0xC0E1, 0xC0D0, 0xC0C0, 0xC0B1,
		0xC0A2, 0xC094,
		0xC087, 0xC07A, 0xC06E, 0xC062, 0xC057, 0xC04D, 0xC043, 0xC03A,
		0xC032, 0xC02A,
		0xC023, 0xC01D, 0xC017, 0xC012, 0xC00E, 0xC00A, 0xC007, 0xC004,
		0xC002, 0xC001,
		0xC001, 0xC001, 0xC001, 0xC003, 0xC005, 0xC007, 0xC00B, 0xC00F,
		0xC013, 0xC018,
		0xC01E, 0xC025, 0xC02C, 0xC034, 0xC03C, 0xC046, 0xC04F, 0xC05A,
		0xC065, 0xC070,
		0xC07D, 0xC08A, 0xC097, 0xC0A6, 0xC0B4, 0xC0C4, 0xC0D4, 0xC0E5,
		0xC0F6, 0xC108,
		0xC11B, 0xC12E, 0xC142, 0xC157, 0xC16C, 0xC182, 0xC199, 0xC1B0,
		0xC1C8, 0xC1E0,
		0xC1F9, 0xC213, 0xC22D, 0xC248, 0xC263, 0xC280, 0xC29C, 0xC2BA,
		0xC2D8, 0xC2F6,
		0xC316, 0xC335, 0xC356, 0xC377, 0xC399, 0xC3BB, 0xC3DE, 0xC401,
		0xC425, 0xC44A,
		0xC470, 0xC495, 0xC4BC, 0xC4E3, 0xC50B, 0xC533, 0xC55C, 0xC586,
		0xC5B0, 0xC5DA,
		0xC606, 0xC632, 0xC65E, 0xC68B, 0xC6B9, 0xC6E7, 0xC716, 0xC745,
		0xC775, 0xC7A5,
		0xC7D6, 0xC808, 0xC83A, 0xC86D, 0xC8A0, 0xC8D4, 0xC908, 0xC93D,
		0xC973, 0xC9A9,
		0xC9E0, 0xCA17, 0xCA4E, 0xCA87, 0xCAC0, 0xCAF9, 0xCB33, 0xCB6D,
		0xCBA8, 0xCBE4,
		0xCC20, 0xCC5C, 0xCC99, 0xCCD7, 0xCD15, 0xCD53, 0xCD92, 0xCDD2,
		0xCE12, 0xCE53,
		0xCE94, 0xCED5, 0xCF18, 0xCF5A, 0xCF9D, 0xCFE1, 0xD025, 0xD06A,
		0xD0AF, 0xD0F4,
		0xD13A, 0xD181, 0xD1C7, 0xD20F, 0xD257, 0xD29F, 0xD2E8, 0xD331,
		0xD37B, 0xD3C5,
		0xD40F, 0xD45A, 0xD4A6, 0xD4F2, 0xD53E, 0xD58B, 0xD5D8, 0xD626,
		0xD674, 0xD6C2,
		0xD711, 0xD760, 0xD7B0, 0xD800, 0xD851, 0xD8A2, 0xD8F3, 0xD945,
		0xD997, 0xD9E9,
		0xDA3C, 0xDA90, 0xDAE3, 0xDB37, 0xDB8C, 0xDBE0, 0xDC35, 0xDC8B,
		0xDCE1, 0xDD37,
		0xDD8D, 0xDDE4, 0xDE3C, 0xDE93, 0xDEEB, 0xDF43, 0xDF9C, 0xDFF5,
		0xE04E, 0xE0A8,
		0xE101, 0xE15C, 0xE1B6, 0xE211, 0xE26C, 0xE2C7, 0xE323, 0xE37F,
		0xE3DB, 0xE438,
		0xE495, 0xE4F2, 0xE54F, 0xE5AD, 0xE60B, 0xE669, 0xE6C7, 0xE726,
		0xE785, 0xE7E4,
		0xE844, 0xE8A3, 0xE903, 0xE963, 0xE9C4, 0xEA24, 0xEA85, 0xEAE6,
		0xEB47, 0xEBA9,
		0xEC0A, 0xEC6C, 0xECCE, 0xED31, 0xED93, 0xEDF6, 0xEE58, 0xEEBB,
		0xEF1F, 0xEF82,
		0xEFE5, 0xF049, 0xF0AD, 0xF111, 0xF175, 0xF1D9, 0xF23E, 0xF2A2,
		0xF307, 0xF36C,
		0xF3D1, 0xF436, 0xF49B, 0xF500, 0xF566, 0xF5CB, 0xF631, 0xF697,
		0xF6FC, 0xF762,
		0xF7C8, 0xF82E, 0xF895, 0xF8FB, 0xF961, 0xF9C7, 0xFA2E, 0xFA94,
		0xFAFB, 0xFB62,
		0xFBC8, 0xFC2F, 0xFC96, 0xFCFC, 0xFD63, 0xFDCA, 0xFE31, 0xFE98,
		0xFEFF, 0xFF65

	};

	u32 i;

	for (i = 0; i < length / 2; i++)
		v_dma_area[i] = sample_table[(i * 10) % SIN_STEPS];
}

#define DMA_TEST
#ifdef DMA_TEST
#include <sound/hda_register.h> /* audio register*/

#define REMAP_ADDR 0xffffc90000968000
#define ADDR_SD(x) (REMAP_ADDR + (0x20 * x + 0x80))


#define TRUSTY_VMCALL_SECURITY_TEST 0x74727509
struct security_test_mem {
	uint64_t vir_addr;
	uint64_t phy_addr;
	uint32_t size;
};

struct security_test_msg {
	uint32_t cmd_code;
	uint32_t ret_code;
	uint64_t msg_params;
};

enum security_test_cmd {
	CMD_START,
	CMD_ALLOC_MEM,
	CMD_FREE_MEM,
	CMD_CHECK_MEM,
	CMD_END
};

struct security_test_mem *g_test_alloc;
struct security_test_mem *g_test_noise;
struct security_test_mem *g_test_vmm_alloc;

struct security_test_msg *security_test_vmcall(
	struct security_test_msg *r0)
{
	__asm__ __volatile__ (
		"vmcall;\n"
		: "=D" (r0)
		: "a" (TRUSTY_VMCALL_SECURITY_TEST), "D" (r0));
	return r0;
}

static inline struct security_test_mem *security_test_alloc(
	uint32_t command,
	struct security_test_mem *test_alloc)
{
	struct security_test_msg *test_msg = NULL;

	test_msg = kmalloc(sizeof(struct security_test_msg), GFP_KERNEL);
	if (!test_msg)
		return NULL;
	test_msg->cmd_code = command;
	test_msg->ret_code = 0xdead;
	test_msg->msg_params = (uint64_t)test_alloc;
	test_msg = security_test_vmcall(test_msg);
	if (test_msg->ret_code != 0)
		pr_err("%s failed, ret_code = 0x%x\n",
				__func__, test_msg->ret_code);

	kfree(test_msg);

	return test_alloc;
}

enum test_mem_type {
	MEM_TYPE_VMM = 0,
	MEM_TYPE_LK = 1,
	MEM_TYPE_LINUX = 2,
	MEM_TYPE_INVALID = 0xFFFFFFFF
};


struct security_test_mem *dma_debug_buffer_alloc(
	struct security_test_mem *test_alloc, enum test_mem_type mem_type,
	uint32_t mem_size)
{
	pr_info("%s start\n", __func__);

	test_alloc = kmalloc(
				sizeof(struct security_test_mem),
				GFP_KERNEL);
	if (!test_alloc) {
		pr_info("dmatest Failed to allocate memory for test_alloc\n");
		return NULL;
	}
	memset((char *)test_alloc, 0, sizeof(struct security_test_mem));
	test_alloc->size = mem_size;

	switch (mem_type) {
	case MEM_TYPE_VMM:
		test_alloc = security_test_alloc(CMD_ALLOC_MEM, test_alloc);

		pr_info("%s alloc-HVA=0x%lx, HPA=0x%lx, Size=0x%x\n ",
			__func__, test_alloc->vir_addr, test_alloc->phy_addr,
			mem_size);
		break;

	case MEM_TYPE_LINUX:
		test_alloc->vir_addr = (uint64_t)kmalloc(mem_size,
							       GFP_KERNEL);
		if (!test_alloc->vir_addr) {
			pr_info("%s kmalloc failed for size 0x%x.\n", __func__,
				mem_size);
			kfree(test_alloc);
			return NULL;
		}
		memset((void *)test_alloc->vir_addr, 0, mem_size);
		test_alloc->phy_addr = virt_to_phys(test_alloc->vir_addr);
		pr_info("%s virt_to_phys %s;[virt=0x%p,\t phy_addr=0x%x]\n",
			__func__,
			test_alloc->phy_addr ? "ok" : "failed",
			test_alloc->vir_addr,
			test_alloc->phy_addr);
		break;
	case MEM_TYPE_LK:
		break;
	default:
		break;
	}

	return test_alloc;
}
EXPORT_SYMBOL(dma_debug_buffer_alloc);

/*
 * dmatest_stream_setup - set up the SD for streaming
 */
static void dmatest_stream_setup(void __iomem *sd_addr)
{
	pr_info("dma_test### %s:%d\n", __func__, __LINE__);
	/* program the BDL address */
	/* lower BDL address */
	writel((u32)g_test_alloc->phy_addr, sd_addr + AZX_REG_SD_BDLPL);
	pr_info("dma_test### %s:%d\n", __func__, __LINE__);
	/* upper BDL address */
	writel(upper_32_bits(
		       g_test_alloc->phy_addr), sd_addr + AZX_REG_SD_BDLPU);
	pr_info("dma_test### %s:%d\n", __func__, __LINE__);
}

static void trigger_dmatest(struct hdac_bus *bus, struct hdac_stream *azx_dev)
{
	pr_info("dma_test### %s:%d\n", __func__, __LINE__);
	/*u8 value = 30;*/
	/*u8 __iomem *addr = 0xffffc90000968160;*/
	/*writeb(value, addr);*/

	pr_info("dma_test### %s:%d\n", __func__, __LINE__);
	/* enable SIE */
	snd_hdac_chip_updatel(bus, INTCTL, 0, 1 << azx_dev->index);
	/* set DMA start and interrupt mask */
	snd_hdac_stream_updateb(azx_dev, SD_CTL,
				0, SD_CTL_DMA_START | SD_INT_MASK);
	azx_dev->running = true;
}

/*#define DMATEST_ATTR(type)					\*/
static ssize_t dma_test_show(struct device *dev,
			     struct device_attribute *attr,
			     char *buf)
{
	struct hdac_device *codec = dev_to_hdac_dev(dev);
	struct hdac_bus *bus = codec->bus;
	void __iomem *remap_addr = bus->remap_addr;

	pr_info("dma_test### %s:%d\n", __func__, __LINE__);
	pr_info("dma_test### remap_addr=%p, %s:%d\n", bus->remap_addr, __func__,
		__LINE__);

	struct hdac_stream *azx_dev = g_azx_dev;

	pr_info("###23 azx_dev->sd_addr=%p, __func__, __LINE__\n",
		azx_dev->sd_addr, __func__,
		__LINE__);

	/*g_test_alloc = dma_debug_buffer_alloc();*/
	/*dma_debug_buffer_alloc_test();*/

	fill_dmaarea_with_16khz((u16 *)g_test_alloc->vir_addr, PAGE_4K_SIZE);

	int i;
	u16 *area = g_test_alloc->vir_addr;

	pr_info("###24 print buffer:\n");
	for (i = 0; i < 50; i++)
		pr_info("###24 vir_addr[%u] = %u\n", i, *area++);

	pr_info("dma_test### %s:%d\n", __func__, __LINE__);

	dmatest_stream_setup(azx_dev->sd_addr);
	pr_info("dma_test### %s:%d\n", __func__, __LINE__);

	trigger_dmatest(bus, azx_dev);
	return sprintf(buf, "hello form hdac_sysfs\n");
}

struct device_attribute dev_attr_dma_test = {
	.attr	= { .name = "dma_test", .mode = S_IRUGO },
	.show	= dma_test_show,
};


void bdl_setup(uint32_t *bdl_vaddr, uint64_t dma_phy_addr,
	       uint64_t noise_phy_addr, uint8_t *bdl_seq,
	       uint32_t bdl_size)
{
	uint16_t i;

	pr_info("chen-%s,bdl_vaddr = %lx\n", __func__, bdl_vaddr);
	for (i = 0; i < bdl_size; i++) {
		if (bdl_seq[i]) {
			bdl_vaddr[4 * i + 0] =
				(uint32_t)(dma_phy_addr & 0xFFFFFFFF);
			bdl_vaddr[4 * i + 1] =
				(uint32_t)(dma_phy_addr >> 32);
		} else {
			bdl_vaddr[4 * i + 0] =
				(uint32_t)(noise_phy_addr & 0xFFFFFFFF);
			bdl_vaddr[4 * i + 1] =
				(uint32_t)(noise_phy_addr >> 32);
		}
		bdl_vaddr[4 * i + 2] = PAGE_4K_SIZE;
		bdl_vaddr[4 * i + 3] = 1;
	}
}

static void *bdl_vir_addr;

void bdl_hack_test(uint64_t addr)
{
	pr_info("chen-%s test_addr = 0x%lx\n", __func__, addr);
	struct hdac_stream *azx_dev = g_azx_dev;
	int i, period_bytes, periods;
	uint64_t bdl_phy_addr;
	uint8_t bdl_output_seq[] = {
		1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1 };
	uint32_t bdl_output_seq_size = sizeof(bdl_output_seq);
	int size;
	int j = 0;

	if (g_azx_dev) {
		period_bytes = azx_dev->period_bytes;
		if (period_bytes)
			periods = azx_dev->bufsize / period_bytes;
		else
			periods = 0;
	}

	if (!bdl_vir_addr) {
		bdl_vir_addr = kmalloc(PAGE_4K_SIZE, GFP_KERNEL);
		if (!bdl_vir_addr) {
			pr_info("%s-Allocate for BDL Area failed\n", __func__);
			dump_stack();
			return;
		}
	}

	memset(bdl_vir_addr, 0, PAGE_4K_SIZE);
	bdl_phy_addr = virt_to_phys(bdl_vir_addr);
	pr_info("%s-phyBDL = 0x%p,\n\t alloc = 0x%p,\n\t noise = 0x%p\n",
		__func__, bdl_phy_addr, g_test_alloc->phy_addr,
		g_test_noise->phy_addr);

	for (i = 0; i < periods; i++) {
		size = period_bytes;
		while (size > 0) {
			bdl_setup((uint32_t *)azx_dev->bdl.area + j * 4, addr,
				  addr, &bdl_output_seq[0],
				  1);
			size -= PAGE_4K_SIZE;
			j++;
		}
	}

	pr_info("chen-%s program hack bdl at 0x%p, to 0x%p\n", __func__,
		azx_dev->bdl.area,
		bdl_vir_addr);
	/* lower BDL address */
	snd_hdac_stream_writel(azx_dev, SD_BDLPL, (u32)bdl_phy_addr);
	/* upper BDL address */
	snd_hdac_stream_writel(azx_dev, SD_BDLPU, upper_32_bits(bdl_phy_addr));
}

void bdl_entry_hack_test(uint64_t addr)
{
	struct hdac_stream *azx_dev = g_azx_dev;
	int i, period_bytes, periods;
	uint8_t bdl_output_seq[] = { 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1 };
	uint32_t bdl_output_seq_size = sizeof(bdl_output_seq);

	pr_info("chen-%s test_addr = 0x%lx\n", __func__, addr);
	if (g_azx_dev) {
		period_bytes = azx_dev->period_bytes;
		if (period_bytes)
			periods = azx_dev->bufsize / period_bytes;
		else{
			pr_info("%s periods = 0\n", __func__);
			periods = 0;
		}
	}

	int j = 0;
	int size = 0;

	for (i = 0; i < periods; i++) {
		size = period_bytes;
		while (size > 0) {
			bdl_setup((uint32_t *)azx_dev->bdl.area + j * 4, addr,
				  addr, &bdl_output_seq[0],
				  1);
			size -= PAGE_4K_SIZE;
			j++;
		}
	}
}


static ssize_t bdl_hack_show(struct device *dev,
			     struct device_attribute *attr,
			     char *buf)
{
	return sprintf(buf, "g_bdl_hack=%d\n", g_bdl_hack);
}


static ssize_t bdl_hack_store(struct device *dev, struct device_attribute *attr,
			      const char *buf, size_t count)
{
	switch (*buf) {
	case '0':
		g_bdl_hack = 0;
		count = 1;
		break;
	case '1':
		count = 1;
		if (g_test_noise) {
			fill_dmaarea_with_16khz(
				(u16 *)g_test_noise->vir_addr, PAGE_4K_SIZE);
			bdl_entry_hack_test(g_test_noise->phy_addr);
		}
		break;
	case '2':
		count = 1;
		if (g_test_alloc) {
			memset((void *)g_test_alloc->vir_addr,
				0x00, PAGE_4K_SIZE);
			bdl_entry_hack_test(g_test_alloc->phy_addr);
		}
		break;
	case '3':
		count = 1;
		if (!g_test_vmm_alloc)
			g_test_vmm_alloc = dma_debug_buffer_alloc(
				g_test_vmm_alloc, MEM_TYPE_VMM, PAGE_4K_SIZE);

		if (g_test_vmm_alloc)
			bdl_entry_hack_test(g_test_vmm_alloc->phy_addr);

		msleep(1000);

		g_test_vmm_alloc = security_test_alloc(
				CMD_CHECK_MEM, g_test_vmm_alloc);
		pr_info("%s check-HVA=0x%lx, HPA=0x%lx\n ",
			__func__,
			g_test_vmm_alloc->vir_addr,
			g_test_vmm_alloc->phy_addr);

		g_test_vmm_alloc = security_test_alloc(
				CMD_FREE_MEM, g_test_vmm_alloc);
		pr_info("%s freed-HVA=0x%lx, HPA=0x%lx\n ",
			__func__,
			g_test_vmm_alloc->vir_addr,
			g_test_vmm_alloc->phy_addr);

		g_test_vmm_alloc = NULL;
		break;

	default:
		break;
	}

	return count;
}

struct device_attribute dev_attr_bdl_hack = {
	.attr	= { .name = "bdl_hack", .mode = S_IRUGO | S_IWUSR },
	.show	= bdl_hack_show,
	.store  = bdl_hack_store,
};


#endif
struct hdac_widget_tree {
	struct kobject *root;
	struct kobject *afg;
	struct kobject **nodes;
};

#define CODEC_ATTR(type)					\
static ssize_t type##_show(struct device *dev,			\
			   struct device_attribute *attr,	\
			   char *buf)				\
{								\
	struct hdac_device *codec = dev_to_hdac_dev(dev);	\
	return sprintf(buf, "0x%x\n", codec->type);		\
} \
static DEVICE_ATTR_RO(type)

#define CODEC_ATTR_STR(type)					\
static ssize_t type##_show(struct device *dev,			\
			     struct device_attribute *attr,	\
					char *buf)		\
{								\
	struct hdac_device *codec = dev_to_hdac_dev(dev);	\
	return sprintf(buf, "%s\n",				\
		       codec->type ? codec->type : "");		\
} \
static DEVICE_ATTR_RO(type)

CODEC_ATTR(type);
CODEC_ATTR(vendor_id);
CODEC_ATTR(subsystem_id);
CODEC_ATTR(revision_id);
CODEC_ATTR(afg);
CODEC_ATTR(mfg);
CODEC_ATTR_STR(vendor_name);
CODEC_ATTR_STR(chip_name);

static ssize_t modalias_show(struct device *dev, struct device_attribute *attr,
			     char *buf)
{
	return snd_hdac_codec_modalias(dev_to_hdac_dev(dev), buf, 256);
}
static DEVICE_ATTR_RO(modalias);

static struct attribute *hdac_dev_attrs[] = {
	&dev_attr_type.attr,
	&dev_attr_vendor_id.attr,
#ifdef DMA_TEST
	&dev_attr_dma_test.attr,
	&dev_attr_bdl_hack.attr,
#endif
	&dev_attr_subsystem_id.attr,
	&dev_attr_revision_id.attr,
	&dev_attr_afg.attr,
	&dev_attr_mfg.attr,
	&dev_attr_vendor_name.attr,
	&dev_attr_chip_name.attr,
	&dev_attr_modalias.attr,
	NULL
};

static struct attribute_group hdac_dev_attr_group = {
	.attrs	= hdac_dev_attrs,
};

const struct attribute_group *hdac_dev_attr_groups[] = {
	&hdac_dev_attr_group,
	NULL
};

/*
 * Widget tree sysfs
 *
 * This is a tree showing the attributes of each widget.  It appears like
 * /sys/bus/hdaudioC0D0/widgets/04/caps
 */

struct widget_attribute;

struct widget_attribute {
	struct attribute	attr;
	ssize_t (*show)(struct hdac_device *codec, hda_nid_t nid,
			struct widget_attribute *attr, char *buf);
	ssize_t (*store)(struct hdac_device *codec, hda_nid_t nid,
			 struct widget_attribute *attr,
			 const char *buf, size_t count);
};

static int get_codec_nid(struct kobject *kobj, struct hdac_device **codecp)
{
	struct device *dev = kobj_to_dev(kobj->parent->parent);
	int nid;
	ssize_t ret;

	ret = kstrtoint(kobj->name, 16, &nid);
	if (ret < 0)
		return ret;
	*codecp = dev_to_hdac_dev(dev);
	return nid;
}

static ssize_t widget_attr_show(struct kobject *kobj, struct attribute *attr,
				char *buf)
{
	struct widget_attribute *wid_attr =
		container_of(attr, struct widget_attribute, attr);
	struct hdac_device *codec;
	int nid;

	if (!wid_attr->show)
		return -EIO;
	nid = get_codec_nid(kobj, &codec);
	if (nid < 0)
		return nid;
	return wid_attr->show(codec, nid, wid_attr, buf);
}

static ssize_t widget_attr_store(struct kobject *kobj, struct attribute *attr,
				 const char *buf, size_t count)
{
	struct widget_attribute *wid_attr =
		container_of(attr, struct widget_attribute, attr);
	struct hdac_device *codec;
	int nid;

	if (!wid_attr->store)
		return -EIO;
	nid = get_codec_nid(kobj, &codec);
	if (nid < 0)
		return nid;
	return wid_attr->store(codec, nid, wid_attr, buf, count);
}

static const struct sysfs_ops widget_sysfs_ops = {
	.show	= widget_attr_show,
	.store	= widget_attr_store,
};

static void widget_release(struct kobject *kobj)
{
	kfree(kobj);
}

static struct kobj_type widget_ktype = {
	.release	= widget_release,
	.sysfs_ops	= &widget_sysfs_ops,
};

#define WIDGET_ATTR_RO(_name) \
	struct widget_attribute wid_attr_##_name = __ATTR_RO(_name)
#define WIDGET_ATTR_RW(_name) \
	struct widget_attribute wid_attr_##_name = __ATTR_RW(_name)

static ssize_t caps_show(struct hdac_device *codec, hda_nid_t nid,
			struct widget_attribute *attr, char *buf)
{
	return sprintf(buf, "0x%08x\n", get_wcaps(codec, nid));
}

static ssize_t pin_caps_show(struct hdac_device *codec, hda_nid_t nid,
			     struct widget_attribute *attr, char *buf)
{
	if (get_wcaps_type(get_wcaps(codec, nid)) != AC_WID_PIN)
		return 0;
	return sprintf(buf, "0x%08x\n",
		       snd_hdac_read_parm(codec, nid, AC_PAR_PIN_CAP));
}

static ssize_t pin_cfg_show(struct hdac_device *codec, hda_nid_t nid,
			    struct widget_attribute *attr, char *buf)
{
	unsigned int val;

	if (get_wcaps_type(get_wcaps(codec, nid)) != AC_WID_PIN)
		return 0;
	if (snd_hdac_read(codec, nid, AC_VERB_GET_CONFIG_DEFAULT, 0, &val))
		return 0;
	return sprintf(buf, "0x%08x\n", val);
}

static bool has_pcm_cap(struct hdac_device *codec, hda_nid_t nid)
{
	if (nid == codec->afg || nid == codec->mfg)
		return true;
	switch (get_wcaps_type(get_wcaps(codec, nid))) {
	case AC_WID_AUD_OUT:
	case AC_WID_AUD_IN:
		return true;
	default:
		return false;
	}
}

static ssize_t pcm_caps_show(struct hdac_device *codec, hda_nid_t nid,
			     struct widget_attribute *attr, char *buf)
{
	if (!has_pcm_cap(codec, nid))
		return 0;
	return sprintf(buf, "0x%08x\n",
		       snd_hdac_read_parm(codec, nid, AC_PAR_PCM));
}

static ssize_t pcm_formats_show(struct hdac_device *codec, hda_nid_t nid,
				struct widget_attribute *attr, char *buf)
{
	if (!has_pcm_cap(codec, nid))
		return 0;
	return sprintf(buf, "0x%08x\n",
		       snd_hdac_read_parm(codec, nid, AC_PAR_STREAM));
}

static ssize_t amp_in_caps_show(struct hdac_device *codec, hda_nid_t nid,
				struct widget_attribute *attr, char *buf)
{
	if (nid != codec->afg && !(get_wcaps(codec, nid) & AC_WCAP_IN_AMP))
		return 0;
	return sprintf(buf, "0x%08x\n",
		       snd_hdac_read_parm(codec, nid, AC_PAR_AMP_IN_CAP));
}

static ssize_t amp_out_caps_show(struct hdac_device *codec, hda_nid_t nid,
				 struct widget_attribute *attr, char *buf)
{
	if (nid != codec->afg && !(get_wcaps(codec, nid) & AC_WCAP_OUT_AMP))
		return 0;
	return sprintf(buf, "0x%08x\n",
		       snd_hdac_read_parm(codec, nid, AC_PAR_AMP_OUT_CAP));
}

static ssize_t power_caps_show(struct hdac_device *codec, hda_nid_t nid,
			       struct widget_attribute *attr, char *buf)
{
	if (nid != codec->afg && !(get_wcaps(codec, nid) & AC_WCAP_POWER))
		return 0;
	return sprintf(buf, "0x%08x\n",
		       snd_hdac_read_parm(codec, nid, AC_PAR_POWER_STATE));
}

static ssize_t gpio_caps_show(struct hdac_device *codec, hda_nid_t nid,
			      struct widget_attribute *attr, char *buf)
{
	return sprintf(buf, "0x%08x\n",
		       snd_hdac_read_parm(codec, nid, AC_PAR_GPIO_CAP));
}

static ssize_t connections_show(struct hdac_device *codec, hda_nid_t nid,
				struct widget_attribute *attr, char *buf)
{
	hda_nid_t list[32];
	int i, nconns;
	ssize_t ret = 0;

	nconns = snd_hdac_get_connections(codec, nid, list, ARRAY_SIZE(list));
	if (nconns <= 0)
		return nconns;
	for (i = 0; i < nconns; i++)
		ret += sprintf(buf + ret, "%s0x%02x", i ? " " : "", list[i]);
	ret += sprintf(buf + ret, "\n");
	return ret;
}

static WIDGET_ATTR_RO(caps);
static WIDGET_ATTR_RO(pin_caps);
static WIDGET_ATTR_RO(pin_cfg);
static WIDGET_ATTR_RO(pcm_caps);
static WIDGET_ATTR_RO(pcm_formats);
static WIDGET_ATTR_RO(amp_in_caps);
static WIDGET_ATTR_RO(amp_out_caps);
static WIDGET_ATTR_RO(power_caps);
static WIDGET_ATTR_RO(gpio_caps);
static WIDGET_ATTR_RO(connections);

static struct attribute *widget_node_attrs[] = {
	&wid_attr_caps.attr,
	&wid_attr_pin_caps.attr,
	&wid_attr_pin_cfg.attr,
	&wid_attr_pcm_caps.attr,
	&wid_attr_pcm_formats.attr,
	&wid_attr_amp_in_caps.attr,
	&wid_attr_amp_out_caps.attr,
	&wid_attr_power_caps.attr,
	&wid_attr_connections.attr,
	NULL,
};

static struct attribute *widget_afg_attrs[] = {
	&wid_attr_pcm_caps.attr,
	&wid_attr_pcm_formats.attr,
	&wid_attr_amp_in_caps.attr,
	&wid_attr_amp_out_caps.attr,
	&wid_attr_power_caps.attr,
	&wid_attr_gpio_caps.attr,
	NULL,
};

static const struct attribute_group widget_node_group = {
	.attrs = widget_node_attrs,
};

static const struct attribute_group widget_afg_group = {
	.attrs = widget_afg_attrs,
};

static void free_widget_node(struct kobject *kobj,
			     const struct attribute_group *group)
{
	if (kobj) {
		sysfs_remove_group(kobj, group);
		kobject_put(kobj);
	}
}

static void widget_tree_free(struct hdac_device *codec)
{
	struct hdac_widget_tree *tree = codec->widgets;
	struct kobject **p;

	if (!tree)
		return;
	free_widget_node(tree->afg, &widget_afg_group);
	if (tree->nodes) {
		for (p = tree->nodes; *p; p++)
			free_widget_node(*p, &widget_node_group);
		kfree(tree->nodes);
	}
	kobject_put(tree->root);
	kfree(tree);
	codec->widgets = NULL;
}

static int add_widget_node(struct kobject *parent, hda_nid_t nid,
			   const struct attribute_group *group,
			   struct kobject **res)
{
	struct kobject *kobj = kzalloc(sizeof(*kobj), GFP_KERNEL);
	int err;

	if (!kobj)
		return -ENOMEM;
	kobject_init(kobj, &widget_ktype);
	err = kobject_add(kobj, parent, "%02x", nid);
	if (err < 0)
		return err;
	err = sysfs_create_group(kobj, group);
	if (err < 0) {
		kobject_put(kobj);
		return err;
	}

	*res = kobj;
	return 0;
}

static int widget_tree_create(struct hdac_device *codec)
{
	struct hdac_widget_tree *tree;
	int i, err;
	hda_nid_t nid;

	tree = codec->widgets = kzalloc(sizeof(*tree), GFP_KERNEL);
	if (!tree)
		return -ENOMEM;

	tree->root = kobject_create_and_add("widgets", &codec->dev.kobj);
	if (!tree->root)
		return -ENOMEM;

	tree->nodes = kcalloc(codec->num_nodes + 1, sizeof(*tree->nodes),
			      GFP_KERNEL);
	if (!tree->nodes)
		return -ENOMEM;

	for (i = 0, nid = codec->start_nid; i < codec->num_nodes; i++, nid++) {
		err = add_widget_node(tree->root, nid, &widget_node_group,
				      &tree->nodes[i]);
		if (err < 0)
			return err;
	}

	if (codec->afg) {
		err = add_widget_node(tree->root, codec->afg,
				      &widget_afg_group, &tree->afg);
		if (err < 0)
			return err;
	}

	kobject_uevent(tree->root, KOBJ_CHANGE);
	return 0;
}

int hda_widget_sysfs_init(struct hdac_device *codec)
{
/*#ifdef USE_DUMP_STACK*/
	/*printk(KERN_ALERT "### [dump_stack] start ###\n");*/
	/*dump_stack();*/
	/*printk(KERN_ALERT "### [dump_stack] over ###\n");*/
/*#endif*/
	int err;

	if (codec->widgets)
		return 0; /* already created */

	err = widget_tree_create(codec);
	if (err < 0) {
		widget_tree_free(codec);
		return err;
	}

	return 0;
}

void hda_widget_sysfs_exit(struct hdac_device *codec)
{
	widget_tree_free(codec);
}
