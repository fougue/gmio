#############################################################################
## gmio
## Copyright Fougue (24 Jun. 2016)
## contact@fougue.pro
##
## This software is a reusable library whose purpose is to provide complete
## I/O support for various CAD file formats (eg. STL)
##
## This software is governed by the CeCILL-B license under French law and
## abiding by the rules of distribution of free software.  You can  use,
## modify and/ or redistribute the software under the terms of the CeCILL-B
## license as circulated by CEA, CNRS and INRIA at the following URL
## "http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html".
#############################################################################

# Returns the list of subdirectories inside fromDir
function(get_subdirs outList fromDir)
    file(GLOB children RELATIVE ${fromDir} ${fromDir}/*)
    set(dirList "")
    foreach(child ${children})
        if(IS_DIRECTORY ${fromDir}/${child})
            list(APPEND dirList ${child})
        endif()
    endforeach()
    set(${outList} ${dirList} PARENT_SCOPE)
endfunction()

# Lists all subdirs(targets) inside benchmark_other
get_subdirs(OTHER_DIRS ${CMAKE_CURRENT_SOURCE_DIR})
foreach(subdir IN LISTS OTHER_DIRS)
    add_subdirectory(${subdir})
endforeach()
