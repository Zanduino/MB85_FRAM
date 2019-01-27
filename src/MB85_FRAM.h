/*******************************************************************************************************************
** Class definition header for the Fujitsu MB85_FRAM family of memories. The I2C M85xxx memories are described    **
** at http://www.fujitsu.com/global/products/devices/semiconductor/memory/fram/lineup/index.html and the list is  **
** detailed below:                                                                                                **
**                                                                                                                **
** MB85RC512T 512Kbit ( 64K x 8bit) ManufacturerID 0x00A, Product ID = 0x658 (Density = 0x6)                      **
** MB85RC256V 256Kbit ( 32K x 8bit) ManufacturerID 0x00A, Product ID = 0x510 (Density = 0x5)                      **
** MB85RC128A 128Kbit ( 16K x 8bit) No ManufacturerID/productID or Density values                                 **
** MB85RC64TA  64Kbit (  8K x 8bit) No ManufacturerID/productID or Density values (1.8 to 3.6V)                   **
** MB85RC64A   64Kbit (  8K x 8bit) No ManufacturerID/productID or Density values (2.7 to 3.6V)                   **
** MB85RC64V   64Kbit (  8K x 8bit) No ManufacturerID/productID or Density values (3.0 to 5.5V)                   **
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
** GNU General Public License v3.0                                                                                **
** ===============================                                                                                **
** This program is free software: you can redistribute it and/or modify it under the terms of the GNU General     **
** Public License as published by the Free Software Foundation, either version 3 of the License, or (at your      **
** option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY     **
** WARRANTY; without even the implied warranty of8585 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the   **
** GNU General Public License for more details. You should have received a copy of the GNU General Public License **
** along with this program.  If not, see <http://www.gnu.org/licenses/>.                                          **
**                                                                                                                **
** Vers.  Date       Developer                     Comments                                                       **
** ====== ========== ============================= ============================================================== **
** 1.0.4  2018-07-22 https://github.com/SV-Zanshin Corrected I2C Datatypes                                        **
** 1.0.4  2018-07-08 https://github.com/SV-Zanshin Corrected and cleaned up c++ code formatting                   **
** 1.0.4  2018-07-02 https://github.com/SV-Zanshin Added guard code against multiple I2C Speed definitions        **
** 1.0.4  2018-06-29 https://github.com/SV-Zanshin Issue #3 added support for faster I2C bus speeds               **
** 1.0.2a 2017-09-06 https://github.com/SV-Zanshin Added fillMemory() function as a template                      **
** 1.0.1  2017-09-06 https://github.com/SV-Zanshin Completed testing for large structures                         **
** 1.0.1b 2017-09-06 https://github.com/SV-Zanshin Allow structures > 32 bytes, optimized memory use              **
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
  #ifndef FRAM_I2C_MODES                                                           // I2C related constants            //
    #define FRAM_I2C_MODES                                                         // Guard code to prevent multiple   //
    const uint32_t I2C_STANDARD_MODE       =  100000;                         // Default normal I2C 100KHz speed  //
    const uint32_t I2C_FAST_MODE           =  400000;                         // Fast mode                        //
    const uint32_t I2C_FAST_MODE_PLUS_MODE = 1000000;                         // Really fast mode                 //
    const uint32_t I2C_HIGH_SPEED_MODE     = 3400000;                         // Turbo mode                       //
  #endif                                                                      //----------------------------------//
  const uint8_t MB85_MIN_ADDRESS           =    0x50;                         // Minimum FRAM address             //
  const uint8_t MB85_MAX_DEVICES           =       8;                         // Maximum number of FRAM devices   //

  /*****************************************************************************************************************
  ** Main MB85_FRAM class for the SRAM memory                                                                     **
  *****************************************************************************************************************/
  class MB85_FRAM_Class {                                                     // Class definition                 //
    public:                                                                   // Publicly visible methods         //
      MB85_FRAM_Class();                                                      // Class constructor                //
      ~MB85_FRAM_Class();                                                     // Class destructor                 //
      uint8_t begin(const uint32_t i2cSpeed = I2C_STANDARD_MODE);             // Start using I2C Communications   //
      uint32_t totalBytes();                                                  // Return the total memory available//
      uint32_t memSize(const uint8_t memNumber);                              // Return memory size in bytes      //
      /*************************************************************************************************************
      ** Declare the read method as a template function, this needs to be done in the header file rather than by  **
      ** declaring the prototype here and putting the body into the cpp library file. All reading is done through **
      ** this function. Multiple memories are treated as if they were one large memory, the space is contiguous   **
      ** and a read will wrap around from the end of memory to the beginning. The function can be called with any **
      ** type of argument as the "&value", including arrays and structures. Although the I2C "wire" has a 32 byte **
      ** buffer limit, this library will automatically split bigger reads into multiple calls. The size of the    **
      ** "&value" parameter is returned as the optional function value                                            **
      *************************************************************************************************************/
      template< typename T > uint8_t &read(const uint32_t addr,T &value) {    // method to read a structure       //
        uint8_t* bytePtr      = (uint8_t*)&value;                             // Pointer to structure beginning   //
        uint8_t  structSize   = sizeof(T);                                    // Number of bytes in structure     //
        uint32_t memAddress   = addr%_TotalMemory;                            // Ensure no value greater than max //
        uint32_t endAddress   = 0;                                            // Last address on current memory   //
        uint8_t  device       = getDevice(memAddress,endAddress);             // Compute the actual device to use //
        for (uint8_t i=0;i<structSize;i++) {                                  // loop for each byte to be read    //
          if(i%(BUFFER_LENGTH-2)==0) {                                        // If the buffer is full, then we   //
            requestI2C(device,memAddress,structSize,true);                    // Get data from memory             //
          } // if our read buffer is full                                     //                                  //
          *bytePtr++ = Wire.read();                                           // Put byte read to pointer address //
          if(memAddress++==endAddress) {                                      // If we've reached the end-of-chip //
            for(uint8_t j=0;j<MB85_MAX_DEVICES;j++) {                         // loop to get the next device      //
              if (device++==MB85_MAX_DEVICES) {                               // Increment device or start at 0   //
                device = 0;                                                   //                                  //
              } // of if-then we've reached the end of device list            //                                  //
              if (_I2C[device]) {                                             // On a match, address device       //
                requestI2C(device,0,structSize,true);                         // Get bytes from new memory chip   //
                memAddress = 0;                                               // New memory address               //
                break;                                                        // And stop looking for a new memory//
              } // of if we've got the next memory                            //                                  //
            } // of for-next loop through each device                         //                                  //
          } // of if-then we've reached the end of the physical memory        //                                  //
        } // of loop for each byte //                                         //                                  //
        return(structSize);                                                   // return the number of bytes read  //
      } // of method read()                                                   //----------------------------------//
      
      /*************************************************************************************************************
      ** Declare the write method as a template function, this needs to be done in the header file rather than by **
      ** declaring the prototype here and putting the body into the cpp library file. All writing is done through **
      ** this function. Multiple memories are treated as if they were one large memory, the space is contiguous   **
      ** and a write will wrap around from the end of memory to the beginning. The function can be called with    **
      ** any type of argument as the "&value", including arrays and structures. Although the I2C "wire" has a 32  **
      ** byte buffer limit, this library will automatically split bigger writes into multiple calls. The size of  **
      ** the "&value" parameter is returned as the optional function value                                        **
      *************************************************************************************************************/
      template<typename T>uint8_t &write(const uint32_t addr,const T &value) {// method to write a structure      //
        const uint8_t* bytePtr = (const uint8_t*)&value;                      // Pointer to structure beginning   //
        uint8_t  structSize   = sizeof(T);                                    // Number of bytes in structure     //
        uint32_t memAddress   = addr%_TotalMemory;                            // Ensure no value greater than max //
        uint32_t endAddress   = 0;                                            // Last address on current memory   //
        uint8_t device        = getDevice(memAddress,endAddress);             // Compute the actual device to use //
        for (uint8_t i=0;i<structSize;i++) {                                  // loop for each byte to be written //
          if(i%(BUFFER_LENGTH-2)==0) {                                        // Check if end of buffer reached   //
            if(i>0) {                                                         // if we've reached wire lib buffer //
              _TransmissionStatus = Wire.endTransmission();                   // limit then close active xmission //
            } // of if-then end of buffer reached                             //                                  // 
            requestI2C(device,memAddress,structSize,false);                   // Position for next buffer data    //
          } // if our write buffer is full                                    //                                  //
          Wire.write(*bytePtr++);                                             // Write current byte to memory     //
          if(memAddress++==endAddress) {                                      // If we've reached the end-of-chip //
            _TransmissionStatus = Wire.endTransmission();                     // Close transmission               //
            for(uint8_t j=0;j<MB85_MAX_DEVICES;j++) {                         // loop to get the next device      //
              if (device++==MB85_MAX_DEVICES) {                               // If at last device then restart   //
                device = 0;                                                   // at 0                             //
              } // of if-then reached end of devices                          //                                  //                
              if (_I2C[device]) {                                             // On a match, address device       //
                requestI2C(device,0,structSize,false);                        // Position memory pointer to begin //
                memAddress = 0;                                               // New memory address               //
                break;                                                        // And stop looking for a new memory//
              } // of if we've got the next memory                            //                                  //
            } // of for-next loop through each device                         //                                  //
          } // of if-then we've reached the end of the physical memory        //                                  //
        } // of for each byte to write                                        //                                  //
        _TransmissionStatus = Wire.endTransmission();                         // Close transmission               //
        return(structSize);                                                   // return number of bytes written   //
      } // of method write()                                                  //----------------------------------//
      
      /*************************************************************************************************************
      ** Declare the fillMemory() method to write as many copies of the "&value" parameter as will fit into the   **
      ** the available memory space. The bigger the "&value" datatype is, the faster this call will function. Any **
      ** extra bytes left over if the memory is not divisible by the length of the "&data" is left untouched.     **
      *************************************************************************************************************/
      template<typename T>uint32_t &fillMemory(const T &value) {              // Fill memory with repeated values //
        uint8_t  structSize = sizeof(T);                                      // Number of bytes in structure     //
        uint32_t i;                                                           // Declare loop counter and return  //
        for ( i=0; i<(_TotalMemory/structSize); i++) {                        // Loop number of times that it fits//
          write(i*structSize,value);                                          // into memory                      //
        } // of for-next loop on structure size                               //                                  //
        return i;                                                             // return the number of copies made //
      } // of method fillMemory()                                             //----------------------------------//
      
    private:                                                                  // -------- Private methods ------- //
      uint8_t getDevice(uint32_t &memAddress, uint32_t &endAddress);          // Compute actual device to use     //
      void    requestI2C(const uint8_t device,const uint32_t memAddress,      // Address device and request data  //
                         const uint16_t dataSize, const bool endTrans);       //                                  //
      uint8_t  _DeviceCount           =     0;                                // Number of memories found         //
      uint32_t _TotalMemory           =     0;                                // Number of bytes in total         //
      uint8_t  _I2C[MB85_MAX_DEVICES] =   {0};                                // List of device kB capacities     //
      bool     _TransmissionStatus    = false;                                // I2C communications status        //
  }; // of MB85_FRAM class definition                                         //                                  //
#endif                                                                        //----------------------------------//
