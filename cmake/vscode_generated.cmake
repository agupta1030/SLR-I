# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Compiler options
target_compile_options(${BUILD_UNIT_0_NAME} PRIVATE
    $<$<COMPILE_LANGUAGE:C>: ${CUBE_CMAKE_C_FLAGS}>
    $<$<COMPILE_LANGUAGE:CXX>: ${CUBE_CMAKE_CXX_FLAGS}>
    $<$<COMPILE_LANGUAGE:ASM>: ${CUBE_CMAKE_ASM_FLAGS}>
)

# Linker options
target_link_options(${BUILD_UNIT_0_NAME} PRIVATE ${CUBE_CMAKE_EXE_LINKER_FLAGS})

# Add sources to executable/library
target_sources(${BUILD_UNIT_0_NAME} PRIVATE
    "Core/Src/dma.c"
    "Core/Src/gpio.c"
    "Core/Src/i2c.c"
    "Core/Src/main.c"
    "Core/Src/spi.c"
    "Core/Src/stm32f7xx_hal_msp.c"
    "Core/Src/stm32f7xx_it.c"
    "Core/Src/syscalls.c"
    "Core/Src/sysmem.c"
    "Core/Src/system_stm32f7xx.c"
    "Core/Src/tim.c"
    "Core/Src/usart.c"
    "Core/Startup/startup_stm32f722retx.s"
    "Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal.c"
    "Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_cortex.c"
    "Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_dma.c"
    "Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_dma_ex.c"
    "Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_exti.c"
    "Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_flash.c"
    "Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_flash_ex.c"
    "Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_gpio.c"
    "Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_i2c.c"
    "Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_i2c_ex.c"
    "Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pcd.c"
    "Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pcd_ex.c"
    "Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pwr.c"
    "Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_pwr_ex.c"
    "Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_rcc.c"
    "Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_rcc_ex.c"
    "Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_spi.c"
    "Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_spi_ex.c"
    "Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_tim.c"
    "Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_tim_ex.c"
    "Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_uart.c"
    "Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_hal_uart_ex.c"
    "Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_ll_usb.c"
    "Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/usbd_cdc.c"
    "Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.c"
    "Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c"
    "Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c"
    "USB_DEVICE/App/usb_device.c"
    "USB_DEVICE/App/usbd_cdc_if.c"
    "USB_DEVICE/App/usbd_desc.c"
    "USB_DEVICE/Target/usbd_conf.c"
)

target_include_directories(${BUILD_UNIT_0_NAME} PRIVATE
    "Core/Inc"
    "USB_DEVICE/App"
    "USB_DEVICE/Target"
    "Drivers/STM32F7xx_HAL_Driver/Inc"
    "Drivers/STM32F7xx_HAL_Driver/Inc/Legacy"
    "Middlewares/ST/STM32_USB_Device_Library/Core/Inc"
    "Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc"
    "Drivers/CMSIS/Device/ST/STM32F7xx/Include"
    "Drivers/CMSIS/Include"
)

configure_file("${CMAKE_SOURCE_DIR}/STM32F722RETX_FLASH.ld" "${CMAKE_BINARY_DIR}" COPYONLY)

set_target_properties(${BUILD_UNIT_0_NAME} PROPERTIES LINK_DEPENDS "STM32F722RETX_FLASH.ld")

