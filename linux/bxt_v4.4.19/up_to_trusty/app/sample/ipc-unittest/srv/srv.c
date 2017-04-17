/*
 * Copyright (C) 2014-2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <err.h>
#include <list.h>
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <trusty_std.h>

#define LOG_TAG "ipc-unittest-srv"

#include <app/ipc_unittest/common.h>

#define SIN_STEPS 1000
#define PAGE_4K_SIZE 0x1000

typedef void (*event_handler_proc_t) (const uevent_t *ev);

typedef struct tipc_event_handler {
	event_handler_proc_t proc;
	void *priv;
} tipc_event_handler_t;

typedef struct tipc_srv {
	const char *name;
	uint   msg_num;
	size_t msg_size;
	uint   port_flags;
	size_t port_state_size;
	size_t chan_state_size;
	event_handler_proc_t port_handler;
	event_handler_proc_t chan_handler;
} tipc_srv_t;

typedef struct tipc_srv_state {
	const struct tipc_srv *service;
	handle_t port;
	void *priv;
	tipc_event_handler_t handler;
} tipc_srv_state_t;

/* closer services */
static void closer1_handle_port(const uevent_t *ev);

typedef struct closer1_state {
	uint conn_cnt;
} closer1_state_t;

static void closer2_handle_port(const uevent_t *ev);

typedef struct closer2_state {
	uint conn_cnt;
} closer2_state_t;

static void closer3_handle_port(const uevent_t *ev);

typedef struct closer3_state {
	handle_t chans[4];
	uint chan_cnt;
} closer3_state_t;

/* connect service */
static void connect_handle_port(const uevent_t *ev);

/* datasync service */
static void datasink_handle_port(const uevent_t *ev);
static void datasink_handle_chan(const uevent_t *ev);

/* datasink service has no per channel state so we can
 * just attach handler struct directly to channel handle
 */
static struct tipc_event_handler _datasink_chan_handler = {
	.proc = datasink_handle_chan,
	.priv = NULL,
};

/* echo service */
static void echo_handle_port(const uevent_t *ev);
static void echo_handle_chan(const uevent_t *ev);

typedef struct echo_chan_state {
	struct tipc_event_handler handler;
	uint msg_max_num;
	uint msg_cnt;
	uint msg_next_r;
	uint msg_next_w;
	struct ipc_msg_info msg_queue[0];
} echo_chan_state_t;

/* uuid service */
static void uuid_handle_port(const uevent_t *ev);

struct security_test_mem {
	uint64_t vir_addr;
	uint64_t phy_addr;
	uint32_t size;
};

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

struct security_test_mem *g_test_alloc;
struct security_test_mem *g_refer_alloc;

/* security test service */
static void security_test_handle_port(const uevent_t *ev);
static void security_test_check_handle_port(const uevent_t *ev);
static void security_test_free_handle_port(const uevent_t *ev);

/* Other globals */
static bool stopped = false;

/************************************************************************/

#define IPC_PORT_ALLOW_ALL  (  IPC_PORT_ALLOW_NS_CONNECT \
                             | IPC_PORT_ALLOW_TA_CONNECT \
                            )

#define SRV_NAME(name)   SRV_PATH_BASE ".srv." name


static const struct tipc_srv _services[] =
{
	{
		.name = SRV_NAME("closer1"),
		.msg_num = 2,
		.msg_size = 64,
		.port_flags = IPC_PORT_ALLOW_ALL,
		.port_handler = closer1_handle_port,
		.port_state_size = sizeof(struct closer1_state),
		.chan_handler = NULL,
	},
	{
		.name = SRV_NAME("closer2"),
		.msg_num = 2,
		.msg_size = 64,
		.port_flags = IPC_PORT_ALLOW_ALL,
		.port_handler = closer2_handle_port,
		.port_state_size = sizeof(struct closer2_state),
		.chan_handler = NULL,
	},
	{
		.name = SRV_NAME("closer3"),
		.msg_num = 2,
		.msg_size = 64,
		.port_flags = IPC_PORT_ALLOW_ALL,
		.port_handler = closer3_handle_port,
		.port_state_size = sizeof(struct closer3_state),
		.chan_handler = NULL,
	},
	{
		.name = SRV_NAME("connect"),
		.msg_num = 2,
		.msg_size = 64,
		.port_flags = IPC_PORT_ALLOW_TA_CONNECT,
		.port_handler = connect_handle_port,
		.chan_handler = NULL,
	},
	/* datasink services */
	{
		.name = SRV_NAME("datasink"),
		.msg_num = 2,
		.msg_size = 64,
		.port_flags = IPC_PORT_ALLOW_ALL,
		.port_handler = datasink_handle_port,
		.chan_handler = NULL,
	},
	{
		.name = SRV_NAME("ns_only"),
		.msg_num = 8,
		.msg_size = 64,
		.port_flags = IPC_PORT_ALLOW_NS_CONNECT,
		.port_handler = datasink_handle_port,
		.chan_handler = NULL,
	},
	{
		.name = SRV_NAME("ta_only"),
		.msg_num = 8,
		.msg_size = 64,
		.port_flags = IPC_PORT_ALLOW_TA_CONNECT,
		.port_handler = datasink_handle_port,
		.chan_handler = NULL,
	},
	/* echo */
	{
		.name = SRV_NAME("echo"),
		.msg_num = 8,
		.msg_size = MAX_PORT_BUF_SIZE,
		.port_flags = IPC_PORT_ALLOW_ALL,
		.port_handler = echo_handle_port,
		.chan_handler = echo_handle_chan,
	},
	/* uuid  test */
	{
		.name = SRV_NAME("uuid"),
		.msg_num = 2,
		.msg_size = 64,
		.port_flags = IPC_PORT_ALLOW_ALL,
		.port_handler = uuid_handle_port,
		.chan_handler = NULL,
	},
	/* security test */
	{
		.name = SRV_NAME("security_test"),
		.msg_num = 2,
		.msg_size = 64,
		.port_flags = IPC_PORT_ALLOW_ALL,
		.port_handler = security_test_handle_port,
		.chan_handler = NULL,
	},
	/* security test heap check*/
	{
		.name = SRV_NAME("security_test_check"),
		.msg_num = 2,
		.msg_size = 64,
		.port_flags = IPC_PORT_ALLOW_ALL,
		.port_handler = security_test_check_handle_port,
		.chan_handler = NULL,
	},
	/* security test free */
	{
		.name = SRV_NAME("security_test_free"),
		.msg_num = 2,
		.msg_size = 64,
		.port_flags = IPC_PORT_ALLOW_ALL,
		.port_handler = security_test_free_handle_port,
		.chan_handler = NULL,
	},
};

static struct tipc_srv_state _srv_states[countof(_services)] = {
	[0 ... (countof(_services) - 1)] = {
		.port = INVALID_IPC_HANDLE,
	},
};

/************************************************************************/

static struct tipc_srv_state *get_srv_state(const uevent_t *ev)
{
	return containerof(ev->cookie, struct tipc_srv_state, handler);
}

static void _destroy_service(struct tipc_srv_state *state)
{
	if (!state) {
		TLOGI("non-null state expected\n");
		return;
	}

	/* free state if any */
	if (state->priv) {
		free(state->priv);
		state->priv = NULL;
	}

	/* close port */
	if (state->port != INVALID_IPC_HANDLE) {
		int rc = close(state->port);
		if (rc != NO_ERROR) {
			TLOGI("Failed (%d) to close port %d\n",
			       rc, state->port);
		}
		state->port = INVALID_IPC_HANDLE;
	}

	/* reset handler */
	state->service = NULL;
	state->handler.proc = NULL;
	state->handler.priv = NULL;
}


/*
 *  Create service
 */
static int _create_service(const struct tipc_srv *srv,
                           struct tipc_srv_state *state)
{
	if (!srv || !state) {
		TLOGI("null service specified: %p: %p\n");
		return ERR_INVALID_ARGS;
	}

	/* create port */
	int rc = port_create(srv->name, srv->msg_num, srv->msg_size,
			     srv->port_flags);
	if (rc < 0) {
		TLOGI("Failed (%d) to create port\n", rc);
		return rc;
	}

	/* setup port state  */
	state->port = (handle_t)rc;
	state->handler.proc = srv->port_handler;
	state->handler.priv = state;
	state->service = srv;
	state->priv = NULL;

	if (srv->port_state_size) {
		/* allocate port state */
		state->priv = calloc(1, srv->port_state_size);
		if (!state->priv) {
			rc = ERR_NO_MEMORY;
			goto err_calloc;
		}
	}

	/* attach handler to port handle */
	rc = set_cookie(state->port, &state->handler);
	if (rc < 0) {
		TLOGI("Failed (%d) to set cookie on port %d\n",
		      rc, state->port);
		goto err_set_cookie;
	}

	return NO_ERROR;

err_calloc:
err_set_cookie:
	_destroy_service(state);
	return rc;
}


/*
 *  Restart specified service
 */
static int restart_service(struct tipc_srv_state *state)
{
	if (!state) {
		TLOGI("non-null state expected\n");
		return ERR_INVALID_ARGS;
	}

	const struct tipc_srv *srv = state->service;
	_destroy_service(state);
	return _create_service(srv, state);
}

/*
 *  Kill all servoces
 */
static void kill_services(void)
{
	TLOGI ("Terminating unittest services\n");

	/* close any opened ports */
	for (uint i = 0; i < countof(_services); i++) {
		_destroy_service(&_srv_states[i]);
	}
}

/*
 *  Initialize all services
 */
static int init_services(void)
{
	TLOGI ("Init unittest services!!!\n");

	for (uint i = 0; i < countof(_services); i++) {
		int rc = _create_service(&_services[i], &_srv_states[i]);
		if (rc < 0) {
			TLOGI("Failed (%d) to create service %s\n",
			      rc, _services[i].name);
			return rc;
		}
	}

	return 0;
}

/*
 *  Handle common port errors
 */
static bool handle_port_errors(const uevent_t *ev)
{
	if ((ev->event & IPC_HANDLE_POLL_ERROR) ||
	    (ev->event & IPC_HANDLE_POLL_HUP) ||
	    (ev->event & IPC_HANDLE_POLL_MSG) ||
	    (ev->event & IPC_HANDLE_POLL_SEND_UNBLOCKED)) {
		/* should never happen with port handles */
		TLOGI("error event (0x%x) for port (%d)\n",
		       ev->event, ev->handle);

		/* recreate service */
		restart_service(get_srv_state(ev));
		return true;
	}

	return false;
}

/****************************** connect test service *********************/

/*
 *  Local wrapper on top of async connect that provides
 *  synchronos connect with timeout.
 */
int sync_connect(const char *path, uint timeout)
{
	int rc;
	uevent_t evt;
	handle_t chan;

	rc = connect(path, IPC_CONNECT_ASYNC | IPC_CONNECT_WAIT_FOR_PORT);
	if (rc >= 0) {
		chan = (handle_t) rc;
		rc = wait(chan, &evt, timeout);
		if (rc == 0) {
			rc = ERR_BAD_STATE;
			if (evt.handle == chan) {
				if (evt.event & IPC_HANDLE_POLL_READY)
					return chan;

				if (evt.event & IPC_HANDLE_POLL_HUP)
					rc = ERR_CHANNEL_CLOSED;
			}
		}
		close(chan);
	}
	return rc;
}

static void connect_handle_port(const uevent_t *ev)
{
	uuid_t peer_uuid;

	if (handle_port_errors(ev))
		return;

	if (ev->event & IPC_HANDLE_POLL_READY) {
		char path[MAX_PORT_PATH_LEN];

		/* accept incomming connection and close it */
		int rc = accept(ev->handle, &peer_uuid);
		if (rc < 0 && rc != ERR_CHANNEL_CLOSED) {
			TLOGI("accept failed (%d)\n", rc);
			return;
		}
		if (rc >= 0) {
			close ((handle_t)rc);
		}

		/* but then issue a series of connect requests */
		for (uint i = 2; i < MAX_USER_HANDLES; i++) {
			sprintf(path, "%s.port.accept%d", SRV_PATH_BASE, i);
			rc = sync_connect(path, 1000);
			close(rc);
		}
	}
}

/****************************** closer services **************************/

static void closer1_handle_port(const uevent_t *ev)
{
	uuid_t peer_uuid;
	struct closer1_state *st = get_srv_state(ev)->priv;

	if (handle_port_errors(ev))
		return;

	if (ev->event & IPC_HANDLE_POLL_READY) {
		/* new connection request, bump counter */
		st->conn_cnt++;

		/* accept it */
		int rc = accept(ev->handle, &peer_uuid);
		if (rc < 0) {
			TLOGI("accept failed (%d)\n", rc);
			return;
		}
		handle_t chan = (handle_t) rc;

		if (st->conn_cnt & 1) {
			/* sleep a bit */
			nanosleep (0, 0, 100 * MSEC);
		}
		/* and close it */
		rc = close(chan);
		if (rc != NO_ERROR) {
			TLOGI("Failed (%d) to close chan %d\n", rc, chan);
		}
	}
}

static void closer2_handle_port(const uevent_t *ev)
{
	struct closer2_state *st = get_srv_state(ev)->priv;

	if (handle_port_errors(ev))
		return;

	if (ev->event & IPC_HANDLE_POLL_READY) {
		/* new connection request, bump counter */
		st->conn_cnt++;
		if (st->conn_cnt & 1) {
			/* sleep a bit */
			nanosleep (0, 0, 100 * MSEC);
		}

		/*
		 * then close the port without accepting any connections
		 * and restart it again
		 */
		restart_service(get_srv_state(ev));
	}
}

static void closer3_handle_port(const uevent_t *ev)
{
	uuid_t peer_uuid;
	struct closer3_state *st = get_srv_state(ev)->priv;

	if (handle_port_errors(ev))
		return;

	if (ev->event & IPC_HANDLE_POLL_READY) {
		/* accept connection */
		int rc = accept(ev->handle, &peer_uuid);
		if (rc < 0) {
			TLOGI("accept failed (%d)\n", rc);
			return;
		}
		/* add it to connection pool */
		st->chans[st->chan_cnt++] = (handle_t) rc;

		/* attach datasink service handler just in case */
		set_cookie((handle_t)rc, &_datasink_chan_handler);

		/* when max number of connection reached */
		if (st->chan_cnt == countof(st->chans)) {
			/* wait a bit */
			nanosleep (0, 0, 100 * MSEC);

			/* and close them all */
			for (uint i = 0; i < st->chan_cnt; i++ )
				close(st->chans[i]);
			st->chan_cnt = 0;
		}
		return;
	}
}

/****************************** datasync service **************************/


static int datasink_handle_msg(const uevent_t *ev)
{
	int rc;
	ipc_msg_info_t inf;

	/* for all messages */
	for (;;) {
		/* get message */
		rc = get_msg(ev->handle, &inf);
		if (rc == ERR_NO_MSG)
			break; /* no new messages */

		if (rc != NO_ERROR) {
			TLOGI("failed (%d) to get_msg for chan (%d)\n",
			      rc, ev->handle);
			return rc;
		}

		/* and retire it without actually reading  */
		rc = put_msg(ev->handle, inf.id);
		if (rc != NO_ERROR) {
			TLOGI("failed (%d) to putt_msg for chan (%d)\n",
			      rc, ev->handle);
			return rc;
		}
	}

	return NO_ERROR;
}

/*
 *  Datasink service channel handler
 */
static void datasink_handle_chan(const uevent_t *ev)
{
	if ((ev->event & IPC_HANDLE_POLL_ERROR) ||
	    (ev->event & IPC_HANDLE_POLL_SEND_UNBLOCKED)) {
		/* close it as it is in an error state */
		TLOGI("error event (0x%x) for chan (%d)\n",
		       ev->event, ev->handle);
		close(ev->handle);
		return;
	}

	if (ev->event & IPC_HANDLE_POLL_MSG) {
		if (datasink_handle_msg(ev) != 0) {
			close(ev->handle);
			return;
		}
	}

	if (ev->event & IPC_HANDLE_POLL_HUP) {
		/* closed by peer */
		close(ev->handle);
		return;
	}
}

/*
 *  Datasink service port event handler
 */
static void datasink_handle_port(const uevent_t *ev)
{
	uuid_t peer_uuid;

	if (handle_port_errors(ev))
		return;

	if (ev->event & IPC_HANDLE_POLL_READY) {
		/* incomming connection: accept it */
		int rc = accept(ev->handle, &peer_uuid);
		if (rc < 0) {
			TLOGI("failed (%d) to accept on port %d\n",
			       rc, ev->handle);
			return;
		}

		handle_t chan = (handle_t) rc;
		rc = set_cookie(chan, &_datasink_chan_handler);
		if (rc) {
			TLOGI("failed (%d) to set_cookie on chan %d\n",
			       rc, chan);
		}
	}
}

/******************************   echo service    **************************/

static uint8_t echo_msg_buf[MAX_PORT_BUF_SIZE];

static int _echo_handle_msg(const uevent_t *ev, int delay)
{
	int rc;
	iovec_t iov;
	ipc_msg_t msg;
	echo_chan_state_t *st = containerof(ev->cookie, echo_chan_state_t, handler);

	/* get all messages */
	while (st->msg_cnt != st->msg_max_num) {
		rc = get_msg(ev->handle, &st->msg_queue[st->msg_next_w]);
		if (rc == ERR_NO_MSG)
			break; /* no new messages */

		if (rc != NO_ERROR) {
			TLOGI("failed (%d) to get_msg for chan (%d)\n",
			      rc, ev->handle);
			return rc;
		}

		st->msg_cnt++;
		st->msg_next_w++;
		if (st->msg_next_w == st->msg_max_num)
			st->msg_next_w = 0;
	}

	/* handle all messages in queue */
	while (st->msg_cnt) {
		/* init message structure */
		iov.base = echo_msg_buf;
		iov.len  = sizeof(echo_msg_buf);
		msg.num_iov = 1;
		msg.iov     = &iov;
		msg.num_handles = 0;
		msg.handles  = NULL;

		/* read msg content */
		rc = read_msg(ev->handle, st->msg_queue[st->msg_next_r].id, 0, &msg);
		if (rc < 0) {
			TLOGI("failed (%d) to read_msg for chan (%d)\n",
			      rc, ev->handle);
			return rc;
		}

		/* update number of bytes received */
		iov.len = (size_t) rc;

		/* optionally sleep a bit an send it back */
		if (delay) {
			nanosleep (0, 0, 1000);
		}

		/* and send it back */
		rc = send_msg(ev->handle, &msg);
		if (rc == ERR_NOT_ENOUGH_BUFFER)
			break;

		if (rc < 0) {
			TLOGI("failed (%d) to send_msg for chan (%d)\n",
			      rc, ev->handle);
			return rc;
		}

		/* retire original message */
		rc = put_msg(ev->handle, st->msg_queue[st->msg_next_r].id);
		if (rc != NO_ERROR) {
			TLOGI("failed (%d) to put_msg for chan (%d)\n",
			      rc, ev->handle);
			return rc;
		}

		/* advance queue */
		st->msg_cnt--;
		st->msg_next_r++;
		if (st->msg_next_r == st->msg_max_num)
			st->msg_next_r = 0;
	}
	return NO_ERROR;
}

static int echo_handle_msg(const uevent_t *ev)
{
	return _echo_handle_msg(ev, false);
}

/*
 * echo service channel handler
 */
static void echo_handle_chan(const uevent_t *ev)
{
	if (ev->event & IPC_HANDLE_POLL_ERROR) {
		/* close it as it is in an error state */
		TLOGI("error event (0x%x) for chan (%d)\n",
		      ev->event, ev->handle);
		goto close_it;
	}

	if (ev->event & (IPC_HANDLE_POLL_MSG |
		         IPC_HANDLE_POLL_SEND_UNBLOCKED)) {
		if (echo_handle_msg(ev) != 0) {
			TLOGI("error event (0x%x) for chan (%d)\n",
			      ev->event, ev->handle);
			goto close_it;
		}
	}

	if (ev->event & IPC_HANDLE_POLL_HUP) {
		goto close_it;
	}

	return;

close_it:
	free(ev->cookie);
	close(ev->handle);
}

/*
 *  echo service port event handler
 */
static void echo_handle_port(const uevent_t *ev)
{
	uuid_t peer_uuid;
	struct echo_chan_state *chan_st;
	const struct tipc_srv *srv = get_srv_state(ev)->service;

	if (handle_port_errors(ev))
		return;

	if (ev->event & IPC_HANDLE_POLL_READY) {
		handle_t chan;

		/* incomming connection: accept it */
		int rc = accept(ev->handle, &peer_uuid);
		if (rc < 0) {
			TLOGI("failed (%d) to accept on port %d\n",
			       rc, ev->handle);
			return;
		}
		chan = (handle_t) rc;

		chan_st = calloc(1, sizeof(struct echo_chan_state) +
		                    sizeof(ipc_msg_info_t) * srv->msg_num);
		if (!chan_st) {
			TLOGI("failed (%d) to callocate state for chan %d\n",
			       rc, chan);
			close(chan);
			return;
		}

		/* init state */
		chan_st->msg_max_num  = srv->msg_num;
		chan_st->handler.proc = srv->chan_handler;
		chan_st->handler.priv = chan_st;

		/* attach it to handle */
		rc = set_cookie(chan, &chan_st->handler);
		if (rc) {
			TLOGI("failed (%d) to set_cookie on chan %d\n",
			       rc, chan);
			free(chan_st);
			close(chan);
			return;
		}
	}
}


/***************************************************************************/

/*
 *  uuid service port event handler
 */
static void uuid_handle_port(const uevent_t *ev)
{
	ipc_msg_t msg;
	iovec_t   iov;
	uuid_t peer_uuid;

	if (handle_port_errors(ev))
		return;

	if (ev->event & IPC_HANDLE_POLL_READY) {
		handle_t chan;

		/* incomming connection: accept it */
		int rc = accept(ev->handle, &peer_uuid);
		if (rc < 0) {
			TLOGI("failed (%d) to accept on port %d\n",
			       rc, ev->handle);
			return;
		}
		chan = (handle_t) rc;

		/* send interface uuid */
		iov.base = &peer_uuid;
		iov.len  = sizeof(peer_uuid);
		msg.num_iov = 1;
		msg.iov     = &iov;
		msg.num_handles = 0;
		msg.handles  = NULL;
		rc = send_msg(chan, &msg);
		if (rc < 0) {
			TLOGI("failed (%d) to send_msg for chan (%d)\n",
			      rc, chan);
		}

		/* and close channel */
		close(chan);
	}
}

/***************************************************************************/

/*
 *  security_test service port event handler
 */
static void security_test_handle_port(const uevent_t *ev)
{
	ipc_msg_t msg;
	iovec_t   iov;
	uuid_t peer_uuid;

	if (handle_port_errors(ev))
		return;

	if (ev->event & IPC_HANDLE_POLL_READY) {
		handle_t chan;

		/* incomming connection: accept it */
		int rc = accept(ev->handle, &peer_uuid);
		if (rc < 0) {
			TLOGI("### failed (%d) to accept on port %d\n",
			       rc, ev->handle);
			return;
		}
		chan = (handle_t) rc;

		// 1. alloc heap mem
		// 2. fill with 16khz
		// 3. return heap addr

		/*rc = get_msg(ev->handle, &inf);*/

		// alloc
		g_test_alloc = calloc(1, sizeof(struct security_test_mem));
		if (!g_test_alloc) {
			rc = ERR_NO_MEMORY;
			return;
		}

		g_test_alloc->vir_addr = (uint64_t)calloc(1, PAGE_4K_SIZE);
		if (!g_test_alloc->vir_addr) {
			TLOGI("### alloc failed for size 0x%x\n", PAGE_4K_SIZE);
			free(g_test_alloc);
			return ;
		}
		memset((void *)g_test_alloc->vir_addr, 0, PAGE_4K_SIZE);
		g_test_alloc->phy_addr = virt_to_phys((void *)g_test_alloc->vir_addr);

		TLOGI("### g_test_alloc->vir_addr = 0x%x, g_test_alloc->phy_addr= 0x%x\n",
				g_test_alloc->vir_addr, g_test_alloc->phy_addr);
		g_test_alloc->size = PAGE_4K_SIZE;

		// fill dma area with 16khz
		fill_dmaarea_with_16khz((u16 *)g_test_alloc->vir_addr,
								PAGE_4K_SIZE);
		TLOGI("### fill dmaarea end\n");

		/* send heap phy addr*/
		iov.base = &g_test_alloc->phy_addr;
		iov.len  = sizeof(g_test_alloc->phy_addr);
		msg.num_iov = 1;
		msg.iov     = &iov;
		msg.num_handles = 0;
		msg.handles  = NULL;
		rc = send_msg(chan, &msg);
		if (rc < 0) {
			TLOGI("### failed (%d) to send_msg for chan (%d)\n",
			      rc, chan);
		}

		/* and close channel */
		close(chan);
	}
}

static void security_test_check_handle_port(const uevent_t *ev)
{
	ipc_msg_t msg;
	iovec_t   iov;
	uuid_t peer_uuid;
	int cmp_flag = 0;	

	if (handle_port_errors(ev))
		return;

	if (ev->event & IPC_HANDLE_POLL_READY) {
		handle_t chan;

		/* incomming connection: accept it */
		int rc = accept(ev->handle, &peer_uuid);
		if (rc < 0) {
			TLOGI("### failed (%d) to accept on port %d\n",
			       rc, ev->handle);
			return;
		}
		chan = (handle_t) rc;

		// alloc
		g_refer_alloc = calloc(1, sizeof(struct security_test_mem));
		if (!g_refer_alloc) {
			rc = ERR_NO_MEMORY;
			return;
		}

		g_refer_alloc->vir_addr = (uint64_t)calloc(1, PAGE_4K_SIZE);
		if (!g_refer_alloc->vir_addr) {
			TLOGI("### alloc failed for size 0x%x\n", PAGE_4K_SIZE);
			free(g_refer_alloc);
			return ;
		}
		memset((void *)g_refer_alloc->vir_addr, 0, PAGE_4K_SIZE);

		TLOGI("###-check g_refer_alloc->vir_addr = 0x%x\n",
				g_refer_alloc->vir_addr);
		g_refer_alloc->size = PAGE_4K_SIZE;

		// fill dma area with 16khz
		fill_dmaarea_with_16khz((u16 *)g_refer_alloc->vir_addr,
								PAGE_4K_SIZE);
		TLOGI("### fill dmaarea end\n");

		int i;
		u16 *v_dma_area_test = (u16 *) g_test_alloc->vir_addr;
		u16 *v_dma_area_refer = (u16 *) g_refer_alloc->vir_addr;

		TLOGI("### print dmaarea\n");
		for (i = 0; i < PAGE_4K_SIZE/2; i++)
			fprintf(stderr, "%d: 0x%x/0x%x\n ", i, v_dma_area_test[i], v_dma_area_refer[i]);


		//memcmp - compare memory areas
		cmp_flag = memcmp((const u16 *)g_refer_alloc->vir_addr,
				(const u16 *)g_test_alloc->vir_addr, PAGE_4K_SIZE/2);

		TLOGI("### cmp_flag = %d\n", cmp_flag);

		free(g_refer_alloc->vir_addr);
		free(g_refer_alloc);

		/* send heap phy addr*/
		iov.base = &cmp_flag;
		iov.len  = sizeof(cmp_flag);
		msg.num_iov = 1;
		msg.iov     = &iov;
		msg.num_handles = 0;
		msg.handles  = NULL;
		rc = send_msg(chan, &msg);
		if (rc < 0) {
			TLOGI("### failed (%d) to send_msg for chan (%d)\n",
			      rc, chan);
		}

		/* and close channel */
		close(chan);
	}
}
/*
 *  security_test_free service port event handler
 */
static void security_test_free_handle_port(const uevent_t *ev)
{
	ipc_msg_t msg;
	iovec_t   iov;
	uuid_t peer_uuid;
	int free_flag = 0;

	if (handle_port_errors(ev))
		return;

	if (ev->event & IPC_HANDLE_POLL_READY) {
		handle_t chan;

		/* incomming connection: accept it */
		int rc = accept(ev->handle, &peer_uuid);
		if (rc < 0) {
			TLOGI("failed (%d) to accept on port %d\n",
			       rc, ev->handle);
			return;
		}
		chan = (handle_t) rc;

		// free
		free(g_test_alloc->vir_addr);
		free(g_test_alloc);
		free_flag = 1;

		/* send interface uuid */
		iov.base = &free_flag;
		iov.len  = sizeof(free_flag);
		msg.num_iov = 1;
		msg.iov     = &iov;
		msg.num_handles = 0;
		msg.handles  = NULL;
		rc = send_msg(chan, &msg);
		if (rc < 0) {
			TLOGI("failed (%d) to send_msg for chan (%d)\n",
			      rc, chan);
		}

		/* and close channel */
		close(chan);
	}
}

/***************************************************************************/

/*
 *  Dispatch event
 */
static void dispatch_event(const uevent_t *ev)
{
	assert(ev);

	if (ev->event == IPC_HANDLE_POLL_NONE) {
		/* not really an event, do nothing */
		TLOGI("got an empty event\n");
		return;
	}

	if (ev->handle == INVALID_IPC_HANDLE) {
		/* not a valid handle  */
		TLOGI("got an event (0x%x) with invalid handle (%d)",
		      ev->event, ev->handle);
		return;
	}

	/* check if we have handler */
	struct tipc_event_handler *handler = ev->cookie;
	if (handler && handler->proc) {
		/* invoke it */
		handler->proc(ev);
		return;
	}

	/* no handler? close it */
	TLOGI("no handler for event (0x%x) with handle %d\n",
	       ev->event, ev->handle);
	close(ev->handle);

	return;
}

/*
 *  Main entry point of service task
 */
int main(void)
{
	int rc;
	uevent_t event;

	/* Initialize service */
	rc = init_services();
	if (rc != NO_ERROR ) {
		TLOGI("Failed (%d) to init service", rc);
		kill_services();
		return -1;
	}

	/* handle events */
	while (!stopped) {
		event.handle = INVALID_IPC_HANDLE;
		event.event  = 0;
		event.cookie = NULL;
		rc = wait_any(&event, -1);
		if (rc < 0) {
			TLOGI("wait_any failed (%d)", rc);
			continue;
		}
		if (rc == NO_ERROR) { /* got an event */
			dispatch_event(&event);
		}
	}

	kill_services();
	return 0;
}

