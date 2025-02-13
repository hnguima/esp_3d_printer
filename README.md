# ESP32-Based 3D Printer Firmware

This project provides an open-source firmware tailored for 3D printers using ESP32 microcontrollers. It includes features such as real-time motion control, precise temperature management, and a modern C++ software architecture. The firmware is designed for reliability, suitable for advanced 3D printing applications. It ensures smooth movements, accurate thermal regulation, and multitasking, all within a clean and modular codebase.

## 🚀 Key Features

- **Advanced Motion Control System**
  - Precise stepper motor control with acceleration planning
  - Coordinated multi-axis movements (X,Y,Z,E)
  - Dynamic speed and acceleration calculations 
  - Optimized for CoreXY kinematics

- **Temperature Management**
  - PID control for hotend and heated bed via [`pid_ctrl_t`](components/thermistor.h)
  - Advanced thermistor reading with multi-sample averaging via [`adc_get`](components/adc.c)
  - Safety thermal runaway protection
  - Independent temperature control loops

- **Modern Software Architecture**
  - Object-oriented C++ design with [`Printer`](components/esp_printer.hpp) class
  - RTOS-based multitasking
  - Clean separation of concerns

## 🔧 Core Components 

### Motion Control System
The motion control system is built around several key components:

- [`StepperDriver`](components/stepper_driver.hpp) - Handles individual stepper motor control
  - Real-time step generation
  - Acceleration profiles
  - Microstepping control
  - Direction management

- [`Printer`](components/esp_printer.hpp) - Coordinates overall machine movement
  - Manages multiple stepper motors
  - Handles kinematics calculations
  - Provides high-level movement APIs
  - Coordinates multiple axes

### Temperature Control
Temperature management is handled by several interacting components:

- [`ADC`](components/adc.c) - Manages analog readings
  - High precision ADC configuration
  - Multi-sample averaging
  - Hardware-specific optimizations

- [`Thermistor`](components/thermistor.c) - Converts ADC readings to temperatures
  - Beta calculation algorithm
  - Temperature lookup tables
  - Noise filtering

- [`PID Control`](components/pid.c) - Manages heater output
  - PID algorithm implementation
  - Safety limits enforcement

### Communication & Control
The firmware includes robust communication capabilities:

- [`Serial`](components/esp_serial.cpp) - Handles serial communication
  - Buffered input/output
  - Command parsing
  - Error handling

- [`FileSystem`](components/esp_filesystem.cpp) - Manages onboard storage
  - SD card interface 
  - G-code file handling
  - Configuration storage
  - Firmware updates

## 🛠️ Technical Details

### Architecture
The firmware uses a hybrid architecture combining:
- Real-time task scheduling
- Event-driven command processing
- Object-oriented component design
- Hardware abstraction layers

### Performance
- Smooth acceleration control
- Real-time temperature sampling

### Memory Management
- Static allocation where possible
- Careful stack usage
- Optimized for embedded constraints
- RTOS tasks

## 🔬 Development Tools

- ESP-IDF development framework
- CMake build system
- Protobuf for structured data
- Unit testing framework

## 🎯 Target Applications

This firmware is ideal for:
- CoreXY 3D printers
- CNC machines
- Other precision motion control applications

## 🔮 Future Developments

### Web Interface
A comprehensive web interface is planned to monitor and control prints remotely. This interface will include:
- Real-time print status updates
- Remote start, pause, and stop functionalities
- Live temperature monitoring and adjustment
- G-code file upload and management
- Detailed print logs and error reporting

### Complete SD Card Filesystem
Enhancements to the SD card filesystem will provide full functionality, including:
- Robust file handling (create, read, write, delete)
- Directory management
- Improved G-code file parsing and execution
- Firmware update capabilities directly from the SD card

### Advanced Trajectory Planning
Future updates will include sophisticated trajectory planning algorithms to ensure smooth acceleration on curves. This will involve:
- Advanced kinematic calculations
- Dynamic speed adjustments
- Improved path accuracy
- Reduced mechanical stress on components

## 👨‍💻 For Recruiters

This project demonstrates expertise in:
- Embedded systems development
- Real-time software architecture
- C/C++ programming
- Motion control algorithms
- Digital signal processing
- RTOS concepts
- Hardware interfaces
- Object-oriented design
- Test-driven development

## 🌟 Acknowledgments

Special thanks to the open source community.

