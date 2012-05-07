/*
 * =====================================================================================
 *
 *       Filename:  Timestamp_test.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/07/2012 10:49:13 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yin Zhongxing (olym), olym.yin@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#include <iostream>
#include "../Timestamp.h"

using namespace yevent;

int main(int argc, const char *argv[])
{
    Timestamp ts = addTime(Timestamp::now(), 5.5);
    std::cout << ts.toString() << std::endl;;
    std::cout << ts.toFormattedString() << std::endl;
    return 0;
}
