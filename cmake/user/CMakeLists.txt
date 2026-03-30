set(USER_Include_Dirs

    ${CMAKE_CURRENT_SOURCE_DIR}/../../User/Peripherals/Motor/Servo/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../User/Tools/PWM/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../User/Tools/Serial/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../User/Tools/Router/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../User/Tools/MemoryPoll
    ${CMAKE_CURRENT_SOURCE_DIR}/../../User/Serivces
    ${CMAKE_CURRENT_SOURCE_DIR}/../../User/Peripherals/Communication
    ${CMAKE_CURRENT_SOURCE_DIR}/../../User/Peripherals/Communication/SerialComm
)

set(USER_Application_Src

    ${CMAKE_CURRENT_SOURCE_DIR}/../../User/Peripherals/Motor/Servo/Src/servo.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../User/Peripherals/Motor/Servo/Src/servo_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../User/Tools/PWM/Src/pwm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../User/Tools/PWM/Src/pwm_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../User/Tools/Serial/Src/serial.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/../../User/Tools/Serial/Src/serial_test.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/../../User/Tools/Router/Src/router.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/../../User/Peripherals/Communication/communication.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../User/Peripherals/Communication/SerialComm/serial_comm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../User/Tools/MemoryPoll/memory_poll.c
)



target_include_directories(stm32cubemx INTERFACE ${USER_Include_Dirs})

target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${USER_Application_Src})
