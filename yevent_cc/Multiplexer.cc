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
#include <stdlib.h>
#include "Multiplexer.h"
#include "MultiplexerSelect.h"
#include "MultiplexerEpoll.h"

namespace yevent {
Multiplexer *NewMultiplexerImp(int type)
{
    switch (type) {
    case Multiplexer_EPOLL:
        return new MultiplexerEpoll();
    //case Multiplexer_SELECT:
    //    return new MultiplexerSelect();
    }
    return NULL;
}
}

