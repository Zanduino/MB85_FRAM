# Fujitsu MB85nnn FRAM memories<br>(https://zanduino.github.io/Badges/GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0) [![Build](https://github.com/Zanduino/MB85_FRAM/workflows/Build/badge.svg)](https://github.com/Zanduino/MB85_FRAM/actions?query=workflow%3ABuild) [![Format](https://github.com/Zanduino/MB85_FRAM/workflows/Format/badge.svg)](https://github.com/Zanduino/MB85_FRAM/actions?query=workflow%3AFormat) [![Wiki](https://zanduino.github.io/Badges/Documentation-Badge.svg)](https://github.com/Zanduino/MB85_FRAM/wiki) [![Doxygen](https://github.com/Zanduino/MB85_FRAM/workflows/Doxygen/badge.svg)](https://Zanduino.github.io/MB85_FRAM/html/index.html) [![arduino-library-badge](https://www.ardu-badge.com/badge/MB85_FRAM.svg?)](https://www.ardu-badge.com/MB85_FRAM)
<img src="https://github.com/Zanduino/MB85_FRAM/blob/master/Images/MB85Breakout.jpg" width="175" align="right"/> *Arduino* library which defines methods for accessing most of the Fujitsu MB85nnn family FRAM memories. The library allows efficient reading from and writing to [Fujitsu FRAM](http://www.fujitsu.com/global/products/devices/semiconductor/memory/fram/overview/features/index.html) memories using I2C and allowing use of objects such as arrays or structures in addition to writing single bytes at a time. The FRAM memory has several advantages over conventional SRAM in that it allows at least 10 trillion read/write cycles which means that the programmer doesn't have to worry about heavy use of FRAM for changing data. The FRAM is 5V tolerant and there is an [Adafruit breakout](https://www.adafruit.com/product/1895) available.
Up to 8 devices can be put on an I2C and the library allows several memories to be treated as one large contiguous memory. The following memories are supported:

<table>
  <tr>
    <td><b>Memory Type</b></td>
    <td><b>Storage Bits</b></td>
    <td><b>Datasheets</b></td>
  </tr>
  <tr>
    <td>MB85RC512T</td>
    <td>512 kbit / 64KB</td>
    <td><a href="http://www.fujitsu.com/global/documents/products/devices/semiconductor/fram/lineup/MB85RC512T-DS501-00028-4v0-E.pdf">MB85RC512T Datasheet</a></td>
  </tr>
  <tr>
    <td>MB85RC256V</td>
    <td>256 kbit / 32KB</td>
    <td><a href="http://www.fujitsu.com/global/documents/products/devices/semiconductor/fram/lineup/MB85RC256V-DS501-00017-5v1-E.pdf">MB85RC256V Datasheet</a></td>
  </tr>
  <tr>
    <td>MB85RC128A</td>
    <td>128 kbit / 16KB</td>
    <td><a href="http://www.fujitsu.com/global/documents/products/devices/semiconductor/fram/lineup/MB85RC128A-DS501-00018-4v0-E.pdf>MB85RC128A Datasheet</a></td>
  </tr>
  <tr>
    <td>MB85RC64TA<br>MB85RC64A<br>MB85RC64V</td>
    <td>64 kbit / 8KB</td>
    <td><a http://www.fujitsu.com/global/documents/products/devices/semiconductor/fram/lineup/MB85RC64TA-DS501-00044-2v0-E.pdf">MB85RC64TA Datasheet</a><br><a href="http://www.fujitsu.com/global/documents/products/devices/semiconductor/fram/lineup/MB85RC64A-DS501-00019-4v0-E.pdf">MB85RC64A Datasheet</a><br><a href="http://www.fujitsu.com/global/documents/products/devices/semiconductor/fram/lineup/MB85RC64V-DS501-00013-7v0-E.pdf">MB85RC64V Datasheet</a></td>
  </tr>
</table>

See the [Wiki pages](https://github.com/Zanduino/MB85_FRAM/wiki) for details of the class and the [Doxygen Documentation](https://Zanduino.github.io/MB85_FRAM/html/index.html) for detailed class documentation.

[![Zanshin Logo](https://zanduino.github.io/Images/zanshinkanjitiny.gif) <img src="https://zanduino.github.io/Images/zanshintext.gif" width="75"/>](https://www.sv-zanshin.com)
