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

#include "zip_utils.h"

#include "../error.h"
#include "byte_codec.h"
#include "helper_stream.h"
#include <stddef.h>

/* ----------
 * Constants
 * ---------- */

enum {
    GMIO_ZIP_SIZE_LOCAL_FILE_HEADER = 4 + 5*2 + 3*4 + 2*2,
    GMIO_ZIP_SIZE_CENTRAL_DIRECTORY_HEADER = 4 + 6*2 + 3*4 + 5*2 +2*4,
    GMIO_ZIP64_SIZE_DATA_DESCRIPTOR = 4 + 2*8,
    GMIO_ZIP_SIZE_DATA_DESCRIPTOR = 4 + 2*4,
    GMIO_ZIP_SIZE_END_OF_CENTRAL_DIRECTORY_RECORD = 4 + 4*2 + 2*4 + 2
};

/* ----------
 * Internal functions
 * ---------- */

/* See http://www.vsft.com/hal/dostime.htm */
static uint32_t gmio_to_msdos_datetime(const struct tm* datetime)
{
    const uint32_t msdos_datetime =
            /* Time */
            (datetime->tm_sec)                  /* bit 0-4 */
            | (datetime->tm_min << 5)           /* bit 5-10 */
            | (datetime->tm_hour << 11)         /* bit 11-15 */
            /* Date */
            | (datetime->tm_mday << 16)         /* bit 16-20 */
            | ((datetime->tm_mon + 1) << 21)    /* bit 21-24 */
            | ((datetime->tm_year - 80) << 25); /* bit 21-24 */
    return msdos_datetime;
}

/* Returns a non-null datetime, if arg is NULL it returns current gmtime() */
static const struct tm* gmio_nonnull_datetime(const struct tm* datetime)
{
    if (datetime == NULL) {
        const time_t current_time = time(NULL);
        return gmtime(&current_time); /* UTC time */
    }
    return datetime;
}

#ifdef GMIO_HAVE_INT64_TYPE
/* Encodes little-endian 64b val in buffer and returns advanced buffer pos */
static uint8_t* gmio_adv_encode_uint64_le(uint64_t val, uint8_t* bytes)
{
    gmio_encode_uint64_le(val, bytes);
    return bytes + 8;
}
#endif

/* Encodes little-endian 32b val in buffer and returns advanced buffer pos */
static uint8_t* gmio_adv_encode_uint32_le(uint32_t val, uint8_t* bytes)
{
    gmio_encode_uint32_le(val, bytes);
    return bytes + 4;
}

/* Encodes little-endian 16b val in buffer and returns advanced buffer pos */
static uint8_t* gmio_adv_encode_uint16_le(uint16_t val, uint8_t* bytes)
{
    gmio_encode_uint16_le(val, bytes);
    return bytes + 2;
}

/* Helper to facilitate return from gmio_zip_write_xxx() API functions */
static size_t gmio_zip_write_returnhelper(
        struct gmio_stream* stream, size_t written, size_t expected, int* error)
{
    if (error != NULL) {
        const bool no_error = written == expected && !gmio_stream_error(stream);
        *error = no_error ? GMIO_ERROR_OK : GMIO_ERROR_STREAM;
    }
    return written;
}

/* ----------
 * API functions
 * ---------- */

size_t gmio_zip_write_local_file_header(
        struct gmio_stream* stream,
        const struct gmio_zip_local_file_header* info,
        int* error)
{
    uint8_t fixed_data[GMIO_ZIP_SIZE_LOCAL_FILE_HEADER];
    uint8_t* buff = fixed_data;

    const bool use_data_descriptor =
            info->general_purpose_flags
            & GMIO_ZIP_GENERAL_PURPOSE_FLAG_USE_DATA_DESCRIPTOR;

    /* 4-bytes magic number 0x04034b50 */
    buff = gmio_adv_encode_uint32_le(0x04034b50, buff);
    /* 2-bytes version needed to extract */
    buff = gmio_adv_encode_uint16_le(info->version_needed_to_extract, buff);
    /* 2-bytes general purpose bit flag */
    buff = gmio_adv_encode_uint16_le(info->general_purpose_flags, buff);
    /* 2-bytes compression method */
    buff = gmio_adv_encode_uint16_le(info->compress_method, buff);
    /* 2-bytes last mod file time */
    /* 2-bytes last mod file date */
    const struct tm* lastmod = gmio_nonnull_datetime(info->lastmod_datetime);
    buff = gmio_adv_encode_uint32_le(gmio_to_msdos_datetime(lastmod), buff);
    /* 4-bytes crc-32 */
    buff = gmio_adv_encode_uint32_le(
                use_data_descriptor ? 0 : info->crc32, buff);
    /* 4-bytes compressed size */
    buff = gmio_adv_encode_uint32_le(
                use_data_descriptor ? 0 : info->compressed_size, buff);
    /* 4-bytes uncompressed size */
    buff = gmio_adv_encode_uint32_le(
                use_data_descriptor ? 0 : info->uncompressed_size, buff);
    /* 2-bytes file name length */
    buff = gmio_adv_encode_uint16_le(info->filename_len, buff);
    /* 2-bytes extra field length */
    buff = gmio_adv_encode_uint16_le(info->extrafield_len, buff);

    /* Write to stream */
    const size_t expected_written_len =
            sizeof(fixed_data) + info->filename_len + info->extrafield_len;
    size_t written_len = 0;
    written_len +=
            gmio_stream_write_bytes(stream, fixed_data, sizeof(fixed_data));
    written_len +=
            gmio_stream_write_bytes(stream, info->filename, info->filename_len);
    written_len +=
            gmio_stream_write_bytes(stream, info->extrafield, info->extrafield_len);
    return gmio_zip_write_returnhelper(
                stream, written_len, expected_written_len, error);
}

size_t gmio_zip_write_data_descriptor(
        struct gmio_stream *stream,
        const struct gmio_zip_data_descriptor *info,
        int* error)
{
    const size_t fixed_data_len =
            info->use_zip64 ?
                GMIO_ZIP64_SIZE_DATA_DESCRIPTOR :
                GMIO_ZIP_SIZE_DATA_DESCRIPTOR;
    uint8_t fixed_data[GMIO_ZIP64_SIZE_DATA_DESCRIPTOR];
    uint8_t* buff = fixed_data;

    /* 4-bytes crc-32 */
    buff = gmio_adv_encode_uint32_le(info->crc32, buff);
    /* Compressed size and uncompressed size (4 or 8 bytes) */
    if (info->use_zip64) {
#ifdef GMIO_HAVE_INT64_TYPE
        buff = gmio_adv_encode_uint64_le(info->compressed_size, buff);
        buff = gmio_adv_encode_uint64_le(info->uncompressed_size, buff);
#else
        return GMIO_ERROR_UNKNOWN; /* TODO: error code */
#endif
    }
    else {
        if (info->compressed_size <= UINT32_MAX
                && info->uncompressed_size <= UINT32_MAX)
        {
            buff = gmio_adv_encode_uint32_le((uint32_t)info->compressed_size, buff);
            buff = gmio_adv_encode_uint32_le((uint32_t)info->uncompressed_size, buff);
        }
        else {
            return GMIO_ERROR_UNKNOWN; /* TODO: error code */
        }
    }

    /* Write to stream */
    const size_t written_len =
            gmio_stream_write_bytes(stream, fixed_data, fixed_data_len);
    return gmio_zip_write_returnhelper(
                stream, written_len, fixed_data_len, error);
}

size_t gmio_zip_write_central_directory_header(
        struct gmio_stream *stream,
        const struct gmio_zip_central_directory_header *info,
        int* error)
{
    uint8_t fixed_data[GMIO_ZIP_SIZE_CENTRAL_DIRECTORY_HEADER];
    uint8_t* buff = fixed_data;

    /* 4-bytes magic number 0x02014b50 */
    buff = gmio_adv_encode_uint32_le(0x02014b50, buff);
    /* 2-bytes version made by */
    buff = gmio_adv_encode_uint16_le(info->version_made_by, buff);
    /* 2-bytes version needed to extract */
    buff = gmio_adv_encode_uint16_le(info->version_needed_to_extract, buff);
    /* 2-bytes general purpose bit flag */
    buff = gmio_adv_encode_uint16_le(info->general_purpose_flags, buff);
    /* 2-bytes compression method */
    buff = gmio_adv_encode_uint16_le(info->compress_method, buff);
    /* 2-bytes last mod file time */
    /* 2-bytes last mod file date */
    const struct tm* lastmod = gmio_nonnull_datetime(info->lastmod_datetime);
    buff = gmio_adv_encode_uint32_le(gmio_to_msdos_datetime(lastmod), buff);
    /* 4-bytes crc-32 */
    buff = gmio_adv_encode_uint32_le(info->crc32, buff);
    /* 4-bytes compressed size */
    const uint32_t compressed_size =
            info->use_zip64 ? UINT32_MAX : info->compressed_size;
    buff = gmio_adv_encode_uint32_le(compressed_size, buff);
    /* 4-bytes uncompressed size */
    const uint32_t uncompressed_size =
            info->use_zip64 ? UINT32_MAX : info->uncompressed_size;
    buff = gmio_adv_encode_uint32_le(uncompressed_size, buff);
    /* 2-bytes file name length */
    buff = gmio_adv_encode_uint16_le(info->filename_len, buff);
    /* 2-bytes extra field length */
    buff = gmio_adv_encode_uint16_le(info->extrafield_len, buff);
    /* 2-bytes file comment length */
    buff = gmio_adv_encode_uint16_le(info->filecomment_len, buff);
    /* 2-bytes disk number start */
    buff = gmio_adv_encode_uint16_le(info->disk_nb_start, buff);
    /* 2-bytes internal file attributes */
    buff = gmio_adv_encode_uint16_le(info->internal_file_attrs, buff);
    /* 4-bytes external file attributes */
    buff = gmio_adv_encode_uint32_le(info->external_file_attrs, buff);
    /* 4-bytes relative offset of local header */
    const uint32_t relative_offset_local_header =
            info->use_zip64 ? UINT32_MAX : info->relative_offset_local_header;
    buff = gmio_adv_encode_uint32_le(relative_offset_local_header, buff);

    /* Write to stream */
    const size_t expected_written_len =
            sizeof(fixed_data)
            + info->filename_len + info->extrafield_len + info->filecomment_len;
    size_t written_len = 0;
    written_len +=
            gmio_stream_write_bytes(stream, fixed_data, sizeof(fixed_data));
    written_len +=
            gmio_stream_write_bytes(stream, info->filename, info->filename_len);
    written_len +=
            gmio_stream_write_bytes(stream, info->extrafield, info->extrafield_len);
    written_len +=
            gmio_stream_write_bytes(stream, info->filecomment, info->filecomment_len);
    return gmio_zip_write_returnhelper(
                stream, written_len, expected_written_len, error);
}

size_t gmio_zip64_write_extrafield_extended_info(
        uint8_t *buff,
        size_t buff_capacity,
        const struct gmio_zip64_extrablock_extended_info *info,
        int* error)
{
    if (buff_capacity < GMIO_ZIP64_SIZE_EXTRAFIELD_EXTENDED_INFO) {
        *error = GMIO_ERROR_INVALID_MEMBLOCK_SIZE;
        return 0;
    }
#ifdef GMIO_HAVE_INT64_TYPE
    /* Tag */
    buff = gmio_adv_encode_uint16_le(0x0001, buff);
    /* Size of the "extra" block */
    buff = gmio_adv_encode_uint16_le(
                GMIO_ZIP64_SIZE_EXTRAFIELD_EXTENDED_INFO - 2, buff);
    /* Original uncompressed file size */
    buff = gmio_adv_encode_uint64_le(info->uncompressed_size, buff);
    /* Size of compressed data */
    buff = gmio_adv_encode_uint64_le(info->compressed_size, buff);
    /* Offset of local header record */
    buff = gmio_adv_encode_uint64_le(info->relative_offset_local_header, buff);
    /* Number of the disk on which this file starts */
    buff = gmio_adv_encode_uint32_le(info->disk_nb_start, buff);
    *error = GMIO_ERROR_OK;
    return GMIO_ZIP64_SIZE_EXTRAFIELD_EXTENDED_INFO;
#else
    *error = GMIO_ERROR_UNKNOWN; /* TODO: error code */
    return 0;
#endif
}

size_t gmio_zip_write_end_of_central_directory_record(
        struct gmio_stream *stream,
        const struct gmio_zip_end_of_central_directory_record *info,
        int* error)
{
    uint8_t fixed_data[GMIO_ZIP_SIZE_END_OF_CENTRAL_DIRECTORY_RECORD];
    uint8_t* buff = fixed_data;

    /* 4-bytes magic number 0x06054b50 */
    buff = gmio_adv_encode_uint32_le(0x06054b50, buff);

    /* 2-bytes number of this disk */
    const uint16_t disk_nb = info->use_zip64 ? UINT16_MAX : info->disk_nb;
    buff = gmio_adv_encode_uint16_le(disk_nb, buff);

    /* 2-bytes number of the disk with the start of the central directory */
    const uint16_t disk_nb_with_start_of_central_dir =
            info->use_zip64 ? UINT16_MAX : info->disk_nb_with_start_of_central_dir;
    buff = gmio_adv_encode_uint16_le(disk_nb_with_start_of_central_dir, buff);

    /* 2-bytes total number of entries in the central directory on this disk */
    const uint16_t total_entry_count_in_central_dir_on_disk =
            info->use_zip64 ? UINT16_MAX : info->total_entry_count_in_central_dir_on_disk;
    buff = gmio_adv_encode_uint16_le(total_entry_count_in_central_dir_on_disk, buff);

    /* 2-bytes total number of entries in the central directory */
    const uint16_t total_entry_count_in_central_dir =
            info->use_zip64 ? UINT16_MAX : info->total_entry_count_in_central_dir;
    buff = gmio_adv_encode_uint16_le(total_entry_count_in_central_dir, buff);

    /* 4-bytes size of the central directory */
    const uint32_t central_dir_size =
            info->use_zip64 ? UINT32_MAX : info->central_dir_size;
    buff = gmio_adv_encode_uint32_le(central_dir_size, buff);

    /* 4-bytes offset of start of central directory with respect to the starting
     * disk number */
    const uint32_t start_offset_central_dir_from_disk_start_nb =
            info->use_zip64 ? UINT32_MAX : info->start_offset_central_dir_from_disk_start_nb;
    buff = gmio_adv_encode_uint32_le(start_offset_central_dir_from_disk_start_nb, buff);

    /* 2-bytes .ZIP file comment length */
    buff = gmio_adv_encode_uint16_le(info->filecomment_len, buff);

    /* Write to stream */
    const size_t expected_written_len =
            sizeof(fixed_data) + info->filecomment_len;
    size_t written_len = 0;
    written_len +=
            gmio_stream_write_bytes(stream, fixed_data, sizeof(fixed_data));
    written_len +=
            gmio_stream_write_bytes(stream, info->filecomment, info->filecomment_len);
    return gmio_zip_write_returnhelper(
                stream, written_len, expected_written_len, error);
}
