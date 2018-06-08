# bleat
Bleat, besides being the cry of a sheep or goat, provides a common C api for using Bluetooth LE features on Linux and Windows 10.  

# Build
As this library is designed to work across two platforms, there will be different dependencies and build steps depending on the 
target platform.

## Linux
On Linux, bleat wraps around the [libblepp](https://github.com/edrosten/libblepp) library, which is included as a submodule.  You 
will need to have BlueZ, Boost headers, and GNU Make installed along with a C++ compiler that support C++14.  

```bash
git submodule update --init
make
```

## Windows 10
Install Visual Studio 2017 along with the FCU Windows 10 SDK (10.0.16299.0).  You can build the solution in either VS2017 or with 
MSBuild.  

```bat
cd vs2017
MSBuild.exe bleat.sln
```

# Examples
Example scripts showcasing how to use the library are in the [example](https://github.com/mbientlab/bleat/blob/master/example) 
folder.  
