/*
 * =====================================================================================
 *
 *       Filename:  Multiplexer.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/25/2012 03:37:05 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yin Zhongxing (olym), olym.yin@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#include "Multiplexer.h"

using namespace yevent;
class MultiplexerEpoll;
class MultiplexerSelect();

Multiplexer *NewMultiplexerImp(int type)
{
    switch (type) {
    case Multiplexer_EPOLL:
        return new MultiplexerEpoll();
    case Multiplexer_SELECT:
        return new MultiplexerSelect();
    }
    return NULL;
}
