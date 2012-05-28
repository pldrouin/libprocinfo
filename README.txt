This directory contains the libprocinfo library which allows to easily retrieve the
user time, system time, nice level, resident memory and virtual memory of a
process using a common interface for Linux and FreeBSD. The information is
retrieved from the operating system using the /proc pseudo filesystem (Linux)
or libkvm (FreeBSD).

Dependencies:
-GNU sed (to generated dependency files)
-GCC (tested with GCC 4.4.5)
-GNU Make
-libkvm (FreeBSD only)
-Linux /proc pseudo filesystem (tested on Scientific Linux 5.5)
