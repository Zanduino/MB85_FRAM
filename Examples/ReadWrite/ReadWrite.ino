/*******************************************************************************************************************
** Example program for reading and writing to one or more Fujitsu SRAM memory devices on the I2C microLAN. FRAM   **
** memory is static RAM which offers many Read/Write cycles and the description of the MB85 family is found at    **
** http://www.fujitsu.com/global/products/devices/semiconductor/memory/fram/ and the corresponding datasheets are **
** at http://www.fujitsu.com/global/products/devices/semiconductor/memory/fram/lineup/#standard                   **
** Adafruit has two of these memories available on breakout boards, https://www.adafruit.com/product/1897 and     **
** https://www.adafruit.com/product/1897 (this one is SPI, but the chip is also available as I2C                  **
**                                                                                                                **
** The program makes use of the https://github.com/SV-Zanshin/MB85_FRAM library, the most recent version of which **
** can be downloaded at https://github.com/SV-Zanshin/MB85_FRAM/archive/master.zip                                **
**                                                                                                                **
** The following memories in the MB85 are detected and supported:                                                 **
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
** What sets this library apart is that it will autmatically detect up to 8 memories, in any combination of those **
** listed as supported above, and treats them as one contiguous block of memory. The read() and write() functions **
** also support structures and arrays. Although the internal I2C library has a 32 byte limitation on the buffer   **
** size, the library allows for larger structures to be read and written.                                         **
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
** 1.0.0b 2017-08-27 https://github.com/SV-Zanshin Initial coding                                                 **
**                                                                                                                **
*******************************************************************************************************************/
#include <MB85_FRAM.h>                                                        // Include the MB85_FRAM library    //
/*******************************************************************************************************************
** Declare all program constants                                                                                  **
*******************************************************************************************************************/
const uint32_t SERIAL_SPEED            = 115200;                              // Set the baud rate for Serial I/O //
/*******************************************************************************************************************
** Declare global variables and instantiate classes                                                               **
*******************************************************************************************************************/
MB85_FRAM_Class   FRAM;                                                      // Create an instance of MB85_FRAM   //
/*******************************************************************************************************************
** Method Setup(). This is an Arduino IDE method which is called upon boot or restart. It is only called one time **
** and then control goes to the main loop, which loop indefinately.                                               **
*******************************************************************************************************************/
void setup() {                                                                // Arduino standard setup method    //
  uint32_t i;                                                                 // Declare loop and work variables  //
  uint8_t  memByte;                                                           //                                  //
  Serial.begin(SERIAL_SPEED);                                                 // Start serial port at Baud rate   //
  #ifdef  __AVR_ATmega32U4__                                                  // If this is a 32U4 processor, then//
    while(!Serial);                                                           // Wait for serial to initialize    //
    delay(1000);                                                              // Then wait one more second        //
  #endif                                                                      // interface to initialize          //
  Serial.println("Starting FRAM example program");                            //                                  //
  uint8_t chips = FRAM.begin();                                               // Return number of memories found  //
  Serial.print("Detected ");                                                  //                                  //
  Serial.print(chips);                                                        //                                  //
  Serial.print(" MB85xxx memories.\nTotal storage ");                         //                                  //
  Serial.print(FRAM.totalBytes());                                            //                                  //
  Serial.println(" bytes.");                                                  //                                  //
                                                                              //                                  //
  Serial.println("\nWriting numbers to first 256 bytes of memory.");          //                                  //
  for(uint32_t i=0;i<256;i++) {                                               //                                  //
    FRAM.write(i,(uint8_t)i);                                                 //                                  //
  } // of for-next loop                                                       //                                  //
                                                                              //                                  //
  Serial.println("Reading data from address 100 onwards.");                   //                                  //
  for(uint32_t i=100;i<111;i++) {                                             //                                  //
    Serial.print(i);                                                          //                                  //
    Serial.print(" = ");                                                      //                                  //
    FRAM.read(i,memByte);                                                     //                                  //
    Serial.println(memByte);                                                  //                                  //
  } // of for-next loop                                                       //                                  //
                                                                              //                                  //
  Serial.println("Writing array to memory.");                                 //                                  //
  char testArray[13] = "Hello World!";                                        //                                  //
  FRAM.write(200,testArray);                                                  //                                  //
  FRAM.read(200,testArray);                                                   //                                  //
  Serial.print("Read string array as \"");                                    //                                  //
  Serial.print(testArray);                                                    //                                  //
  Serial.println("\".");                                                      //                                  //
                                                                              //                                  //
  if (chips>1) {                                                              // Demonstrate overlapping memories //
    Serial.println("\nMultiple memories found:");                             //                                  //
    for (uint8_t i=0;i<chips;i++) {                                           // Display information for each chip//
      Serial.print("Memory ");                                                //                                  //
      Serial.print(i);                                                        //                                  //
      Serial.print(" has ");                                                  //                                  //
      Serial.print(FRAM.memSize(i));                                          //                                  //
      Serial.println(" bytes.");                                              //                                  //
    } // for-next each memory chip found                                      //                                  //
    Serial.println("\nDemonstrating memory overlapping.");                    //                                  //
    uint32_t memAddress = FRAM.memSize(0);                                    // Set to beginning of 2nd memory   //
    FRAM.write(memAddress-6,testArray);                                       // Split test string across 2 chips //
    Serial.println("Splitting text write across 2 memory chips.");            //                                  //
    Serial.print("Reading from memory chip 2 gives text \"");                 //                                  //
    FRAM.read(memAddress,testArray);                                          // Read array from 2nd memory       //
    Serial.print(testArray);                                                  //                                  //
    Serial.println("\".");                                                    //                                  //

Serial.println("Test big data strings");
  char testArray2[65] = "123456789012345678901234567890123456789012345678901234567890abcd";
  FRAM.write(0,testArray2);
  Serial.print("Reading from memory chip 2 gives text \"");                 //                                  //
  FRAM.read(5,testArray2);                                          // Read array from 2nd memory       //
  Serial.print(testArray2);                                                  //                                  //






  } // of if-then-else we have more than one memory                           //                                  //
  Serial.println("\n\nFinished.");                                            //                                  //
} // of method setup()                                                        //                                  //
/*******************************************************************************************************************
** This is the main program for the Arduino IDE, it is an infinite loop and keeps on repeating.                   **
*******************************************************************************************************************/
void loop() {                                                                 //                                  //
} // of method loop()                                                         //----------------------------------//

/*
  uint32_t memAddress = FRAM.memSize(0)*1024;                               // Set to beginning of 2nd memory   //
  char testArray2[64] = "1234567890123456789012345678901234567890123456789012345678901234";
  FRAM.write(memAddress-50,testArray2);
  Serial.print("Reading from memory chip 2 gives text \"");                 //                                  //
  FRAM.read(memAddress,testArray);                                          // Read array from 2nd memory       //
  Serial.print(testArray);                                                  //                                  //
*/