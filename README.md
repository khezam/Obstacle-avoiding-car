# Obstacle-avoiding-car

## Table of Contents:

Introduction

Prototype Design

### Project description
The goal of the senior design project is to design an obstacle avoiding sweeper from scratch. This project involves obstacle detection and maneuverability. In order to detect obstacles, two ultrasonic sensors, along with DC motors will be implemented. The cleaner itself, will be powered using a high power DC motor and will be made of reusable plastic. The motivation behind the project is to have the sweeper work without human interference. This will be beneficial and assist people with mobility issues, such as arthritis, so they don’t have to keep bending over to change the direction of the sweeper when it collides with an obstacle. Instead, the sensors will detect obstacles on their own and decide which alternative route to take. Other motivation behind the project include the sweeper being convenient and saving time, as well as being compact and easy to use. Another motivation is to have the sweeper be easy maintenance and work on all floor types such as tile, hardwood, and carpet. There are several robotic cleaners out on the market already, but what differentiates our product from those is the cost, as well as our features. The body of our design is going to consist of reusable plastic so the cost of it will be significantly lower. Additionally in the future, the sweeper will have the ability to spot clean. Instead of having the sweeper clean a large area, the customer will have the ability to have the sweeper clean a specific spot.

### Prototype Design
Auto-sweeper consist of TIVA launchpad, 2 HC-SR04 Ultrasonic sensor, 2 DC motors and wheels, 1 DRV8833 dual motor driver. Two timers of the ARM Cortex M4 are used for calculating the distance of the obstacles using ultrasonic sensors. DC motor is driven by providing PWM voltage from the microcontroller. The architecture of the system is as follows:

<img width="630" alt="Screen Shot 2019-05-01 at 6 21 37 AM" src="https://user-images.githubusercontent.com/35792614/57019181-9813fc80-6bda-11e9-8af4-8926a90d7b80.png">

 Here, Timer0A and Timer0B are connected to Ultrasonic sensors (USS1 and USS2) for calculating distance using the capture mode. In this mode, the USS sends the trigger signal via TRIGGER and as we know, it is a sound wave it is not an analog signal it is a pulse signal. So, when the signal hits the obstacle it reflects and is received by the ECHO. When the high edge is detected the interrupt is generated for and the timer captures the high edge in Timer register and it waits for the low edge. When the low edge of the signal is detected then interrupt is generated again and the timer captures the low edge. Difference between the high edge and the low edge is the distance between the sensor and the obstacle which can be calculated. The motor driver is provided PWM signals from the microcontroller to drive the motors.

The flowchart for software coding is as follows:

#### Main code

<img width="206" alt="Screen Shot 2019-05-01 at 6 39 10 AM" src="https://user-images.githubusercontent.com/35792614/57019503-90088c80-6bdb-11e9-9469-8f7ba27a67fa.png">

#### Interrupt handler

<img width="319" alt="Screen Shot 2019-05-01 at 6 41 52 AM" src="https://user-images.githubusercontent.com/35792614/57019565-c7773900-6bdb-11e9-8859-d394f28d88a6.png">

There are three interrupts used in this project. Two for within the timer to detect the signal coming from the obstacle and one timer interrupt to periodically generate interrupt. In the main Program, port initialization and interrupt initialization is done. Then both the motors are driven in the forward direction, so that the auto-sweeper moves in front. Periodic interrupt is given to Timer 2A after every half second. In the interrupt handler, both the USS are checked simultaneously if there is an obstacle at both the sensors (less than 15 cm) then both the motors are driven backwards (reverse) and one of the motor is driven, so that the auto-sweeper takes a U turn. If only one of the motor detects the obstacle, then the corresponding motor is driven in forward direction and the other wheel is stopped and the auto-sweeper takes a left or right turn accordingly. Cliff sensor can also be used and placed at the bottom of the auto-sweeper, due to which cliff detection is done. Firstly, the motor will move in forward direction. when it gets any obstacles such as a wall in front of it, it will take a reverse for predefined amount of time and then takes a U turn. When it detects any obstacle at such as a table or chair it takes a left or right turn accordingly.
