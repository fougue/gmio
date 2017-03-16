include(CheckCXXCompilerFlag)

if(CMAKE_COMPILER_IS_GNUCXX
       OR CMAKE_CXX_COMPILER MATCHES ".*clang")
    check_cxx_compiler_flag("-std=c++11" GMIO_COMPILER_SUPPORTS_CXX11)
    if(GMIO_COMPILER_SUPPORTS_CXX11)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    else()
        check_cxx_compiler_flag("-std=c++0x" GMIO_COMPILER_SUPPORTS_CXX0X)
        if(GMIO_COMPILER_SUPPORTS_CXX0X)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
        else()
            message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support")
        endif()
    endif()
endif()

# Note : cmake 3.1 introduced set_property(TARGET tgt PROPERTY CXX_STANDARD 11)
