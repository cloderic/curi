# CURI - a tiny URI parser written in C #

A URI parser respecting [RFC-3986](http://tools.ietf.org/html/rfc3986), inspired by [yajl](http://lloyd.github.io/yajl/), tested with [catch](https://github.com/philsquared/Catch).

## Compilers ##

The library is developped and tested on:
- Mac OS X Mountain Lion (x64),
    - clang 4.2 (clang-425.0.24);
- Windows 7 (x86 & x64),
    - Visual Studio 2010 SP1 (10.0.40219.1 SP1Rel);
- Debian Linux 6.05 (x86),
    - GCC 4.4.5.

Additionally, it is continuously integrated on [Travis CI](https://travis-ci.org/cloderic/curi) (where master is currently ![Build Status](https://travis-ci.org/cloderic/curi.png?branch=master)), the platforms are:
- Ubuntu Linux 12.04 (x86),
    - GCC 4.6.x,
    - Clang 3.1.x.
    
## TODO ##

- More unit tests (especially failing unit tests)
- Handle relative URI (i.e. schemeless URI)
- Benchmarks
- Performances tweaking
- Strict enforcing of IPv6 grammar rule
- Handle non-ascii characters url decoding

