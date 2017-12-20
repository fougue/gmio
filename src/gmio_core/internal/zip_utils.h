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

#pragma once

#if 0

#include "../global.h"
#include "../iodevice.h"
#include "../span.h"
#include <ctime>
#include <string>
#include <vector>

namespace gmio {

enum ZIP_CompressMethod {
    ZIP_COMPRESS_METHOD_NO_COMPRESSION = 0,
    ZIP_COMPRESS_METHOD_SHRUNK = 1,
    ZIP_COMPRESS_METHOD_REDUCE_FACTOR_1 = 2,
    ZIP_COMPRESS_METHOD_REDUCE_FACTOR_2 = 3,
    ZIP_COMPRESS_METHOD_REDUCE_FACTOR_3 = 4,
    ZIP_COMPRESS_METHOD_REDUCE_FACTOR_4 = 5,
    ZIP_COMPRESS_METHOD_IMPLODE = 6,
    ZIP_COMPRESS_METHOD_DEFLATE = 8,
    ZIP_COMPRESS_METHOD_DEFLATE64 = 9,
    ZIP_COMPRESS_METHOD_PKWARE_IMPLODE = 10,
    ZIP_COMPRESS_METHOD_BZIP2 = 12,
    ZIP_COMPRESS_METHOD_LZMA = 14,
    ZIP_COMPRESS_METHOD_IBM_TERSE = 18,
    ZIP_COMPRESS_METHOD_IBM_LZ77 = 19,
    ZIP_COMPRESS_METHOD_WAVPACK = 97,
    ZIP_COMPRESS_METHOD_PPMD_VERSION_I_REV_1 = 98
};

enum ZIP_GeneralPurposeFlag {
    //! Bit_0: if set, indicates that the file is encrypted
    ZIP_GENERAL_PURPOSE_FLAG_FILE_ENCRYPTED = 0x0001,

    // For Method 6 - Imploding
    //! Bit_1: if set, indicates an 8K sliding dictionary was used. If clear,
    //!        then a 4K sliding dictionary was used
    ZIP_GENERAL_PURPOSE_FLAG_IMPLODE_8K_SLIDING_DICT = 0x0002,
    //! Bit_2: if set, indicates 3 Shannon-Fano trees were used to encode the
    //!        sliding dictionary output. If clear, then 2 Shannon-Fano trees
    //!        were used
    ZIP_GENERAL_PURPOSE_FLAG_IMPLODE_3_SHANNON_FANO_TREES = 0x0004,

    // For Methods 8 and 9 - Deflating
    //! Bit_2=0  Bit_1=0  ->  Normal (-en) compression option was used
    ZIP_GENERAL_PURPOSE_FLAG_DEFLATE_COMPRESS_NORMAL = 0x0000,
    //! Bit_2=0  Bit_1=1  ->  Maximum (-exx/-ex) compression option was used
    ZIP_GENERAL_PURPOSE_FLAG_DEFLATE_COMPRESS_MAX = 0x0002,
    //! Bit_2=1  Bit_1=0  ->  Fast (-ef) compression option was used
    ZIP_GENERAL_PURPOSE_FLAG_DEFLATE_COMPRESS_FAST = 0x0004,
    //! Bit_2=1  Bit_1=1  ->  Super Fast (-es) compression option was used
    ZIP_GENERAL_PURPOSE_FLAG_DEFLATE_COMPRESS_SUPER_FAST = 0x0006,

    // For Method 14 - LZMA
    //! Bit_1: if set, indicates an end-of-stream (EOS) marker is used to mark
    //!        the end of the compressed data stream. If clear, then an EOS
    //!        marker is not present and the compressed data size must be known
    //!        to extract
    ZIP_GENERAL_PURPOSE_FLAG_LZMA_USE_EOS_MARKER = 0x0002,

    //! Bit_3: if set, the fields crc-32, compressed size and uncompressed size
    //!        are set to zero in the local header.
    //!        The correct values are put in the data descriptor immediately
    //!        following the compressed data
    ZIP_GENERAL_PURPOSE_FLAG_USE_DATA_DESCRIPTOR = 0x0008,

    //! Bit_5: if set, indicates that the file is compressed patched data.
    //!        (Note: Requires PKZIP version 2.70 or greater)
    ZIP_GENERAL_PURPOSE_FLAG_COMPRESSED_PATCHED_DATA = 0x0020,

    //! Bit_6: Strong encryption. If set, you MUST set the version needed to
    //!        extract value to at least 50 and you MUST also set bit 0. If AES
    //!        encryption is used, the version needed to extract value MUST be
    //!        at least 51.
    ZIP_GENERAL_PURPOSE_FLAG_STRONG_ENCRYPTION = 0x0040,

    //! Bit_11: Language encoding flag (EFS). If set, the filename and comment
    //!         fields for this file MUST be encoded using UTF-8
    ZIP_GENERAL_PURPOSE_FLAG_ENCODING_UTF8 = 0x0800,

    //! Bit_13: Set when encrypting the Central Directory to indicate selected
    //!         data values in the Local Header are masked to hide their actual
    //!         values
    ZIP_GENERAL_PURPOSE_FLAG_MASK_LOCAL_HEADER = 0x2000
};

//! Header ID mapping defined by PKWARE
enum ZIP_PkwareHeaderId {
    ZIP_PKWARE_HEADERID_ZIP64_EXTENDED_INFO = 0x0001,
    ZIP_PKWARE_HEADERID_AV_INFO = 0x0007,
    ZIP_PKWARE_HEADERID_EXTENDED_LANGUAGE_ENCODE_DATA = 0x0008,
    ZIP_PKWARE_HEADERID_OS2 = 0x0009,
    ZIP_PKWARE_HEADERID_NTFS = 0x000A,
    ZIP_PKWARE_HEADERID_OPENVMS = 0x000C,
    ZIP_PKWARE_HEADERID_UNIX = 0x000D,
    ZIP_PKWARE_HEADERID_STREAM_AND_FORK_DESCRIPTORS = 0x000E,
    ZIP_PKWARE_HEADERID_PATCH_DESCRIPTOR = 0x000F,
    ZIP_PKWARE_HEADERID_PKCS7_X509_CERT = 0x0014,
    ZIP_PKWARE_HEADERID_X509_CERTID_FOR_FILE = 0x0015,
    ZIP_PKWARE_HEADERID_X509_CERTID_FOR_CENTRAL_DIRECTORY = 0x0016,
    ZIP_PKWARE_HEADERID_STRONG_ENCRYPTION_HEADER = 0x0017,
    ZIP_PKWARE_HEADERID_RECORD_MANAGEMENT_CONTROLS = 0x0018,
    ZIP_PKWARE_HEADERID_PKCS7_ENCRYPTION_RECIPIENT_CERT_LIST = 0x0019
};

enum ZIP_FeatureVersion {
    ZIP_FEATURE_VERSION_DEFAULT = 10,
    ZIP_FEATURE_VERSION_FILE_VOLUME_LABEL = 11,
    ZIP_FEATURE_VERSION_FILE_FOLDER = 20,
    ZIP_FEATURE_VERSION_FILE_COMPRESSED_DEFLATE = 20,
    ZIP_FEATURE_VERSION_FILE_ENCRYPTED_PKWARE = 20,
    ZIP_FEATURE_VERSION_FILE_COMPRESSED_DEFLATE64 = 21,
    ZIP_FEATURE_VERSION_FILE_COMPRESSED_PKWARE_DCL_IMPLODE = 25,
    ZIP_FEATURE_VERSION_FILE_PATCH_DATA_SET = 27,
    ZIP_FEATURE_VERSION_FILE_ZIP64_FORMAT_EXTENSIONS = 45,
    ZIP_FEATURE_VERSION_FILE_COMPRESSED_BZIP2 = 46,
    ZIP_FEATURE_VERSION_FILE_ENCRYPTED_DES = 50,
    ZIP_FEATURE_VERSION_FILE_ENCRYPTED_3DES = 50,
    ZIP_FEATURE_VERSION_FILE_ENCRYPTED_RC2 = 50,
    ZIP_FEATURE_VERSION_FILE_ENCRYPTED_RC4 = 50,
    ZIP_FEATURE_VERSION_FILE_ENCRYPTED_AES = 51,
    ZIP_FEATURE_VERSION_FILE_ENCRYPTED_RC2_CORRECTED = 51,
    ZIP_FEATURE_VERSION_FILE_ENCRYPTED_RC2_64_CORRECTED = 52,
    ZIP_FEATURE_VERSION_FILE_ENCRYPTED_NON_OAEP_KEY_WRAP = 61,
    ZIP_FEATURE_VERSION_CENTRAL_DIRECTORY_ENCRYPTION = 62,
    ZIP_FEATURE_VERSION_FILE_COMPRESSED_LZMA = 63,
    ZIP_FEATURE_VERSION_FILE_COMPRESSED_PPMD_PLUS = 63,
    ZIP_FEATURE_VERSION_FILE_ENCRYPTED_BLOWFISH = 63,
    ZIP_FEATURE_VERSION_FILE_ENCRYPTED_TWOFISH = 63
};

struct ZIP_LocalFileHeader {
    ZIP_FeatureVersion version_needed_to_extract;
    uint16_t general_purpose_flags;
    ZIP_CompressMethod compress_method;
    const std::tm* lastmod_datetime;
    uint32_t crc32;
    uint32_t compressed_size;
    uint32_t uncompressed_size;
    std::string filename;
    std::vector<uint8_t> extrafield;
};

struct ZIP_DataDescriptor {
    bool use_zip64;
    uint32_t crc32;
    uint64_t compressed_size;
    uint64_t uncompressed_size;
};

struct ZIP_CentralDirectoryHeader {
    bool use_zip64;
    uint16_t version_made_by;
    ZIP_FeatureVersion version_needed_to_extract;
    uint16_t general_purpose_flags;
    ZIP_CompressMethod compress_method;
    const std::tm* lastmod_datetime;
    uint32_t crc32;
    uint32_t compressed_size;
    uint32_t uncompressed_size;
    uint16_t internal_file_attrs;
    uint32_t external_file_attrs;
    uint32_t local_header_offset;
    std::string filename;
    std::vector<uint8_t> extrafield;
    std::string filecomment;
};

enum {
    ZIP_SIZE_LOCAL_FILE_HEADER = 4 + 5*2 + 3*4 + 2*2,
    ZIP_SIZE_CENTRAL_DIRECTORY_HEADER = 4 + 6*2 + 3*4 + 5*2 +2*4,
    ZIP_SIZE_DATA_DESCRIPTOR = 4 + 4 + 4 + 4,
    ZIP64_SIZE_EXTRAFIELD = 2*2 + 3*8 + 4,
    ZIP64_SIZE_DATA_DESCRIPTOR = 4 + 4 + 2*8,
    ZIP64_SIZE_END_OF_CENTRAL_DIRECTORY_RECORD = 4 + 8 + 2*2 + 2*4 + 4*8,
    ZIP64_SIZE_END_OF_CENTRAL_DIRECTORY_LOCATOR = 4 + 4 + 8 + 4,
    ZIP_SIZE_END_OF_CENTRAL_DIRECTORY_RECORD = 4 + 4*2 + 2*4 + 2
};

//! Zip64 extended info (extra field)
struct ZIP64_ExtraField {
    uint64_t compressed_size;
    uint64_t uncompressed_size;
    uint64_t local_header_offset;
};

struct ZIP_EndOfCentralDirectoryRecord {
    bool use_zip64;
    uint16_t entry_count;
    uint32_t central_dir_size;
    uint32_t central_dir_offset;
    std::string filecomment;
};

struct ZIP64_EndOfCentralDirectoryRecord {
    uint16_t version_made_by;
    enum ZIP_FeatureVersion version_needed_to_extract;
    uint64_t entry_count;
    uint64_t central_dir_size;
    uint64_t central_dir_offset;
};

struct ZIP64_EndOfCentralDirectoryLocator {
    uint64_t end_of_central_dir_offset;
};

const unsigned ZIP_TagError = 0x00100000;
enum ZIP_UtilsError {
    ZIP_Error_BadMagic = ZIP_TagError + 1,
    ZIP_Error_BadExtraFieldTag,
    ZIP_Error_BadExtraFieldSize
};

//
// ZIP read functions
//

size_t ZIP_readLocalFileHeader(
        const FuncReadData& func_read,
        ZIP_LocalFileHeader* info,
        int* ptr_error = nullptr);

size_t ZIP_readDataDescriptor(
        const FuncReadData& func_read,
        ZIP_DataDescriptor* info,
        int* ptr_error = nullptr);

size_t ZIP64_readDataDescriptor(
        const FuncReadData& func_read,
        ZIP_DataDescriptor* info,
        int* ptr_error = nullptr);

size_t ZIP_readCentralDirectoryHeader(
        const FuncReadData& func_read,
        ZIP_CentralDirectoryHeader* info,
        int* ptr_error = nullptr);

size_t ZIP64_readExtraField(
        const FuncReadData& func_read,
        ZIP64_ExtraField* info,
        int* ptr_error = nullptr);

size_t ZIP64_readEndOfCentralDirectoryRecord(
        const FuncReadData& func_read,
        ZIP64_EndOfCentralDirectoryRecord* info,
        int* ptr_error = nullptr);

size_t ZIP64_readEndOfCentralDirectoryLocator(
        const FuncReadData& func_read,
        ZIP64_EndOfCentralDirectoryLocator* info,
        int* ptr_error = nullptr);

size_t ZIP_readEndOfCentralDirectoryRecord(
        const FuncReadData& func_read,
        ZIP_EndOfCentralDirectoryRecord* info,
        int* ptr_error = nullptr);

//
// ZIP write functions
//

size_t ZIP_writeLocalFileHeader(
        const FuncWriteData& func_write,
        const ZIP_LocalFileHeader& info,
        int* ptr_error = nullptr);

size_t ZIP_writeDataDescriptor(
        const FuncWriteData& func_write,
        const ZIP_DataDescriptor& info,
        int* ptr_error = nullptr);

size_t ZIP_writeCentralDirectoryHeader(
        const FuncWriteData& func_write,
        const ZIP_CentralDirectoryHeader& info,
        int* ptr_error = nullptr);

//! Writes Zip64 extra field to \p buff.
//! Returns \c GMIO_ERROR_INVALID_MEMBLOCK_SIZE if \p buff_capacity is less than
//! \c GMIO_ZIP64_SIZE_EXTRAFIELD */
size_t ZIP64_writeExtraField(
        Span<uint8_t> buff,
        const ZIP64_ExtraField& info,
        int* ptr_error = nullptr);

size_t ZIP64_writeEndOfCentralDirectoryRecord(
        const FuncWriteData& func_write,
        const ZIP64_EndOfCentralDirectoryRecord& info,
        int* ptr_error = nullptr);

size_t ZIP64_writeEndOfCentralDirectoryLocator(
        const FuncWriteData& func_write,
        const ZIP64_EndOfCentralDirectoryLocator& info,
        int* ptr_error = nullptr);

size_t ZIP_writeEndOfCentralDirectoryRecord(
        const FuncWriteData& func_write,
        const ZIP_EndOfCentralDirectoryRecord& info,
        int* ptr_error = nullptr);

//! Is Zip64 format required for this file uncompress/compress sizes ?
bool ZIP64_required(
        uint64_t uncompressed_size, uint64_t compressed_size);

//! Defines a file entry in a ZIP archive
struct ZIP_FileEntry {
    ZIP_CompressMethod compress_method;
    ZIP_FeatureVersion feature_version;
    std::string filename;
    void* cookie_func_write_file_data;
    int (*func_write_file_data)(void* cookie, ZIP_DataDescriptor* dd);
};

//! Writes a ZIP archive containing a single file
bool ZIP_writeSingleFile(
        const FuncWriteData& func_write,
        const ZIP_FileEntry& file_entry,
        int* ptr_error = nullptr);


} // namespace gmio
#endif
