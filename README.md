# Function parser for C++, development version

NOTE: This is the development version of the library. It is NOT intended to be used in projects
directly (nor is it even usable as-is, because it requires running some code generators).

Creating the release version of the library can only be done in Linux, and is done by
running `make distro_pack`.

Running `make` on its own will compile the programs `testbed` (unit tests), `speedtest`
(performance tests) and `functioninfo` (a utility program for manual testing from the
command line).
