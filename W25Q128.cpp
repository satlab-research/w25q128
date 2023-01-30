#include "W25Q128.h"

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"



W25Q128 :: W25Q128(spi_inst_t *inst, int miso, int cs, int sck, int mosi)
{
    this->inst = inst;

    this->miso = miso;
    this->cs = cs;
    this->sck = sck;
    this->mosi = mosi;

    spi_init(this->inst, 1000*1000);
    gpio_set_function(this->miso, GPIO_FUNC_SPI);
    gpio_set_function(this->cs,   GPIO_FUNC_SIO);
    gpio_set_function(this->sck,  GPIO_FUNC_SPI);
    gpio_set_function(this->mosi, GPIO_FUNC_SPI);
    
    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_set_dir(this->cs, GPIO_OUT);
    gpio_put(this->cs, 1);


}

W25Q128::~W25Q128()
{
}

//  PRIVATE FUNCTIONS

void W25Q128 :: csnLow()
{
    gpio_set_dir(this->cs, 0);
}

void W25Q128 :: csnHigh()
{
    gpio_set_dir(this->cs, 1);
}

void W25Q128 :: write_enable()
{
    uint8_t buf[1] = { ENABLE_INST };
    csnLow();
    spi_write_blocking(inst, buf, 1);
    csnHigh();
}

void W25Q128 :: write_disable()
{
    uint8_t buf[1] = { DISABLE_INST };
    csnLow();
    spi_write_blocking(inst, buf, 1);
    csnHigh();
}




// Public Functions

void W25Q128 :: jed_id()
{
    uint8_t x = 0x00;
    uint8_t y = 0x00;
    uint8_t z = 0x00;
    uint8_t buf[1] = { JEDEC_ID_INST };

    csnLow();
    spi_write_blocking(inst, buf, 1);
    spi_read_blocking(inst, 0xff, &x, 1);
    spi_read_blocking(inst, 0xff, &y, 1);
    spi_read_blocking(inst, 0xff, &z, 1);

    csnHigh();
    sleep_us(100);

    printf("Jedec_Id Manufactuere Id: %x, Device Id: %x\n", x, ((y<<8)|z) );

}


// void W25Q128::reset_flash()
// {

//     csnLow();
//     spi_write_blocking(inst, ENABLE_RESET, 1);
//     csnHigh();

//     csnLow();
//     spi_write_blocking(inst, RESET_INST, 1);
//     csnHigh();

// }


void W25Q128 :: chip_erase()
{
    uint8_t buf[1] = { CHIP_ERASE_INST };
    printf("Chip is being erased. Wait for 200 seconds .....\n");

    write_enable();
    csnLow();
    spi_write_blocking(inst, buf, 1);
    csnHigh();

    sleep_ms(200000);
}


void W25Q128 :: sector_erase(uint32_t addr)
{
    uint8_t buf[1] = { SECTOR_ERASE_INST };
    uint8_t msb[3] = { ((addr >> 16) & (0xff)), ((addr >> 8) & (0xff)), (addr & 0xff) };
    
    printf("Sector %x is being erased.", addr);

    write_enable();
    csnLow();
    spi_write_blocking(inst, buf, 1);
    spi_write_blocking(inst, msb, 3);
    csnHigh();
    
    sleep_ms(400);
}

void W25Q128 :: block_erase_32KB(uint32_t addr)
{
    uint8_t buf[1] = { BLOCK_ERASE_32KB_INST };
    uint8_t msb[3] = { ((addr >> 16) & (0xff)), ((addr >> 8) & (0xff)), (addr & 0xff) };
    
    printf("Block of 32 KB %x is being erased.", addr);

    write_enable();
    csnLow();
    spi_write_blocking(inst, buf, 1);
    spi_write_blocking(inst, msb, 3);
    csnHigh();
    
    sleep_ms(400);
}

void W25Q128 :: block_erase_64KB(uint32_t addr)
{
    uint8_t buf[1] = { BLOCK_ERASE_64KB_INST };
    uint8_t msb[3] = { ((addr >> 16) & (0xff)), ((addr >> 8) & (0xff)), (addr & 0xff) };
    
    printf("Block of 64 KB %x is being erased.", addr);

    write_enable();
    csnLow();
    spi_write_blocking(inst, buf, 1);
    spi_write_blocking(inst, msb, 3);
    csnHigh();
    
    sleep_ms(400);
}

uint8_t W25Q128 :: read_status_reg(uint8_t addr)
{
    uint8_t result=0;
    uint8_t buf[1] = { addr };

    csnLow();
    
    spi_write_blocking(inst, buf, 1);
    spi_read_blocking(inst, 0xff, &result, 1);
    
    csnHigh();

    sleep_us(100);

    return result;
}

uint8_t W25Q128 :: write_status_reg(uint8_t addr, uint8_t data)
{
    uint8_t buf[1] = { addr };
    uint8_t dbuf[1] = { data };
    
    write_enable();
    
    csnLow();
    
    spi_write_blocking(inst, buf, 1);
    spi_write_blocking(inst, dbuf, 1);
    
    csnHigh();
}

void W25Q128 :: write_data(uint32_t addr, char block[], uint8_t size)
{
    uint8_t buf[1] = { PAGE_PROGRAM_INST };
    uint8_t msb[3] = { ((addr >> 16) & (0xff)), ((addr >> 8) & (0xff)), (addr & 0xff) };

    write_enable();
    
    csnLow();
    
    spi_write_blocking(inst, buf, 1);
    spi_write_blocking(inst, msb, 3);

    for(uint8_t i = 0; i<size; i++)
    {
        uint8_t dbuf[1] = { block[i] };
        spi_write_blocking(inst, dbuf, 1);
    }
    
    csnHigh();

}

void W25Q128 :: read_data(uint32_t addr,char block[], uint8_t len)
{
    uint8_t i = 0;
    
    while (i < len)
    {
        block[i] = read_byte( ( addr + i) );
        i++;
    }

}


