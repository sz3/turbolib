# turbolib

General purpose C++11 support library for turbopump.

### Build dependencies

* linux (for now)
* clang >= 3.4 or gcc >= 4.8* (YMMV. [modern gcc does not reliably compile multithreaded applications](https://bugs.launchpad.net/ubuntu/+source/gcc-defaults/+bug/1228201)) Other compilers are untested. Maybe they work! Maybe they don't!
* cmake

### Bundled dependencies (aka "what's included")

* CATCH unit test library (https://github.com/philsquared/Catch)
* http-parser (https://github.com/joyent/http-parser)
* tiger hash implementation from fluks (https://github.com/markuspeloquin/fluks)
* modified base64 encoding based on René Nyffenegger's code

### Library dependencies (aka "what do I need to go get before building")

* libmsgpack (https://github.com/msgpack/msgpack-c)
* intel's TBB library (libtbb)
* UDT (libudt)
* boost::uuid


