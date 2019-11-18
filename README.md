# Example: GPIO

This app demonstrates simple setup and usage of GPIO interrupts on the Espressif ESP32 using esp-idf v3.3. Toggle LED with button press. 

### Configure the project

```
make menuconfig
```
* Set serialport under Serial flasher config

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
make -j4 flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

## GPIO functions:

 * GPIO23: LED output
 * GPIO4:  input, pulled up, interrupt from falling edge

## Test:
 * Connect GPIO4 with button 
 * Connect GPIO23 with LED

 ![Diagram](images/gpio_led_toggle_bb.png)
