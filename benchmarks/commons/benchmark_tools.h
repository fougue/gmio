/****************************************************************************
** gmio benchmarks
** Copyright Fougue (2 Mar. 2015)
** contact@fougue.pro
**
** This software provides performance benchmarks for the gmio library
** (https://github.com/fougue/gmio)
**
** This software is governed by the CeCILL-B license under French law and
** abiding by the rules of distribution of free software.  You can  use,
** modify and/ or redistribute the software under the terms of the CeCILL-B
** license as circulated by CEA, CNRS and INRIA at the following URL
** "http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html".
****************************************************************************/

#ifndef BENCHMARK_TOOLS_H
#define BENCHMARK_TOOLS_H

#include <gmio_core/global.h>

GMIO_C_LINKAGE_BEGIN

typedef void (*benchmark_file_func_t)(const char*);

void benchmark_list(
        benchmark_file_func_t func, const char* title, int argc, char** argv);

void benchmark_forward_list(
        benchmark_file_func_t func, int argc, char** argv);

void benchmark(
        benchmark_file_func_t func, const char* title, const char* filepath);

GMIO_C_LINKAGE_END

#endif /* BENCHMARK_TOOLS_H */
