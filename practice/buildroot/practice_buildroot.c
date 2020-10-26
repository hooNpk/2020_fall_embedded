#define PERIPHERAL_BASE 0x3F000000UL
#define GPIO_BASE (PERIPHERAL_BASE + 0x200000)

void set_gpio_input(void* gpio_ctr, int gpio_nr){
    int reg_id = gpio_nr/10;
    int pos = gpio_nr % 10;

    uint32_t* fsel_reg = (uint32_t *)(gpio_ctr + 0x4 * reg_id);
    uint32_t fsel_val = *fsel_reg;
    uint32_t mask = 0x7 << (pos*3);
    fsel_val = fsel_val & ~mask;

    *fsel_reg = fsel_val;
}

void set_gpio_pullup(void* gpio_ctr, int gpio_nr){
    int reg_id = gpio_nr / 32;
    int pos = gpio_nr % 32;

    #define GIP_PUD_OFFSET 0x94
    #define GPIO_PUDCLK_OFFSET 0x98
    uint32_t* pud_reg = (uint32_t*) (gpio_ctr + GPIO_PUD_OFFSET);
    uint32_t* pudclk_reg = (uint32_t*)(gpio_ctr + GPIO_PUDCLK_OFFSET + 0x4 * reg_id);

    #define GPIO_PUD_PULLUP 0x2
    *pud_reg = GPIO_PUD_PULLUP;
    usleep(1);
    *pudclk_reg = (0x1 << pos);
    usleep(1);
    *pud_reg = 0;
    *pudclk_reg = 0;
}

void get_gpio_input_value(void* gpio_ctr, int gpio_nr, int* value){
    int reg_id = gpio_nr / 32;
    int pos = gpio_nr % 32;

    #define GPIO_LEV_OFFSET 0x34
    uint32_t* level_reg = (uint32_t*) (gpio_ctr + GPIO_LEV_OFFSET + 0x4 * reg_id);
    uint32_t level = *level_reg & (0x1 << pos);

    *value = level?1:0;
}

int main(){
    set_gpio_output(gpio_ctr, 13);
    set_gpio_output(gpio_ctr, 19);
    set_gpio_output(gpio_ctr, 26);

    set_gpio_input(gpio_ctr, 4);
    set_gpio_pullup(gpio_ctr, 4);

    int gpio_4_value;
    get_gpio_input_value(gpio_ctr, 4, &gpio_4_value);

    if(gpio_t_value){
        set_gpio_output_value(gpio_ctr, 13, 1);
        set_gpio_output_value(gpio_ctr, 19, 0);
        set_gpio_output_value(gpio_ctr, 26, 0);
    }
    else{
        set_gpio_output_value(gpio_ctr, 13, 0);
        set_gpio_output_value(gpio_ctr, 19, 0);
        set_gpio_output_value(gpio_ctr, 26, 1);
    }

    return 0;
}
