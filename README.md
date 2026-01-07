# ssd1306
SSD1306 OLED display firmware

# Getting started
## Usage
Before any display operation the ssd1306 HAL must be initialized by calling the `ssd1306_device_startup()` function:
```
ssd1306_device_startup(
	S1306_SLAVE_ADDR,
	i2c_write,
	i2c_init);
```

This function implements the device abstraction required for the SSD1306 firmware. Three arguments are required, the first is the slave address which by default is 0x3C but in some cases is 0x3D, then there are two function pointers that must be implemented by the user:
* An I2C writer function
* An I2C peripheral init function


## Examples
### Basic print message
Start the SSD1306, clear the display and write a test message
```
ssd1306_device_startup(
	S1306_SLAVE_ADDR,
	i2c_write,
	i2c_init);

  ssd1306_display_clear();
  ssd1306_print_text("Hello world", 0);
```

## Adding support to different architectures
This firmware is ready to be used by different architectures. Any new implementation must be added in the `ssd_hw_iface.c` file.
This firmware has been tested in STM, LPC and ESP microcontrollers but it could be even ported to Linux device drivers.

Only two functions must be implemented in order to use this firmware.

### I2C write function

The `i2c_write_fn` function pointer prototype:
```
void i2c_write(uint32_t const slave_addr, uint8_t *data, uint32_t nbytes)
```

This I2C writer function must be blocking since interrupts are not supported for now and is not thread safe. 
The I2C writer function will get the slave's address, a buffer to the I2C frame to be send and the amount of bytes to be send.

### I2C peripheral init function
The `i2c_hw_init_fn` function pointer prototype:
```
void i2c_init(void)
```

This I2C init function will start and configure the I2C peripheral. The requirements are the ones detailed in the SSD1306 datasheet.
Also notice that there's no need to configure internal pull-ups (if available) since the SSD1306 boards include pull-up resistors for the SDA/SCL lines. When in doubt double check the hardware.