/*
 * =====================================================================================
 *
 *       Filename:  tcp_connection.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/21/2012 11:52:58 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef _TCP_CONNECTION_H
#define _TCP_CONNEcTION_H

#define set_connect_cb(server, conn_cb) \
    ((server)->connection_callback = (conn_cb))
#define set_message_cb(server, msg_cb) \
    ((server)->message_callback = (msg_cb))
#define set_write_complete_cb(server, write_cb) \
    ((server)->write_complete_callback = (write_cb))

#endif
