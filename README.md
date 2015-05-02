# turbolib

General purpose C++11 support library for turbopump.

### Build dependencies

* linux (for now)
* clang >= 3.4 or gcc >= 4.8* (YMMV. [modern gcc does not reliably compile multithreaded applications](https://bugs.launchpad.net/ubuntu/+source/gcc-defaults/+bug/1228201)) Other compilers are untested. Maybe they work! Maybe they don't!
* cmake

### Bundled dependencies (aka "what's included")

* CATCH unit test library (https://github.com/philsquared/Catch)
* ezOptionParser (http://ezoptionparser.sourceforge.net/)
* http-parser (https://github.com/joyent/http-parser)
* libmsgpack (https://github.com/msgpack/msgpack-c)
* tiger hash implementation from fluks (https://github.com/markuspeloquin/fluks)
* custom base64 encoding with a filesystem name-safe, lexicographically ordered character set

### Library dependencies (aka "what do I need to go get before building")

Your package manager (apt, rpm) hopefully will have reasonably up-to-date versions of these.
* libtbb, intel's TBB library (https://www.threadingbuildingblocks.org/)
* libudt (http://udt.sourceforge.net/)


