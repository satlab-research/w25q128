#ifndef W25Q128_H
#define W25Q128_H 


#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"



#define ENABLE_INST _u(0x06)
#define DISABLE_INST _u(0x04)

#define READ_STATUS_REG_1_INST _u(0x05)
#define READ_STATUS_REG_2_INST _u(0x35)
#define READ_STATUS_REG_3_INST _u(0x15)

#define WRITE_STATUS_REG_1_INST _u(0x01)
#define WRITE_STATUS_REG_2_INST _u(0x31)
#define WRITE_STATUS_REG_3_INST _u(0x11)

#define JEDEC_ID_INST _u(0x9F)

#define CHIP_ERASE_INST _u(0xC7)
#define SECTOR_ERASE_INST _u(0x20)
#define BLOCK_ERASE_32KB_INST _u(0x52)
#define BLOCK_ERASE_64KB_INST _u(0xD8)

#define PAGE_PROGRAM_INST _u(0x02)
#define READ_DATA_INST _u(0x03)

// #define ENABLE_RESET _u(0x66)
// #define RESET_INST _u(0x99)






class W25Q128
{

    private:
        // Private Variables or Functions
        spi_inst_t *inst;

        int miso;
        int cs;
        int sck;
        int mosi;


        void csnLow();

        void csnHigh();

        void write_enable();

        void write_disable();


    public:
        
        void jed_id();

        // void reset_flash();

        void chip_erase();

        void sector_erase(uint32_t addr);
        
        void block_erase_32KB(uint32_t addr);
        
        void block_erase_64KB(uint32_t addr);

        uint8_t read_status_reg(uint8_t addr);
        
        uint8_t write_status_reg(uint8_t addr, uint8_t data);

        void write_int_data(uint32_t addr, uint8_t data[], uint8_t size);

        uint8_t read_int_data(uint32_t addr);

        void write_data(uint32_t addr, char block[], uint8_t size);

        char read_byte(uint32_t addr)
        {
            uint8_t rec_char = 0;
            uint8_t buf[1] = { READ_DATA_INST };
            uint8_t msb[3] = { ((addr >> 16) & (0xff)), ((addr >> 8) & (0xff)), (addr & 0xff) };

            csnLow();

            spi_write_blocking(this->inst, buf, 1);
            spi_write_blocking(this->inst, msb, 3);

            spi_read_blocking(this->inst, 0xff, &rec_char, 1);

            csnHigh();

            return rec_char;

        }

        void read_data(uint32_t addr, char block[], uint8_t len);


        W25Q128(spi_inst_t *inst, int miso, int cs, int sck, int mosi);
        ~W25Q128();


};


#endif