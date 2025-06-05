#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "../include/config.h"

#include "I2C.h"

// Inicializa o I2C utiliza pelo display OLED
void i2c_bitdoglab_init(){
    gpio_pull_up(     BITDOGLAB_I2C_BUS_GPIO_SCL);
    gpio_pull_up(     BITDOGLAB_I2C_BUS_GPIO_SDA);
    gpio_set_function(BITDOGLAB_I2C_BUS_GPIO_SCL, GPIO_FUNC_I2C);
    gpio_set_function(BITDOGLAB_I2C_BUS_GPIO_SDA, GPIO_FUNC_I2C);
    i2c_init(         BITDOGLAB_I2C_BUS,          BITDOGLAB_I2C_BUS_BAUDRATE);
}
