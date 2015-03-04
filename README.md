GeomIO: C library for geometry input/output
===========================================

This is GeomIO v0.1dev

GeomIO is a reusable C library whose purpose is to provide complete I/O
support for various CAD file formats (eg. STL)

GeomIO aims to be fast, portable (ISO-C90 conformance) and feature-rich.
Main highlights:
    * "Abstract" streams that does not tie the user to C stream (`FILE*`).        
      GeomIO provides a general stream structure based on callbacks, so that
      any kind of device can be used (memory, file, socket, ...)
    * Operations can be easily aborted
    * Buffering of input/ouput for efficient device usage
    * Available under the CeCILL-B license, which is fully BSD compatible

Current version only supports STL file format (STereoLithgraphy). Support of
STL is complete:
    * Binary(little/big endian) and ASCII formats
    * Header data and "attribute byte count" for binary format
    * Name of solid for ASCII format
    * Detection of the input data format

In addition, the STL module has the following advatanges:
    * The user keeps its own geometry data structures, no conversion needed.
      This reduces the effort so that the user just specifies callbacks for
      retrieval/creation of geometry data
    * No dynamic memory allocations
    * Extended support of OpenCascade's `StlMesh_Mesh`


Building GeomIO
===============

GeomIO can be built with CMake, by default a shared library is generated.
    1. For an out-of-source build, create a directory where the build will take
         place (say `build/`)
    2. Call `cmake`, passing as argument the directory where is located
         CMakeLists.txt
    3. `(n)make`
    4. `(n)make install`

Read CMakeLists.txt to figure out how to build debug and release libraries.

For the first use of GeomIO, you should run unit tests:
    `(n)make check`


How to report a bug
===================

If you think you have found a bug in GeomIO, we would like to hear
about it so that we can fix it. The GeomIO bug tracking system is
open to the public at https://github.com/fougsys/fougtools/issues.

Before reporting a bug, please use the bug-tracker's search functions
to see if the issue is already known.

Always include the following information in your bug report: the name
and version number of your compiler; the name and version number of
your operating system; the version of GeomIO you are using, and
what configure options it was compiled with.

If the problem you are reporting is only visible at run-time, try to
create a small test program that shows the problem when run.


License
=======

This software is governed by the CeCILL-B license under French law and
abiding by the rules of distribution of free software.  You can  use,
modify and/ or redistribute the software under the terms of the CeCILL-B
license as circulated by CEA, CNRS and INRIA at the following URL
http://www.cecill.info

As a counterpart to the access to the source code and  rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty  and the software's author,  the holder of the
economic rights,  and the successive licensors  have only  limited
liability.

In this respect, the user's attention is drawn to the risks associated
with loading,  using,  modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean  that it is complicated to manipulate,  and  that  also
therefore means  that it is reserved for developers  and  experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or
data to be ensured and,  more generally, to use and operate it in the
same conditions as regards security.

The fact that you are presently reading this means that you have had
knowledge of the CeCILL-B license and that you accept its terms.
