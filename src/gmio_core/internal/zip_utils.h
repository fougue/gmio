/****************************************************************************
** Copyright (c) 2016, Fougue Ltd. <http://www.fougue.pro>
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

#ifndef GMIO_INTERNAL_ZIP_UTILS_H
#define GMIO_INTERNAL_ZIP_UTILS_H

#include "../global.h"

#include "../stream.h"
#include <time.h>

enum gmio_zip_compress_method {
    GMIO_ZIP_COMPRESS_METHOD_NO_COMPRESSION = 0,
    GMIO_ZIP_COMPRESS_METHOD_SHRUNK = 1,
    GMIO_ZIP_COMPRESS_METHOD_REDUCE_FACTOR_1 = 2,
    GMIO_ZIP_COMPRESS_METHOD_REDUCE_FACTOR_2 = 3,
    GMIO_ZIP_COMPRESS_METHOD_REDUCE_FACTOR_3 = 4,
    GMIO_ZIP_COMPRESS_METHOD_REDUCE_FACTOR_4 = 5,
    GMIO_ZIP_COMPRESS_METHOD_IMPLODE = 6,
    GMIO_ZIP_COMPRESS_METHOD_DEFLATE = 8,
    GMIO_ZIP_COMPRESS_METHOD_DEFLATE64 = 9,
    GMIO_ZIP_COMPRESS_METHOD_PKWARE_IMPLODE = 10,
    GMIO_ZIP_COMPRESS_METHOD_BZIP2 = 12,
    GMIO_ZIP_COMPRESS_METHOD_LZMA = 14,
    GMIO_ZIP_COMPRESS_METHOD_IBM_TERSE = 18,
    GMIO_ZIP_COMPRESS_METHOD_IBM_LZ77 = 19,
    GMIO_ZIP_COMPRESS_METHOD_WAVPACK = 97,
    GMIO_ZIP_COMPRESS_METHOD_PPMD_VERSION_I_REV_1 = 98
};

enum gmio_zip_general_purpose_flag {
    /*! Bit_0: if set, indicates that the file is encrypted */
    GMIO_ZIP_GENERAL_PURPOSE_FLAG_FILE_ENCRYPTED = 0x0001,

    /* For Method 6 - Imploding */
    /*! Bit_1: if set, indicates an 8K sliding dictionary was used. If clear,
     *         then a 4K sliding dictionary was used */
    GMIO_ZIP_GENERAL_PURPOSE_FLAG_IMPLODE_8K_SLIDING_DICT = 0x0002,
    /*! Bit_2: if set, indicates 3 Shannon-Fano trees were used to encode the
     *         sliding dictionary output. If clear, then 2 Shannon-Fano trees
     *         were used */
    GMIO_ZIP_GENERAL_PURPOSE_FLAG_IMPLODE_3_SHANNON_FANO_TREES = 0x0004,

    /* For Methods 8 and 9 - Deflating */
    /*! Bit_2=0  Bit_1=0  ->  Normal (-en) compression option was used */
    GMIO_ZIP_GENERAL_PURPOSE_FLAG_DEFLATE_COMPRESS_NORMAL = 0x0000,
    /*! Bit_2=0  Bit_1=1  ->  Maximum (-exx/-ex) compression option was used */
    GMIO_ZIP_GENERAL_PURPOSE_FLAG_DEFLATE_COMPRESS_MAX = 0x0002,
    /*! Bit_2=1  Bit_1=0  ->  Fast (-ef) compression option was used */
    GMIO_ZIP_GENERAL_PURPOSE_FLAG_DEFLATE_COMPRESS_FAST = 0x0004,
    /*! Bit_2=1  Bit_1=1  ->  Super Fast (-es) compression option was used */
    GMIO_ZIP_GENERAL_PURPOSE_FLAG_DEFLATE_COMPRESS_SUPER_FAST = 0x0006,

    /* For Method 14 - LZMA */
    /*! Bit_1: if set, indicates an end-of-stream (EOS) marker is used to mark
     *         the end of the compressed data stream. If clear, then an EOS
     *         marker is not present and the compressed data size must be known
     *         to extract */
    GMIO_ZIP_GENERAL_PURPOSE_FLAG_LZMA_USE_EOS_MARKER = 0x0002,

    /*! Bit_3: if set, the fields crc-32, compressed size and uncompressed size
     *         are set to zero in the local header.
     *         The correct values are put in the data descriptor immediately
     *         following the compressed data */
    GMIO_ZIP_GENERAL_PURPOSE_FLAG_USE_DATA_DESCRIPTOR = 0x0008,

    /*! Bit_5: if set, indicates that the file is compressed patched data.
     *         (Note: Requires PKZIP version 2.70 or greater) */
    GMIO_ZIP_GENERAL_PURPOSE_FLAG_COMPRESSED_PATCHED_DATA = 0x0020,

    /*! Bit_6: Strong encryption. If set, you MUST set the version needed to
     *         extract value to at least 50 and you MUST also set bit 0. If AES
     *         encryption is used, the version needed to extract value MUST be
     *         at least 51. */
    GMIO_ZIP_GENERAL_PURPOSE_FLAG_STRONG_ENCRYPTION = 0x0040,

    /*! Bit_11: Language encoding flag (EFS). If set, the filename and comment
     *          fields for this file MUST be encoded using UTF-8 */
    GMIO_ZIP_GENERAL_PURPOSE_FLAG_ENCODING_UTF8 = 0x0800,

    /*! Bit_13: Set when encrypting the Central Directory to indicate selected
     *          data values in the Local Header are masked to hide their actual
     *          values */
    GMIO_ZIP_GENERAL_PURPOSE_FLAG_MASK_LOCAL_HEADER = 0x2000
};

/*! Header ID mapping defined by PKWARE */
enum gmio_zip_pkware_headerid
{
    GMIO_ZIP_PKWARE_HEADERID_ZIP64_EXTENDED_INFO = 0x0001,
    GMIO_ZIP_PKWARE_HEADERID_AV_INFO = 0x0007,
    GMIO_ZIP_PKWARE_HEADERID_EXTENDED_LANGUAGE_ENCODE_DATA = 0x0008,
    GMIO_ZIP_PKWARE_HEADERID_OS2 = 0x0009,
    GMIO_ZIP_PKWARE_HEADERID_NTFS = 0x000A,
    GMIO_ZIP_PKWARE_HEADERID_OPENVMS = 0x000C,
    GMIO_ZIP_PKWARE_HEADERID_UNIX = 0x000D,
    GMIO_ZIP_PKWARE_HEADERID_STREAM_AND_FORK_DESCRIPTORS = 0x000E,
    GMIO_ZIP_PKWARE_HEADERID_PATCH_DESCRIPTOR = 0x000F,
    GMIO_ZIP_PKWARE_HEADERID_PKCS7_X509_CERT = 0x0014,
    GMIO_ZIP_PKWARE_HEADERID_X509_CERTID_FOR_FILE = 0x0015,
    GMIO_ZIP_PKWARE_HEADERID_X509_CERTID_FOR_CENTRAL_DIRECTORY = 0x0016,
    GMIO_ZIP_PKWARE_HEADERID_STRONG_ENCRYPTION_HEADER = 0x0017,
    GMIO_ZIP_PKWARE_HEADERID_RECORD_MANAGEMENT_CONTROLS = 0x0018,
    GMIO_ZIP_PKWARE_HEADERID_PKCS7_ENCRYPTION_RECIPIENT_CERT_LIST = 0x0019
};

enum gmio_zip_feature_version {
    GMIO_ZIP_FEATURE_VERSION_DEFAULT = 10,
    GMIO_ZIP_FEATURE_VERSION_FILE_VOLUME_LABEL = 11,
    GMIO_ZIP_FEATURE_VERSION_FILE_FOLDER = 20,
    GMIO_ZIP_FEATURE_VERSION_FILE_COMPRESSED_DEFLATE = 20,
    GMIO_ZIP_FEATURE_VERSION_FILE_ENCRYPTED_PKWARE = 20,
    GMIO_ZIP_FEATURE_VERSION_FILE_COMPRESSED_DEFLATE64 = 21,
    GMIO_ZIP_FEATURE_VERSION_FILE_COMPRESSED_PKWARE_DCL_IMPLODE = 25,
    GMIO_ZIP_FEATURE_VERSION_FILE_PATCH_DATA_SET = 27,
    GMIO_ZIP_FEATURE_VERSION_FILE_ZIP64_FORMAT_EXTENSIONS = 45,
    GMIO_ZIP_FEATURE_VERSION_FILE_COMPRESSED_BZIP2 = 46,
    GMIO_ZIP_FEATURE_VERSION_FILE_ENCRYPTED_DES = 50,
    GMIO_ZIP_FEATURE_VERSION_FILE_ENCRYPTED_3DES = 50,
    GMIO_ZIP_FEATURE_VERSION_FILE_ENCRYPTED_RC2 = 50,
    GMIO_ZIP_FEATURE_VERSION_FILE_ENCRYPTED_RC4 = 50,
    GMIO_ZIP_FEATURE_VERSION_FILE_ENCRYPTED_AES = 51,
    GMIO_ZIP_FEATURE_VERSION_FILE_ENCRYPTED_RC2_CORRECTED = 51,
    GMIO_ZIP_FEATURE_VERSION_FILE_ENCRYPTED_RC2_64_CORRECTED = 52,
    GMIO_ZIP_FEATURE_VERSION_FILE_ENCRYPTED_NON_OAEP_KEY_WRAP = 61,
    GMIO_ZIP_FEATURE_VERSION_CENTRAL_DIRECTORY_ENCRYPTION = 62,
    GMIO_ZIP_FEATURE_VERSION_FILE_COMPRESSED_LZMA = 63,
    GMIO_ZIP_FEATURE_VERSION_FILE_COMPRESSED_PPMD_PLUS = 63,
    GMIO_ZIP_FEATURE_VERSION_FILE_ENCRYPTED_BLOWFISH = 63,
    GMIO_ZIP_FEATURE_VERSION_FILE_ENCRYPTED_TWOFISH = 63
};

/*! Zip local file header */
struct gmio_zip_local_file_header {
    enum gmio_zip_feature_version version_needed_to_extract;
    uint16_t general_purpose_flags;
    enum gmio_zip_compress_method compress_method;
    const struct tm* lastmod_datetime;
    uint32_t crc32;
    uint32_t compressed_size;
    uint32_t uncompressed_size;
    uint16_t filename_len;
    uint16_t extrafield_len;
    const char* filename;
    const uint8_t* extrafield;
};

/*! Zip data descriptor */
struct gmio_zip_data_descriptor {
    bool use_zip64;
    uint32_t crc32;
    uintmax_t compressed_size;
    uintmax_t uncompressed_size;
};

/*! Zip central directory header */
struct gmio_zip_central_directory_header {
    bool use_zip64;
    uint16_t version_made_by;
    enum gmio_zip_feature_version version_needed_to_extract;
    uint16_t general_purpose_flags;
    enum gmio_zip_compress_method compress_method;
    const struct tm* lastmod_datetime;
    uint32_t crc32;
    uint32_t compressed_size;
    uint32_t uncompressed_size;
    uint16_t filename_len;
    uint16_t extrafield_len;
    uint16_t filecomment_len;
    uint16_t disk_nb_start;
    uint16_t internal_file_attrs;
    uint32_t external_file_attrs;
    uint32_t relative_offset_local_header;
    const char* filename;
    const uint8_t* extrafield;
    const char* filecomment;
};

enum {
    GMIO_ZIP64_SIZE_EXTRAFIELD_EXTENDED_INFO = 2*2 + 3*8 + 4
};

/*! Zip64 extended info (extra field) */
struct gmio_zip64_extrablock_extended_info {
    uintmax_t compressed_size;
    uintmax_t uncompressed_size;
    uintmax_t relative_offset_local_header;
    uint32_t disk_nb_start;
};

/*! Zip end of central directory record */
struct gmio_zip_end_of_central_directory_record {
    bool use_zip64;
    uint16_t disk_nb;
    uint16_t disk_nb_with_start_of_central_dir;
    uint16_t total_entry_count_in_central_dir_on_disk;
    uint16_t total_entry_count_in_central_dir;
    uint32_t central_dir_size;
    uint32_t start_offset_central_dir_from_disk_start_nb;
    uint16_t filecomment_len;
    const char* filecomment;
};

/*! Zip64 end of central directory record */
struct gmio_zip64_end_of_central_directory_record {
    uintmax_t remaining_record_size;                       /* should be 64b */
    uint16_t version_made_by;
    enum gmio_zip_feature_version version_needed_to_extract;
    uint32_t disk_nb;
    uint32_t disk_nb_with_start_of_central_dir;
    uintmax_t total_entry_count_in_central_dir_on_disk;    /* should be 64b */
    uintmax_t total_entry_count_in_central_dir;            /* should be 64b */
    uintmax_t central_dir_size;                            /* should be 64b */
    uintmax_t start_offset_central_dir_from_disk_start_nb; /* should be 64b */
    const uint8_t* extensible_data_sector; /* Reserved for use by PKWARE */
};

struct gmio_zip_write_result {
    int error;
    size_t written_len;
};

enum { GMIO_ZIP_UTILS_ERROR_TAG = 0x00AA0000 };
enum gmio_zip_utils_error {
    GMIO_ZIP_UTILS_ERROR_BAD_MAGIC = GMIO_ZIP_UTILS_ERROR_TAG + 0x01
};

/*! Reads ZIP local file header from \p stream */
size_t gmio_zip_read_local_file_header(
        struct gmio_stream* stream,
        struct gmio_zip_local_file_header* info,
        int* ptr_error);

/*! Reads ZIP data descriptor from \p stream */
size_t gmio_zip_read_data_descriptor(
        struct gmio_stream* stream,
        struct gmio_zip_data_descriptor* info,
        int* ptr_error);

/*! Reads Zip64 data descriptor from \p stream */
size_t gmio_zip64_read_data_descriptor(
        struct gmio_stream* stream,
        struct gmio_zip_data_descriptor* info,
        int* ptr_error);

/*! Reads ZIP central directory header from \p stream */
size_t gmio_zip_read_central_directory_header(
        struct gmio_stream* stream,
        struct gmio_zip_central_directory_header* info,
        int* ptr_error);

/*! Reads ZIP end of central directory record from \p stream */
size_t gmio_zip_read_end_of_central_directory_record(
        struct gmio_stream* stream,
        struct gmio_zip_end_of_central_directory_record* info,
        int* ptr_error);

/*! Writes ZIP local file header to \p stream */
size_t gmio_zip_write_local_file_header(
        struct gmio_stream* stream,
        const struct gmio_zip_local_file_header* info,
        int* ptr_error);

/*! Writes ZIP data descriptor to \p stream */
size_t gmio_zip_write_data_descriptor(
        struct gmio_stream* stream,
        const struct gmio_zip_data_descriptor* info,
        int* ptr_error);

/*! Writes ZIP central directory header to \p stream */
size_t gmio_zip_write_central_directory_header(
        struct gmio_stream* stream,
        const struct gmio_zip_central_directory_header* info,
        int* ptr_error);

/*! Writes ZIP local file header to \p buff
 *
 *  Returns \c GMIO_ERROR_INVALID_MEMBLOCK_SIZE if \p buff_capacity is less than
 *  \c GMIO_ZIP64_SIZE_EXTRAFIELD_EXTENDED_INFO
 */
size_t gmio_zip64_write_extrafield_extended_info(
        uint8_t* buff,
        size_t buff_capacity,
        const struct gmio_zip64_extrablock_extended_info* info,
        int* ptr_error);

/*! Writes ZIP end of central directory record to \p stream */
size_t gmio_zip_write_end_of_central_directory_record(
        struct gmio_stream* stream,
        const struct gmio_zip_end_of_central_directory_record* info,
        int* ptr_error);

#endif /* GMIO_INTERNAL_ZIP_UTILS_H */
