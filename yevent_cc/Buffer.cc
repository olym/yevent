/*
 * =====================================================================================
 *
 *       Filename:  Buffer.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2012年06月13日 15时06分14秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yin Zhongxing (olym), zhongxing.yin@cs2c.com.cn
 *        Company:  www.CS2C.com.cn
 *
 * =====================================================================================
 */
#include <sys/uio.h>
#include <errno.h>
#include "Buffer.h"
using namespace yevent;
using std::string;

const char Buffer::CRLF[] = "\r\n";

int Buffer::read(int fd, int *error)
{
  char extrabuf[65536];
  struct iovec vec[2];
  size_t writable = writableBytes();
  vec[0].iov_base = beginWrite();
  vec[0].iov_len = writable;
  vec[1].iov_base = extrabuf;
  vec[1].iov_len = sizeof extrabuf;
  ssize_t n = ::readv(fd, vec, 2);
  if (n < 0) {
    *error = errno;
  } else if (n <= writable) {
    writeIndex_ += n;
  } else {
    writeIndex_ = buf_.size();
    append(extrabuf, n - writable);
  }
  return n;
}
void Buffer::makeSpace(int len)
{
    if (writableBytes() + readIndex_ > len) {
        std::copy(beginRead(), beginWrite(), begin());
        writeIndex_ = beginWrite() - beginRead();
        readIndex_ = 0;
    } else {
        buf_.resize(writeIndex_ + len);
        size_ = writeIndex_ + len;
    }
}

