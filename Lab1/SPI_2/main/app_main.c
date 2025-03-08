/* SPI Slave example, sender (uses SPI master driver)

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_log.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include <sys/unistd.h>


#define GPIO_MOSI           23
#define GPIO_MISO           19
#define GPIO_SCLK           18
#define GPIO_CS             5

#define ESP_HOST VSPI_HOST
esp_err_t ret;
spi_device_handle_t spi_handle;

void SPI_Init (void)
{
    //Configuration for the SPI bus
    spi_bus_config_t buscfg = {
        .mosi_io_num = GPIO_MOSI,
        .miso_io_num = GPIO_MISO,
        .sclk_io_num = GPIO_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1
    };

    //Configuration for the SPI device on the other side of the bus
    spi_device_interface_config_t devcfg = {
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
        .clock_speed_hz = 2000000,
        .duty_cycle_pos = 128,      //50% duty cycle
        .mode = 0,
        .spics_io_num = GPIO_CS,
        .queue_size = 3
    };
    
    //Initialize the SPI bus and add the device we want to send stuff to.
    ret = spi_bus_initialize(ESP_HOST, &buscfg, SPI_DMA_CH_AUTO);
    assert(ret == ESP_OK);
    ret = spi_bus_add_device(ESP_HOST, &devcfg, &spi_handle);
    assert(ret == ESP_OK);    
}

void SPI_Transmit (uint8_t *data, int bytes)
{
    spi_transaction_t trans;
    memset(&trans, 0, sizeof(trans));
    
    trans.tx_buffer = data;
    trans.length = bytes*8;
    
    if (spi_device_transmit(spi_handle, &trans) != ESP_OK)
    {
		printf ("writing ERROR\n");
	}	
}

void SPI_Receive (uint64_t Addr, size_t addrLen, uint8_t *buffertoStore, size_t dataLen)
{
    spi_transaction_t trans;
    memset(&trans, 0, sizeof(trans));
    
    spi_transaction_ext_t trans_ext;
    memset(&trans_ext, 0, sizeof(trans_ext));
    
    trans.flags = SPI_TRANS_VARIABLE_ADDR;
    trans.addr = Addr;
    trans.rx_buffer = buffertoStore;
    trans.length = 	(dataLen)*8;
    
    trans_ext.address_bits = addrLen*8;
    trans_ext.base = trans;
    
    if (spi_device_transmit(spi_handle, (spi_transaction_t *)&trans_ext) != ESP_OK)
    {
		printf ("writing ERROR\n");
	}	
	
}

//Main application
uint8_t Address = 0x9F;
uint8_t RxData[10];
unsigned long int ID = 0;


void app_main(void)
{
	SPI_Init();
	
	while (1)
	{
		SPI_Receive(Address, 1, RxData, 3);
		ID = (RxData[0]<<16)|(RxData[1]<<8)|RxData[2];
		printf("ID = %lX\n", ID);
		sleep(2);
	}
}
