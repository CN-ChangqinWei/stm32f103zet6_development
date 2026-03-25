set(USER_Include_Dirs

    ${CMAKE_CURRENT_SOURCE_DIR}/../../User/Peripherals/Motor/Servo/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../User/Tools/PWM/Inc

)

set(USER_Application_Src

    ${CMAKE_CURRENT_SOURCE_DIR}/../../User/Peripherals/Motor/Servo/Src/servo.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../User/Peripherals/Motor/Servo/Src/servo_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../User/Tools/PWM/Src/pwm.c

)

target_include_directories(stm32cubemx INTERFACE ${USER_Include_Dirs})

target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${USER_Application_Src})
