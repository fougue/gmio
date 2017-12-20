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

#if 0

#include "../error.h"
#include "byte_codec.h"

namespace gmio {

namespace {

// ----------
// Constants
// ----------

const uint16_t zip64_extrablock_tag = 0x0001;
const uint16_t zip64_extrablock_size = 3*8 + 4;

// ----------
// Internal functions
// ----------

// See http://www.vsft.com/hal/dostime.htm
uint32_t to_msdos_datetime(const std::tm* datetime)
{
    const uint32_t msdos_datetime =
            // Time
            (datetime->tm_sec)                  // bit 0-4
            | (datetime->tm_min << 5)           // bit 5-10
            | (datetime->tm_hour << 11)         // bit 11-15
            // Date
            | (datetime->tm_mday << 16)         // bit 16-20
            | ((datetime->tm_mon + 1) << 21)    // bit 21-24
            | ((datetime->tm_year - 80) << 25); // bit 21-24
    return msdos_datetime;
}

// Returns a non-null datetime, if arg is nullptr it returns current gmtime()
const std::tm* nonnull_datetime(const std::tm* datetime)
{
    if (datetime == nullptr) {
        const std::time_t current_time = std::time(nullptr);
        return std::gmtime(&current_time); // UTC time
    }
    return datetime;
}

// Helper to facilitate return from ZIP_write_xxx() API functions
size_t ZIP_write_returnhelper(
        size_t written, size_t expected, int* ptr_error)
{
    if (ptr_error != nullptr) {
        const bool no_error = written == expected;
        *ptr_error = no_error ? Error_OK : Error_Stream;
    }
    return written;
}

// Helper to facilitate return from ZIP_[read,write]_xxx() API functions
size_t ZIP_io_returnerr(size_t io_len, int error, int* ptr_error)
{
    if (ptr_error != nullptr)
        *ptr_error = error;
    return io_len;
}

// Helper to read and check a 32b uint from buffer
bool ZIP_readcheckmagic(
        const uint8_t** ptr_buff, uint32_t expected_magic, int* ptr_error)
{
    if (adv_decode_uint32_le(ptr_buff) != expected_magic) {
        if (ptr_error != nullptr)
            *ptr_error = ZIP_Error_BadMagic;
        return false;
    }
    return true;
}

// Helper to read and check some bytes from buffer
bool ZIP_readcheckbytes(
        const FuncReadData& funcRead,
        uint8_t* buff,
        size_t expected_len,
        size_t* ptr_read_len,
        int* ptr_error)
{
    ReadState readstate;
    *ptr_read_len = funcRead(buff, expected_len, &readstate);
    const int error =
            (*ptr_read_len == expected_len && readstate != ReadState::Error) ?
                Error_OK :
                Error_Stream;
    if (ptr_error != nullptr)
        *ptr_error = error;
    return error == Error_OK;
}

} // namespace

// ----------
// API functions
// ----------

size_t ZIP_readLocalFileHeader(
        const FuncReadData& funcRead,
        ZIP_LocalFileHeader* info,
        int* ptr_error)
{
    uint8_t bytes[ZIP_SIZE_LOCAL_FILE_HEADER];
    size_t read_len = 0;
    if (!ZIP_readcheckbytes(funcRead, bytes, sizeof(bytes), &read_len, ptr_error))
        return read_len;

    const uint8_t* buff = bytes;
    if (!ZIP_readcheckmagic(&buff, 0x04034b50, ptr_error))
        return read_len;
    info->version_needed_to_extract = adv_decode_uint16_le(&buff);
    info->general_purpose_flags = adv_decode_uint16_le(&buff);
    info->compress_method = adv_decode_uint16_le(&buff);
    // 2-bytes last mod file time + 2-bytes last mod file date
    // TODO: convert DOS datetime to struct tm
    const uint32_t dos_datetime = adv_decode_uint32_le(&buff);
    GMIO_UNUSED(dos_datetime);
    info->lastmod_datetime = nullptr;
    info->crc32 = adv_decode_uint32_le(&buff);
    info->compressed_size = adv_decode_uint32_le(&buff);
    info->uncompressed_size = adv_decode_uint32_le(&buff);
    const uint16_t filename_len = adv_decode_uint16_le(&buff);
    const uint16_t extrafield_len = adv_decode_uint16_le(&buff);

    info->filename.resize(filename_len);
    func_read(&info->filename[0], filename_len);
    info->extrafield.resize(extrafield_len);
    func_read(&info->extrafield.data(), extrafield_len);

    return ZIP_io_returnerr(read_len, Error_OK, ptr_error);
}

size_t ZIP_writeDataDescriptor(
        const FuncWriteData& func_write,
        const ZIP_DataDescriptor& info,
        int* ptr_error)
{
    const size_t bytes_len =
            info.use_zip64 ?
                ZIP64_SIZE_DATA_DESCRIPTOR :
                ZIP_SIZE_DATA_DESCRIPTOR;
    uint8_t bytes[ZIP64_SIZE_DATA_DESCRIPTOR];
    uint8_t* buff = bytes;

    adv_encode_uint32_le(0x08074b50, &buff);
    adv_encode_uint32_le(info.crc32, &buff);
    // Compressed size and uncompressed size (4 or 8 bytes)
    if (info.use_zip64) {
        adv_encode_uint64_le(info.compressed_size, &buff);
        adv_encode_uint64_le(info.uncompressed_size, &buff);
    }
    else if (!ZIP64_required(info.uncompressed_size, info.compressed_size)) {
        adv_encode_uint32_le(info.compressed_size, &buff);
        adv_encode_uint32_le(info.uncompressed_size, &buff);
    }
    else {
        return ZIP_io_returnerr(0, Error_Zip64_FormatRequired, ptr_error);
    }

    // Write to device
    const size_t written_len = func_write(bytes, bytes_len);
    return ZIP_write_returnhelper(written_len, bytes_len, ptr_error);
}

size_t ZIP_readCentralDirectoryHeader(
        const FuncReadData& func_read,
        ZIP_CentralDirectoryHeader *info,
        int *ptr_error)
{
    uint8_t bytes[ZIP_SIZE_CENTRAL_DIRECTORY_HEADER];
    const uint8_t* buff = bytes;
    size_t read_len = 0;
    if (!ZIP_readcheckbytes(func_read, bytes, sizeof(bytes), &read_len, ptr_error))
        return read_len;
    if (!ZIP_readcheckmagic(&buff, 0x02014b50, ptr_error))
        return read_len;
    info->version_made_by = adv_decode_uint16_le(&buff);
    info->version_needed_to_extract = adv_decode_uint16_le(&buff);
    info->general_purpose_flags = adv_decode_uint16_le(&buff);
    info->compress_method = adv_decode_uint16_le(&buff);
    // 2-bytes last mod file time + 2-bytes last mod file date
    // TODO: convert DOS datetime to struct tm
    const uint32_t dos_datetime = adv_decode_uint32_le(&buff);
    GMIO_UNUSED(dos_datetime);
    info->lastmod_datetime = nullptr;
    info->crc32 = adv_decode_uint32_le(&buff);
    info->compressed_size = adv_decode_uint32_le(&buff);
    info->uncompressed_size = adv_decode_uint32_le(&buff);
    const uint16_t filename_len = adv_decode_uint16_le(&buff);
    const uint16_t extrafield_len = adv_decode_uint16_le(&buff);
    const uint16_t filecomment_len = adv_decode_uint16_le(&buff);
    adv_decode_uint16_le(&buff); // disk_nb_start
    info->internal_file_attrs = adv_decode_uint16_le(&buff);
    info->external_file_attrs = adv_decode_uint32_le(&buff);
    info->local_header_offset = adv_decode_uint32_le(&buff);

    info->filename.resize(filename_len);
    func_read(&info->filename[0], filename_len);
    info->extrafield.resize(extrafield_len);
    func_read(&info->extrafield.data(), extrafield_len);
    info->filecomment.resize(filecomment_len);
    func_read(&info->filecomment[0], filecomment_len);

    return ZIP_io_returnerr(read_len, Error_OK, ptr_error);
}

size_t ZIP64_readExtraField(
        const FuncReadData& func_read,
        ZIP64_ExtraField *info,
        int *ptr_error)
{
    uint8_t bytes[ZIP64_SIZE_EXTRAFIELD];
    const uint8_t* buff = bytes;
    size_t read_len = 0;
    if (!ZIP_readcheckbytes(func_read, bytes, sizeof(bytes), &read_len, ptr_error))
        return read_len;
    if (adv_decode_uint16_le(&buff) != zip64_extrablock_tag) {
        return ZIP_io_returnerr(
                    read_len, ZIP_Error_BadExtraFieldTag, ptr_error);
    }
    if (adv_decode_uint16_le(&buff) != zip64_extrablock_size) {
        return ZIP_io_returnerr(
                    read_len, ZIP_Error_BadExtraFieldSize, ptr_error);
    }
    info->uncompressed_size = adv_decode_uint64_le(&buff);
    info->compressed_size = adv_decode_uint64_le(&buff);
    info->local_header_offset = adv_decode_uint64_le(&buff);
    adv_decode_uint32_le(&buff); // Disk start number
    return ZIP_io_returnerr(read_len, Error_OK, ptr_error);
}

size_t ZIP64_readEndOfCentralDirectoryRecord(
        const FuncReadData& func_read,
        ZIP64_EndOfCentralDirectoryRecord *info,
        int *ptr_error)
{
    uint8_t bytes[ZIP64_SIZE_END_OF_CENTRAL_DIRECTORY_RECORD];
    const uint8_t* buff = bytes;
    size_t read_len = 0;
    if (!ZIP_readcheckbytes(func_read, bytes, sizeof(bytes), &read_len, ptr_error))
        return read_len;
    if (!ZIP_readcheckmagic(&buff, 0x06064b50, ptr_error))
        return read_len;
    // Size of zip64 end of central directory record
    adv_decode_uint64_le(&buff);
    info->version_made_by = adv_decode_uint16_le(&buff);
    info->version_needed_to_extract = adv_decode_uint16_le(&buff);
    adv_decode_uint32_le(&buff); // disk_nb
    adv_decode_uint32_le(&buff); // disk_nb_with_start_of_central_dir
    adv_decode_uint64_le(&buff); // total_entry_count_in_central_dir_on_disk
    info->entry_count = adv_decode_uint64_le(&buff);
    info->central_dir_size = adv_decode_uint64_le(&buff);
    info->central_dir_offset = adv_decode_uint64_le(&buff);
    return ZIP_io_returnerr(read_len, Error_OK, ptr_error);
}

size_t ZIP64_readEndOfCentralDirectoryLocator(
        const FuncReadData& func_read,
        ZIP64_EndOfCentralDirectoryLocator* info,
        int* ptr_error)
{
    uint8_t bytes[ZIP64_SIZE_END_OF_CENTRAL_DIRECTORY_LOCATOR];
    const uint8_t* buff = bytes;
    size_t read_len = 0;
    if (!ZIP_readcheckbytes(func_read, bytes, sizeof(bytes), &read_len, ptr_error))
        return read_len;
    if (!ZIP_readcheckmagic(&buff, 0x07064b50, ptr_error))
        return read_len;
    // Number of the disk with the start of the zip64 end of central directory
    adv_decode_uint32_le(&buff);
    // Relative offset of the zip64 end of central directory record
    info->end_of_central_dir_offset = adv_decode_uint64_le(&buff);
    // Total number of disks
    adv_decode_uint32_le(&buff);
    return ZIP_io_returnerr(read_len, Error_OK, ptr_error);
}

size_t ZIP_readEndOfCentralDirectoryRecord(
        const FuncReadData& func_read,
        ZIP_EndOfCentralDirectoryRecord *info,
        int *ptr_error)
{
    uint8_t bytes[ZIP_SIZE_END_OF_CENTRAL_DIRECTORY_RECORD];
    const uint8_t* buff = bytes;
    size_t read_len = 0;
    if (!ZIP_readcheckbytes(func_read, bytes, sizeof(bytes), &read_len, ptr_error))
        return read_len;
    if (!ZIP_readcheckmagic(&buff, 0x06054b50, ptr_error))
        return read_len;
    adv_decode_uint16_le(&buff); // disk_nb
    adv_decode_uint16_le(&buff); // disk_nb_with_start_of_central_dir
    adv_decode_uint16_le(&buff); // total_entry_count_in_central_dir_on_disk
    info->entry_count = adv_decode_uint16_le(&buff);
    info->central_dir_size = adv_decode_uint32_le(&buff);
    info->central_dir_offset = adv_decode_uint32_le(&buff);
    info->filecomment_len = adv_decode_uint16_le(&buff);
    info->filecomment = nullptr;
    return ZIP_io_returnerr(read_len, Error_OK, ptr_error);
}

size_t ZIP_writeLocalFileHeader(
        const FuncWriteData& func_write,
        const ZIP_LocalFileHeader& info,
        int* ptr_error)
{
    uint8_t bytes[ZIP_SIZE_LOCAL_FILE_HEADER];
    uint8_t* buff = bytes;

    const bool use_data_descriptor =
            info.general_purpose_flags
            & ZIP_GENERAL_PURPOSE_FLAG_USE_DATA_DESCRIPTOR;

    adv_encode_uint32_le(0x04034b50, &buff);
    adv_encode_uint16_le(info.version_needed_to_extract, &buff);
    adv_encode_uint16_le(info.general_purpose_flags, &buff);
    adv_encode_uint16_le(info.compress_method, &buff);
    // 2-bytes last mod file time + 2-bytes last mod file date
    const struct tm* lastmod = nonnull_datetime(info.lastmod_datetime);
    adv_encode_uint32_le(to_msdos_datetime(lastmod), &buff);
    adv_encode_uint32_le(use_data_descriptor ? 0 : info.crc32, &buff);
    adv_encode_uint32_le(use_data_descriptor ? 0 : info.compressed_size, &buff);
    adv_encode_uint32_le(use_data_descriptor ? 0 : info.uncompressed_size, &buff);
    adv_encode_uint16_le(info.filename_len, &buff);
    adv_encode_uint16_le(info.extrafield_len, &buff);

    // Write to device
    const size_t expected_written_len =
            sizeof(bytes) + info.filename.size() + info.extrafield.size();
    size_t written_len = 0;
    written_len += func_write(bytes, sizeof(bytes));
    written_len += func_write(info.filename.data(), info.filename.size());
    written_len += func_write(info.extrafield.data(), info.extrafield.size());
    return ZIP_write_returnhelper(written_len, expected_written_len, ptr_error);
}

size_t ZIP_readDataDescriptor(
        const FuncReadData& func_read,
        ZIP_DataDescriptor *info,
        int* ptr_error)
{
    uint8_t bytes[ZIP_SIZE_DATA_DESCRIPTOR];
    const uint8_t* buff = bytes;
    size_t read_len = 0;
    if (!ZIP_readcheckbytes(func_read, bytes, sizeof(bytes), &read_len, ptr_error))
        return read_len;
    if (!ZIP_readcheckmagic(&buff, 0x08074b50, ptr_error))
        return read_len;
    info->crc32 = adv_decode_uint32_le(&buff);
    info->compressed_size = adv_decode_uint32_le(&buff);
    info->uncompressed_size = adv_decode_uint32_le(&buff);
    return ZIP_io_returnerr(read_len, Error_OK, ptr_error);
}

size_t ZIP64_readDataDescriptor(
        const FuncReadData& func_read,
        ZIP_DataDescriptor *info,
        int* ptr_error)
{
    uint8_t bytes[ZIP64_SIZE_DATA_DESCRIPTOR];
    const uint8_t* buff = bytes;
    size_t read_len = 0;
    if (!ZIP_readcheckbytes(func_read, bytes, sizeof(bytes), &read_len, ptr_error))
        return read_len;
    if (!ZIP_readcheckmagic(&buff, 0x08074b50, ptr_error))
        return read_len;
    info->crc32 = adv_decode_uint32_le(&buff);
    info->compressed_size = adv_decode_uint64_le(&buff);
    info->uncompressed_size = adv_decode_uint64_le(&buff);
    return ZIP_io_returnerr(read_len, Error_OK, ptr_error);
}

size_t ZIP_writeCentralDirectoryHeader(
        const FuncWriteData& func_write,
        const ZIP_CentralDirectoryHeader& info,
        int* ptr_error)
{
    uint8_t bytes[ZIP_SIZE_CENTRAL_DIRECTORY_HEADER];
    uint8_t* buff = bytes;

    adv_encode_uint32_le(0x02014b50, &buff);
    adv_encode_uint16_le(info.version_made_by, &buff);
    adv_encode_uint16_le(info.version_needed_to_extract, &buff);
    adv_encode_uint16_le(info.general_purpose_flags, &buff);
    adv_encode_uint16_le(info.compress_method, &buff);
    // 2-bytes last mod file time + 2-bytes last mod file date
    const struct tm* lastmod = gmio_nonnull_datetime(info.lastmod_datetime);
    adv_encode_uint32_le(gmio_to_msdos_datetime(lastmod), &buff);
    adv_encode_uint32_le(info.crc32, &buff);
    const uint32_t compressed_size =
            info.use_zip64 ? UINT32_MAX : info.compressed_size;
    const uint32_t uncompressed_size =
            info.use_zip64 ? UINT32_MAX : info.uncompressed_size;
    adv_encode_uint32_le(compressed_size, &buff);
    adv_encode_uint32_le(uncompressed_size, &buff);
    adv_encode_uint16_le(info.filename.size(), &buff);
    adv_encode_uint16_le(info.extrafield.size(), &buff);
    adv_encode_uint16_le(info.filecomment.size(), &buff);
    adv_encode_uint16_le(0, &buff); // Disk number start
    adv_encode_uint16_le(info.internal_file_attrs, &buff);
    adv_encode_uint32_le(info.external_file_attrs, &buff);
    const uint32_t local_header_offset =
            info.use_zip64 ? UINT32_MAX : info.local_header_offset;
    adv_encode_uint32_le(local_header_offset, &buff);

    // Write to device
    const size_t expected_written_len =
            sizeof(bytes)
            + info.filename.size() + info.extrafield.size() + info.filecomment.size();
    size_t written_len = 0;
    written_len += func_write(bytes, sizeof(bytes));
    written_len += func_write(info.filename.data(), info.filename.size());
    written_len += func_write(info.extrafield.data(), info.extrafield.size());
    written_len += func_write(info.filecomment.data(), info.filecomment.size());
    return ZIP_write_returnhelper(written_len, expected_written_len, ptr_error);
}

size_t ZIP64_writeExtraField(
        Span<uint8_t> buff,
        const ZIP64_ExtraField& info,
        int* ptr_error)
{
    if (buff.size() < ZIP64_SIZE_EXTRAFIELD)
        return ZIP_io_returnerr(0, Error_BufferOverflow, ptr_error);
    uint8_t* buffpos = buff.data();
    adv_encode_uint16_le(zip64_extrablock_tag, &buffpos);
    adv_encode_uint16_le(zip64_extrablock_size, &buffpos);
    adv_encode_uint64_le(info.uncompressed_size, &buffpos);
    adv_encode_uint64_le(info.compressed_size, &buffpos);
    adv_encode_uint64_le(info.local_header_offset, &buffpos);
    adv_encode_uint32_le(0, &buffpos); // Disk start number
    return ZIP_io_returnerr(ZIP64_SIZE_EXTRAFIELD, Error_OK, ptr_error);
}

size_t ZIP64_writeEndOfCentralDirectoryRecord(
        const FuncWriteData& func_write,
        const ZIP64_EndOfCentralDirectoryRecord& info,
        int *ptr_error)
{
    uint8_t bytes[ZIP64_SIZE_END_OF_CENTRAL_DIRECTORY_RECORD];
    uint8_t* buff = bytes;
    const uint32_t disk_nb = 0;
    const uint32_t disk_nb_start = 0;
    const uint64_t disk_entry_count = info.entry_count;
    adv_encode_uint32_le(0x06064b50, &buff);
    adv_encode_uint64_le(sizeof(bytes) - 12, &buff);
    adv_encode_uint16_le(info.version_made_by, &buff);
    adv_encode_uint16_le(info.version_needed_to_extract, &buff);
    adv_encode_uint32_le(disk_nb, &buff);
    adv_encode_uint32_le(disk_nb_start, &buff);
    adv_encode_uint64_le(disk_entry_count, &buff);
    adv_encode_uint64_le(info.entry_count, &buff);
    adv_encode_uint64_le(info.central_dir_size, &buff);
    adv_encode_uint64_le(info.central_dir_offset, &buff);
    // Note: don't write any PKWARE reserved "zip64 extensible data sector"
    // Write to stream
    const size_t expected_written_len = sizeof(bytes);
    const size_t written_len = func_write(bytes, sizeof(bytes));
    return ZIP_write_returnhelper(written_len, expected_written_len, ptr_error);
}

size_t ZIP64_writeEndOfCentralDirectoryLocator(
        const FuncWriteData& func_write,
        const ZIP64_EndOfCentralDirectoryLocator& info,
        int *ptr_error)
{
    uint8_t bytes[ZIP64_SIZE_END_OF_CENTRAL_DIRECTORY_LOCATOR];
    uint8_t* buff = bytes;
    const uint32_t disk_nb = 0;
    const uint32_t disk_count = 1;
    adv_encode_uint32_le(0x07064b50, &buff);
    adv_encode_uint32_le(disk_nb, &buff);
    adv_encode_uint64_le(info.zip64_end_of_central_dir_offset, &buff);
    adv_encode_uint32_le(disk_count, &buff);
    // Write to stream
    const size_t expected_written_len = sizeof(bytes);
    const size_t written_len = func_write(bytes, sizeof(bytes));
    return ZIP_write_returnhelper(written_len, expected_written_len, ptr_error);
}

size_t ZIP_writeEndOfCentralDirectoryRecord(
        const FuncWriteData& func_write,
        const ZIP_EndOfCentralDirectoryRecord& info,
        int* ptr_error)
{
    uint8_t bytes[ZIP_SIZE_END_OF_CENTRAL_DIRECTORY_RECORD];
    uint8_t* buff = bytes;

    adv_encode_uint32_le(0x06054b50, &buff);

    const uint16_t disk_nb = info.use_zip64 ? UINT16_MAX : 0;
    adv_encode_uint16_le(disk_nb, &buff);

    const uint16_t disk_nb_start = info.use_zip64 ? UINT16_MAX : 0;
    adv_encode_uint16_le(disk_nb_start, &buff);

    const uint16_t disk_entry_count =
            info.use_zip64 ? UINT16_MAX : info.entry_count;
    adv_encode_uint16_le(disk_entry_count, &buff);

    const uint16_t entry_count =
            info.use_zip64 ? UINT16_MAX : info.entry_count;
    adv_encode_uint16_le(entry_count, &buff);

    const uint32_t central_dir_size =
            info.use_zip64 ? UINT32_MAX : info.central_dir_size;
    adv_encode_uint32_le(central_dir_size, &buff);

    const uint32_t central_dir_offset =
            info.use_zip64 ? UINT32_MAX : info.central_dir_offset;
    adv_encode_uint32_le(central_dir_offset, &buff);

    adv_encode_uint16_le(info.filecomment_len, &buff);

    // Write to device
    const size_t expected_written_len = sizeof(bytes) + info.filecomment_len;
    size_t written_len = 0;
    written_len += func_write(bytes, sizeof(bytes));
    written_len += func_write(info.filecomment, info.filecomment_len);
    return ZIP_write_returnhelper(written_len, expected_written_len, ptr_error);
}

bool ZIP64_required(
        uint64_t uncompressed_size, uint64_t compressed_size)
{
    return uncompressed_size > UINT32_MAX || compressed_size > UINT32_MAX;
}

bool ZIP_writeSingleFile(
        const FuncWriteData& func_write,
        const ZIP_FileEntry& file_entry,
        int *ptr_error)
{
    const bool use_zip64_format_extensions =
            file_entry.feature_version
            >= ZIP_FEATURE_VERSION_FILE_ZIP64_FORMAT_EXTENSIONS;
    uint8_t extrafield[ZIP64_SIZE_EXTRAFIELD];
    uint64_t zip_write_pos = 0;

    // Write local file header
    struct ZIP_LocalFileHeader lfh = {};
    lfh.version_needed_to_extract = file_entry.feature_version;
    lfh.general_purpose_flags = ZIP_GENERAL_PURPOSE_FLAG_USE_DATA_DESCRIPTOR;
    lfh.compress_method = file_entry.compress_method;
    lfh.filename = file_entry.filename;
    if (use_zip64_format_extensions) {
        lfh.compressed_size = UINT32_MAX;
        lfh.uncompressed_size = UINT32_MAX;
        const struct ZIP64_extrafield zip64extra = {};
        ZIP64_writeExtraField(makeSpan(extrafield), &zip64extra, ptr_error);
        if (*ptr_error != Error_OK)
            return false;
        lfh.extrafield = extrafield;
        lfh.extrafield_len = sizeof(extrafield);
    }
    zip_write_pos += ZIP_writeLocalFileHeader(func_write, &lfh, ptr_error);
    if (*ptr_error != Error_OK)
        return false;

    // Write file data
    ZIP_DataDescriptor dd = {};
    *ptr_error =
            file_entry.func_write_file_data(
                file_entry.cookie_func_write_file_data, &dd);
    zip_write_pos += dd.compressed_size;
    if (*ptr_error != Error_OK)
        return false;

    // Guess version needed
    const bool needs_zip64 =
            use_zip64_format_extensions
            || ZIP64_required(dd.uncompressed_size, dd.compressed_size);
    const enum ZIP_FeatureVersion version_needed =
            needs_zip64 && !use_zip64_format_extensions ?
                ZIP_FEATURE_VERSION_FILE_ZIP64_FORMAT_EXTENSIONS :
                file_entry.feature_version;

    // Write data descriptor
    dd.use_zip64 = needs_zip64;
    zip_write_pos += ZIP_writeDataDescriptor(func_write, &dd, ptr_error);
    if (*ptr_error != Error_OK)
        return false;

    // Write central directory header
    const uint64_t pos_central_dir = zip_write_pos;
    ZIP_CentralDirectoryHeader cdh = {};
    cdh.use_zip64 = needs_zip64;
    cdh.version_needed_to_extract = version_needed;
    cdh.general_purpose_flags = lfh.general_purpose_flags;
    cdh.compress_method = file_entry.compress_method;
    cdh.crc32 = dd.crc32;
    cdh.compressed_size = static_cast<uint32_t>(dd.compressed_size);
    cdh.uncompressed_size = static_cast<uint32_t>(dd.uncompressed_size);
    cdh.filename = file_entry.filename;
    cdh.filename_len = file_entry.filename_len;
    if (needs_zip64) {
        ZIP64_ExtraField zip64extra = {};
        zip64extra.compressed_size = dd.compressed_size;
        zip64extra.uncompressed_size = dd.uncompressed_size;
        ZIP64_writeExtraField(makeSpan(extrafield), &zip64extra, ptr_error);
        if (*ptr_error != Error_OK)
            return false;
        cdh.extrafield = extrafield;
        cdh.extrafield_len = sizeof(extrafield);
    }
    const uint64_t central_dir_pos = zip_write_pos;
    const size_t central_dir_size =
            ZIP_writeCentralDirectoryHeader(func_write, &cdh, ptr_error);
    zip_write_pos += central_dir_size;
    if (*ptr_error != Error_OK)
        return false;

    if (needs_zip64) {
        // Write Zip64 end of central directory record
        const uint64_t pos_zip64_end_of_central_dir = zip_write_pos;
        ZIP64_EndOfCentralDirectoryRecord eocdr64 = {};
        eocdr64.version_needed_to_extract = version_needed;
        eocdr64.entry_count = 1;
        eocdr64.central_dir_size = central_dir_size;
        eocdr64.central_dir_offset = pos_central_dir;
        zip_write_pos += ZIP64_writeEndOfCentralDirectoryRecord(func_write, &eocdr64, ptr_error);
        if (*ptr_error != Error_OK)
            return false;

        // Write Zip64 end of central directory locator
        ZIP64_EndOfCentralDirectoryLocator eocdl64 = {};
        eocdl64.end_of_central_dir_offset = pos_zip64_end_of_central_dir;
        zip_write_pos += ZIP64_writeEndOfCentralDirectoryLocator(func_write, &eocdl64, ptr_error);
        if (*ptr_error != Error_OK)
            return false;
    }

    // Write end of central directory record
    ZIP_EndOfCentralDirectoryRecord eocdr = {};
    eocdr.use_zip64 = needs_zip64;
    eocdr.entry_count = 1;
    eocdr.central_dir_size = static_cast<uint32_t>(central_dir_size);
    eocdr.central_dir_offset = static_cast<uint32_t>(central_dir_pos);
    zip_write_pos += ZIP_writeEndOfCentralDirectoryRecord(func_write, &eocdr, ptr_error);

    return *ptr_error == Error_OK;
}

} // namespace gmio

#endif
