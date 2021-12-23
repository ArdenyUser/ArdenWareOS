==============
HolyC compiler
==============

About
-----
* An compiler which can compiler HolyC files into native elf binaries for unix like systems.
* Provides a minimal standard library for common functions.

Cloning
-------
* Make sure that you clone the repository with `--recursive` and clone the submodules.

Building
--------
* Unix like systems. ``make all && make install``
* To build in release mode ->  ``make CFLAGS='-O3 -pipe' all && make install``

Cross building
--------------
* As the build system runs a program built in tools, either have qemu-static-ARCH or run all the steps manually. There are not too many.

License
-------
* All code under the GNU GPLv3 license.

Note
----
* HolyC and TempleOS are creations of Terry Davis.

