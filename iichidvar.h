/*-
 * Copyright (c) 2018-2019 Marc Priggemeyer <marc.priggemeyer@gmail.com>
 * Copyright (c) 2019 Vladimir Kondratyev <wulf@FreeBSD.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _IICHIDVAR_H_
#define _IICHIDVAR_H_
/*
#define	I2C_HID_REPORT_TYPE_INPUT	0x1
#define	I2C_HID_REPORT_TYPE_OUTPUT	0x2
#define	I2C_HID_REPORT_TYPE_FEATURE	0x3
*/
/* 7.2 */
enum {
        I2C_HID_CMD_DESCR       = 0x0,
        I2C_HID_CMD_RESET       = 0x1,
        I2C_HID_CMD_GET_REPORT  = 0x2,
        I2C_HID_CMD_SET_REPORT  = 0x3,
        I2C_HID_CMD_GET_IDLE    = 0x4,
        I2C_HID_CMD_SET_IDLE    = 0x5,
        I2C_HID_CMD_GET_PROTO   = 0x6,
        I2C_HID_CMD_SET_PROTO   = 0x7,
        I2C_HID_CMD_SET_POWER   = 0x8,

        /* pseudo commands */
        I2C_HID_REPORT_DESCR    = 0x100,
};

/*
 * Since interrupt resource acquisition is not always possible (in case of GPIO
 * interrupts) iichid now supports a sampling_mode.
 * Set dev.<name>.<unit>.sampling_rate to a value greater then 0 to activate
 * sampling. A value of 0 is possible but will not reset the callout and,
 * thereby, disable further report requests. Do not set the sampling_rate value
 * too high as it may result in periodical lags of cursor motion.
 */
#define	IICHID_DEFAULT_SAMPLING_RATE	60

/* 5.1.1 - HID Descriptor Format */
struct i2c_hid_desc {
	uint16_t wHIDDescLength;
	uint16_t bcdVersion;
	uint16_t wReportDescLength;
	uint16_t wReportDescRegister;
	uint16_t wInputRegister;
	uint16_t wMaxInputLength;
	uint16_t wOutputRegister;
	uint16_t wMaxOutputLength;
	uint16_t wCommandRegister;
	uint16_t wDataRegister;
	uint16_t wVendorID;
	uint16_t wProductID;
	uint16_t wVersionID;
	uint32_t reserved;
} __packed;

struct iichid_softc {
	device_t		dev;
	device_t		child;
	struct sx		lock;

	bool			probe_done;
	int			probe_result;

	struct iichid_hw	hw;
	uint16_t		config_reg;
	struct i2c_hid_desc	desc;

	iichid_intr_t		*intr_handler;
	void			*intr_context;
	struct mtx		*intr_mtx;

	uint8_t			*rep_desc;
	uint8_t			*ibuf;
	int			isize;
	uint8_t			iid;

	int			irq_rid;
	struct resource		*irq_res;
	void			*irq_cookie;

	int			sampling_rate;
	struct callout		periodic_callout;
	bool			callout_setup;

	struct taskqueue	*taskqueue;
	struct task		event_task;
	struct task		power_task;

	/* XXX: Need barriers or atomic type? */
	volatile bool		open;
	bool			suspend;
	bool			power_on;
};

#endif	/* _IICHIDVAR_H_ */