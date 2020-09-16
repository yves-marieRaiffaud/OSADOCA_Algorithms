//! @file gpio_template.cpp
//! @author Gustav Johansson <gusjohan@kth.se>
//! @brief A template for using RTDM GPIO on RPi3 with Xenomai
//! Cobalt kernel. Don’t forget to load the GPIO device
//! driver module with "modprobe xeno-gpio-bcm2835"
//! before running.
//! The device driver in mainline linux tree is very different from the rpi
//! linux tree. A problem with this is that the gpio pins that appears with the
//! gpio module does not have the same numbers as on a regular RPi. The pins can
//! be seen in /dev/rtdm/pinctrl-bcm2835/. For me started the pins at 970 and
//! ended at 1023. The pins work however without any problems, you only need to
//! know which pin corresponds to a regular pin which is easy. 970+16=986
//! corresponds to gpio pin 16 (bcm).
#include <stdio.h>
#include <alchemy/task.h>
#include <rtdm/gpio.h>
//! @brief Initialize a GPIO pin either as input/inputIRQ or output.
//! @param path Path to the pin.
//! Example /dev/rtdm/pinctrl-bcm2835/gpio986
//! @param openArg Arguments for open. O_RDONLY,
//! O_RDONLY|O_NONBLOCK
//! @param ioctlArg Arguments for ioctl
//! -input: GPIO_RTIOC_DIR_IN
//! -output: GPIO_RTIOC_DIR_OUT
//! -input IRQ: GPIO_RTIOC_IRQEN
//! @param edge Specify edges or start value
//! -input: NULL
//! -output: Start value
//! -input IRQ: GPIO_TRIGGER_EDGE_RISING or
//! GPIO_TRIGGER_EDGE_FALLING
int pinInit(const char* path, int openArg, int ioctlArg, int* edge)
{
int retval;
int pin = open(path, openArg);
if(pin < 0){
rt_printf("Error:%d couldn’t open file. Check path or load module\n", pin);
}
retval = ioctl(pin, ioctlArg, edge);
if(retval < 0){
rt_printf("Error:%d couldn’t request ioctl. Run as sudo\n", retval);
68
}
return pin;
}
// read from GPIO pin
int pinGet(int pin)
{
int retval;
int val=-1;
retval = read(pin, &val, sizeof(val));
if(retval < 0){
rt_printf("Error:%d couldn’t read pin.\n", retval);
}
return val;
}
// write to GPIO pin
void pinSet(int pin, int val)
{
int retval;
retval = write(pin, &val, sizeof(val));
if(retval < 0){
rt_printf("Error:%d couldn’t write pin.\n", retval);
}
}
// real-time task
RT_TASK task;
void rtTask(void* arg)
{
int edges = GPIO_TRIGGER_EDGE_RISING | GPIO_TRIGGER_EDGE_FALLING;
int writeValue = 0;
int retval;
//! @brief open and read value from GPIO21 (BCM).
rt_printf("Input\n");
int pinInput = pinInit("/dev/rtdm/pinctrl-bcm2835/gpio991",
O_RDONLY|O_NONBLOCK,
GPIO_RTIOC_DIR_IN,
NULL);
rt_printf("read value:%d\n", pinGet(pinInput));
//! @brief open and read value from GPIO20 (BCM) using interrupt.
//! input change detect raising and falling edge.
rt_printf("INPUT IRQ\n");
int pinInputIRQ = pinInit("/dev/rtdm/pinctrl-bcm2835/gpio990",
O_RDONLY,
GPIO_RTIOC_IRQEN,
&edges);
rt_printf("read value:%d\n", pinGet(pinInputIRQ));
//! @brief open and write value to GPIO16 (BCM).
rt_printf("OUTPUT\n");
int pinOutput = pinInit("/dev/rtdm/pinctrl-bcm2835/gpio986",
O_WRONLY,
GPIO_RTIOC_DIR_OUT,
69
&writeValue);
for(int i=0; i<5; ++i){
pinSet(pinOutput, writeValue = !writeValue);
retval = rt_task_sleep(500000000);
if(retval < 0){
rt_printf("Error:%d couldn’t put task to sleep", retval);
}
}
}
int main(int argv, char* argc[])
{
int retval;
// create task
retval = rt_task_create(&task, "rtTask", 0, 99, T_JOINABLE);
if(retval < 0){
rt_printf("Error:%d couldn’t create task.\n", -retval);
}
// start task
retval = rt_task_start(&task, rtTask, NULL);
if(retval < 0){
rt_printf("Error:%d couldn’t start task.\n", -retval);
}
// join task
retval = rt_task_join(&task);
if(retval < 0){
rt_printf("Error:%d couldn’t join task.\n", -retval);
}
return retval;
}