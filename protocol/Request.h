/*
 * @FilePath: /simtochat/protocol/Request.h
 * @Author: CGL
 * @Date: 2021-04-19 15:47:41
 * @LastEditors: CGL
 * @LastEditTime: 2021-05-01 15:43:17
 * @Description: 
 *  Application layer protocol that specifies the format
 *  for data exchanged between client and server.
 */
#ifndef SIMTOCHAT_INCLUDE_REQUEST_H
#define SIMTOCHAT_INCLUDE_REQUEST_H

/**
 * @author: CGL
 * @enum RequestType
 * @description: Request types.
 */
enum RequestType
{
    RT_UNKNOW,
    RT_LOGIN,
    RT_REGISTER,
    RT_SENDMESSAGE
};

/**
 * @author: CGL
 * @struct Request
 * @description: Request data format.
 */
struct Request
{
    char type;
    long length;
    char* msg;
};

/**
 * @author: CGL
 * @struct msg_login
 * @description: Login message.
 */
struct msg_login
{
    char username[16];
    char password[20];
};

/**
 * @author: CGL
 * @struct msg_register
 * @description: Register message.
 */
struct msg_register
{
    char username[16];
    char password[20];
    char nickname[28];
};

/**
 * @author: CGL
 * @struct msg_sendmessage
 * @description: The message that sends the message.
 */
struct msg_sendmessage
{
    char sender[16];
    char reciver[16];
    long sendtime;
    char message[1024];
};

#endif // !SIMTOCHAT_INCLUDE_REQUEST_H