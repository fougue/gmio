/****************************************************************************
** Copyright (c) 2017, Fougue Ltd. <http://www.fougue.pro>
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
**     1. Redistributions of source code must retain the above copyright
**        notice, this list of conditions and the following disclaimer.
**
**     2. Redistributions in binary form must reproduce the above
**        copyright notice, this list of conditions and the following
**        disclaimer in the documentation and/or other materials provided
**        with the distribution.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
****************************************************************************/

#include "utest_assert.h"

#include "stl_testcases.h"
#include "stl_utils.h"

#include "../src/gmio_core/error.h"
#include "../src/gmio_core/internal/locale_utils.h"
#include "../src/gmio_stl/stl_error.h"
#include "../src/gmio_stl/stl_infos.h"
#include "../src/gmio_stl/stl_io.h"
#include "../src/gmio_stl/stl_io_options.h"

#include <clocale>
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iostream>

namespace gmio {

static const char* TestSTL_read()
{
    for (const STL_ReadTestCase& testcase : STL_spanReadTestCase()) {
        const STL_Format format = STL_probeFormat(testcase.filepath);
        STL_MeshCreatorBasic mesh_creator;
        const int err = STL_read(testcase.filepath, &mesh_creator);

        // Check format
        if (format != testcase.format) {
            std::cerr << "\nfilepath : " << testcase.filepath
                      << "\nexpected format : " << testcase.format
                      << "\nactual format   : " << format << '\n';
        }
        UTEST_COMPARE(testcase.format, format);

        // Check error code
        if (err != testcase.errorcode) {
            std::cerr << "\nfilepath : " << testcase.filepath
                      << std::hex
                      << "\nexpected error : 0x" << testcase.errorcode
                      << "\nactual error   : 0x" << err << '\n';
        }
        UTEST_COMPARE(testcase.errorcode, err);

        // Check solid name
        if (testcase.format == STL_Format_Ascii) {
            const std::string testcase_solid_name =
                    testcase.solid_name != nullptr ? testcase.solid_name : "";
            if (mesh_creator.infos().ascii_solid_name != testcase_solid_name) {
                std::cerr << "\nfilepath : " << testcase.filepath
                          << "\nexpected solidname : " << testcase_solid_name
                          << "\nactual solidname   : " << mesh_creator.infos().ascii_solid_name
                          << '\n';
            }
            UTEST_COMPARE(testcase_solid_name, mesh_creator.infos().ascii_solid_name);
        }
    }

    return nullptr;
}

static const char* TestSTL_readBinary()
{
    // This file contains only a header and facet count(100) but no triangles
    FILE* file = std::fopen(filepath_stlb_header_nofacets, "rb");
    if (file != nullptr) {
        STL_MeshCreatorBasic creator;
        const int error = STL_readBinary(
                    Endianness_Little, FILE_funcReadData(file), &creator);
        std::fclose(file);
        UTEST_COMPARE(STL_Error_FacetCount, error);
    }
    else {
        UTEST_FAIL("file is NULL");
    }
    return nullptr;
}

static const char* TestSTL_writeBinaryHeader()
{
    const char* filepath = "temp/solid.stlb";
    const char header_str[] = "temp/solid.stlb generated with gmio library";
    const STL_BinaryHeader header = STL_binaryHeaderFromString(header_str);

    {
        FILE* outfile = std::fopen(filepath, "wb");
        UTEST_ASSERT(outfile != nullptr);
        const int error = STL_writeBinaryHeader(
                    Endianness_Little, FILE_funcWriteData(outfile), header, 0);
        std::fclose(outfile);
        UTEST_COMPARE(Error_OK, error);
    }

    {
        FILE* infile = std::fopen(filepath, "rb");
        STL_MeshCreatorBasic meshcreator;
        const int error = STL_readBinary(
                    Endianness_Little, FILE_funcReadData(infile), &meshcreator);
        std::fclose(infile);
        UTEST_COMPARE(Error_OK, error);
        UTEST_ASSERT(meshcreator.infos().binary_header == header);
        UTEST_COMPARE(0, meshcreator.triangles().size());
    }

    return nullptr;
}

static const char* TestSTL_writeBinary()
{
    const char* model_fpath = filepath_stlb_grabcad_arm11;
    const char* model_fpath_out = "temp/solid.le_stlb";
    const char* model_fpath_out_be = "temp/solid.be_stlb";
    STL_MeshCreatorInfos mesh_infos;
    std::vector<STL_Triangle> mesh_triangles;

    {
        // Read input model file
        STL_MeshCreatorBasic creator;
        {
            const int error = STL_read(model_fpath, &creator);
            UTEST_COMPARE(Error_OK, error);
        }

        // Write back input model file
        // Write also the model file in big-endian STL format
        const STL_MeshBasic mesh(creator.triangles());
        STL_WriteOptions opts = {};
        opts.binary_header = creator.infos().binary_header;
        {
            const int error = STL_write(
                        STL_Format_BinaryLittleEndian, model_fpath_out, mesh, opts);
            UTEST_COMPARE(Error_OK, error);
        }

        // Big-endian version
        {
            const int error = STL_write(
                        STL_Format_BinaryBigEndian, model_fpath_out_be, mesh, opts);
            UTEST_COMPARE(Error_OK, error);
        }

        mesh_infos = std::move(creator.infos());
        mesh_triangles = std::move(creator.triangles());
    }

    // Check input and output models are equal
    {
        size_t bytes_read_in = 0;
        size_t bytes_read_out = 0;

        std::ifstream in(model_fpath, std::ios_base::in | std::ios_base::binary);
        std::ifstream out(model_fpath_out, std::ios_base::in | std::ios_base::binary);
        if (!in.is_open() || !out.is_open()) {
            UTEST_FAIL("std::ifstream error for in/out model files");
        }
        do {
            static const size_t buff_size = 2048;
            uint8_t buffer_in[buff_size];
            uint8_t buffer_out[buff_size];
            in.read(reinterpret_cast<char*>(buffer_in), buff_size);
            bytes_read_in = in.gcount();
            out.read(reinterpret_cast<char*>(buffer_out), buff_size);
            bytes_read_out = out.gcount();
            if (bytes_read_in != bytes_read_out) {
                UTEST_FAIL("Different byte count between in/out");
            }
            if (std::memcmp(buffer_in, buffer_out, buff_size) != 0) {
                UTEST_FAIL("Different buffer contents between in/out");
            }
        } while (!in.eof() && !out.eof()
                 && bytes_read_in > 0 && bytes_read_out > 0);
    }

    // Check output LE/BE models are equal
    {
        STL_MeshCreatorBasic creator;
        const int error = STL_read(model_fpath_out_be, &creator);
        UTEST_COMPARE(Error_OK, error);
        UTEST_ASSERT(mesh_infos.binary_header == creator.infos().binary_header);
        UTEST_COMPARE(
                    mesh_infos.binary_triangle_count,
                    creator.infos().binary_triangle_count);
        UTEST_COMPARE(mesh_triangles.size(), creator.triangles().size());
        UTEST_ASSERT(std::memcmp(
                         mesh_triangles.data(),
                         creator.triangles().data(),
                         mesh_triangles.size() * sizeof(STL_Triangle))
                     == 0);
    }

    return nullptr;
}

static const char* TestSTL_writeAscii()
{
    const char* model_filepath = filepath_stlb_grabcad_arm11;
    const char* model_filepath_out = "temp/solid.stla";
    STL_MeshCreatorInfos mesh_infos;
    std::vector<STL_Triangle> mesh_triangles;

    // Read input model file
    {
        STL_MeshCreatorBasic creator;
        const int error = STL_read(model_filepath, &creator);
        UTEST_COMPARE(Error_OK, error);
        mesh_infos = std::move(creator.infos());
        mesh_triangles = std::move(creator.triangles());
    }
    const std::string header_str =
            STL_binaryHeaderToString(mesh_infos.binary_header, '_').data();


    // Write the model to STL ascii format
    {
        STL_WriteOptions opts = {};
        opts.ascii_solid_name = header_str;
        opts.ascii_float32_prec = 7;
        opts.ascii_float32_format = FloatTextFormat::ShortestLowercase;
        const STL_MeshBasic mesh(mesh_triangles);
        const int error =
                STL_write(STL_Format_Ascii, model_filepath_out, mesh, opts);
        UTEST_COMPARE(Error_OK, error);
    }

    // Read the output STL ascii model
    {
        STL_MeshCreatorBasic creator;
        const int error = STL_read(model_filepath_out, &creator);
        UTEST_COMPARE(Error_OK, error);
        UTEST_COMPARE(mesh_triangles.size(), creator.triangles().size());
        UTEST_COMPARE(header_str, creator.infos().ascii_solid_name);
        for (size_t i = 0; i < mesh_triangles.size(); ++i) {
            const STL_Triangle& lhs = mesh_triangles.at(i);
            const STL_Triangle& rhs = creator.triangles().at(i);
            const bool tri_equal = STL_triangleEquals(lhs, rhs, 5);
            UTEST_ASSERT(tri_equal);
        }
    }

    return nullptr;
}

namespace {

class STL_MeshSolidCounter : public STL_MeshCreator {
public:
    unsigned solid_count = 0;
    void beginSolid(const STL_MeshCreatorInfos& infos) override {
        if (infos.format != STL_Format_Unknown)
            ++(this->solid_count);
    }
};

const char* TestSTL_checkReadMultiSolid(
        const char* filepath, unsigned expected_solid_count)
{
    FILE* infile = std::fopen(filepath, "rb");
    const FuncReadData func_read = FILE_funcReadData(infile);
    if (infile != nullptr) {
        int error = Error_OK;
        STL_ReadOptions roptions = {};
        STL_MeshSolidCounter mesh_counter;
        while (error == Error_OK && !std::feof(infile))
            error = STL_read(func_read, &mesh_counter, roptions);
        std::fclose(infile);
        UTEST_COMPARE(Error_OK, error);
        UTEST_COMPARE(expected_solid_count, mesh_counter.solid_count);
    }
    else {
        std::perror(nullptr);
        UTEST_FAIL("");
    }
    return nullptr;
}

} // namespace

static const char* TestSTL_readMultiSolid()
{
    const char* res = nullptr;
#if 0
    res = TestSTL_checkReadMultiSolid(filepath_stla_4meshs, 4);
    if (res != nullptr)
        return res;
#endif
    res = TestSTL_checkReadMultiSolid(filepath_stlb_4meshs, 4);
    return res;
}

static const char* TestSTL_asciiLcNumeric()
{
    lc_numeric_save();
    std::setlocale(LC_NUMERIC, ""); // "" -> environment's default locale
    if (!lc_numeric_is_C()) {
        const FuncReadData null_func_read;
        const FuncWriteData null_func_write;
        const STL_MeshBasic null_mesh({});
        STL_MeshCreatorBasic meshcreator;
        int error[4] = {0};
        // By default, check LC_NUMERIC
        error[0] = STL_readAscii(null_func_read, &meshcreator);
        error[1] = STL_write(STL_Format_Ascii, null_func_write, null_mesh);
        error[2] = STL_readAscii(null_func_read, &meshcreator);
        error[3] = STL_write(STL_Format_Ascii, null_func_write, null_mesh);
        for (size_t i = 0; i < GMIO_ARRAY_SIZE(error); ++i) {
            UTEST_COMPARE(Error_BadLcNumeric, error[i]);
        }
    }
    else {
        std::cerr << "\nskip: default locale is NULL or already C/POSIX" << "\n";
    }
    lc_numeric_restore();

    return nullptr;
}

static void STL_generateBinaryTestModels()
{
    {
        FILE* outfile = std::fopen(filepath_stlb_empty, "wb");
        STL_writeBinaryHeader(Endianness_Little, FILE_funcWriteData(outfile), {}, 0);
        std::fclose(outfile);
    }

    {
        const char model_fpath_le[] = "models/solid_one_facet.le_stlb";
        const char model_fpath_be[] = "models/solid_one_facet.be_stlb";
        const STL_Triangle tri = {
            { 0.f, 0.f, 1.f },  // normal
            { 0.f, 0.f, 0.f },  // v1
            { 10.f, 0.f, 0.f }, // v2
            { 5.f, 10.f, 0.f }, // v3
            0                   // attr
        };
        const STL_MeshBasic mesh({tri});
        STL_write(STL_Format_BinaryLittleEndian, model_fpath_le, mesh);
        STL_write(STL_Format_BinaryBigEndian, model_fpath_be, mesh);
    }

    {
        FILE* infile = std::fopen(filepath_stla_4meshs, "rb");
        FILE* outfile = std::fopen(filepath_stlb_4meshs, "wb");
        const FuncReadData funcRead = FILE_funcReadData(infile);
        const FuncWriteData funcWrite = FILE_funcWriteData(outfile);
        int read_error = Error_OK;
        STL_ReadOptions ropts = {};
        ropts.ascii_solid_size = STL_probeAsciiSolidSize(funcRead);
        std::rewind(infile);
        while (noError(read_error)) {
            STL_MeshCreatorBasic creator;
            read_error = STL_readAscii(funcRead, &creator, ropts);
            const STL_MeshBasic mesh(creator.triangles());
            STL_WriteOptions wopts = {};
            wopts.binary_header =
                    STL_binaryHeaderFromString(creator.infos().ascii_solid_name);
            STL_write(STL_Format_BinaryLittleEndian, funcWrite, mesh, wopts);
        }
        std::fclose(infile);
        std::fclose(outfile);
    }

    {
        FILE* outfile = std::fopen(filepath_stlb_header_nofacets, "wb");
        STL_writeBinaryHeader(Endianness_Little, FILE_funcWriteData(outfile), {}, 100);
        std::fclose(outfile);
    }
}

} // namespace gmio
