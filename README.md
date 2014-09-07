# turbolib

General purpose C++11 support library for turbopump.

### Build dependencies

* linux (for now)
* g++ >= 4.8 (for proper c++11)
* cmake
* may or may not compile on 64-bit until I get an automated build box set up ¯\_(ツ)_/¯

### Bundled dependencies (aka "what I'm using")

* CATCH unit test library (https://github.com/philsquared/Catch)
* http-parser (https://github.com/joyent/http-parser)
* tiger hash implementation from fluks (https://github.com/markuspeloquin/fluks)
* modified base64 encoding based on René Nyffenegger's code

### Library dependencies

* UDT (libudt)
* intel's TBB library (libtbb)
* boost::uuid


