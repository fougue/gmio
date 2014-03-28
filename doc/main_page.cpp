/*!

\mainpage


\section intro_sec Introduction

This is gmio library v0.1

Check source code at : https://github.com/fougsys/gmio

gmio is a fast and portable C library for data exchange.


\section build_sec BUILDING gmio library

The gmio library can be built with qmake or cmake.

<b>With qmake :</b>
\code
  .../gmio$ cd qmake
  .../gmio$ qmake PREFIX_DIR=../gcc-linux64
  .../gmio$ make all
  .../gmio$ make install
\endcode

If you want static libraries (instead of DLLs) just add "CONFIG+=staticlib" to the qmake command :
\code
  .../gmio$ qmake PREFIX_DIR=../gcc-linux64 CONFIG+=staticlib
\endcode

Note that on Windows, qmake complains about deprecated support of backslash '\' characters.
So you may have to escape backslashes like C:\\path\\to\\opencascade or simply use Unix separators
instead C:/path/to/opencascade


<b>With CMake :</b>
<TO_BE_DONE>


\section reportbug_sec HOW TO REPORT A BUG

If you think you have found a bug in gmio, we would like to hear
about it so that we can fix it. The gmio bug tracking system is
open to the public at https://github.com/fougsys/gmio/issues.

Before reporting a bug, please use the bug-tracker's search functions
to see if the issue is already known.

Always include the following information in your bug report: the name
and version number of your compiler; the name and version number of
your operating system; the version of gmio you are using, and
what configure options it was compiled with.

If the problem you are reporting is only visible at run-time, try to
create a small test program that shows the problem when run.

       
\section license_sec LICENSE

This software is governed by the CeCILL-C license under French law and
abiding by the rules of distribution of free software.  You can  use,
modify and/ or redistribute the software under the terms of the CeCILL-C
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
knowledge of the CeCILL-C license and that you accept its terms.

*/
