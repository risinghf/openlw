## 介绍

基于[semtech LoRaWAN开源协议栈](https://github.com/Lora-net/LoRaMac-node/)进行封装，简化使用，方便用户二次开发。目前协议栈支持[LoRaWAN V1.0.4](https://lora-alliance.org/resource_hub/lorawan-104-specification-package/)版本。



## 平台支持

* RHF0M0E5

  * 内置STM32WLE5JC MCU，相关开发手册和数据手册可以官网下载。
  * [硬件设计技术规格书](doc/[RHF-DS119180]RHF0M0E5模块技术规格书_V1.4.pdf)

* RHF0M063

  * 内置STM32L071 MCU 和SX126x射频芯片，相关开发手册和数据手册可以官网下载。

* RHF3M076

  * 内置STM32L071 MCU 和SX1276射频芯片，相关开发手册和数据手册可以官网下载。

* RHF0M0003HF22

  * 内置STM32WLE5CC MCU，相关开发手册和数据手册可以官网下载。

* RHF0M0003LF22

  * 内置STM32WLE5CC MCU，相关开发手册和数据手册可以官网下载。

  

## 快速开始

开始之前请搭建好构建环境，参考[开发环境搭建](doc/开发环境搭建.md)文档。

以下操作均使用命令的形式。

* 克隆仓库

  `git clone git@github.com:risinghf/openlw.git`

* 以`RHF0M0E5`硬件平台`demo`项目为例，在根目录下执行下列指令，查看源码支持的模组型号

  `scons list`
  ```
  scons list
  scons: Reading SConscript files ...
  Suppor module list:
  - RHF0M063
  - RHF3M076
  - RHF0M0E5
  - RHF0M003HF22
  - RHF0M003LF22
  ```

* 选择对应的模组型号，debug=1 -> -O0  debug=0 -> -Os

  `scons debug=0 project=RHF0M0E5`

* 如果需要清除生成的目标文件，则运行`scons debug=0 project=RHF0M0E5 -c`即可

  ```
  scons debug=0 project=rhf0m0e5
  scons: Reading SConscript files ...
  rhf0m0e5
  Link script exists path: src/boards/RHF0M0E5/gcc\stm32wle5xx_flash.ld
  TOOL_PATH:  C:\Program Files (x86)\GNU Arm Embedded Toolchain\10 2020-q4-major\bin
  scons: done reading SConscript files.
  scons: Building targets ...
  CC bulid\rhf0m0e5\obj\bsp\board\board.o
  CC bulid\rhf0m0e5\obj\bsp\board\delay-board.o
  CC bulid\rhf0m0e5\obj\bsp\board\gpio-board.o
  CC bulid\rhf0m0e5\obj\bsp\board\lpm-board.o
  CC bulid\rhf0m0e5\obj\bsp\board\rtc-board.o
  CC bulid\rhf0m0e5\obj\bsp\board\spi-board.o
  CC bulid\rhf0m0e5\obj\bsp\board\sx1262-board.o
  CC bulid\rhf0m0e5\obj\bsp\board\sysIrqHandlers.o
  CC bulid\rhf0m0e5\obj\bsp\board\uart-board.o
  AS bulid\rhf0m0e5\obj\bsp\board\gcc\startup_stm32wle5xx.o
  CC bulid\rhf0m0e5\obj\bsp\stm32hal\CMSIS\Device\stm32wl\system_stm32wlxx.o
  CC bulid\rhf0m0e5\obj\bsp\stm32hal\STM32WLxx_HAL_Driver\Src\stm32wlxx_hal.o
  CC bulid\rhf0m0e5\obj\bsp\stm32hal\STM32WLxx_HAL_Driver\Src\stm32wlxx_hal_cortex.o
  CC bulid\rhf0m0e5\obj\bsp\stm32hal\STM32WLxx_HAL_Driver\Src\stm32wlxx_hal_dma.o
  CC bulid\rhf0m0e5\obj\bsp\stm32hal\STM32WLxx_HAL_Driver\Src\stm32wlxx_hal_dma_ex.o
  CC bulid\rhf0m0e5\obj\bsp\stm32hal\STM32WLxx_HAL_Driver\Src\stm32wlxx_hal_flash.o
  CC bulid\rhf0m0e5\obj\bsp\stm32hal\STM32WLxx_HAL_Driver\Src\stm32wlxx_hal_flash_ex.o
  CC bulid\rhf0m0e5\obj\bsp\stm32hal\STM32WLxx_HAL_Driver\Src\stm32wlxx_hal_gpio.o
  CC bulid\rhf0m0e5\obj\bsp\stm32hal\STM32WLxx_HAL_Driver\Src\stm32wlxx_hal_pwr.o
  CC bulid\rhf0m0e5\obj\bsp\stm32hal\STM32WLxx_HAL_Driver\Src\stm32wlxx_hal_pwr_ex.o
  CC bulid\rhf0m0e5\obj\bsp\stm32hal\STM32WLxx_HAL_Driver\Src\stm32wlxx_hal_rcc.o
  CC bulid\rhf0m0e5\obj\bsp\stm32hal\STM32WLxx_HAL_Driver\Src\stm32wlxx_hal_rcc_ex.o
  CC bulid\rhf0m0e5\obj\bsp\stm32hal\STM32WLxx_HAL_Driver\Src\stm32wlxx_hal_rtc.o
  CC bulid\rhf0m0e5\obj\bsp\stm32hal\STM32WLxx_HAL_Driver\Src\stm32wlxx_hal_rtc_ex.o
  CC bulid\rhf0m0e5\obj\bsp\stm32hal\STM32WLxx_HAL_Driver\Src\stm32wlxx_hal_spi.o
  CC bulid\rhf0m0e5\obj\bsp\stm32hal\STM32WLxx_HAL_Driver\Src\stm32wlxx_hal_spi_ex.o
  CC bulid\rhf0m0e5\obj\bsp\stm32hal\STM32WLxx_HAL_Driver\Src\stm32wlxx_hal_uart.o
  CC bulid\rhf0m0e5\obj\bsp\stm32hal\STM32WLxx_HAL_Driver\Src\stm32wlxx_hal_uart_ex.o
  CC bulid\rhf0m0e5\obj\bsp\stm32hal\STM32WLxx_HAL_Driver\Src\stm32wlxx_hal_usart.o
  CC bulid\rhf0m0e5\obj\bsp\stm32hal\STM32WLxx_HAL_Driver\Src\stm32wlxx_hal_usart_ex.o
  CC bulid\rhf0m0e5\obj\system\delay.o
  CC bulid\rhf0m0e5\obj\system\fifo.o
  CC bulid\rhf0m0e5\obj\system\gpio.o
  CC bulid\rhf0m0e5\obj\system\log.o
  CC bulid\rhf0m0e5\obj\system\systime.o
  CC bulid\rhf0m0e5\obj\system\timer.o
  CC bulid\rhf0m0e5\obj\system\uart.o
  CC bulid\rhf0m0e5\obj\system\utilities.o
  CC bulid\rhf0m0e5\obj\radio\radio-log.o
  CC bulid\rhf0m0e5\obj\radio\sx126x\radio.o
  CC bulid\rhf0m0e5\obj\radio\sx126x\sx126x.o
  CC bulid\rhf0m0e5\obj\mac\LoRaMac.o
  CC bulid\rhf0m0e5\obj\mac\LoRaMacAdr.o
  CC bulid\rhf0m0e5\obj\mac\LoRaMacClassB.o
  CC bulid\rhf0m0e5\obj\mac\LoRaMacCommands.o
  CC bulid\rhf0m0e5\obj\mac\LoRaMacConfirmQueue.o
  CC bulid\rhf0m0e5\obj\mac\LoRaMacCrypto.o
  CC bulid\rhf0m0e5\obj\mac\LoRaMacParser.o
  CC bulid\rhf0m0e5\obj\mac\LoRaMacSerializer.o
  CC bulid\rhf0m0e5\obj\mac\soft-se\aes.o
  CC bulid\rhf0m0e5\obj\mac\soft-se\cmac.o
  CC bulid\rhf0m0e5\obj\mac\soft-se\soft-se-hal.o
  CC bulid\rhf0m0e5\obj\mac\soft-se\soft-se.o
  CC bulid\rhf0m0e5\obj\mac\region\Region.o
  CC bulid\rhf0m0e5\obj\mac\region\RegionCommon.o
  CC bulid\rhf0m0e5\obj\mac\region\RegionBaseUS.o
  CC bulid\rhf0m0e5\obj\mac\region\RegionCN470OLD.o
  CC bulid\rhf0m0e5\obj\mac\region\RegionEU868.o
  CC bulid\rhf0m0e5\obj\app\demo\RHF0M0E5\main.o
  CC bulid\rhf0m0e5\obj\app\common\lw.o
  CC bulid\rhf0m0e5\obj\app\common\print.o
  LINK bulid\rhf0m0e5\exe\rhf0m0e5.elf
  arm-none-eabi-objcopy -O ihex bulid\rhf0m0e5\exe\rhf0m0e5.elf bulid\rhf0m0e5\exe\rhf0m0e5.hex
  arm-none-eabi-objcopy -O binary bulid\rhf0m0e5\exe\rhf0m0e5.elf bulid\rhf0m0e5\exe\rhf0m0e5.bin
  arm-none-eabi-size bulid\rhf0m0e5\exe\rhf0m0e5.elf
     text    data     bss     dec     hex filename
    61216     192   15864   77272   12dd8 bulid\rhf0m0e5\exe\rhf0m0e5.elf
  scons: done building targets.

  ```

* 相关目标文件会在`./build/rhf0m0e5`目录下生成

  ```
  ├─bulid
  │  └─rhf0m0e5
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
  │          │  └─stm32hal
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
  └─
  ```



## 二次开发

请参考[API使用说明](doc/API使用说明.md)

