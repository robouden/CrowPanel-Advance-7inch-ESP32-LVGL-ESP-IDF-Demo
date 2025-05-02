// This file allows selecting different User_Setup configurations
// without editing library files. This makes it easier to switch
// between different hardware setups.

// Include the User_Setup.h file directly from the project
#if __has_include(<User_Setup.h>)
  #include <User_Setup.h>
#else
  #if defined(USING_ARDUINO_FRAMEWORK)
    // Custom setup for CrowPanel display when using Arduino framework
    // RGB display with parallel interface
    
    // Define the pins used by our ESP32-S3 parallel TFT interface
    #define ESP32_PARALLEL

    // The parallel interface based on ESP32-S3
    #define TFT_CS    10  // Chip select control pin (library pulls permanently low)
    #define TFT_DC     9  // Data Command control pin - must use a pin in the range 0-31
    #define TFT_RST    8  // Reset pin
    
    // Parallel interface setup for RGB LCD
    #define TFT_WR     7  // Write strobe control pin - must use a pin in the range 0-31
    #define TFT_RD     6  // Read strobe control pin
    
    // For 8-bit parallel displays, these carry the data to the display
    #define TFT_D0    21  // Must use pins in the range 0-31 for the data bus
    #define TFT_D1    22  // so a single register write sets/clears all bits
    #define TFT_D2    23
    #define TFT_D3    24
    #define TFT_D4    25
    #define TFT_D5    26
    #define TFT_D6    27
    #define TFT_D7    28
    
    // Display parameters
    #define TFT_WIDTH  800
    #define TFT_HEIGHT 480
    
    // Use the ESP32-S3 DMA option for faster screen updates
    #define DMA_ENABLE
    
    // Specify the display controller
    #define ILI9488_DRIVER  // RGB LCD likely uses ILI9488 or similar
    
    // Backlight control
    #define TFT_BL   40  // LED back-light control pin (on ESP32-S3)
    #define TFT_BACKLIGHT_ON HIGH  // Level to turn backlights on
    
    // Use the 16-bit color depth
    #define COLOR_DEPTH 16
  #else
    // For ESP-IDF builds, use a default configuration
    #include <User_Setups/Setup2_ST7735.h>
  #endif
#endif 