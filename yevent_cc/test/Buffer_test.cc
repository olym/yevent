/*
 * =====================================================================================
 *
 *       Filename:  Buffer_test.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2012年06月13日 16时15分37秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yin Zhongxing (olym), zhongxing.yin@cs2c.com.cn
 *        Company:  www.CS2C.com.cn
 *
 * =====================================================================================
 */

#include "../Buffer.h"
#include <string.h>
#include <iostream>

using namespace std;
using namespace yevent;

int main(int argc, char *argv[])
{
    Buffer buffer;
    char data[] = "qwertyuiopasdfghjkl\r\n123456789123456789";
    cout << "data size = " << sizeof data<< endl;

    buffer.append(data, strlen(data));

    cout << "buffer size = " << buffer.bufferSize() << endl;

    cout << buffer.readableBytes() << " " << buffer.writableBytes() << endl;

    const char *p = buffer.findCRLF();
    cout << "until CRLF: " << buffer.getUntil(p) << endl;
    buffer.retrieve(2);
    cout << buffer.readableBytes() << " " << buffer.writableBytes() << endl;

    cout << "buffer data: " << buffer.getAsString(10) << endl;
    cout << buffer.readableBytes() << " " << buffer.writableBytes() << endl;

    buffer.append("11111", 5);

    cout << "buffer data: " << buffer.getAllAsString() << endl;
    cout << buffer.readableBytes() << " " << buffer.writableBytes() << endl;
    return 0;
}
