#include "plus.h"
#include "portable.h"

// ========== 静态辅助函数 ==========

/**
 * @brief 启动主定时器PWM
 */
static int StartMasterPWM(Plus* plus) {
    if (plus == NULL || plus->timMaster == NULL) return -1;
    
    if (HAL_TIM_PWM_Start(plus->timMaster, plus->masterChannel) != HAL_OK) {
        return -1;
    }
    return 0;
}

/**
 * @brief 停止主定时器PWM
 */
static int StopMasterPWM(Plus* plus) {
    if (plus == NULL || plus->timMaster == NULL) return -1;
    
    if (HAL_TIM_PWM_Stop(plus->timMaster, plus->masterChannel) != HAL_OK) {
        return -1;
    }
    return 0;
}

/**
 * @brief 启动从定时器计数（外部时钟模式）带中断
 */
static int StartSlaveCounter_IT(Plus* plus) {
    if (plus == NULL || plus->timSlave == NULL) return -1;
    
    // 清除更新中断标志
    __HAL_TIM_CLEAR_FLAG(plus->timSlave, TIM_FLAG_UPDATE);
    
    // 使能更新中断
    __HAL_TIM_ENABLE_IT(plus->timSlave, TIM_IT_UPDATE);
    
    // 启动从定时器（带中断）
    if (HAL_TIM_Base_Start_IT(plus->timSlave) != HAL_OK) {
        // 启动失败，禁用中断
        __HAL_TIM_DISABLE_IT(plus->timSlave, TIM_IT_UPDATE);
        return -1;
    }
    return 0;
}

/**
 * @brief 停止从定时器计数
 */
static int StopSlaveCounter(Plus* plus) {
    if (plus == NULL || plus->timSlave == NULL) return -1;
    
    // 停止定时器
    if (HAL_TIM_Base_Stop_IT(plus->timSlave) != HAL_OK) {
        return -1;
    }
    
    // 禁用更新中断
    __HAL_TIM_DISABLE_IT(plus->timSlave, TIM_IT_UPDATE);
    
    // 清除更新中断标志
    __HAL_TIM_CLEAR_FLAG(plus->timSlave, TIM_FLAG_UPDATE);
    
    return 0;
}

/**
 * @brief 清零从定时器计数器
 */
static void ClearSlaveCounter(Plus* plus) {
    if (plus == NULL || plus->timSlave == NULL) return;
    __HAL_TIM_SET_COUNTER(plus->timSlave, 0);
}

// ========== 构造函数 ==========
Plus* NewPlus(TIM_HandleTypeDef* timMaster, TIM_HandleTypeDef* timSlave, uint32_t masterChannel) {
    if (timMaster == NULL || timSlave == NULL) return NULL;
    
    Plus* plus = (Plus*)pvPortMalloc(sizeof(Plus));
    if (plus == NULL) return NULL;
    
    plus->timMaster = timMaster;
    plus->timSlave = timSlave;
    plus->masterChannel = masterChannel;
    plus->slaveChannel = 0;  // 默认不使用
    plus->isRunning = 0;
    
    // 创建FreeRTOS互斥锁
    plus->lock = xSemaphoreCreateMutex();
    if (plus->lock == NULL) {
        vPortFree(plus);
        return NULL;
    }
    
    return plus;
}

// ========== 析构函数 ==========
void DeletePlus(Plus* plus) {
    if (plus == NULL) return;
    
    // 如果正在运行，先停止
    if (plus->isRunning) {
        PlusStop(plus);
    }
    
    // 删除互斥锁
    if (plus->lock != NULL) {
        vSemaphoreDelete(plus->lock);
    }
    
    vPortFree(plus);
}

// ========== 功能实现 ==========

int PlusSend(Plus* plus, uint16_t count) {
    if (plus == NULL || count == 0) return -1;
    
    if(plus->isRunning) return 1;

    // 获取锁，检查运行状态
    if (xSemaphoreTake(plus->lock, portMAX_DELAY) != pdTRUE) {
        return -1;
    }
    
    // 如果正在运行，直接返回非0
    if (plus->isRunning) {
        xSemaphoreGive(plus->lock);
        return 1;
    }
    
    // 设置从定时器ARR（脉冲计数目标）
    // 注意：STM32 ARR是0-based，所以count-1
    __HAL_TIM_SET_AUTORELOAD(plus->timSlave, count - 1);
    
    // 清零从定时器计数器
    ClearSlaveCounter(plus);
    
    // 标记为运行状态
    plus->isRunning = 1;
    
    // 释放锁
    xSemaphoreGive(plus->lock);
    
    // 启动从定时器中断模式（必须先启动，准备接收脉冲）
    if (StartSlaveCounter_IT(plus) != 0) {
        plus->isRunning = 0;
        return -1;
    }
    
    // 启动主定时器PWM（开始发送脉冲）
    if (StartMasterPWM(plus) != 0) {
        StopSlaveCounter(plus);
        plus->isRunning = 0;
        return -1;
    }
    
    return 0;
}

int PlusStop(Plus* plus) {
    if (plus == NULL) return -1;
    
    // 获取锁
    if (xSemaphoreTake(plus->lock, portMAX_DELAY) != pdTRUE) {
        return -1;
    }
    
    // 停止主定时器PWM
    StopMasterPWM(plus);
    
    // 停止从定时器
    StopSlaveCounter(plus);
    
    // 清零从定时器计数器
    ClearSlaveCounter(plus);
    
    // 标记为停止状态
    plus->isRunning = 0;
    
    // 释放锁
    xSemaphoreGive(plus->lock);
    
    return 0;
}

int PlusIsRunning(Plus* plus) {
    if (plus == NULL) return 0;
    
    int running = 0;
    
    // 获取锁，读取状态
    if (xSemaphoreTake(plus->lock, portMAX_DELAY) == pdTRUE) {
        running = plus->isRunning;
        xSemaphoreGive(plus->lock);
    }
    
    return running;
}

uint16_t PlusGetSentCount(Plus* plus) {
    if (plus == NULL || plus->timSlave == NULL) return 0;
    
    return (uint16_t)__HAL_TIM_GET_COUNTER(plus->timSlave);
}
