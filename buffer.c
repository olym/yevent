/*
 * =====================================================================================
 *
 *       Filename:  buffer.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/18/2012 05:24:07 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yin Zhongxing (olym), zhongxing.yin@cs2c.com.cn
 *        Company:  www.CS2C.com.cn
 *
 * =====================================================================================
 */

/// A buffer class modeled after org.jboss.netty.buffer.ChannelBuffer
///
/// @code
/// +-------------------+------------------+------------------+
/// | prependable bytes |  readable bytes  |  writable bytes  |
/// |                   |     (CONTENT)    |                  |
/// +-------------------+------------------+------------------+
/// |                   |                  |                  |
/// 0      <=      readerIndex   <=   writerIndex    <=     size
/// @endcode
//

#define BUFFER_PREPEND 8
#define BUFFER_INITIALSIZE 1024

static void buffer_make_space(struct buffer *buf, size_t len);

struct buffer *buffer_init()
{
    struct buffer *buf;

    buf = malloc(sizeof(struct buffer));
    if (!buf) {
        fprintf(stderr, "%s: malloc error\n", __func__);
        return NULL;
    }

    buf->buffer_size = BUFFER_PREPEND + BUFFER_INITIALSIZE;
    buf->buffer_ = malloc(buf->buffer_size);
    if (!buf->buffer) {
        fprintf(stderr, "%s: malloc error\n", __func__);
        return NULL;
    }

    buf->reader_index = BUFFER_PREPEND;
    buf->writer_index = BUFFER_PREPEND;

    return buf;
}

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

void
buffer_retrieve(struct buffer *buf, size_t len)
{
    if (len <= buffer_readable_bytes(buf))
        buf->reader_index += len;
    else
        buf->reader_index += buffer_readable_bytes(buf);
}

void
buffer_retrieve_until(struct buffer *buf, const char *end)
{
    if (buffer_peek(buf) > end)
        buffer_retrieve(end - buffer_peek(buf));
    else
        buffer_retrieve(0);
}

void
buffer_retrieve_all(struct buffer *buf)
{
    buf->reader_index = BUFFER_PREPEND;
    buf->writer_index = BUFFER_PREPEND;
}

char *
buffer_retrieve_as_string(struct buffer *buf)
{
    char *str;
    size_t readable_bytes = buffer_readable_bytes(buf);
    str = malloc(readable_bytes+1);
    if (!str) {
        fprintf(stderr, "%s: malloc error\n", __func__);
        return NULL;
    }
    mempcpy(str, buffer_peek(buf), readable_bytes);
    str[readable_bytes] = '\0';
    buffer_retrieve_all(buf);
    return str;
}

const char *
findCRLF(struct buffer *buf, const char* start)
{
    const char *pread = buffer_peek(buf);
    const char *pwrite = buffer_begin_write(buf);
    const char *p;

    for (p = pread; p != pwrite; p++) {
        if (*p == "\n" && *(p+1) == "\r")
            return p;
        else
            continue;
    }

    return NULL;
}

void buffer_append(struct buffer *buf, const void *data, size_t len)
{
    if (buffer_writeable_bytes(buf) < len)
        buffer_make_space(buf, len);
    memcpy(buffer_begin_write(buf), data, len);
    buf->writer_index += len;
}

void buffer_prepend(struct buffer *buf, const void *data, size_t len)
{
    assert(len < buffer_prependable_bytes(buf));
    buf->reader_index -= len;
    memcpy(buffer_peek(buf), data, len);
}

static void buffer_make_space(struct buffer *buf, size_t len)
{
    if (buffer_writeable_bytes(buf) + buffer_prependable_bytes(buf) - BUFFER_PREPEND < len) 
        buf->buffer_ = realloc(buf->buffer_, buf->writer_index + len);
    else {
        //move readable data to the front
        memmove(buf->buffer_+BUFFER_PREPEND, buffer_peek(buf), buf->writer_index - buf->reader_index);
        buf->writer_index = buf->reader_index - BUFFER_PREPEND;
        buf->reader_index = BUFFER_PREPEND;
    }
}

ssize_t buffer_read(int fd, struct buffer *buf)
{
    char extrabuf[65536];
    struct iovec vec[2];
    size_t writable = buffer_writeable_bytes(buf);
    vec[0].iov_base = buffer_begin_write(buf);
    vec[0].iov_len = writable;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof(extrabuf);

    ssize_t n = readv(fd, vec, 2);
    if (n < 0) {
        perror("read error");
    } else if (n <= writable) {
        buf->writer_index += n;
    } else {
        buf->writer_index = buf->buffer_size;
        buffer_append(buf, extrabuf, n - writable);
    }

    return n;
}
