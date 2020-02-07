# PIC32MX_CPP
### Testing C++ on PIC32MX
The following modules are present in the current repository:

| Module | Code support for PIC32MX795F512L | tested |
| ------- | -------------------------------:| ------:|
| Adc     | ✔️ | ✔️ |
| Ccp     | ➖ | ➖ |
| Clc     | ➖ | ➖ |
| Comp    | ➖ | ➖ |
| Cp0     | ✔️ | partially (count works) |
| Cvref   | ➖ | ➖ |
| Delay   | ✔️ | ✔️ |
| Dma     | ➖ | ➖ |
| I2c     | ✔️ | ✔️ |
| Irq     | ✔️ | ✔️ |
| Nvm     | ✔️ | ➖ |
| Oc      | ✔️ | ✔️ |
| Osc     | ✔️ (clean required) | ✔️ |
| Pins    | ✔️ | partially (an pins not) |
| Pmd     | ➖ | ➖ |
| Pmp     | ➖ | ➖ |
| Reg     | ✔️ | ✔️ |
| Resets  | ✔️ | ➖ |
| Rtcc    | ✔️ | ✔️ |
| Spi     | ✔️ | ✔️ |
| Sys     | ✔️ (added support for prefetch cache and wait states) | ✔️ |
| Timer1  | ✔️ | ✔️ |
| Timer   | ✔️ | ✔️ |
| Uart    | ✔️ | ✔️ |
| Usb     | ✔️ | ✔️ |
| Vdetect | ➖ | ➖ |
| Wdt     | ✔️ | ➖ |

Peripheral access library for PIC32MX795F512L and alike PIC32MX family microcontrollers. Written in C++ which is supported now by the Microchip XC32++ compiler by default.
