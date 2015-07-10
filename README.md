gmio: C library for geometry input/output
===========================================

gmio is a reusable C library whose purpose is to provide complete I/O
support for various CAD file formats (eg. STL)

gmio aims to be fast, portable (ISO-C90 conformance) and feature-rich.
Main highlights:

  * "Abstract" streams that does not tie the user to C stream (`FILE*`).
    gmio provides a general stream structure based on callbacks, so that
    any kind of device can be used (memory, file, socket, ...)
  * Operations can be easily aborted
  * Buffering of input/ouput for efficient device usage
  * Available under the CeCILL-B license, which is fully BSD compatible


Supported CAD files format
==========================

Current version only supports the STL file format (STereoLithography), but support is complete :

  * Binary(little/big endian) and ASCII formats
  * Header data and "attribute byte count" for binary format
  * Name of solid for ASCII format
  * Detection of the input data format
  * Ability to specify the text formatting to represent float values (ASCII formats)

In addition, the STL module has the following advatanges:

  * The user keeps its own geometry data structures, no conversion needed.
    This reduces the effort so that the user just specifies callbacks for
    retrieval/creation of geometry data
  * No dynamic memory allocations
  * Extended support of OpenCascade's `StlMesh_Mesh`


Building gmio
===============

gmio can be built with CMake, by default a static library is generated.

  1. For an out-of-source build, create a directory where the build will take
     place (say `build/`)
  2. Call `cmake`, passing as argument the directory where is located
     CMakeLists.txt
  3. `(n)make`
  4. `(n)make install`

Read CMakeLists.txt to figure out how to build debug and release libraries.

For the first use of gmio, you should run unit tests:
    `(n)make check`


How to report a bug
===================

If you think you have found a bug in gmio, we would like to hear
about it so that we can fix it. The gmio bug tracking system is
open to the public at https://github.com/fougue/gmio/issues.

Before reporting a bug, please use the bug-tracker's search functions
to see if the issue is already known.

Always include the following information in your bug report: the name
and version number of your compiler; the name and version number of
your operating system; the version of gmio you are using, and
what configure options it was compiled with.

If the problem you are reporting is only visible at run-time, try to
create a small test program that shows the problem when run.


License
=======

This software is governed by the CeCILL-B license under French law and
abiding by the rules of distribution of free software.  You can  use,
modify and/ or redistribute the software under the terms of the CeCILL-B
license as circulated by CEA, CNRS and INRIA at the following URL
http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html


Build status
============

Travis (linux_x64) : [![Build Status](https://travis-ci.org/fougue/gmio.svg?branch=master)](https://travis-ci.org/fougue/gmio)  
AppVeyor (Windows) : [![Build status](https://ci.appveyor.com/api/projects/status/0q4f69lqo6sw9108?svg=true)](https://ci.appveyor.com/project/HuguesDelorme/gmio)

<a href="https://scan.coverity.com/projects/5721">
  <img alt="Coverity Scan Build Status"
       src="https://scan.coverity.com/projects/5721/badge.svg"/>
</a>

