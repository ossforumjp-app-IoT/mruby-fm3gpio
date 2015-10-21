# mruby-fm3gpio
FM3 GPIO class

## Methods

|method|parameter(s)|return|
|---|---|---|
|GPIO#mode|Pin No, OUTPUT/INPUT|(nil)|
|GPIO#write|Pin No, HIGH/LOW|(nil)|
|GPIO#read|Pin No|HIGH/LOW|

## Constants

|constant||
|---|---|
|GPIO::OUTPUT|use as digital output|
|GPIO::INPUT|use as digital input|
|GPIO::HIGH|output/input HIGH(1)|
|GPIO::LOW|output/input LOW(0)|


## Pin No
|port No|||
|---|---|---|
|Pmn|0xmn|m:0-F, n:0-F|
ex: P23 -> 0x23

## Sample
使い方はおおむね[mruby-WiringPi](https://github.com/akiray03/mruby-WiringPi "akiray03/mruby-WiringPi")に合わせてあります。

    # LED port: P10, STATUS port: P11
    GPIO_LED = 0x10  
    GPIO_STATUS = 0x11  
    
    a = GPIO.new  
    a.mode(GPIO_LED, GPIO::OUTPUT)  
    a.mode(GPIO_STATUS, GPIO::INPUT)  
    case a.read(GPIO_STATUS)  
      # LED ON  
      when GPIO::LOW  
        a.write(GPIO_LED, GPIO::LOW)  
      # LED OFF  
      when GPIO::HIGH  
        a.write(GPIO_LED, GPIO::HIGH)  
    end  

## License
Copyright 2015 Japan OSS Promotion Forum

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
