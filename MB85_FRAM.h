/*******************************************************************************************************************
** Class definition header for the Fujitsu MB85_FRAM family of memories.                                          **
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
** Vers.  Date       Developer           Comments                                                                 **
** ====== ========== =================== ======================================================================== **
** 1.0.0a 2017-08-27 Arnd@SV-Zanshin.Com Started coding                                                           **
**                                                                                                                **
*******************************************************************************************************************/
#include "Arduino.h"                                                          // Arduino data type definitions    //
#include <Wire.h>                                                             // Standard I2C "Wire" library      //
#ifndef MB85_FRAM                                                             // Guard code definition            //
  #define MB85_FRAM                                                           // Define the name inside guard code//
  /*****************************************************************************************************************
  ** Declare constants used in the class                                                                          **
  *****************************************************************************************************************/
  const uint8_t MB85_MAX_DEVICES        =    8;                               // Maximum number of FRAM devices   //
  const uint8_t MB85_MIN_ADDRESS        = 0x50;                               // Minimum FRAM address             //
  const uint8_t MB85_SLAVE_ID_ADDRESS   = 0xF8;                               // Slave ID address                 //

  /*****************************************************************************************************************
  ** Declare enumerated types used in the class                                                                   **
  *****************************************************************************************************************/
  /*****************************************************************************************************************
  ** Main MB85_FRAM class for the temperature / humidity / pressure sensor                                        **
  *****************************************************************************************************************/
  class MB85_FRAM_Class {                                                     // Class definition                 //
    public:                                                                   // Publicly visible methods         //
      MB85_FRAM_Class();                                                      // Class constructor                //
      ~MB85_FRAM_Class();                                                     // Class destructor                 //
      bool     begin();                                                       // Start using I2C Communications   //
    private:                                                                  // -------- Private methods ------- //
      uint8_t _DeviceCount =0;                                                // Number of memories found         //
      uint8_t _I2C[MB85_MAX_DEVICES]={0};                                     // List of device addresses         //
      uint8_t _MemSize[MB85_MAX_DEVICES]={0};                                 // Memory size in KB                //
      bool     _TransmissionStatus = false;                                   // I2C communications status        //

      /*************************************************************************************************************
      ** Declare the getData and putData methods as template functions. All device I/O is done through these two  **
      ** functions. The two functions are designed so that only the device, the address and a variable are passed **
      ** in and the functions determine the size of the parameter variable and reads or writes that many bytes.   **
      ** So if a read is called using a character array[10] then 10 bytes are read, if called with a int8 then    **
      ** only one byte is read. The return value, if used, is the number of bytes read or written                 **
      ** This is done by using template function definitions which need to be defined in this header file rather  **
      ** than in the c++ program library file.                                                                    **
      *************************************************************************************************************/
      template< typename T > uint8_t &getData(const uint8_t device,           // method to write a structure      //
                                              const uint16_t addr,T &value) { //                                  //
        uint8_t* bytePtr    = (uint8_t*)&value;                               // Pointer to structure beginning   //
        uint8_t  structSize = sizeof(T);                                      // Number of bytes in structure     //
        if (device>=_DeviceCount) return;                                     // Ignore out of bounds indices     //
        if (_I2C[device]==0) return;                                          // Ignore non-existent devices      //
        Wire.beginTransmission(device+MB85_MIN_ADDRESS);                      // Address the I2C device           //
        Wire.write(addr>>8);                                                  // Send MSB register address        //
        Wire.write((uint8_t)addr);                                            // Send LSB address to read         //
        _TransmissionStatus = Wire.endTransmission();                         // Close transmission               //
        Wire.requestFrom(device+MB85_MIN_ADDRESS, sizeof(T));                 // Request 1 byte of data           //
        structSize = Wire.available();                                        // Use the actual number of bytes   //
        for (uint8_t i=0;i<structSize;i++) *bytePtr++ = Wire.read();          // loop for each byte to be read    //
        return(structSize);                                                   // return the number of bytes read  //
      } // of method getData()                                                //----------------------------------//
      template<typename T>uint8_t &putData(const uint8_t device,              // method to write a structure      //
                                           const uint16_t addr,const T &value){//                                  //
        const uint8_t* bytePtr = (const uint8_t*)&value;                      // Pointer to structure beginning   //
        uint8_t  structSize   = sizeof(T);                                    // Number of bytes in structure     //
        if (device>=_DeviceCount) return;                                     // Ignore out of bounds indices     //
        if (_I2C[device]==0) return;                                          // Ignore non-existent devices      //
        Wire.beginTransmission(device+MB85_MIN_ADDRESS);                      // Address the I2C device           //
        Wire.write(addr>>8);                                                  // Send MSB register address        //
        Wire.write((uint8_t)addr);                                            // Send LSB address to read         //
        for (uint8_t i=0;i<sizeof(T);i++) Wire.write(*bytePtr++);             // loop for each byte to be written //
        _TransmissionStatus = Wire.endTransmission();                         // Close transmission               //
        return(structSize);                                                   // return number of bytes written   //
      } // of method putData()                                                //----------------------------------//
  }; // of MB85_FRAM class definition                                         //                                  //
#endif                                                                        //----------------------------------//

