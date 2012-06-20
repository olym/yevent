/*
 * =====================================================================================
 *
 *       Filename:  Event.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/18/2012 12:31:26 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yin Zhongxing (olym), zhongxing.yin@cs2c.com.cn
 *        Company:  www.CS2C.com.cn
 *
 * =====================================================================================
 */

#include "EventLoop.h"

using namespace yevent;


void Event::updateEvent() {pLoop_->updateEvent(this);} 
void Event::deleteEvent() {pLoop_->deleteEvent(this);}
