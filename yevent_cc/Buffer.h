/*
 * =====================================================================================
 *
 *       Filename:  Buffer.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/26/2012 12:49:16 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#ifndef __BUFFER_H
#define __BUFFER_H
#include <string>
#include <vector>
#include <algorithm>
#include <stdio.h>
namespace yevent {
class Buffer {
    public:
        Buffer() :
            buf_(BufferInitialSize),
            size_(BufferInitialSize),
            readIndex_(0),
            writeIndex_(0)
        { }
        Buffer(Buffer &buf) {
            buf_.swap(buf.buf_);
            std::swap(readIndex_, buf.readIndex_);
            std::swap(writeIndex_, buf.writeIndex_);
        }

        ~Buffer() {}
        int read(int fd, int *error);
        void makeSpace(int len);

        void append(const char *data, int len)
        {
            if (writableBytes() < len) {
                makeSpace(len);
            }

            std::copy(data, data+len, beginWrite());
            writeIndex_ += len;
        }
        size_t bufferSize() {
            return size_;
        }
        const size_t readableBytes() const 
        {
            return writeIndex_ - readIndex_;
        }
        const size_t writableBytes() const {
            return size_ - writeIndex_;
        }
        std::string getAsString(size_t len) 
        {
            if (len <= readableBytes()) {
                std::string str(beginRead(), len);
                readIndex_ += len;
                return str;
            } else {
                std::string str(beginRead(), readableBytes());
                readIndex_ = 0;
                writeIndex_ = 0;
                return str;
            }
        }
        void retrieve(size_t len)
        {
            readIndex_ += len;
        }
        std::string getAllAsString() {
            std::string str(beginRead(), readableBytes());
            readIndex_ = 0;
            writeIndex_ = 0;
            return str;
        }
        std::string getUntil(const char *end)
        {
            std::string str(beginRead(), end - beginRead());
            readIndex_ += end - beginRead();
            return str;
        }
        const char *findCRLF() 
        {
            return findCRLF(NULL);
        }
        const char *findCRLF(char* start) {
            char *s = (start == NULL ? beginRead() : start);
            const char *pCRLF = std::search(s, beginWrite(), CRLF, CRLF+2);
            return pCRLF == NULL ? NULL:pCRLF;
        }

        char *beginRead() 
        {
            return begin() + readIndex_;
        }

        char *beginWrite()
        {
            return begin() + writeIndex_;
        }
    private:
        char *begin()
        {
            return &*buf_.begin();
        }
        
        void makeSapce(size_t len);

        static const int BufferInitialSize = 1024;
        static const char CRLF[];

        std::vector<char> buf_;
        int size_;
        int readIndex_;
        int writeIndex_;
};
}

#endif
