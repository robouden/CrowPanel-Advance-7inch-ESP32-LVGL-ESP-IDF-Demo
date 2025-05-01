# CrowPanel Advance 7" ESP32-S3 LVGL Demo

This project implements a display interface using LVGL (Light and Versatile Graphics Library) on an ESP32-S3 microcontroller. It's designed to work with the CrowPanel Advance 7-inch display.

## Framework Migration

This project has been migrated from ESP-IDF to Arduino framework to simplify maintenance and make updates from EEZ Studio easier. The migration preserves all functionality while making the code more accessible and easier to modify.

### Key Changes

- Switched from ESP-IDF to Arduino framework
- Replaced ESP-IDF logging with Arduino Serial logging
- Adapted display and touch drivers to use TFT_eSPI
- Maintained the same UI structure and functionality
- Preserved FreeRTOS task management for LVGL

## Features

- High-performance LVGL graphics library integration
- Touch screen support with multi-touch capability
- Double buffering for smooth display updates
- PSRAM utilization for better performance
- Interactive UI with buttons and sliders
- Optimized display refresh rates

## Hardware Requirements

- ESP32-S3 microcontroller
- CrowPanel Advance 7-inch display (800x480 resolution)
- I2C touch interface
- PSRAM for display buffering

## Software Requirements

- ESP-IDF v5.0 or later
- LVGL v8.4.0
- PlatformIO (recommended for development)

## Project Structure

```
├── src/
│   ├── main.c              # Main application code
│   ├── display/
│   │   └── esp32_s3.c      # Display and LVGL initialization
│   └── components/
│       └── lvgl/           # LVGL library
```

## Installation

1. Clone the repository:
```bash
git clone [repository-url]
```

2. Install dependencies:
```bash
cd [project-directory]
pio pkg install
```

3. Build the project:
```bash
pio run
```

4. Upload to device:
```bash
pio run -t upload
```

## Configuration

The project uses the following key configurations:

- Display resolution: 800x480
- Color depth: 16-bit (RGB565)
- Buffer size: 40 lines
- Refresh rate: 5ms
- Touch interface: I2C

## Performance Optimizations

- Double buffering implementation
- PSRAM usage for display buffers
- Optimized LVGL task priority
- Reduced refresh intervals
- Efficient memory management

## Future Development

- Integration with EEZ Studio for UI design
- Additional display features
- Touch gesture support
- Custom widget implementations

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- LVGL team for the excellent graphics library
- Espressif for the ESP32-S3 platform
- CrowPanel for the display hardware
