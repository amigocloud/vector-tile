## Vector Tile Library

C++14 library for decoding [Mapbox Vector Tiles](https://www.mapbox.com/vector-tiles/). 

It was updated with [xabbu42](https://github.com/xabbu42) pull request [56](https://github.com/mapbox/vector-tile/pull/56) to handle [getValue does not handle layer with key duplicates](https://github.com/mapbox/vector-tile/issues/55) issue.

[![Build Status](https://travis-ci.org/mapbox/vector-tile.svg?branch=master)](https://travis-ci.org/mapbox/vector-tile)

## Depends

 - C++14 compiler
 - [protozero](https://github.com/mapbox/protozero)
 - [variant](https://github.com/mapbox/variant)
 - [geometry](https://github.com/mapbox/geometry.hpp)


## Building

Call
```sh
git submodule init
git submodule update
```

to install test fixtures from an external git repository.

To install all dependencies and build the tests in this repo do:

```sh
make test
```

## To bundle the `demo` program do:

```sh
make demo
```

This copies all the includes into the `demo/include` folder such that the demo can be build like:

```sh
make -C demo/
```

Or also like:

```sh
cd demo
make
```

# Who is using vector-tile?

These are the applications targeted to upgrade to this library:

* [AmigoCloud Mapbox GL Native](https://github.com/amigocloud/mapbox-gl-native)
* [Mapnik Vector Tile](https://github.com/mapbox/mapnik-vector-tile)
* [Tippecanoe](https://github.com/mapbox/tippecanoe)
