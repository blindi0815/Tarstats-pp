# Tarstats-++
Tool to provide stats of a tarball.

## Genereral information
This is a little command line tool to provide statistics on a GNU tar file. It is a personal project to further self study programming using C++.

The project is inspired by https://github.com/isotopp/tarstats by @github/isotopp and the Twitter thread https://twitter.com/isotopp/status/1489549907841146882

### Limitations
This tool works on standard .tar files as well on tarfiles compressed by GNU zip. Other compression methods are not supported. This does not yet parse tarballs inside a tarball.

## Prerequisites
Building this requires a version of zlib (http://zlib.net) on the target system. Common Linux distributions include zlib, yet the dev portions might be missing and have to installed first. In case of Ubuntu `zlib1g-dev`. Consult the provider of the distribution of your choice.

## Installation
```
cmake .
sudo cmake --build .
sudo cmake --install .
```

## Usage
```
$ tarstats_pp
usage: tarstats-pp [-h] [-j] [-f] tarfile.
A tool to calculate basics statistics on tarball. Shamelessly inspired by github.com/isotopp/tarstats!

mandatory argument:
tarfile   a tarfile to calculate stats on

optional arguments
-h        this helptext
-j        stats in JSON format printed to stdout
-f        print stats to file <tarfilename>.txt or .json if -j invoked
```
```
$ tarstats_pp -j zlib-1.2.11.tar.gz 
{"type": "gzarchive", "name": "zlib-1.2.11.tar.gz", "size": 607698, "itemsize": 2984023, "files": 253, "dir": 40, "symlinks": 0, "hardlinks": 0, "other": 0}

GLOBAL STATS
File amount:          1
Size of all archives: 607698 Bytes
Size of all items:    2984023 Bytes
DIRECTORIES: 40
FILES: 253
HARDLINKS: 0
OTHER: 0
SYMLINKS: 0
```
