# Warble
Warble, besides being the chirping/singing of birds, is a wrapper around various Bluetooth LE libraries, providing a common C API for 
Linux and Windows 10.

Developers can use this library to connect to Bluetooth LE devices, read and write GATT characteristic values, and enable chatacteristic 
notifications.

# Build
Beacuse this library is designed to work across two OSes, there will be different dependencies and build steps depending on the 
target OS.

Regardless of the platform, the build relies on Git tag information, so you must clone the code instead of doing a source download.  

## Linux
On Linux, Warble wraps around the [libblepp](https://github.com/edrosten/libblepp) library, which is included as a submodule.  You 
will need to have BlueZ, Boost headers, and GNU Make installed along with a C++ compiler that support C++14.  

```bash
git submodule update --init
make
```

## Windows 10
Install Visual Studio 2017 along with the FCU Windows 10 SDK (10.0.16299.0).  You can build the solution in VS2017 or with MSBuild.  

```bat
cd vs2017
MSBuild.exe warble.sln
```

# Examples
Example scripts showcasing how to use the library are in the [example](https://github.com/mbientlab/warble/blob/master/example) folder.  
