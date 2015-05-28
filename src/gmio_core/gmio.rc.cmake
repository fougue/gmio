#include <winresrc.h>

VS_VERSION_INFO VERSIONINFO
 FILEVERSION    @GMIO_VERSION_MAJOR@,@GMIO_VERSION_MINOR@,@GMIO_VERSION_PATCH@,0
 PRODUCTVERSION @GMIO_VERSION_MAJOR@,@GMIO_VERSION_MINOR@,@GMIO_VERSION_PATCH@,0
 FILETYPE VFT_DLL
 BEGIN
   BLOCK "StringFileInfo"
   BEGIN
     BLOCK "04090000"
     BEGIN
       VALUE "FileDescription", "gmio DLL"
       VALUE "FileVersion", "@GMIO_VERSION@"
       VALUE "InternalName", "gmio"
       VALUE "LegalCopyright", "Licensed under CeCILL-B v1.0"
       VALUE "ProductName", "gmio library, by Fougue Ltd."
       VALUE "ProductVersion", "@GMIO_VERSION@"
     END
   END
   BLOCK "VarFileInfo"
   BEGIN
     VALUE "Translation", 0x409, 1200
   END
 END
 
