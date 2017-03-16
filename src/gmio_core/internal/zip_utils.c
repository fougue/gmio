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

#include "zip_utils.h"

#include "../error.h"
#include "byte_codec.h"
#include "helper_stream.h"
#include <stddef.h>

/* ----------
 * Constants
 * ---------- */

static const uint16_t zip64_extrablock_tag = 0x0001;
static const uint16_t zip64_extrablock_size = 3*8 + 4;

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

/* Helper to facilitate return from gmio_zip_write_xxx() API functions */
static size_t gmio_zip_write_returnhelper(
        struct gmio_stream* stream,
        size_t written,
        size_t expected,
        int* ptr_error)
{
    if (ptr_error != NULL) {
        const bool no_error = written == expected && !gmio_stream_error(stream);
        *ptr_error = no_error ? GMIO_ERROR_OK : GMIO_ERROR_STREAM;
    }
    return written;
}

/* Helper to facilitate return from gmio_zip_[read,write]_xxx() API functions */
static size_t gmio_zip_io_returnerr(size_t io_len, int error, int* ptr_error)
{
    if (ptr_error != NULL)
        *ptr_error = error;
    return io_len;
}

/* Helper to read and check a 32b uint from buffer */
static bool gmio_zip_readcheckmagic(
        const uint8_t** ptr_buff, uint32_t expected_magic, int* ptr_error)
{
    if (gmio_adv_decode_uint32_le(ptr_buff) != expected_magic) {
        if (ptr_error != NULL)
            *ptr_error = GMIO_ZIP_UTILS_ERROR_BAD_MAGIC;
        return false;
    }
    return true;
}

/* Helper to read and check some bytes from buffer */
static bool gmio_zip_readcheckbytes(
        struct gmio_stream* stream,
        uint8_t* buff,
        size_t expected_len,
        size_t* ptr_read_len,
        int* ptr_error)
{
    *ptr_read_len = gmio_stream_read_bytes(stream, buff, expected_len);
    const int error =
            (*ptr_read_len == expected_len && !gmio_stream_error(stream)) ?
                GMIO_ERROR_OK :
                GMIO_ERROR_STREAM;
    if (ptr_error != NULL)
        *ptr_error = error;
    return gmio_no_error(error);
}

/* ----------
 * API functions
 * ---------- */

size_t gmio_zip_read_local_file_header(
        struct gmio_stream *stream,
        struct gmio_zip_local_file_header *info,
        int *ptr_error)
{
    uint8_t bytes[GMIO_ZIP_SIZE_LOCAL_FILE_HEADER];
    size_t read_len = 0;
    if (!gmio_zip_readcheckbytes(stream, bytes, sizeof(bytes), &read_len, ptr_error))
        return read_len;

    const uint8_t* buff = bytes;
    if (!gmio_zip_readcheckmagic(&buff, 0x04034b50, ptr_error))
        return read_len;
    info->version_needed_to_extract = gmio_adv_decode_uint16_le(&buff);
    info->general_purpose_flags = gmio_adv_decode_uint16_le(&buff);
    info->compress_method = gmio_adv_decode_uint16_le(&buff);
    /* 2-bytes last mod file time + 2-bytes last mod file date */
    /* TODO: convert DOS datetime to struct tm */
    const uint32_t dos_datetime = gmio_adv_decode_uint32_le(&buff);
    GMIO_UNUSED(dos_datetime);
    info->lastmod_datetime = NULL;
    info->crc32 = gmio_adv_decode_uint32_le(&buff);
    info->compressed_size = gmio_adv_decode_uint32_le(&buff);
    info->uncompressed_size = gmio_adv_decode_uint32_le(&buff);
    info->filename_len = gmio_adv_decode_uint16_le(&buff);
    info->extrafield_len = gmio_adv_decode_uint16_le(&buff);

    info->filename = NULL;
    info->extrafield = NULL;

    return gmio_zip_io_returnerr(read_len, GMIO_ERROR_OK, ptr_error);
}

size_t gmio_zip_write_data_descriptor(
        struct gmio_stream *stream,
        const struct gmio_zip_data_descriptor *info,
        int* ptr_error)
{
    const size_t bytes_len =
            info->use_zip64 ?
                GMIO_ZIP64_SIZE_DATA_DESCRIPTOR :
                GMIO_ZIP_SIZE_DATA_DESCRIPTOR;
    uint8_t bytes[GMIO_ZIP64_SIZE_DATA_DESCRIPTOR];
    uint8_t* buff = bytes;

    gmio_adv_encode_uint32_le(0x08074b50, &buff);
    gmio_adv_encode_uint32_le(info->crc32, &buff);
    /* Compressed size and uncompressed size (4 or 8 bytes) */
    if (info->use_zip64) {
#ifdef GMIO_HAVE_INT64_TYPE
        gmio_adv_encode_uint64_le(info->compressed_size, &buff);
        gmio_adv_encode_uint64_le(info->uncompressed_size, &buff);
#else
        return gmio_zip_io_returnerr(
                    0, GMIO_ERROR_ZIP_INT64_TYPE_REQUIRED, ptr_error);
#endif
    }
    else if (!gmio_zip64_required(
                 info->uncompressed_size, info->compressed_size))
    {
        gmio_adv_encode_uint32_le((uint32_t)info->compressed_size, &buff);
        gmio_adv_encode_uint32_le((uint32_t)info->uncompressed_size, &buff);
    }
    else {
        return gmio_zip_io_returnerr(
                    0, GMIO_ERROR_ZIP64_FORMAT_REQUIRED, ptr_error);
    }

    /* Write to stream */
    const size_t written_len =
            gmio_stream_write_bytes(stream, bytes, bytes_len);
    return gmio_zip_write_returnhelper(
                stream, written_len, bytes_len, ptr_error);
}

size_t gmio_zip_read_central_directory_header(
        struct gmio_stream *stream,
        struct gmio_zip_central_directory_header *info,
        int *ptr_error)
{
    uint8_t bytes[GMIO_ZIP_SIZE_CENTRAL_DIRECTORY_HEADER];
    const uint8_t* buff = bytes;
    size_t read_len = 0;
    if (!gmio_zip_readcheckbytes(stream, bytes, sizeof(bytes), &read_len, ptr_error))
        return read_len;
    if (!gmio_zip_readcheckmagic(&buff, 0x02014b50, ptr_error))
        return read_len;
    info->version_made_by = gmio_adv_decode_uint16_le(&buff);
    info->version_needed_to_extract = gmio_adv_decode_uint16_le(&buff);
    info->general_purpose_flags = gmio_adv_decode_uint16_le(&buff);
    info->compress_method = gmio_adv_decode_uint16_le(&buff);
    /* 2-bytes last mod file time + 2-bytes last mod file date */
    /* TODO: convert DOS datetime to struct tm */
    const uint32_t dos_datetime = gmio_adv_decode_uint32_le(&buff);
    GMIO_UNUSED(dos_datetime);
    info->lastmod_datetime = NULL;
    info->crc32 = gmio_adv_decode_uint32_le(&buff);
    info->compressed_size = gmio_adv_decode_uint32_le(&buff);
    info->uncompressed_size = gmio_adv_decode_uint32_le(&buff);
    info->filename_len = gmio_adv_decode_uint16_le(&buff);
    info->extrafield_len = gmio_adv_decode_uint16_le(&buff);
    info->filecomment_len = gmio_adv_decode_uint16_le(&buff);
    gmio_adv_decode_uint16_le(&buff); /* disk_nb_start */
    info->internal_file_attrs = gmio_adv_decode_uint16_le(&buff);
    info->external_file_attrs = gmio_adv_decode_uint32_le(&buff);
    info->local_header_offset = gmio_adv_decode_uint32_le(&buff);

    info->filename = NULL;
    info->extrafield = NULL;
    info->filecomment = NULL;

    return gmio_zip_io_returnerr(read_len, GMIO_ERROR_OK, ptr_error);
}

size_t gmio_zip64_read_extrafield(
        struct gmio_stream *stream,
        struct gmio_zip64_extrafield *info,
        int *ptr_error)
{
#ifdef GMIO_HAVE_INT64_TYPE
    uint8_t bytes[GMIO_ZIP64_SIZE_EXTRAFIELD];
    const uint8_t* buff = bytes;
    size_t read_len = 0;
    if (!gmio_zip_readcheckbytes(stream, bytes, sizeof(bytes), &read_len, ptr_error))
        return read_len;
    if (gmio_adv_decode_uint16_le(&buff) != zip64_extrablock_tag) {
        return gmio_zip_io_returnerr(
                    read_len, GMIO_ZIP_UTILS_ERROR_BAD_EXTRAFIELD_TAG, ptr_error);
    }
    if (gmio_adv_decode_uint16_le(&buff) != zip64_extrablock_size) {
        return gmio_zip_io_returnerr(
                    read_len, GMIO_ZIP_UTILS_ERROR_BAD_EXTRAFIELD_SIZE, ptr_error);
    }
    info->uncompressed_size = gmio_adv_decode_uint64_le(&buff);
    info->compressed_size = gmio_adv_decode_uint64_le(&buff);
    info->local_header_offset = gmio_adv_decode_uint64_le(&buff);
    gmio_adv_decode_uint32_le(&buff); /* Disk start number */
    return gmio_zip_io_returnerr(read_len, GMIO_ERROR_OK, ptr_error);
#else
    return gmio_zip_io_returnerr(
                0, GMIO_ERROR_ZIP_INT64_TYPE_REQUIRED, ptr_error);
#endif
}

size_t gmio_zip64_read_end_of_central_directory_record(
        struct gmio_stream *stream,
        struct gmio_zip64_end_of_central_directory_record *info,
        int *ptr_error)
{
#ifdef GMIO_HAVE_INT64_TYPE
    uint8_t bytes[GMIO_ZIP64_SIZE_END_OF_CENTRAL_DIRECTORY_RECORD];
    const uint8_t* buff = bytes;
    size_t read_len = 0;
    if (!gmio_zip_readcheckbytes(stream, bytes, sizeof(bytes), &read_len, ptr_error))
        return read_len;
    if (!gmio_zip_readcheckmagic(&buff, 0x06064b50, ptr_error))
        return read_len;
    /* Size of zip64 end of central directory record */
    gmio_adv_decode_uint64_le(&buff);
    info->version_made_by = gmio_adv_decode_uint16_le(&buff);
    info->version_needed_to_extract = gmio_adv_decode_uint16_le(&buff);
    gmio_adv_decode_uint32_le(&buff); /* disk_nb */
    gmio_adv_decode_uint32_le(&buff); /* disk_nb_with_start_of_central_dir */
    gmio_adv_decode_uint64_le(&buff); /* total_entry_count_in_central_dir_on_disk */
    info->entry_count = gmio_adv_decode_uint64_le(&buff);
    info->central_dir_size = gmio_adv_decode_uint64_le(&buff);
    info->central_dir_offset = gmio_adv_decode_uint64_le(&buff);
    return gmio_zip_io_returnerr(read_len, GMIO_ERROR_OK, ptr_error);
#else
    return gmio_zip_io_returnerr(
                0, GMIO_ERROR_ZIP_INT64_TYPE_REQUIRED, ptr_error);
#endif
}

size_t gmio_zip64_read_end_of_central_directory_locator(
        struct gmio_stream *stream,
        struct gmio_zip64_end_of_central_directory_locator *info,
        int *ptr_error)
{
#ifdef GMIO_HAVE_INT64_TYPE
    uint8_t bytes[GMIO_ZIP64_SIZE_END_OF_CENTRAL_DIRECTORY_LOCATOR];
    const uint8_t* buff = bytes;
    size_t read_len = 0;
    if (!gmio_zip_readcheckbytes(stream, bytes, sizeof(bytes), &read_len, ptr_error))
        return read_len;
    if (!gmio_zip_readcheckmagic(&buff, 0x07064b50, ptr_error))
        return read_len;
    /* Number of the disk with the start of the zip64 end of central directory */
    gmio_adv_decode_uint32_le(&buff);
    /* Relative offset of the zip64 end of central directory record */
    info->zip64_end_of_central_dir_offset = gmio_adv_decode_uint64_le(&buff);
    /* Total number of disks */
    gmio_adv_decode_uint32_le(&buff);
    return gmio_zip_io_returnerr(read_len, GMIO_ERROR_OK, ptr_error);
#else
    return gmio_zip_io_returnerr(
                0, GMIO_ERROR_ZIP_INT64_TYPE_REQUIRED, ptr_error);
#endif
}

size_t gmio_zip_read_end_of_central_directory_record(
        struct gmio_stream *stream,
        struct gmio_zip_end_of_central_directory_record *info,
        int *ptr_error)
{
    uint8_t bytes[GMIO_ZIP_SIZE_END_OF_CENTRAL_DIRECTORY_RECORD];
    const uint8_t* buff = bytes;
    size_t read_len = 0;
    if (!gmio_zip_readcheckbytes(stream, bytes, sizeof(bytes), &read_len, ptr_error))
        return read_len;
    if (!gmio_zip_readcheckmagic(&buff, 0x06054b50, ptr_error))
        return read_len;
    gmio_adv_decode_uint16_le(&buff); /* disk_nb */
    gmio_adv_decode_uint16_le(&buff); /* disk_nb_with_start_of_central_dir */
    gmio_adv_decode_uint16_le(&buff); /* total_entry_count_in_central_dir_on_disk */
    info->entry_count = gmio_adv_decode_uint16_le(&buff);
    info->central_dir_size = gmio_adv_decode_uint32_le(&buff);
    info->central_dir_offset = gmio_adv_decode_uint32_le(&buff);
    info->filecomment_len = gmio_adv_decode_uint16_le(&buff);
    info->filecomment = NULL;
    return gmio_zip_io_returnerr(read_len, GMIO_ERROR_OK, ptr_error);
}

size_t gmio_zip_write_local_file_header(
        struct gmio_stream* stream,
        const struct gmio_zip_local_file_header* info,
        int* ptr_error)
{
    uint8_t bytes[GMIO_ZIP_SIZE_LOCAL_FILE_HEADER];
    uint8_t* buff = bytes;

    const bool use_data_descriptor =
            info->general_purpose_flags
            & GMIO_ZIP_GENERAL_PURPOSE_FLAG_USE_DATA_DESCRIPTOR;

    gmio_adv_encode_uint32_le(0x04034b50, &buff);
    gmio_adv_encode_uint16_le(info->version_needed_to_extract, &buff);
    gmio_adv_encode_uint16_le(info->general_purpose_flags, &buff);
    gmio_adv_encode_uint16_le(info->compress_method, &buff);
    /* 2-bytes last mod file time + 2-bytes last mod file date */
    const struct tm* lastmod = gmio_nonnull_datetime(info->lastmod_datetime);
    gmio_adv_encode_uint32_le(gmio_to_msdos_datetime(lastmod), &buff);
    gmio_adv_encode_uint32_le(use_data_descriptor ? 0 : info->crc32, &buff);
    gmio_adv_encode_uint32_le(
                use_data_descriptor ? 0 : info->compressed_size, &buff);
    gmio_adv_encode_uint32_le(
                use_data_descriptor ? 0 : info->uncompressed_size, &buff);
    gmio_adv_encode_uint16_le(info->filename_len, &buff);
    gmio_adv_encode_uint16_le(info->extrafield_len, &buff);

    /* Write to stream */
    const size_t expected_written_len =
            sizeof(bytes) + info->filename_len + info->extrafield_len;
    size_t written_len = 0;
    written_len +=
            gmio_stream_write_bytes(stream, bytes, sizeof(bytes));
    written_len +=
            gmio_stream_write_bytes(stream, info->filename, info->filename_len);
    written_len +=
            gmio_stream_write_bytes(stream, info->extrafield, info->extrafield_len);
    return gmio_zip_write_returnhelper(
                stream, written_len, expected_written_len, ptr_error);
}

size_t gmio_zip_read_data_descriptor(
        struct gmio_stream *stream,
        struct gmio_zip_data_descriptor *info,
        int* ptr_error)
{
    uint8_t bytes[GMIO_ZIP_SIZE_DATA_DESCRIPTOR];
    const uint8_t* buff = bytes;
    size_t read_len = 0;
    if (!gmio_zip_readcheckbytes(stream, bytes, sizeof(bytes), &read_len, ptr_error))
        return read_len;
    if (!gmio_zip_readcheckmagic(&buff, 0x08074b50, ptr_error))
        return read_len;
    info->crc32 = gmio_adv_decode_uint32_le(&buff);
    info->compressed_size = gmio_adv_decode_uint32_le(&buff);
    info->uncompressed_size = gmio_adv_decode_uint32_le(&buff);
    return gmio_zip_io_returnerr(read_len, GMIO_ERROR_OK, ptr_error);
}

size_t gmio_zip64_read_data_descriptor(
        struct gmio_stream* stream,
        struct gmio_zip_data_descriptor* info,
        int* ptr_error)
{
#ifdef GMIO_HAVE_INT64_TYPE
    uint8_t bytes[GMIO_ZIP64_SIZE_DATA_DESCRIPTOR];
    const uint8_t* buff = bytes;
    size_t read_len = 0;
    if (!gmio_zip_readcheckbytes(stream, bytes, sizeof(bytes), &read_len, ptr_error))
        return read_len;
    if (!gmio_zip_readcheckmagic(&buff, 0x08074b50, ptr_error))
        return read_len;
    info->crc32 = gmio_adv_decode_uint32_le(&buff);
    info->compressed_size = gmio_adv_decode_uint64_le(&buff);
    info->uncompressed_size = gmio_adv_decode_uint64_le(&buff);
    return gmio_zip_io_returnerr(read_len, GMIO_ERROR_OK, ptr_error);
#else
    return gmio_zip_io_returnerr(
                0, GMIO_ERROR_ZIP_INT64_TYPE_REQUIRED, ptr_error);
#endif
}

size_t gmio_zip_write_central_directory_header(
        struct gmio_stream *stream,
        const struct gmio_zip_central_directory_header *info,
        int* ptr_error)
{
    uint8_t bytes[GMIO_ZIP_SIZE_CENTRAL_DIRECTORY_HEADER];
    uint8_t* buff = bytes;

    gmio_adv_encode_uint32_le(0x02014b50, &buff);
    gmio_adv_encode_uint16_le(info->version_made_by, &buff);
    gmio_adv_encode_uint16_le(info->version_needed_to_extract, &buff);
    gmio_adv_encode_uint16_le(info->general_purpose_flags, &buff);
    gmio_adv_encode_uint16_le(info->compress_method, &buff);
    /* 2-bytes last mod file time + 2-bytes last mod file date */
    const struct tm* lastmod = gmio_nonnull_datetime(info->lastmod_datetime);
    gmio_adv_encode_uint32_le(gmio_to_msdos_datetime(lastmod), &buff);
    gmio_adv_encode_uint32_le(info->crc32, &buff);
    const uint32_t compressed_size =
            info->use_zip64 ? UINT32_MAX : info->compressed_size;
    const uint32_t uncompressed_size =
            info->use_zip64 ? UINT32_MAX : info->uncompressed_size;
    gmio_adv_encode_uint32_le(compressed_size, &buff);
    gmio_adv_encode_uint32_le(uncompressed_size, &buff);
    gmio_adv_encode_uint16_le(info->filename_len, &buff);
    gmio_adv_encode_uint16_le(info->extrafield_len, &buff);
    gmio_adv_encode_uint16_le(info->filecomment_len, &buff);
    gmio_adv_encode_uint16_le(0, &buff); /* Disk number start */
    gmio_adv_encode_uint16_le(info->internal_file_attrs, &buff);
    gmio_adv_encode_uint32_le(info->external_file_attrs, &buff);
    const uint32_t local_header_offset =
            info->use_zip64 ? UINT32_MAX : info->local_header_offset;
    gmio_adv_encode_uint32_le(local_header_offset, &buff);

    /* Write to stream */
    const size_t expected_written_len =
            sizeof(bytes)
            + info->filename_len + info->extrafield_len + info->filecomment_len;
    size_t written_len = 0;
    written_len +=
            gmio_stream_write_bytes(stream, bytes, sizeof(bytes));
    written_len +=
            gmio_stream_write_bytes(stream, info->filename, info->filename_len);
    written_len +=
            gmio_stream_write_bytes(stream, info->extrafield, info->extrafield_len);
    written_len +=
            gmio_stream_write_bytes(stream, info->filecomment, info->filecomment_len);
    return gmio_zip_write_returnhelper(
                stream, written_len, expected_written_len, ptr_error);
}

size_t gmio_zip64_write_extrafield(
        uint8_t *buff,
        size_t buff_capacity,
        const struct gmio_zip64_extrafield *info,
        int* ptr_error)
{
    if (buff_capacity < GMIO_ZIP64_SIZE_EXTRAFIELD) {
        return gmio_zip_io_returnerr(
                    0, GMIO_ERROR_INVALID_MEMBLOCK_SIZE, ptr_error);
    }
#ifdef GMIO_HAVE_INT64_TYPE
    gmio_adv_encode_uint16_le(zip64_extrablock_tag, &buff);
    gmio_adv_encode_uint16_le(zip64_extrablock_size, &buff);
    gmio_adv_encode_uint64_le(info->uncompressed_size, &buff);
    gmio_adv_encode_uint64_le(info->compressed_size, &buff);
    gmio_adv_encode_uint64_le(info->local_header_offset, &buff);
    gmio_adv_encode_uint32_le(0, &buff); /* Disk start number */
    return gmio_zip_io_returnerr(
                GMIO_ZIP64_SIZE_EXTRAFIELD, GMIO_ERROR_OK, ptr_error);
#else
    return gmio_zip_io_returnerr(
                0, GMIO_ERROR_ZIP_INT64_TYPE_REQUIRED, ptr_error);
#endif
}

size_t gmio_zip64_write_end_of_central_directory_record(
        struct gmio_stream *stream,
        const struct gmio_zip64_end_of_central_directory_record *info,
        int *ptr_error)
{
#ifdef GMIO_HAVE_INT64_TYPE
    uint8_t bytes[GMIO_ZIP64_SIZE_END_OF_CENTRAL_DIRECTORY_RECORD];
    uint8_t* buff = bytes;
    const uint32_t disk_nb = 0;
    const uint32_t disk_nb_start = 0;
    const uint64_t disk_entry_count = info->entry_count;
    gmio_adv_encode_uint32_le(0x06064b50, &buff);
    gmio_adv_encode_uint64_le(sizeof(bytes) - 12, &buff);
    gmio_adv_encode_uint16_le(info->version_made_by, &buff);
    gmio_adv_encode_uint16_le(info->version_needed_to_extract, &buff);
    gmio_adv_encode_uint32_le(disk_nb, &buff);
    gmio_adv_encode_uint32_le(disk_nb_start, &buff);
    gmio_adv_encode_uint64_le(disk_entry_count, &buff);
    gmio_adv_encode_uint64_le(info->entry_count, &buff);
    gmio_adv_encode_uint64_le(info->central_dir_size, &buff);
    gmio_adv_encode_uint64_le(info->central_dir_offset, &buff);
    /* Note: don't write any PKWARE reserved "zip64 extensible data sector" */
    /* Write to stream */
    const size_t expected_written_len = sizeof(bytes);
    const size_t written_len =
            gmio_stream_write_bytes(stream, bytes, sizeof(bytes));
    return gmio_zip_write_returnhelper(
                stream, written_len, expected_written_len, ptr_error);
#else
    return gmio_zip_io_returnerr(
                0, GMIO_ERROR_ZIP_INT64_TYPE_REQUIRED, ptr_error);
#endif
}

size_t gmio_zip64_write_end_of_central_directory_locator(
        struct gmio_stream *stream,
        const struct gmio_zip64_end_of_central_directory_locator *info,
        int *ptr_error)
{
#ifdef GMIO_HAVE_INT64_TYPE
    uint8_t bytes[GMIO_ZIP64_SIZE_END_OF_CENTRAL_DIRECTORY_LOCATOR];
    uint8_t* buff = bytes;
    const uint32_t disk_nb = 0;
    const uint32_t disk_count = 1;
    gmio_adv_encode_uint32_le(0x07064b50, &buff);
    gmio_adv_encode_uint32_le(disk_nb, &buff);
    gmio_adv_encode_uint64_le(info->zip64_end_of_central_dir_offset, &buff);
    gmio_adv_encode_uint32_le(disk_count, &buff);
    /* Write to stream */
    const size_t expected_written_len = sizeof(bytes);
    const size_t written_len =
            gmio_stream_write_bytes(stream, bytes, sizeof(bytes));
    return gmio_zip_write_returnhelper(
                stream, written_len, expected_written_len, ptr_error);
#else
    return gmio_zip_io_returnerr(
                0, GMIO_ERROR_ZIP_INT64_TYPE_REQUIRED, ptr_error);
#endif
}

size_t gmio_zip_write_end_of_central_directory_record(
        struct gmio_stream* stream,
        const struct gmio_zip_end_of_central_directory_record* info,
        int* ptr_error)
{
    uint8_t bytes[GMIO_ZIP_SIZE_END_OF_CENTRAL_DIRECTORY_RECORD];
    uint8_t* buff = bytes;

    gmio_adv_encode_uint32_le(0x06054b50, &buff);

    const uint16_t disk_nb = info->use_zip64 ? UINT16_MAX : 0;
    gmio_adv_encode_uint16_le(disk_nb, &buff);

    const uint16_t disk_nb_start = info->use_zip64 ? UINT16_MAX : 0;
    gmio_adv_encode_uint16_le(disk_nb_start, &buff);

    const uint16_t disk_entry_count =
            info->use_zip64 ? UINT16_MAX : info->entry_count;
    gmio_adv_encode_uint16_le(disk_entry_count, &buff);

    const uint16_t entry_count =
            info->use_zip64 ? UINT16_MAX : info->entry_count;
    gmio_adv_encode_uint16_le(entry_count, &buff);

    const uint32_t central_dir_size =
            info->use_zip64 ? UINT32_MAX : info->central_dir_size;
    gmio_adv_encode_uint32_le(central_dir_size, &buff);

    const uint32_t central_dir_offset =
            info->use_zip64 ? UINT32_MAX : info->central_dir_offset;
    gmio_adv_encode_uint32_le(central_dir_offset, &buff);

    gmio_adv_encode_uint16_le(info->filecomment_len, &buff);

    /* Write to stream */
    const size_t expected_written_len =
            sizeof(bytes) + info->filecomment_len;
    size_t written_len = 0;
    written_len +=
            gmio_stream_write_bytes(stream, bytes, sizeof(bytes));
    written_len +=
            gmio_stream_write_bytes(stream, info->filecomment, info->filecomment_len);
    return gmio_zip_write_returnhelper(
                stream, written_len, expected_written_len, ptr_error);
}

bool gmio_zip64_required(
        uintmax_t uncompressed_size, uintmax_t compressed_size)
{
    return uncompressed_size > UINT32_MAX || compressed_size > UINT32_MAX;
}

bool gmio_zip_write_single_file(
        struct gmio_stream *stream,
        const struct gmio_zip_file_entry *file_entry,
        int *ptr_error)
{
    const bool use_zip64_format_extensions =
            file_entry->feature_version
            >= GMIO_ZIP_FEATURE_VERSION_FILE_ZIP64_FORMAT_EXTENSIONS;
    uint8_t extrafield[GMIO_ZIP64_SIZE_EXTRAFIELD];
    uintmax_t zip_write_pos = 0;

    /* Write local file header */
    struct gmio_zip_local_file_header lfh = {0};
    lfh.version_needed_to_extract = file_entry->feature_version;
    lfh.general_purpose_flags = GMIO_ZIP_GENERAL_PURPOSE_FLAG_USE_DATA_DESCRIPTOR;
    lfh.compress_method = file_entry->compress_method;
    lfh.filename = file_entry->filename;
    lfh.filename_len = file_entry->filename_len;
    if (use_zip64_format_extensions) {
        lfh.compressed_size = UINT32_MAX;
        lfh.uncompressed_size = UINT32_MAX;
        const struct gmio_zip64_extrafield zip64extra = {0};
        gmio_zip64_write_extrafield(
                    extrafield, sizeof(extrafield), &zip64extra, ptr_error);
        if (gmio_error(*ptr_error))
            return false;
        lfh.extrafield = extrafield;
        lfh.extrafield_len = sizeof(extrafield);
    }
    zip_write_pos += gmio_zip_write_local_file_header(stream, &lfh, ptr_error);
    if (gmio_error(*ptr_error))
        return false;

    /* Write file data */
    struct gmio_zip_data_descriptor dd = {0};
    *ptr_error =
            file_entry->func_write_file_data(
                file_entry->cookie_func_write_file_data, &dd);
    zip_write_pos += dd.compressed_size;
    if (gmio_error(*ptr_error))
        return false;

    /* Guess version needed */
    const bool needs_zip64 =
            use_zip64_format_extensions
            || gmio_zip64_required(dd.uncompressed_size, dd.compressed_size);
    const enum gmio_zip_feature_version version_needed =
            needs_zip64 && !use_zip64_format_extensions ?
                GMIO_ZIP_FEATURE_VERSION_FILE_ZIP64_FORMAT_EXTENSIONS :
                file_entry->feature_version;

    /* Write data descriptor */
    dd.use_zip64 = needs_zip64;
    zip_write_pos += gmio_zip_write_data_descriptor(stream, &dd, ptr_error);
    if (gmio_error(*ptr_error))
        return false;

    /* Write central directory header */
    const uintmax_t pos_central_dir = zip_write_pos;
    struct gmio_zip_central_directory_header cdh = {0};
    cdh.use_zip64 = needs_zip64;
    cdh.version_needed_to_extract = version_needed;
    cdh.general_purpose_flags = lfh.general_purpose_flags;
    cdh.compress_method = file_entry->compress_method;
    cdh.crc32 = dd.crc32;
    cdh.compressed_size = (uint32_t)dd.compressed_size;
    cdh.uncompressed_size = (uint32_t)dd.uncompressed_size;
    cdh.filename = file_entry->filename;
    cdh.filename_len = file_entry->filename_len;
    if (needs_zip64) {
        struct gmio_zip64_extrafield zip64extra = {0};
        zip64extra.compressed_size = dd.compressed_size;
        zip64extra.uncompressed_size = dd.uncompressed_size;
        gmio_zip64_write_extrafield(
                    extrafield, sizeof(extrafield), &zip64extra, ptr_error);
        if (gmio_error(*ptr_error))
            return false;
        cdh.extrafield = extrafield;
        cdh.extrafield_len = sizeof(extrafield);
    }
    const uintmax_t central_dir_pos = zip_write_pos;
    const size_t central_dir_size =
            gmio_zip_write_central_directory_header(stream, &cdh, ptr_error);
    zip_write_pos += central_dir_size;
    if (gmio_error(*ptr_error))
        return false;

    if (needs_zip64) {
        /* Write Zip64 end of central directory record */
        const uintmax_t pos_zip64_end_of_central_dir = zip_write_pos;
        struct gmio_zip64_end_of_central_directory_record eocdr64 = {0};
        eocdr64.version_needed_to_extract = version_needed;
        eocdr64.entry_count = 1;
        eocdr64.central_dir_size = central_dir_size;
        eocdr64.central_dir_offset = pos_central_dir;
        zip_write_pos +=
                gmio_zip64_write_end_of_central_directory_record(
                    stream, &eocdr64, ptr_error);
        if (gmio_error(*ptr_error))
            return false;

        /* Write Zip64 end of central directory locator */
        struct gmio_zip64_end_of_central_directory_locator eocdl64 = {0};
        eocdl64.zip64_end_of_central_dir_offset = pos_zip64_end_of_central_dir;
        zip_write_pos +=
                gmio_zip64_write_end_of_central_directory_locator(
                    stream, &eocdl64, ptr_error);
        if (gmio_error(*ptr_error))
            return false;
    }

    /* Write end of central directory record */
    struct gmio_zip_end_of_central_directory_record eocdr = {0};
    eocdr.use_zip64 = needs_zip64;
    eocdr.entry_count = 1;
    eocdr.central_dir_size = (uint32_t)central_dir_size;
    eocdr.central_dir_offset = (uint32_t)central_dir_pos;
    zip_write_pos +=
            gmio_zip_write_end_of_central_directory_record(
                stream, &eocdr, ptr_error);

    return gmio_no_error(*ptr_error);
}
