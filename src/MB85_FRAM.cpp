/*! @file MB85_FRAM.cpp
 @section MB85_FRAM_intro_section Description

 Arduino Library Header to access the MB85 Family of SRAM Memories\n\n
See main library header file for details
*/
#include "MB85_FRAM.h"  // Include the header definition

MB85_FRAM_Class::MB85_FRAM_Class() {
  /*!
   * @brief   Class constructor
   * @details Currently empty and unused
   */
}
MB85_FRAM_Class::~MB85_FRAM_Class() {
  /*!
   * @brief   Class destructor
   * @details Currently empty and unused
   */
}
uint32_t MB85_FRAM_Class::totalBytes() {
  /*!
   * @brief   Return the total memory availabe in all MB85 memories
   * @return Total number of bytes available in all memories
   */
  return _TotalMemory;
}  // method "totalBytes()"
uint8_t MB85_FRAM_Class::begin(const uint32_t i2cSpeed) {
  /*!
    @brief   starts communications with the device
    @details There are 4 possible memory sizes, 8kB, 16kB, 32kB, and 64kB and this function will
             automatically determine the size of each memory found. The memories will wrap around
             from the highest-address back to 0 on reads/writes and the detection process makes use
             of this feature. The contents of memory address 0 is saved and overwritten with 0x00.
             Then, starting with the smallest memory, the value of the highest value for that memory
             plus 1 is stored and written as 0xFF. If address 0 has changed to 0xFF then we know
             we've had a wrap-around and have identified the chip, otherwise we repeat the procedure
             for the next possible memory size address and so on.
  @param[in] i2cSpeed I2C Bus speed in Herz
  @return    Number of MB85 devices detected
  */
  Wire.begin();
  Wire.setClock(i2cSpeed);
  for (uint8_t i = MB85_MIN_ADDRESS; i < MB85_MIN_ADDRESS + 8; i++)  // loop all possible addresses
  {
    Wire.beginTransmission(i);
    if (Wire.endTransmission() == 0)  // If no error we have a device at this address
    {
      for (uint16_t memSize = 8192; memSize != 0; memSize = memSize * 2)  // Check each memory size
      {
        // Read the contents of address 0 and store into "minimumByte"
        Wire.beginTransmission(i);                     // Start transmission
        Wire.write((uint8_t)0);                        // Start at MSB address 0
        Wire.write((uint8_t)0);                        // Start at LSB address 0
        _TransmissionStatus = Wire.endTransmission();  // Close transmission
        Wire.requestFrom(i, (uint8_t)1);               // Request 1 byte of data
        uint8_t minimumByte = Wire.read();             // Store value of byte 0

        // Write 0xFF to address 0
        Wire.beginTransmission(i);                     // Start transmission
        Wire.write((uint8_t)0);                        // Start at MSB address 0
        Wire.write((uint8_t)0);                        // Start at LSB address 0
        Wire.write(0xFF);                              // write high value to address 0
        _TransmissionStatus = Wire.endTransmission();  // Close transmission

        // Read the value at address "memSize" to "maximumByte"
        Wire.beginTransmission(i);                     // Start transmission
        Wire.write((uint8_t)(memSize >> 8));           // Write MSB of address
        Wire.write((uint8_t)memSize);                  // Write LSB of address
        _TransmissionStatus = Wire.endTransmission();  // Close transmission
        Wire.requestFrom(i, (uint8_t)1);               // Request 1 byte of data
        uint8_t maximumByte = Wire.read();             // Store value of high byte for chip

        // Write a 0x00 to address "memSize"
        Wire.beginTransmission(i);                     // Start transmission
        Wire.write((uint8_t)(memSize >> 8));           // Write MSB of address
        Wire.write((uint8_t)memSize);                  // Write LSB of address
        Wire.write(0x00);                              // write low value to max address
        _TransmissionStatus = Wire.endTransmission();  // Close transmission

        // Read the value at 0x00
        Wire.beginTransmission(i);                     // Start transmission
        Wire.write((uint8_t)0);                        // Start at MSB address 0
        Wire.write((uint8_t)0);                        // Start at LSB address 0
        _TransmissionStatus = Wire.endTransmission();  // Close transmission
        Wire.requestFrom(i, (uint8_t)1);               // Request 1 byte of data
        uint8_t newMinimumByte = Wire.read();          // Store value of byte 0

        // Write the original value back to memory
        Wire.beginTransmission(i);                     // Start transmission
        Wire.write((uint8_t)(memSize >> 8));           // Write MSB of address
        Wire.write((uint8_t)memSize);                  // Write LSB of address
        Wire.write(maximumByte);                       // restore original value
        _TransmissionStatus = Wire.endTransmission();  // Close transmission

        if (newMinimumByte != 0xFF)                      // Check if the value has changed
        {                                                //
          _I2C[i - MB85_MIN_ADDRESS] = memSize / 1024;   // Store memory size in kB
          _TotalMemory += memSize;                       // Add value to total
          Wire.beginTransmission(i);                     // Start transmission
          Wire.write((uint8_t)0);                        // Write MSB of address
          Wire.write((uint8_t)0);                        // Write LSB of address
          Wire.write(minimumByte);                       // restore original value
          _TransmissionStatus = Wire.endTransmission();  // Close transmission
          break;                                         // Exit the loop
        }                                                // of if-then we've got a wraparound
      }                                                  // of for-next loop for each memory size
      _DeviceCount++;                                    // Increment the found count
    }                                                    // of if-then we have found a device
  }                                                      // of for-next each I2C address loop
  return _DeviceCount;                                   // return number of memories found
}  // of method begin()
uint8_t MB85_FRAM_Class::getDevice(uint32_t &memAddress, uint32_t &endAddress) {
  /*!
    @brief      returns the device index for the given memory addres
    @param[in]  memAddress Memory address to check
    @param[out] endAddress end of memory chip
    @return     device index number
  */
  uint8_t device = 0;
  endAddress     = UINT32_MAX;  // Start at max so adding 1 = 0
  for (device = 0; device < MB85_MAX_DEVICES; device++) {
    if (_I2C[device])  // If there's a memory at address
    {
      endAddress += (uint32_t)_I2C[device] * 1024;  // Compute end of memory chip
      if (endAddress >= memAddress) {
        break;                                      // Exit if we are in range
      }                                             // of if we need to exit loop
      memAddress -= (uint32_t)_I2C[device] * 1024;  // adjust memory address
    }                                               // of if we have a device at address
  }                                                 // of for-next all possible devices
  return device;
}  // of internal method getDevice()
uint32_t MB85_FRAM_Class::memSize(const uint8_t memNumber) {
  /*!
    @brief    returns the device size in Bytes
    @param[in]  memNumber Memory index
    @return     Memory size in Bytes
  */
  if (memNumber <= _DeviceCount) {
    return ((uint32_t)_I2C[memNumber] * 1024);  // Return memory size
  } else {
    return 0;  // return 0 if out of range
  }            // of if-then-else device number in range
}  // of method memSize()
int8_t MB85_FRAM_Class::requestI2C(const uint8_t device, const uint32_t memAddr,
                                   const uint16_t dataSize, const bool endTrans) {
  /*!
    @brief    Request I2C data
    details   Method requestI2C() is an internal call used in the read() template to send the 2 byte
              address and request a number of bytes to be read from that address. The "endTrans"
              parameter specified whether or not to end the transmission. If specified then it is a
              read call, otherwise it is a write and the data follows
  @param[in]  device MB85 Device number
  @param[in]  memAddr Memory address
  @param[in]  dataSize Number of bytes
  @param[in]  endTrans End transmission when set to "true"
  @return     Number of bytes read
  */
  Wire.beginTransmission(device + MB85_MIN_ADDRESS);        // Address the I2C device
  Wire.write(memAddr >> 8);                                 // Send MSB register address
  Wire.write((uint8_t)memAddr);                             // Send LSB address to read
  if (endTrans) {                                           // Read request, so end transmission
    _TransmissionStatus = Wire.endTransmission();           // Close transmission
    Wire.requestFrom(device + MB85_MIN_ADDRESS, dataSize);  // Request n-bytes of data
    return Wire.available();                                // Return actual bytes read
  }                 // of if-then endTransmission switch is set
  return dataSize;  // Return the dataSize on write
}  // of internal method requestI2C()
