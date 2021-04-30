## 介绍

基于[semtech LoRaWAN开源协议栈](https://github.com/Lora-net/LoRaMac-node/)进行封装，简化使用，方便用户二次开发。目前协议栈支持[LoRaWAN V1.0.4](https://lora-alliance.org/resource_hub/lorawan-104-specification-package/)版本。



## 平台支持

* RHF0M0E5

  * 内置STM32WLE5JC MCU，相关开发手册和数据手册可以官网下载。
  * [硬件设计技术规格书](doc/[RHF-DS119180]RHF0M0E5模块技术规格书_V1.4.pdf)

  

## 快速开始

开始之前请搭建好构建环境，参考[开发环境搭建](doc/开发环境搭建.md)文档。

以下操作均使用命令的形式。

* 克隆仓库

  `git clone git@github.com:risinghf/openlw.git`

* 切换当前目录至某一板级支持目录，以`RHF0M0E5`硬件平台`demo`项目为例

  `cd openlw/src/boards/RHF0M0E5`

* 运行`scons`即可。如果需要清除生成的目标文件，则运行`socns -c`即可

  ```
  scons
  scons: Reading SConscript files ...
  scons: done reading SConscript files.
  scons: Building targets ...
  scons: building associated VariantDir targets: bulid\demo\obj\bsp\board
  CC bulid\demo\obj\bsp\board\board.o
  CC bulid\demo\obj\bsp\board\delay-board.o
  AS bulid\demo\obj\bsp\board\gcc\startup_stm32wle5xx.o
  CC bulid\demo\obj\bsp\board\gpio-board.o
  CC bulid\demo\obj\bsp\board\lpm-board.o
  CC bulid\demo\obj\bsp\board\rtc-board.o
  CC bulid\demo\obj\bsp\board\spi-board.o
  CC bulid\demo\obj\bsp\board\sx1262-board.o
  CC bulid\demo\obj\bsp\board\sysIrqHandlers.o
  CC bulid\demo\obj\bsp\board\uart-board.o
  CC bulid\demo\obj\bsp\stm32wl\CMSIS\Device\stm32wl\system_stm32wlxx.o
  CC bulid\demo\obj\bsp\stm32wl\STM32WLxx_HAL_Driver\Src\stm32wlxx_hal.o
  CC bulid\demo\obj\bsp\stm32wl\STM32WLxx_HAL_Driver\Src\stm32wlxx_hal_cortex.o
  CC bulid\demo\obj\bsp\stm32wl\STM32WLxx_HAL_Driver\Src\stm32wlxx_hal_dma.o
  CC bulid\demo\obj\bsp\stm32wl\STM32WLxx_HAL_Driver\Src\stm32wlxx_hal_dma_ex.o
  CC bulid\demo\obj\bsp\stm32wl\STM32WLxx_HAL_Driver\Src\stm32wlxx_hal_flash.o
  CC bulid\demo\obj\bsp\stm32wl\STM32WLxx_HAL_Driver\Src\stm32wlxx_hal_flash_ex.o
  CC bulid\demo\obj\bsp\stm32wl\STM32WLxx_HAL_Driver\Src\stm32wlxx_hal_gpio.o
  CC bulid\demo\obj\bsp\stm32wl\STM32WLxx_HAL_Driver\Src\stm32wlxx_hal_pwr.o
  CC bulid\demo\obj\bsp\stm32wl\STM32WLxx_HAL_Driver\Src\stm32wlxx_hal_pwr_ex.o
  CC bulid\demo\obj\bsp\stm32wl\STM32WLxx_HAL_Driver\Src\stm32wlxx_hal_rcc.o
  CC bulid\demo\obj\bsp\stm32wl\STM32WLxx_HAL_Driver\Src\stm32wlxx_hal_rcc_ex.o
  CC bulid\demo\obj\bsp\stm32wl\STM32WLxx_HAL_Driver\Src\stm32wlxx_hal_rtc.o
  CC bulid\demo\obj\bsp\stm32wl\STM32WLxx_HAL_Driver\Src\stm32wlxx_hal_rtc_ex.o
  CC bulid\demo\obj\bsp\stm32wl\STM32WLxx_HAL_Driver\Src\stm32wlxx_hal_spi.o
  CC bulid\demo\obj\bsp\stm32wl\STM32WLxx_HAL_Driver\Src\stm32wlxx_hal_spi_ex.o
  CC bulid\demo\obj\bsp\stm32wl\STM32WLxx_HAL_Driver\Src\stm32wlxx_hal_uart.o
  CC bulid\demo\obj\bsp\stm32wl\STM32WLxx_HAL_Driver\Src\stm32wlxx_hal_uart_ex.o
  CC bulid\demo\obj\bsp\stm32wl\STM32WLxx_HAL_Driver\Src\stm32wlxx_hal_usart.o
  CC bulid\demo\obj\bsp\stm32wl\STM32WLxx_HAL_Driver\Src\stm32wlxx_hal_usart_ex.o
  CC bulid\demo\obj\system\delay.o
  CC bulid\demo\obj\system\fifo.o
  CC bulid\demo\obj\system\gpio.o
  CC bulid\demo\obj\system\log.o
  CC bulid\demo\obj\system\systime.o
  CC bulid\demo\obj\system\timer.o
  CC bulid\demo\obj\system\uart.o
  CC bulid\demo\obj\system\utilities.o
  CC bulid\demo\obj\radio\radio-log.o
  CC bulid\demo\obj\radio\sx126x\radio.o
  CC bulid\demo\obj\radio\sx126x\sx126x.o
  CC bulid\demo\obj\mac\LoRaMac.o
  CC bulid\demo\obj\mac\LoRaMacAdr.o
  CC bulid\demo\obj\mac\LoRaMacClassB.o
  CC bulid\demo\obj\mac\LoRaMacCommands.o
  CC bulid\demo\obj\mac\LoRaMacConfirmQueue.o
  CC bulid\demo\obj\mac\LoRaMacCrypto.o
  CC bulid\demo\obj\mac\LoRaMacParser.o
  CC bulid\demo\obj\mac\LoRaMacSerializer.o
  CC bulid\demo\obj\mac\soft-se\aes.o
  CC bulid\demo\obj\mac\soft-se\cmac.o
  CC bulid\demo\obj\mac\soft-se\soft-se-hal.o
  CC bulid\demo\obj\mac\soft-se\soft-se.o
  CC bulid\demo\obj\mac\region\Region.o
  CC bulid\demo\obj\mac\region\RegionCommon.o
  CC bulid\demo\obj\mac\region\RegionEU868.o
  CC bulid\demo\obj\app\common\lw.o
  CC bulid\demo\obj\app\demo\RHF0M0E5\main.o
  LINK bulid\demo\exe\demo.elf
  arm-none-eabi-objcopy -O ihex bulid\demo\exe\demo.elf bulid\demo\exe\demo.hex
  arm-none-eabi-objcopy -O binary bulid\demo\exe\demo.elf bulid\demo\exe\demo.bin
  arm-none-eabi-size bulid\demo\exe\demo.elf
     text    data     bss     dec     hex filename
    65600     200   14896   80696   13b38 bulid\demo\exe\demo.elf
  scons: done building targets.
  ```

* 相关目标文件会在`./build/demo`目录下生成

  ```
  ├─arm
  ├─bulid
  │  └─demo
  │      ├─exe
  │      ├─list
  │      └─obj
  │          ├─app
  │          │  ├─common
  │          │  └─demo
  │          │      └─RHF0M0E5
  │          ├─bsp
  │          │  ├─board
  │          │  │  └─gcc
  │          │  └─stm32wl
  │          │      ├─CMSIS
  │          │      │  └─Device
  │          │      │      └─stm32wl
  │          │      └─STM32WLxx_HAL_Driver
  │          │          └─Src
  │          ├─mac
  │          │  ├─region
  │          │  └─soft-se
  │          ├─radio
  │          │  └─sx126x
  │          └─system
  ├─gcc
  └─iar
  ```

  

## 二次开发

请参考[API使用说明](doc/API使用说明.md)

