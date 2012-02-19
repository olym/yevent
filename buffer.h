/*
 * =====================================================================================
 *
 *       Filename:  buffer.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/19/2012 09:21:32 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#ifndef _BUFFER_H
#define _BUFFER_H


struct buffer
{
    char *buffer_;
    size_t buffer_size;
    size_t reader_index;
    size_t writer_index;
};

#define buffer_readable_bytes(buf) \
    ((buf->writer_index) - (buf->reader_index))

#define buffer_writeable_bytes(buf) \
    ((buf->buffer_size) - (buf->writer_index))

#define buffer_prependable_bytes(buf) \
    (buf->reader_index)

#define buffer_peek(buf) \
    ((&buf->buffer_[0]) + (buf->reader_index))

#define buffer_begin_write(buf) \
    ((&buf->buffer_[0]) + (buf->writer_index))

struct buffer *buffer_init();
void buffer_retrieve(struct buffer *buf, size_t len);
void buffer_retrieve_until(struct buffer *buf, const char *end);
void buffer_retrieve_all(struct buffer *buf);
char * buffer_retrieve_as_string(struct buffer *buf);
const char * findCRLF(struct buffer *buf, const char* start);
void buffer_append(struct buffer *buf, const void *data, size_t len);
void buffer_prepend(struct buffer *buf, const void *data, size_t len);
ssize_t buffer_read(int fd, struct buffer *buf);
#endif
