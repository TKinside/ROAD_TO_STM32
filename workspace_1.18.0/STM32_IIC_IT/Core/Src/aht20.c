#include "aht20.h"

#define AHT20_ADDRESS 0x70

//uint8_t readBuffer[6] = {0};

/**
 * @brief  初始化AHT20
 */
//状态机
volatile  AHT20_State AHT20_STATE = FREE;
static uint8_t readBuffer[6] = {0};
void AHT20_Init()
{
    AHT20_STATE=INIT;
    HAL_UART_Transmit(&huart2, "INIT_START", 10, HAL_MAX_DELAY);
  uint8_t readBuffer;
  HAL_Delay(40);
  HAL_I2C_Master_Receive(&hi2c1, AHT20_ADDRESS, &readBuffer, 1, HAL_MAX_DELAY);
  if ((readBuffer & 0x08) == 0x00)
  {
	  HAL_UART_Transmit(&huart2, "INIT_SUCCESS", 12, HAL_MAX_DELAY);
    uint8_t sendBuffer[3] = {0xBE, 0x08, 0x00};
    HAL_I2C_Master_Transmit(&hi2c1, AHT20_ADDRESS, sendBuffer, 3, HAL_MAX_DELAY);
      HAL_Delay(10);
  }
    AHT20_STATE=FREE;
    HAL_UART_Transmit(&huart2, "INIT_COMPLETE", 13, HAL_MAX_DELAY);
}

/**
 * @brief  获取温度和湿度
 * @param  Temperature: 存储获取到的温度
 * @param  Humidity: 存储获取到的湿度
 */

void AHT20_Measure()
{
	HAL_UART_Transmit(&huart2, "MEASURE_START", 13, HAL_MAX_DELAY);
    AHT20_STATE=MEASURING;
    static uint8_t sendBuffer[3] = {0xAC, 0x33, 0x00};
    //中断发送函数不会等待，有可能还没发送完成sendBuffer就已经被释放了，需要让它变成静态的避免发送错误数据
    HAL_I2C_Master_Transmit_IT(&hi2c1, AHT20_ADDRESS, sendBuffer, 3);
    HAL_Delay(75);

}

void AHT20_Get()
{
	HAL_UART_Transmit(&huart2, "GET_START", 9, HAL_MAX_DELAY);

    if(AHT20_STATE==MEASURE_DONE){
        AHT20_STATE=GETTING;
        HAL_I2C_Master_Receive_IT(&hi2c1, AHT20_ADDRESS, readBuffer, 6);
    }


}
void AHT20_FIGURE(float *Temperature, float *Humidity)
{
	if (AHT20_STATE==GET_DONE) {

	        if ((readBuffer[0] & 0x80) == 0x00) {
	            uint32_t data = 0;
	            data = ((uint32_t) readBuffer[3] >> 4) + ((uint32_t) readBuffer[2] << 4) + ((uint32_t) readBuffer[1] << 12);
	            *Humidity = data * 100.0f / (1 << 20);

	            data = (((uint32_t) readBuffer[3] & 0x0F) << 16) + ((uint32_t) readBuffer[4] << 8) +
	                   (uint32_t) readBuffer[5];
	            *Temperature = data * 200.0f / (1 << 20) - 50;
	        }
	        AHT20_STATE=READY;
	    }
	HAL_UART_Transmit(&huart2, "READY", 5, HAL_MAX_DELAY);

}
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c==&hi2c1){
        if (HAL_I2C_GetState(&hi2c1) == HAL_I2C_STATE_READY)
        	HAL_UART_Transmit(&huart2, "MEASURE_COMPLETE", 16, HAL_MAX_DELAY);
        AHT20_STATE=MEASURE_DONE;
    }

}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c==&hi2c1){
        if (HAL_I2C_GetState(&hi2c1) == HAL_I2C_STATE_READY)
        AHT20_STATE=GET_DONE;
        HAL_UART_Transmit(&huart2, "GET_COMPLETE", 12, HAL_MAX_DELAY);
    }

}
