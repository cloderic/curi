*Work in progress*

# CURI - a tiny C URI parser #

A URI C parser respecting [RFC-3986](http://tools.ietf.org/html/rfc3986), inspired by [yajl](http://lloyd.github.io/yajl/), tested with [catch](https://github.com/philsquared/Catch).

## Continuous integration ##

Handled by [Travis CI](https://travis-ci.org/cloderic/curi).

- master: ![Build Status](https://travis-ci.org/cloderic/curi.png?branch=master)

## TODO ##

- More unit tests (especially failing unit tests)
- Supporting IP host (e.g. 192.168.0.1)
- Handle relative URI (i.e. schemeless URI)
- Handle path segments callbacks
- Handle query key/value callbacks
- Add on-demand url decoding
- Performance tests and tweaking

