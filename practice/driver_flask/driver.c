#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/smp.h>
#include <linux/kthread.h>
#include <linux/semaphore.h>
#include <asm/io.h>

MODULE_LICENSE("GPL");

#define PERIPHERAL_BASE 0x3F000000UL
#define GPIO_BASE (PERIPHERAL_BASE + 0x200000)

void* gpio_ctr = NULL;

void set_gpio_output(void* gpio_ctr, int gpio_nr){
    int reg_id = gpio_nr / 10;
    int pos = gpio_nr % 10;

    uint32_t* fsel_reg = (uint32_t*)(gpio_ctr + 0x4 * reg_id);
    uint32_t fsel_val = *fsel_reg;
    uint32_t mask = 0x7 << (pos*3);
    fsel_val = fsel_val & ~mask;

    uint32_t gpio_output_select = 0x1 << (pos*3);
    fsel_val = fsel_val | gpio_output_select;

    *fsel_reg = fsel_val;
}

void set_gpio_input(void* gpio_ctr, int gpio_nr){
    int reg_id = gpio_nr / 10;
    int pos = gpio_nr % 10;

    uint32_t* fsel_reg = (uint32_t*)(gpio_ctr + 0x4 * reg_id);

    uint32_t fsel_val = *fsel_reg;

    uint32_t mask = 0x7 << (pos* 3);
    fsel_val = fsel_val & ~mask;
    *fsel_reg = fsel_val;
}

void set_gpio_output_value(void* gpio_ctr, int gpio_nr, int value){
    int reg_id = gpio_nr / 32;
    int pos = gpio_nr % 32;

    if(value){
        #define GPIO_SET_OFFSET 0x1c
        uint32_t* output_set = (uint32_t*)(gpio_ctr + GPIO_SET_OFFSET + 0x4 * reg_id);
        *output_set = 0x1 << pos;
    }
    else{
        #define GPIO_CLR_OFFSET 0x28
        uint32_t* output_clr = (uint32_t*)(gpiod_ctr + GPIO_CLR_OFFSET + 0x4 * reg_id);
        *output_clr = 0x1 << pos;
    }
}

void get_gpio_input_value(void* gpio_ctr, int gpio_nr, int *value){
    int reg_id = gpio_nr /32;
    int pos= gpio_nr % 32;

    #define GPIO_LEV_OFFSET 0x34
    uint32_t* level_reg = (uint32_t*)(gpio_ctr + GPIO_LEV_OFFSET + 0x4 * reg_id);
    uint32_t level = *level_reg & (0x1 << pos);
    
    *value = level ? 1 : 0;
}

void set_gpio_pullup(void* gpio_ctr, int gpio_nr){
    int reg_id = gpio_nr / 32;
    int pos = gpio_nr % 32;

    #define GPIO_PUT_OFFSET 0x94
    #define GPIO_PUDCLK_OFFSET 0x98
    uint32_t* pud_reg = (uint32_t*)(gpio_ctr + GPIO_PUD_OFFSET);
    uint32_t* pudclk_reg = (uint32_t*)(gpio_ctr + GPIO_PUDCLK_OFFSET + 0x4 * reg_id);

    #define GPIO_PUD_PULLUP 0x2
    *pud_reg = GPIO_PUD_PULLUP;
    udelay(1);
    *pudclk_reg = (0x1 << pos);
    udelay(1);
    *pud_reg = 0;
    *pudclk_reg = 0;
}

#define MAJOR_NUM 0
#define DEVICE_NAME "rpikey"
#define CLASS_NAME "rpikey_class"

static int majorNumber;
static struct class* cRpiKeyClass = NULL;
static struct device* cRpiKeyDevice = NULL;

struct file_operations Fops = {
    .unlocked_ioctl = device_ioctl,
    .open = device_open, 
    .release = device_release,
};

static int device_open(struct inode* inode, struct file* file){
    printk(KERN_INFO "rpi_key device_open(%p)\n", file);
    return 0;
}

static int device_relase(struct inode* inode, struct file* file){
    printk(KERN_INFO "rpi_key device_relase(%p)\n", file);
    return 0;
}

long device_ioctl(struct file* file, unsigned int ioctl_num, unsigned long ioctl_param){
    if(ioctl_num == 100){
        uint32_t param_value[2];
        get_gpio_input_value(gpio_ctr, 20, &param_value[0]);
        set_gpio_input_value(gpio_ctr, 21, &param_value[1]);
        copy_to_user((void*)ioctl_param, (void*)param_value, sizeof(uint32_t)*2);
    }
    if(ioctl_num == 101){
        uint32_t param_value[3];
        int gpio13;
        int gpio19;
        int gpio26;

        copy_from_user((void*)param_value, (void*)ioctl_param, sizeof(uint32_t)*3);
        gpio13 = (int)param_value[0];
        gpio19 = (int)param_value[1];
        gpio26 = (int)param_value[2];

        set_gpio_output_value(gpio_ctr, 13, gpio13);
        set_gpio_output_value(gpio_ctr, 19, gpio19);
        set_gpio_output_value(gpio_ctr, 26, gpio26);
    }
    return 0;
}

static int __init rpi_key_init(void){
    majorNumber = register_chrdev(MAJOR_NUM, DEVICE_NAME, &Fops);
    cRpiKeyClass = class_create(THIS_MODULE, CLASS_NAME);
    cRpiKeyDevice = device_create(cRpiKeyClass, NULL, MKDEV(majorNumber, 0 ), NULL, DEVICE_NAME);

    gpio_ctr = ioremap(GPIO_BASE, 0x1000);
    set_gpio_output(gpio_ctr, 13);
    set_gpio_output(gpio_ctr, 19);
    set_gpio_output(gpio_ctr, 26);
    set_gpio_input(gpio_ctr, 20);
    set_gpio_input(gpio_ctr, 21);
    set_gpio_pullup(gpio_ctr, 20);
    set_gpio_pullup(gpio_ctr, 21);

    return 0;
}

static void __exit rpi_key_exit(void){
    iounmap(gpio_ctr);
    device_destroy(cRpiKeyClass, MKDEV(majorNumber, 0));
    class_unregister(cRpiKeyClass);
    class_destroy(cRpiKeyClass);
    unregister_chrdev(majorNumber, DEVICE_NAME);
}

module_init(rpi_key_init);
module_exit(rpi_key_init);

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/ioctl.h>

int main(int argc, char* argv[]){
    int opt, gpio13, gpio19, gpio26, read;
    gpio13 = 0;
    gpio19 = 0;
    gpio26 = 0;
    read = 0;

    while((opt = getopt(argc, argv "r:g:b:k")) != -1){
        switch(opt){
            case 'r':
                gpio26 = atoi(optarg);
                break;
            case 'g':
                gpio19 = atoi(optarg);
                break;
            case 'b':
                gpio13 = atoi(optarg);
                break;
            case 'k':
                read = 1;
                break;
            default:
                return -1;
        }
    }

    int fd, res;
    fd = open("/dev/rpikey", O_RDWR);
    if(fd<0){
        printf("Can't open device\n");
        return -1;
    }
    if(read){
        uint32_t values[2];
        res = ioctl(fd, 100, values);
        printf("value 20 : %d 21:%d\n", values[0], values[1]);
    }
    else{
        uint32_t values[3];
        values[0] = gpio13;
        values[1] = gpio19;
        values[2] = gpio26;
        res = ioctl(fd, 101, values);
    }
    close(fd);
    return 0;
}