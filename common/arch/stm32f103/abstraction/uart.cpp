#include "uart.h"
#include <cassert>

std::function<void()> txCallbacks[4] = {};
std::function<void(uint8_t)> rxCallbacks[4] = {};
UART_HandleTypeDef *uartHandlers[4] = {};
uint8_t mRxData;

Uart::Uart(USART_TypeDef *instance, uint32_t baudrate)
{
    if (instance == USART1)
    {
        __HAL_RCC_USART1_CLK_ENABLE();
    }
    else if (instance == USART2)
    {
        __HAL_RCC_USART2_CLK_ENABLE();
    }
    else if (instance == USART3)
    {
        __HAL_RCC_USART3_CLK_ENABLE();
    }
    else
    {
        assert("Uart instance not supported");
    }

    uartHandlers[uartInstanceToIndex(instance)] = &mHandle;

    mHandle.Instance = instance;
    mHandle.Init.BaudRate = baudrate;
    mHandle.Init.WordLength = UART_WORDLENGTH_8B;
    mHandle.Init.StopBits = UART_STOPBITS_1;
    mHandle.Init.Parity = UART_PARITY_NONE;
    mHandle.Init.Mode = UART_MODE_TX_RX;
    mHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    mHandle.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&mHandle);

    /* Peripheral interrupt init*/
    HAL_NVIC_SetPriority(USART1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);

    HAL_UART_Receive_IT(&mHandle, &mRxData, sizeof(mRxData));
}

Uart::~Uart()
{
}

bool Uart::send(const uint8_t *data, std::size_t len)
{
    return HAL_UART_Transmit_IT(&mHandle, const_cast<uint8_t *>(data), len) == HAL_OK;
}

bool Uart::isSending() const
{
    return ! (mHandle.Instance->SR & USART_SR_TXE);
}

void Uart::registerTxCallback(std::function<void()> callback)
{
    size_t idx = Uart::uartInstanceToIndex(mHandle.Instance);
    txCallbacks[idx] = callback;
}

void Uart::registerRxCallback(std::function<void(uint8_t)> callback)
{
    size_t idx = Uart::uartInstanceToIndex(mHandle.Instance);
    rxCallbacks[idx] = callback;
}

size_t Uart::uartInstanceToIndex(USART_TypeDef *instance)
{
    if (instance == USART1)
    {
        return 0;
    }
    else if (instance == USART2)
    {
        return 1;
    }
    else if (instance == USART3)
    {
        return 2;
    }
    else
    {
        assert("Uart instance not supported");
    }
    return 0;
}

extern "C"
{

    void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
    {
        size_t idx = Uart::uartInstanceToIndex(huart->Instance);
        if (txCallbacks[idx])
        {
            txCallbacks[idx]();
        }
    }

    void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
    {
        //size_t idx = Uart::uartInstanceToIndex(huart->Instance);
        //if (rxCallbacks[idx])
        //{
            rxCallbacks[0](mRxData);
        //}
        HAL_UART_Receive_IT(huart, &mRxData, sizeof(mRxData));

    }

    void USART1_IRQHandler(void)
    {
        size_t idx = Uart::uartInstanceToIndex(USART1);
        if (uartHandlers[idx])
        {
            HAL_UART_IRQHandler(uartHandlers[idx]);
        }
    }
}