# AMABot

## Dependencies

### Build requirements
* [cmake](https://cmake.org/) (version 3.14+)
* A supported C++ compiler like g++ (https://gcc.gnu.org)

### External Dependencies (Must be installed before trying to build)

#### DPP requires:

* [OpenSSL](https://openssl.org/) (whichever `-dev` package comes with your OS)
* [zlib](https://zlib.net) (whichever `-dev` package comes with your OS)

#### Curlpp requires:

* [libcurl](https://curl.se/) (whichever `-dev` package. Using openssl currently)

## Building

Once all the depencies have been acquired:

```
cmake -B ./build
make -C ./build
```
