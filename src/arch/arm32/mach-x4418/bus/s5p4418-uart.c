/*
 * bus/s5p4418-uart.c
 *
 * Copyright(c) 2007-2015 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <xboot.h>
#include <s5p4418-uart.h>

static bool_t s5p4418_uart_setup(struct uart_t * uart, enum baud_rate_t baud, enum data_bits_t data, enum parity_bits_t parity, enum stop_bits_t stop)
{
	struct resource_t * res = (struct resource_t *)uart->priv;
	struct s5p4418_uart_data_t * dat = (struct s5p4418_uart_data_t *)res->data;
	u32_t ibaud, divider, fraction;
	u32_t temp, remainder;
	u8_t data_bit_reg, parity_reg, stop_bit_reg;
	u64_t uclk;

	switch(baud)
	{
	case B50:
		ibaud = 50;
		break;
	case B75:
		ibaud = 75;
		break;
	case B110:
		ibaud = 110;
		break;
	case B134:
		ibaud = 134;
		break;
	case B200:
		ibaud = 200;
		break;
	case B300:
		ibaud = 300;
		break;
	case B600:
		ibaud = 600;
		break;
	case B1200:
		ibaud = 1200;
		break;
	case B1800:
		ibaud = 1800;
		break;
	case B2400:
		ibaud = 2400;
		break;
	case B4800:
		ibaud = 4800;
		break;
	case B9600:
		ibaud = 9600;
		break;
	case B19200:
		ibaud = 19200;
		break;
	case B38400:
		ibaud = 38400;
		break;
	case B57600:
		ibaud = 57600;
		break;
	case B76800:
		ibaud = 76800;
		break;
	case B115200:
		ibaud = 115200;
		break;
	case B230400:
		ibaud = 230400;
		break;
	case B380400:
		ibaud = 380400;
		break;
	case B460800:
		ibaud = 460800;
		break;
	case B921600:
		ibaud = 921600;
		break;
	default:
		return FALSE;
	}

	switch(data)
	{
	case DATA_BITS_5:
		data_bit_reg = 0x0;
		break;
	case DATA_BITS_6:
		data_bit_reg = 0x1;
		break;
	case DATA_BITS_7:
		data_bit_reg = 0x2;
		break;
	case DATA_BITS_8:
		data_bit_reg = 0x3;
		break;
	default:
		return FALSE;
	}

	switch(parity)
	{
	case PARITY_NONE:
		parity_reg = 0x0;
		break;
	case PARITY_EVEN:
		parity_reg = 0x2;
		break;
	case PARITY_ODD:
		parity_reg = 0x1;
		break;
	default:
		return FALSE;
	}

	switch(stop)
	{
	case STOP_BITS_1:
		stop_bit_reg = 0;		break;
	case STOP_BITS_1_5:
		return -1;
	case STOP_BITS_2:
		stop_bit_reg = 1;		break;
	default:
		return -1;
	}

	switch(res->id)
	{
	case 0:
		uclk = clk_get_rate("GATE-UART0");
		break;
	case 1:
		uclk = clk_get_rate("GATE-UART1");
		break;
	case 2:
		uclk = clk_get_rate("GATE-UART2");
		break;
	case 3:
		uclk = clk_get_rate("GATE-UART3");
		break;
	default:
		return FALSE;
	}

	/*
	 * IBRD = UART_CLK / (16 * BAUD_RATE)
	 * FBRD = ROUND((64 * MOD(UART_CLK, (16 * BAUD_RATE))) / (16 * BAUD_RATE))
	 */
	temp = 16 * ibaud;
	divider = (u32_t)(uclk / temp);
	remainder = (u32_t)(uclk % temp);
	temp = (8 * remainder) / ibaud;
	fraction = (temp >> 1) + (temp & 1);

	write32(phys_to_virt(dat->regbase + UART_IBRD), divider);
	write32(phys_to_virt(dat->regbase + UART_FBRD), fraction);
	write32(phys_to_virt(dat->regbase + UART_LCRH), (1 << 4) | (data_bit_reg<<5 | stop_bit_reg<<3 | parity_reg<<1));

	return TRUE;
}

static void s5p4418_uart_init(struct uart_t * uart)
{
	struct resource_t * res = (struct resource_t *)uart->priv;
	struct s5p4418_uart_data_t * dat = (struct s5p4418_uart_data_t *)res->data;

	switch(res->id)
	{
	case 0:
		clk_enable("GATE-UART0");
		gpio_set_cfg(S5P4418_GPIOD(14), 0x1);
		gpio_set_cfg(S5P4418_GPIOD(18), 0x1);
		gpio_set_direction(S5P4418_GPIOD(14), GPIO_DIRECTION_INPUT);
		gpio_set_direction(S5P4418_GPIOD(18), GPIO_DIRECTION_OUTPUT);
		break;

	case 1:
		clk_enable("GATE-UART1");
		gpio_set_cfg(S5P4418_GPIOD(15), 0x1);
		gpio_set_cfg(S5P4418_GPIOD(19), 0x1);
		gpio_set_direction(S5P4418_GPIOD(15), GPIO_DIRECTION_INPUT);
		gpio_set_direction(S5P4418_GPIOD(19), GPIO_DIRECTION_OUTPUT);
		break;

	case 2:
		clk_enable("GATE-UART2");
		gpio_set_cfg(S5P4418_GPIOD(16), 0x1);
		gpio_set_cfg(S5P4418_GPIOD(20), 0x1);
		gpio_set_direction(S5P4418_GPIOD(16), GPIO_DIRECTION_INPUT);
		gpio_set_direction(S5P4418_GPIOD(20), GPIO_DIRECTION_OUTPUT);
		break;

	case 3:
		clk_enable("GATE-UART3");
		gpio_set_cfg(S5P4418_GPIOD(17), 0x1);
		gpio_set_cfg(S5P4418_GPIOD(21), 0x1);
		gpio_set_direction(S5P4418_GPIOD(17), GPIO_DIRECTION_INPUT);
		gpio_set_direction(S5P4418_GPIOD(21), GPIO_DIRECTION_OUTPUT);
		break;

	default:
		break;
	}

	/* Disable everything */
	write32(phys_to_virt(dat->regbase + UART_CR), 0x0);

	/* Configure uart */
	s5p4418_uart_setup(uart, dat->baud, dat->data, dat->parity, dat->stop);

	/* Enable uart */
	write32(phys_to_virt(dat->regbase + UART_CR), (1 << 0) | (1 << 8) | (1 << 9));
}

static void s5p4418_uart_exit(struct uart_t * uart)
{
	struct resource_t * res = (struct resource_t *)uart->priv;

	switch(res->id)
	{
	case 0:
		clk_disable("GATE-UART0");
		break;
	case 1:
		clk_disable("GATE-UART1");
		break;
	case 2:
		clk_disable("GATE-UART2");
		break;
	case 3:
		clk_disable("GATE-UART3");
		break;
	default:
		break;
	}
}

static ssize_t s5p4418_uart_read(struct uart_t * uart, u8_t * buf, size_t count)
{
	struct resource_t * res = (struct resource_t *)uart->priv;
	struct s5p4418_uart_data_t * dat = (struct s5p4418_uart_data_t *)res->data;
	ssize_t i;

	for(i = 0; i < count; i++)
	{
		if( !(read8(phys_to_virt(dat->regbase + UART_FR)) & UART_FR_RXFE) )
			buf[i] = read8(phys_to_virt(dat->regbase + UART_DATA));
		else
			break;
	}

	return i;
}

static ssize_t s5p4418_uart_write(struct uart_t * uart, const u8_t * buf, size_t count)
{
	struct resource_t * res = (struct resource_t *)uart->priv;
	struct s5p4418_uart_data_t * dat = (struct s5p4418_uart_data_t *)res->data;
	ssize_t i;

	for(i = 0; i < count; i++)
	{
		while( (read8(phys_to_virt(dat->regbase + UART_FR)) & UART_FR_TXFF) );
		write8(phys_to_virt(dat->regbase + UART_DATA), buf[i]);
	}

	return i;
}

static bool_t s5p4418_register_bus_uart(struct resource_t * res)
{
	struct uart_t * uart;
	char name[64];

	uart = malloc(sizeof(struct uart_t));
	if(!uart)
		return FALSE;

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	uart->name = strdup(name);
	uart->init = s5p4418_uart_init;
	uart->exit = s5p4418_uart_exit;
	uart->read = s5p4418_uart_read;
	uart->write = s5p4418_uart_write;
	uart->setup = s5p4418_uart_setup;
	uart->priv = res;

	if(register_bus_uart(uart))
		return TRUE;

	free(uart->name);
	free(uart);
	return FALSE;
}

static bool_t s5p4418_unregister_bus_uart(struct resource_t * res)
{
	struct uart_t * uart;
	char name[64];

	snprintf(name, sizeof(name), "%s.%d", res->name, res->id);

	uart = search_bus_uart(name);
	if(!uart)
		return FALSE;

	if(!unregister_bus_uart(uart))
		return FALSE;

	free(uart->name);
	free(uart);
	return TRUE;
}

static __init void s5p4418_bus_uart_init(void)
{
	resource_for_each_with_name("s5p4418-uart", s5p4418_register_bus_uart);
}

static __exit void s5p4418_bus_uart_exit(void)
{
	resource_for_each_with_name("s5p4418-uart", s5p4418_unregister_bus_uart);
}

bus_initcall(s5p4418_bus_uart_init);
bus_exitcall(s5p4418_bus_uart_exit);