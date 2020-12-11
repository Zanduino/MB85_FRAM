/*! @file MB85_FRAM.h

 @mainpage Arduino Library Header to access the MB85 Family of SRAM Memories

 @section intro_section Description

Class definition header for the Fujitsu MB85_FRAM family of memories. The I2C M85xxx memories are described at
http://www.fujitsu.com/global/products/devices/semiconductor/memory/fram/lineup/index.html and the list is
detailed below:\n\n

MB85RC512T 512Kbit ( 64K x 8bit) ManufacturerID 0x00A, Product ID = 0x658 (Density = 0x6)\n
MB85RC256V 256Kbit ( 32K x 8bit) ManufacturerID 0x00A, Product ID = 0x510 (Density = 0x5)\n
MB85RC128A 128Kbit ( 16K x 8bit) No ManufacturerID/productID or Density values\n
MB85RC64TA  64Kbit (  8K x 8bit) No ManufacturerID/productID or Density values (1.8 to 3.6V)\n
MB85RC64A   64Kbit (  8K x 8bit) No ManufacturerID/productID or Density values (2.7 to 3.6V)\n
MB85RC64V   64Kbit (  8K x 8bit) No ManufacturerID/productID or Density values (3.0 to 5.5V)\n\n
- unsupported memories --\n\n
MB85RC1MT    1Mbit (128K x 8bit) ManufacturerID 0x00A, Product ID = 0x758 (Density = 0x7)        (unsupported)\n
MB85RC16    16Kbit (  2K x 8bit) No ManufacturerID/productID or Density values  1 Address byte   (unsupported)\n
MB85RC16V   16Kbit (  2K x 8bit) No ManufacturerID/productID or Density values  1 Address byte   (unsupported)\n
MB85RC04V    4Kbit ( 512 x 8bit) No ManufacturerID/productID or Density values  1 Address byte   (unsupported)\n\n

There is no direct means of identifying the various chips (apart from the top 3 in the list), so a software method
is used which makes use of the fact that writing past the end of memory automatically wraps back around to the
beginning. Thus if we write something 1 byte past the end of a chip's address range then byte 0 of the memory will
have changed.

@section MicroChipSRAMlicense GNU General Public License v3.0
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation, either version 3 of the License, or (at your
option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details. You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

@section author Author

Written by Arnd\@SV-Zanshin

@section versions Changelog

Version| Date       | Developer  | Comments
------ | ---------- | ---------- | --------
1.0.5  | 2019-01-26 | SV-Zanshin | Issue #4 - converted documentation to doxygen
1.0.4  | 2018-07-22 | SV-Zanshin | Corrected I2C Datatypes
1.0.4  | 2018-07-08 | SV-Zanshin | Corrected and cleaned up c++ code formatting
1.0.4  | 2018-07-02 | SV-Zanshin | Added guard code against multiple I2C Speed definitions
1.0.4  | 2018-06-29 | SV-Zanshin | Issue #3 added support for faster I2C bus speeds
1.0.2a | 2017-09-06 | SV-Zanshin | Added fillMemory() function as a template
1.0.1  | 2017-09-06 | SV-Zanshin | Completed testing for large structures
1.0.1b | 2017-09-06 | SV-Zanshin | Allow structures > 32 bytes, optimized memory use
1.0.0b | 2017-09-04 | SV-Zanshin | Prepared for release, final testing
1.0.0a | 2017-08-27 | SV-Zanshin | Started coding
*/

#include "Arduino.h" // Arduino data type definitions
#include <Wire.h>    // Standard I2C "Wire" library
#ifndef MB85_FRAM
/** @brief  Guard code to prevent multiple definitions of the class*/
#define MB85_FRAM
  /*****************************************************************************************************************
  ** Declare constants used in the class                                                                          **
  *****************************************************************************************************************/
  #ifndef FRAM_I2C_MODES
  /** @brief  Guard code to prevent multiple definitions of I2C modes */
#define FRAM_I2C_MODES
#ifndef BUFFER_LENGTH
  /** @brief  Define I2C Buffer length if not specified in library */
  #define BUFFER_LENGTH 32
#endif
    const uint32_t I2C_STANDARD_MODE       =  100000; ///< Default normal I2C 100KHz speed
    const uint32_t I2C_FAST_MODE           =  400000; ///< Fast mode
    const uint32_t I2C_FAST_MODE_PLUS_MODE = 1000000; ///< Really fast mode
    const uint32_t I2C_HIGH_SPEED_MODE     = 3400000; ///< Turbo mode
  #endif
  const uint8_t MB85_MIN_ADDRESS           =    0x50; ///< Minimum FRAM address
  const uint8_t MB85_MAX_DEVICES           =       8; ///< Maximum number of FRAM devices

  /*****************************************************************************************************************
  ** Main MB85_FRAM class for the SRAM memory                                                                     **
  *****************************************************************************************************************/
  /*!
  * @class   MB85_FRAM_Class
  * @brief   Access the MB85_FRAM Family of memories
  */
  class MB85_FRAM_Class
  {
    public:
      MB85_FRAM_Class();
      ~MB85_FRAM_Class();
      uint8_t begin(const uint32_t i2cSpeed = I2C_STANDARD_MODE);
      uint32_t totalBytes();
      uint32_t memSize(const uint8_t memNumber);
/*!
* @brief     Declare the read method as a template function
* @details   Declare the read method as a template function, this needs to be done in the header file rather than by
*            declaring the prototype here and putting the body into the cpp library file. All reading is done through
*            this function. Multiple memories are treated as if they were one large memory, the space is contiguous
*            and a read will wrap around from the end of memory to the beginning. The function can be called with any
*            type of argument as the "&value", including arrays and structures. Although the I2C "wire" has a 32 byte
*            buffer limit, this library will automatically split bigger reads into multiple calls. The size of the
*           "&value" parameter is returned as the optional function value
* @param[in] addr Memory address
* @param[in] value Data Type "T" to read
* @return    Number of bytes read
*/
      template< typename T > uint8_t &read(const uint32_t addr,T &value)
      {
        uint8_t* bytePtr            = (uint8_t*)&value;                 // Pointer to structure beginning
        static uint8_t structSize   = sizeof(T);                        // Number of bytes in structure
        uint32_t memAddress         = addr%_TotalMemory;                // Ensure no value greater than max
        uint32_t endAddress         = 0;                                // Last address on current memory
        uint8_t  device             = getDevice(memAddress,endAddress); // Compute the actual device to use
        for (uint8_t i=0;i<structSize;i++)
        {
          if(i%(BUFFER_LENGTH-2)==0)
          {
            requestI2C(device,memAddress,structSize,true); // If the buffer is full, then we get data from memory
          } // if our read buffer is full
          *bytePtr++ = Wire.read();    // Put byte read to pointer address
          if(memAddress++==endAddress) // If we've reached the end-of-chip
          {
            for(uint8_t j=0;j<MB85_MAX_DEVICES;j++) // loop to get the next device
            {
              if (device++==MB85_MAX_DEVICES)
              {
                device = 0;
              } // of if-then we've reached the end of device list
              if (_I2C[device]) // On a match, address device
              {
                requestI2C(device,0,structSize,true); // Get bytes from new memory chip
                memAddress = 0;                       // New memory address
                break;                                // And stop looking for a new memory
              } // of if we've got the next memory
            } // of for-next loop through each device
          } // of if-then we've reached the end of the physical memory
        } // of loop for each byte
        return(structSize); // return the number of bytes read
      } // of method read()

/*!
* @brief     Declare the write method as a template function
* @details   Declare the write method as a template function, this needs to be done in the header file rather than by
*            declaring the prototype here and putting the body into the cpp library file. All reading is done through
*            this function. Multiple memories are treated as if they were one large memory, the space is contiguous
*            and a read will wrap around from the end of memory to the beginning. The function can be called with any
*            type of argument as the "&value", including arrays and structures. Although the I2C "wire" has a 32 byte
*            buffer limit, this library will automatically split bigger reads into multiple calls. The size of the
*           "&value" parameter is returned as the optional function value
* @param[in] addr Memory address
* @param[in] value Data Type "T" to write
* @return    Number of bytes written
*/
      template<typename T>uint8_t &write(const uint32_t addr,const T &value)
      {
        const uint8_t* bytePtr       = (const uint8_t*)&value;           // Pointer to structure beginning
        static uint8_t  structSize   = sizeof(T);                        // Number of bytes in structure
        uint32_t memAddress          = addr%_TotalMemory;                // Ensure no value greater than max
        uint32_t endAddress          = 0;                                // Last address on current memory
        uint8_t device               = getDevice(memAddress,endAddress); // Compute the actual device to use
        for (uint8_t i=0;i<structSize;i++)
        {
          if(i%(BUFFER_LENGTH-2)==0)
          {
            if(i>0)
            {
              _TransmissionStatus = Wire.endTransmission(); // Close active xmission
            } // of if-then end of buffer reached
            requestI2C(device,memAddress,structSize,false); // Position for next buffer data
          } // if our write buffer is full
          Wire.write(*bytePtr++); // Write current byte to memory
          if(memAddress++==endAddress)
          {
            _TransmissionStatus = Wire.endTransmission(); // Close transmission
            for(uint8_t j=0;j<MB85_MAX_DEVICES;j++)       // loop to get the next device
            {
              if (device++==MB85_MAX_DEVICES) // If at last device then restart
              {
                device = 0;
              } // of if-then reached end of devices
              if (_I2C[device]) // If at last device then restart
              {
                requestI2C(device,0,structSize,false); // Position memory pointer to begin
                memAddress = 0;                        // New memory address
                break;                                 // And stop looking for a new memory
              } // of if we've got the next memory
            } // of for-next loop through each device
          } // of if-then we've reached the end of the physical memory
        } // of for each byte to write
        _TransmissionStatus = Wire.endTransmission(); // Close transmission
        return(structSize);                           // return number of bytes written
      } // of method write()

/*!
* @brief     Declare the fillMemory() method to write as many copies of the "&value" parameter as will fit into the
*            the available memory space
* @details   The bigger the "&value" datatype is, the faster this call will function. Any extra bytes left over if
*            the memory is not divisible by the length of the "&data" is left untouched
* @param[in] value Data Type "T" to fill memory with
* @return    Number of loop iterations used to fill memory
*/
      template<typename T>uint32_t &fillMemory(const T &value)
      {
        uint8_t  structSize = sizeof(T); // Number of bytes in structure
        uint32_t i;
        for ( i=0; i<(_TotalMemory/structSize); i++)
        {
          write(i*structSize,value); // Write structure to memory
        } // of for-next loop on structure size
        return i; // return the number of copies written
      } // of method fillMemory()

    private:
      uint8_t   getDevice(uint32_t &memAddress, uint32_t &endAddress);
      int8_t    requestI2C(const uint8_t device,const uint32_t memAddress, const uint16_t dataSize, const bool endTrans);       //                                  //
      uint8_t  _DeviceCount           =     0; ///< Number of memories found
      uint32_t _TotalMemory           =     0; ///< Number of bytes in total
      uint8_t  _I2C[MB85_MAX_DEVICES] =   {0}; ///< List of device kB capacities
      bool     _TransmissionStatus    = false; ///< I2C communications status
  }; // of MB85_FRAM class definition
#endif
