/*******************************************************************************************************************
** Class definition header for the Fujitsu MB85_FRAM family of memories. The I2C M85xxx memories are described    **
** at http://www.fujitsu.com/global/products/devices/semiconductor/memory/fram/lineup/index.html and the list is  **
** detailed below:                                                                                                **
**                                                                                                                **
** MB85RC512T 512Kbit ( 64K x 8bit) ManufacturerID 0x00A, Product ID = 0x658 (Density = 0x6)                      **
** MB85RC256V 256Kbit ( 32K x 8bit) ManufacturerID 0x00A, Product ID = 0x510 (Density = 0x5)                      **
** MB85RC128A 128Kbit ( 16K x 8bit) No ManufacturerID/productID or Density values                                 **
** MB85RC64TA  64Kbit (  8K x 8bit) No ManufacturerID/productID or Density values                                 **
** MB85RC64A   64Kbit (  8K x 8bit) No ManufacturerID/productID or Density values                                 **
** MB85RC64V   64Kbit (  8K x 8bit) No ManufacturerID/productID or Density values                                 **
** - unsupported memories --                                                                                      **
** MB85RC1MT    1Mbit (128K x 8bit) ManufacturerID 0x00A, Product ID = 0x758 (Density = 0x7)        (unsupported) **
** MB85RC16    16Kbit (  2K x 8bit) No ManufacturerID/productID or Density values  1 Address byte   (unsupported) **
** MB85RC16V   16Kbit (  2K x 8bit) No ManufacturerID/productID or Density values  1 Address byte   (unsupported) **
** MB85RC04V    4Kbit ( 512 x 8bit) No ManufacturerID/productID or Density values  1 Address byte   (unsupported) **
**                                                                                                                **
** There is no direct means of identifying the various chips (apart from the top 3 in the list), so a software    **
** method is used which makes use of the fact that writing past the end of memory automatically wraps back around **
** to the beginning. Thus if we write something 1 byte past the end of a chip's address range then byte 0 of the  **
** memory will have changed.                                                                                      **
**                                                                                                                **
** Although programming for the Arduino and in c/c++ is new to me, I'm a professional programmer and have learned,**
** over the years, that it is much easier to ignore superfluous comments than it is to decipher non-existent ones;**
** so both my comments and variable names tend to be verbose. The code is written to fit in the first 80 spaces   **
** and the comments start after that and go to column 117 - allowing the code to be printed in A4 landscape mode. **
**                                                                                                                **
** This program is free software: you can redistribute it and/or modify it under the terms of the GNU General     **
** Public License as published by the Free Software Foundation, either version 3 of the License, or (at your      **
** option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY     **
** WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the   **
** GNU General Public License for more details. You should have received a copy of the GNU General Public License **
** along with this program.  If not, see <http://www.gnu.org/licenses/>.                                          **
**                                                                                                                **
** Vers.  Date       Developer                     Comments                                                       **
** ====== ========== ============================= ============================================================== **
** 1.0.0b 2017-09-04 https://github.com/SV-Zanshin Prepared for release, final testing                            **
** 1.0.0a 2017-08-27 https://github.com/SV-Zanshin Started coding                                                 **
**                                                                                                                **
*******************************************************************************************************************/
#include "Arduino.h"                                                          // Arduino data type definitions    //
#include <Wire.h>                                                             // Standard I2C "Wire" library      //
#ifndef MB85_FRAM                                                             // Guard code definition            //
  #define MB85_FRAM                                                           // Define the name inside guard code//
  /*****************************************************************************************************************
  ** Declare constants used in the class                                                                          **
  *****************************************************************************************************************/
  const uint8_t MB85_MIN_ADDRESS        = 0x50;                               // Minimum FRAM address             //
  const uint8_t MB85_MAX_DEVICES        =    8;                               // Maximum number of FRAM devices   //

  /*****************************************************************************************************************
  ** Main MB85_FRAM class for the temperature / humidity / pressure sensor                                        **
  *****************************************************************************************************************/
  class MB85_FRAM_Class {                                                     // Class definition                 //
    public:                                                                   // Publicly visible methods         //
      MB85_FRAM_Class();                                                      // Class constructor                //
      ~MB85_FRAM_Class();                                                     // Class destructor                 //
      uint8_t begin();                                                        // Start using I2C Communications   //
      uint32_t totalBytes();                                                  // Return the total memory available//
      uint16_t memSize(const uint8_t memNumber);                              // Return memory size in bytes      //
      /*************************************************************************************************************
      ** Declare the read and write methods as template functions. All device I/O is done through these two       **
      ** functions. If multiple memories have been found they are treated as if they were just one large memory,  **
      ** the read and write methods take care of calls that span multiple devices. The two functions are declared **
      ** as template functions and thus need to be defined in this header rather than in the function body.       **
      ** As templates they determine the size of structure to be read or written at compile time. Both return the **
      ** data structure's size in bytes.                                                                          **
      *************************************************************************************************************/
      template< typename T > uint8_t &read(const uint32_t addr,T &value) {    // method to read a structure       //
        uint8_t* bytePtr      = (uint8_t*)&value;                             // Pointer to structure beginning   //
        uint8_t  structSize   = sizeof(T);                                    // Number of bytes in structure     //
        uint32_t memAddress   = addr%_TotalMemory;                            // Ensure no value greater than max //
        uint32_t endAddress   = 0;                                            // Last address on current memory   //
        uint8_t device        = getDevice(memAddress,endAddress);             // Compute the actual device to use //
        Wire.beginTransmission(device+MB85_MIN_ADDRESS);                      // Address the I2C device           //
        Wire.write(memAddress>>8);                                            // Send MSB register address        //
        Wire.write((uint8_t)memAddress);                                      // Send LSB address to read         //
        _TransmissionStatus = Wire.endTransmission();                         // Close transmission               //
        Wire.requestFrom(device+MB85_MIN_ADDRESS, sizeof(T));                 // Request n-bytes of data          //
        structSize = Wire.available();                                        // Use the actual number of bytes   //
        for (uint8_t i=0;i<structSize;i++) {                                  // loop for each byte to be read    //
          *bytePtr++ = Wire.read();                                           // Put byte read to pointer address //
          if(memAddress++==endAddress) {                                      // If we've reached the end-of-chip //
            for(uint8_t j=0;j<MB85_MAX_DEVICES;j++) {                         // loop to get the next device      //
              if (device++==MB85_MAX_DEVICES) device = 0;                     // Increment device or start at 0   //
              if (_I2C[device]) {                                             // On a match, address device       //
                Wire.beginTransmission(device+MB85_MIN_ADDRESS);              // Address the I2C device           //
                Wire.write((uint8_t)0);                                       // Send MSB register address        //
                Wire.write((uint8_t)0);                                       // Send LSB address to read         //
                _TransmissionStatus = Wire.endTransmission();                 // Close transmission               //
                Wire.requestFrom(device+MB85_MIN_ADDRESS, sizeof(T));         // Request n-bytes of data          //
                memAddress = 1;                                               // New memory address               //
                break;                                                        // And stop looking for a new memory//
              } // of if we've got the next memory                            //                                  //
            } // of for-next loop through each device                         //                                  //
          } // of if-then we've reached the end of the physical memory        //                                  //
        } // of loop for each byte //                                         //                                  //
        return(structSize);                                                   // return the number of bytes read  //
      } // of method read()                                                   //----------------------------------//
      template<typename T>uint8_t &write(const uint32_t addr,const T &value) {// method to write a structure      //
        const uint8_t* bytePtr = (const uint8_t*)&value;                      // Pointer to structure beginning   //
        uint8_t  structSize   = sizeof(T);                                    // Number of bytes in structure     //
        uint32_t memAddress   = addr%_TotalMemory;                            // Ensure no value greater than max //
        uint32_t endAddress   = 0;                                            // Last address on current memory   //
        uint8_t device        = getDevice(memAddress,endAddress);             // Compute the actual device to use //
        Wire.beginTransmission(device+MB85_MIN_ADDRESS);                      // Address the I2C device           //
        Wire.write(memAddress>>8);                                            // Send MSB register address        //
        Wire.write((uint8_t)memAddress);                                      // Send LSB address to read         //
        for (uint8_t i=0;i<sizeof(T);i++) {                                   // loop for each byte to be written //
          Wire.write(*bytePtr++);                                             // Write current byte to memory     //
          if(memAddress++==endAddress) {                                      // If we've reached the end-of-chip //
            _TransmissionStatus = Wire.endTransmission();                     // Close transmission               //
            for(uint8_t j=0;j<MB85_MAX_DEVICES;j++) {                         // loop to get the next device      //
              if (device++==MB85_MAX_DEVICES) device = 0;                     // Increment device or start at 0   //
              if (_I2C[device]) {                                             // On a match, address device       //
                Wire.beginTransmission(device+MB85_MIN_ADDRESS);              // Address the I2C device           //
                Wire.write((uint8_t)0);                                       // Send MSB register address        //
                Wire.write((uint8_t)0);                                       // Send LSB address to read         //
                memAddress = 1;                                               // New memory address               //
                break;                                                        // And stop looking for a new memory//
              } // of if we've got the next memory                            //                                  //
            } // of for-next loop through each device                         //                                  //
          } // of if-then we've reached the end of the physical memory        //                                  //
        } // of for each byte to write                                        //                                  //
        _TransmissionStatus = Wire.endTransmission();                         // Close transmission               //
        return(structSize);                                                   // return number of bytes written   //
      } // of method write()                                                  //----------------------------------//
    private:                                                                  // -------- Private methods ------- //
      uint8_t getDevice(uint32_t &memAddress, uint32_t &endAddress);          // Compute actual device to use     //
      uint8_t  _DeviceCount           =     0;                                // Number of memories found         //
      uint32_t _TotalMemory           =     0;                                // Number of bytes in total         //
      uint8_t  _I2C[MB85_MAX_DEVICES] =   {0};                                // List of device kB capacities     //
      bool     _TransmissionStatus    = false;                                // I2C communications status        //
  }; // of MB85_FRAM class definition                                         //                                  //
#endif                                                                        //----------------------------------//
