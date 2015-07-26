# turbolib

General purpose C++11 support library for [turbopump](https://github.com/sz3/turbopump).

### Build dependencies

* linux (for now)
* clang >= 3.4 or gcc >= 4.9. Other compilers are untested. Maybe they work! Maybe they don't!
* cmake

### Bundled dependencies (aka "what's included")

* [CATCH unit test library](https://github.com/philsquared/Catch)
* [cppformat](https://github.com/cppformat/cppformat)
* [ezOptionParser](http://ezoptionparser.sourceforge.net/)
* [http-parser](https://github.com/joyent/http-parser)
* [libcuckoo](https://github.com/efficient/libcuckoo)
* [moodyCamel::concurrentqueue](https://github.com/cameron314/concurrentqueue)
* tiger hash implementation from [fluks](https://github.com/markuspeloquin/fluks)
* custom base64 encoding with a filesystem name-safe, lexicographically ordered character set

### Library dependencies (aka "what do I need to go get before building")

Your package manager (apt, rpm) hopefully will have reasonably up-to-date versions of these.
* [libmsgpack](https://github.com/msgpack/msgpack-c)
* [libudt](http://udt.sourceforge.net/)


