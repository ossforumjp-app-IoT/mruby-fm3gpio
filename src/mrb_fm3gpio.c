/*
** mrb_fm3gpio.c - FM3 GPIO
**
**
*/

#include "mruby.h"
#include "mruby/class.h"
#include "mruby/value.h"

#define INPUT 0
#define OUTPUT 1
#define LOW 0
#define HIGH 1

#define PFR_GPIO 0
#define PFR_INOUT 1

#define T_ADE 0
#define T_SPSR 1
#define NUM_GPIO 16
#define T_PFR 0
#define T_PCR 1
#define T_DDR 2
#define T_PDIR 3
#define T_PDOR 4

/* Register address table */
static const uint32_t registerTable_config[] = {
	0x40033500,		/* ADE */
	0x40033580		/* SPSR */
};

static const uint32_t registerTable_port[NUM_GPIO][5] = {
	/*	PFR				PCR				DDR				PDIR			PDOR		*/
	{	0x40033000,		0x40033100,		0x40033200,		0x40033300,		0x40033400	},	/*P0n*/
	{	0x40033004,		0x40033104,		0x40033204,		0x40033304,		0x40033404	},	/*P1n*/
	{	0x40033008,		0x40033108,		0x40033208,		0x40033308,		0x40033408	},	/*P2n*/
	{	0x4003300C,		0x4003310C,		0x4003320C,		0x4003330C,		0x4003340C	},	/*P3n*/
	{	0x40033010,		0x40033110,		0x40033210,		0x40033310,		0x40033410	},	/*P4n*/
	{	0x40033014,		0x40033114,		0x40033214,		0x40033314,		0x40033414	},	/*P5n*/
	{	0x40033018,		0x40033118,		0x40033218,		0x40033318,		0x40033418	},	/*P6n*/
	{	0x4003301C,		0x4003311C,		0x4003321C,		0x4003331C,		0x4003341C	},	/*P7n*/
	{	0x40033020,		0x40033120,		0x40033220,		0x40033320,		0x40033420	},	/*P8n*/
	{	0x40033024,		0x40033124,		0x40033224,		0x40033324,		0x40033424	},	/*P9n*/
	{	0x40033028,		0x40033128,		0x40033228,		0x40033328,		0x40033428	},	/*PAn*/
	{	0x4003302C,		0x4003312C,		0x4003322C,		0x4003332C,		0x4003342C	},	/*PBn*/
	{	0x40033030,		0x40033130,		0x40033230,		0x40033330,		0x40033430	},	/*PCn*/
	{	0x40033034,		0x40033134,		0x40033234,		0x40033334,		0x40033434	},	/*PDn*/
	{	0x40033038,		0x40033138,		0x40033238,		0x40033338,		0x40033438	},	/*PEn*/
	{	0x4003303C,		0x4003313C,		0x4003323C,		0x4003333C,		0x4003343C	},	/*PFn*/
};

void setADE(void);
void setPFR(uint8_t pin_hi, uint8_t pin_lo, uint8_t mode);
void setDDR(uint8_t pin_hi, uint8_t pin_lo, uint8_t mode);
void setPDOR(uint8_t pin_hi, uint8_t pin_lo, uint8_t hilo);
uint8_t readPFR(uint8_t pin_hi, uint8_t pin_lo);
uint8_t readDDR(uint8_t pin_hi, uint8_t pin_lo);
uint8_t readPDIR(uint8_t pin_hi, uint8_t pin_lo);
uint8_t readPDOR(uint8_t pin_hi, uint8_t pin_lo);

mrb_value mrb_FM3_digitalPinMode(mrb_state *mrb, mrb_value self);
mrb_value mrb_FM3_digitalWrite(mrb_state *mrb, mrb_value self);
mrb_value mrb_FM3_digitalRead(mrb_state *mrb, mrb_value self);
void mrb_mruby_fm3gpio_gem_init(mrb_state *mrb);
void mrb_mruby_fm3gpio_gem_final(mrb_state *mrb);

void setADE(void)
{
	volatile uint32_t *add;
	
	add = (uint32_t *)(registerTable_config[T_ADE]);
	*add = 0;
}

void setPFR(uint8_t pin_hi, uint8_t pin_lo, uint8_t mode)
{
	uint32_t bit = (0x00000001 << pin_lo);
	volatile uint32_t *add;

	if(pin_hi < NUM_GPIO)
	{
		add = (uint32_t *)(registerTable_port[pin_hi][T_PFR]);		/*PFRn*/
		if(mode)	/* INOUT:1 */
			*add |= bit;
		else		/* GPIO:0 */
			*add &= (~bit & 0x0000FFFF);
	}
	else
	{
		/* parameter error */
	}
}

void setDDR(uint8_t pin_hi, uint8_t pin_lo, uint8_t mode)
{
	uint32_t bit = (0x00000001 << pin_lo);
	volatile uint32_t *add;
	
	if(pin_hi < NUM_GPIO)
	{
		add = (uint32_t *)(registerTable_port[pin_hi][T_DDR]);		/*DDRn*/
		if(mode)	/* output:1 */
			*add |= bit;
		else		/* input:0 */
			*add &= (~bit & 0x0000FFFF);
	}
	else
	{
		/* parameter error */
	}
}

void setPDOR(uint8_t pin_hi, uint8_t pin_lo, uint8_t hilo)
{
	uint32_t bit = (0x00000001 << pin_lo);
	volatile uint32_t *add;
	
	if(pin_hi < NUM_GPIO)
	{
		add = (uint32_t *)(registerTable_port[pin_hi][T_PDOR]);		/*PDORn*/
		if(hilo)	/* Hi:1 */
			*add |= bit;
		else		/* Lo:0 */
			*add &= (~bit & 0x0000FFFF);
	}
	else
	{
		/* parameter error */
	}
}

uint8_t readPFR(uint8_t pin_hi, uint8_t pin_lo)
{
	uint32_t data;
	uint8_t ret;
	volatile uint32_t *add;
	
	if(pin_hi < NUM_GPIO)
	{
		add = (uint32_t *)(registerTable_port[pin_hi][T_PFR]);		/*PFRn*/
		data = *add;
		ret = (uint8_t)((data >> pin_lo) & 0x00000001);
	}
	else
	{
		/* parameter error */
		ret = 2;
	}
	
	return ret;
}

uint8_t readDDR(uint8_t pin_hi, uint8_t pin_lo)
{
	uint32_t data;
	uint8_t ret;
	volatile uint32_t *add;
	
	if(pin_hi < NUM_GPIO)
	{
		add = (uint32_t *)(registerTable_port[pin_hi][T_DDR]);		/*DDRn*/
		data = *add;
		ret = (uint8_t)((data >> pin_lo) & 0x00000001);
	}
	else
	{
		/* parameter error */
		ret = 2;
	}
	
	return ret;
}

uint8_t readPDIR(uint8_t pin_hi, uint8_t pin_lo)
{
	uint32_t data;
	uint8_t ret;
	volatile uint32_t *add;
	
	if(pin_hi < NUM_GPIO)
	{
		add = (uint32_t *)(registerTable_port[pin_hi][T_PDIR]);		/*PDIRn*/
		data = *add;
		ret = (uint8_t)((data >> pin_lo) & 0x00000001);
	}
	else
	{
		/* parameter error */
		ret = 2;
	}
	
	return ret;
}

uint8_t readPDOR(uint8_t pin_hi, uint8_t pin_lo)
{
	uint32_t data;
	uint8_t ret;
	volatile uint32_t *add;
	
	if(pin_hi < NUM_GPIO)
	{
		add = (uint32_t *)(registerTable_port[pin_hi][T_PDOR]);		/*PDORn*/
		data = *add;
		ret = (uint8_t)((data >> pin_lo) & 0x00000001);
	}
	else
	{
		/* parameter error */
		ret = 2;
	}
	
	return ret;
}

/* GPIO.initialize */
mrb_value mrb_FM3_digitalInitialize(mrb_state *mrb, mrb_value self)
{
	return mrb_nil_value();
}

/* GPIO.mode(pin, inout) */
mrb_value mrb_FM3_digitalPinMode(mrb_state *mrb, mrb_value self)
{
	mrb_int pin;
	mrb_int inout;
	uint8_t pin_hi;
	uint8_t pin_lo;
	
	mrb_get_args(mrb, "ii", &pin, &inout);
	pin_hi = ((uint8_t)pin & 0xF0) >> 4;
	pin_lo = ((uint8_t)pin & 0x0F);
	
	/* Set PFR and DDR */
	setPFR(pin_hi, pin_lo, PFR_GPIO);
	setDDR(pin_hi, pin_lo, (uint8_t)inout);

	return mrb_nil_value();
}

/* GPIO.write(pin, hilo) */
mrb_value mrb_FM3_digitalWrite(mrb_state *mrb, mrb_value self)
{
	mrb_int pin;
	mrb_int hilo;
	uint8_t pin_hi;
	uint8_t pin_lo;
	
	mrb_get_args(mrb, "ii", &pin, &hilo);
	pin_hi = ((uint8_t)pin & 0xF0) >> 4;
	pin_lo = ((uint8_t)pin & 0x0F);
	
	/* Set PDOR */
	setPDOR(pin_hi, pin_lo, (uint8_t)hilo);
	
	return mrb_nil_value();
}

/* GPIO.read(pin) */
mrb_value mrb_FM3_digitalRead(mrb_state *mrb, mrb_value self)
{
	mrb_int pin;
	mrb_int hilo;
	uint8_t pin_hi;
	uint8_t pin_lo;
	uint8_t inout;
	
	mrb_get_args(mrb, "i", &pin);
	pin_hi = ((uint8_t)pin & 0xF0) >> 4;
	pin_lo = ((uint8_t)pin & 0x0F);
	
	inout = readDDR(pin_hi, pin_lo);
	/* Read PDIR or PDOR */
	if(inout == OUTPUT)	/* output:1 */
		hilo = (mrb_int)readPDOR(pin_hi, pin_lo);
	else if(inout == INPUT) /* input:0 */
		hilo = (mrb_int)readPDIR(pin_hi, pin_lo);
	else /* error */
		hilo = (mrb_int)inout;
	
	return mrb_fixnum_value(hilo);
}

void mrb_mruby_fm3gpio_gem_init(mrb_state *mrb)
{
	struct RClass *gpio;
	
	/* define class */
	gpio = mrb_define_class(mrb, "GPIO", mrb->object_class);
	MRB_SET_INSTANCE_TT(gpio, MRB_TT_OBJECT);						/* MRB_TT_PROC? */
	
	/* define method */
	mrb_define_method(mrb, gpio, "initialize", mrb_FM3_digitalInitialize, MRB_ARGS_NONE());
	mrb_define_method(mrb, gpio, "mode", mrb_FM3_digitalPinMode, MRB_ARGS_REQ(2));
	mrb_define_method(mrb, gpio, "write", mrb_FM3_digitalWrite, MRB_ARGS_REQ(2));
	mrb_define_method(mrb, gpio, "read", mrb_FM3_digitalRead, MRB_ARGS_REQ(1));
	
	/* define constracts */
	mrb_define_const(mrb, gpio, "INPUT", mrb_fixnum_value(INPUT));
	mrb_define_const(mrb, gpio, "OUTPUT", mrb_fixnum_value(OUTPUT));
	mrb_define_const(mrb, gpio, "LOW", mrb_fixnum_value(LOW));
	mrb_define_const(mrb, gpio, "HIGH", mrb_fixnum_value(HIGH));
}

void mrb_mruby_fm3gpio_gem_final(mrb_state *mrb)
{
	
}
