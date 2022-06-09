# cdson

A pure C parsing and serialization library for the [DSON](https://dogeon.xyz/)
data serialization format, for humans.  cdson is believed to have complete
spec coverage, though as with any project, there may still be bugs.

## Installation

If you're running Fedora, RHEL 8+ (or a rebuild), CentOS 8+, or Mageia 8, I
provide a [COPR
repository](https://copr.fedorainfracloud.org/coprs/rharwood/cdson/).  On
Fedora, it can be used with:

```shell
dnf copr enable rharwood/cdson
dnf install libcdson-devel
```

Otherwise, keep reading for instructions to build from source.

## Building from source and testing

```shell
mkdir build
cd build
meson ..
ninja
meson test # optional
sudo meson install
```

## Usage

```C
#include <cdson.h>
```

and build with however your buildsystem integrates pkg-config output as usual.

Further documentation is inline in
[cdson.h](https://github.com/frozencemetery/cdson/blob/main/cdson.h).

## Contributing

PRs welcome!  Test suite must pass, and more tests are welcome too.

## Dogs

If you are in the US, and have capacity and desire to care for an animal
friend, the ASPCA has a searchable database of adoptable
[dogs](https://www.aspca.org/adopt-pet/adoptable-dogs-your-local-shelter) (and
[cats](https://www.aspca.org/adopt-pet/adoptable-cats-your-local-shelter)) in
your area.
