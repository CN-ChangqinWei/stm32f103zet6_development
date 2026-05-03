#ifndef _PLUS_H
#define _PLUS_H
#include "cmsis_os.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_tim.h"


// ========== Plus实例类结构体 ==========
typedef struct {
    TIM_HandleTypeDef* timMaster;   // 主定时器（PWM输出）
    TIM_HandleTypeDef* timSlave;    // 从定时器（脉冲计数）
    uint32_t masterChannel;         // 主定时器PWM通道
    uint32_t slaveChannel;          // 从定时器输入捕获/触发通道（可选）
    SemaphoreHandle_t lock;         // FreeRTOS互斥锁，用于状态保护
    volatile uint8_t isRunning;     // 运行状态标志
} Plus;

// ========== 构造函数与析构函数 ==========
Plus* NewPlus(TIM_HandleTypeDef* timMaster, TIM_HandleTypeDef* timSlave, uint32_t masterChannel);
void DeletePlus(Plus* plus);

// ========== 功能接口 ==========

/**
 * @brief 发送指定数量的脉冲
 * @param plus Plus实例指针
 * @param count 脉冲数量（设置到slave的ARR）
 * @return 0: 成功启动，非0: 失败（如正在运行或参数错误）
 */
int PlusSend(Plus* plus, uint16_t count);

/**
 * @brief 停止脉冲发送
 * @param plus Plus实例指针
 * @return 0: 成功停止，非0: 失败
 */
int PlusStop(Plus* plus);

/**
 * @brief 检查Plus是否正在发送脉冲
 * @param plus Plus实例指针
 * @return 0: 未运行，非0: 正在运行
 */
int PlusIsRunning(Plus* plus);

/**
 * @brief 获取已发送的脉冲数（从slave计数器读取）
 * @param plus Plus实例指针
 * @return 当前已发送的脉冲数
 */
uint16_t PlusGetSentCount(Plus* plus);

#endif
