# curi #

## Motivation ##

As the [RFC-3986](http://tools.ietf.org/html/rfc3986) so poetically states,
> A Uniform Resource Identifier (URI) is a compact sequence of characters that identifies an abstract or physical resource.

Pretty vague, hu!

For example, <http://www.google.com/search?q=URI> is a URI composed of:
    - a **scheme**, `http`;
    - a **host**, `www.google.com`;
    - a **path**, `search`;
    - and a **query**, `q=URI`.

**curi** is able to parse this kind of query in sub-elements.

**curi** started as a week-end project to:
- do some C (it's always cool to go back to basics);
- try [Travis](https://travis-ci.org/cloderic/curi);
- try [Catch](https://github.com/philsquared/Catch).

In the end, it is pretty stable and is even used in the next version of [MASA LIFE](www.masalife.net).

## Using ##

**curi** provides functions to [parse URIs](\ref parsing) and URI's paths and queries. It is an almost straight implementation of the [RFC-3986](http://tools.ietf.org/html/rfc3986). The only current limitation being a permissive IPv6 host parsing.

Aside from that, **curi** also features [URL-encoded strings decoding](\ref url_decoding), that is to say the ability to decode percent encoded strings.

The best way to use **curi** in your C/C++ project is simply to include directly its sources in your project. Two files needs to be downloaded in their latest stable version:
- [curi.h](https://github.com/cloderic/curi/blob/master/src/curi.h);
- [curi.c](https://github.com/cloderic/curi/blob/master/src/curi.c).

### Compatibility ###

**curi** is written in Visual Studio compliant **C90**. It has been tested on several platforms including:
- Mac OS X Mountain Lion (x64),
    - clang 4.2 (clang-425.0.24);
- Windows 7 (x86 & x64),
    - Visual Studio 2010 SP1 (10.0.40219.1 SP1Rel);
- Debian Linux 6.05 (x86),
    - GCC 4.4.5;
- Ubuntu Linux 12.04 (x86),
    - GCC 4.6.x,
    - Clang 3.1.x.

### Licence ###

\verbinclude curi.LICENSE.txt

## Contributing ##

Helping to the development is as easy as reporting [issues](https://github.com/cloderic/curi/issues/new).

If you have the time and skills to actually develop new features or fix issues, 
1. [fork](https://github.com/cloderic/curi/fork) the repository, 
2. develop,
3. create a [pull request](https://github.com/cloderic/curi/compare).
