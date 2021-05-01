/*
 * @FilePath: /simtochat/lib/socklib/include/Socket.h
 * @Author: CGL
 * @Date: 2021-04-14 12:37:34
 * @LastEditors: CGL
 * @LastEditTime: 2021-04-16 14:08:57
 * @Description: 
 *  Various TCP communication modes such as BIO and EPOLL + Reactor model.
 *  Socket: TCP socket. -> client  -Provide io interface;
 *  SingleServer: single-thread blocking-io tcp server -It can be extended to multithreading server.
 *  EpollServer: epoll + Reactor
 */
#ifndef SOCKLIB_INCLUDE_SOCKET_SERVER_H
#define SOCKLIB_INCLUDE_SOCKET_SERVER_H

#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <map>
#include <exception>
#include <string>

/**
 * @class SocketException
 * @extends std::exception
 * @author: CGL
 * @description: Throw this exception when a socket error occurs.
 */
class SocketException : std::exception
{
public:
    /**
     * @author: CGL
     * @description: Default exception without any message.
     */
    SocketException();

    /**
     * @author: CGL
     * @param msg Custom error message.
     * @description: An socket exception whit a custom error message.
     */
    SocketException(const std::string& msg);

    /**
     * @author: CGL
     * @param errid Error ID.
     * @param msg Error message. Default is an unnamed error message.
     * @description: An socket exception whit an id and a custom error message.
     */
    SocketException(int errid, const std::string& msg = "");

    /**
     * @author: CGL
     * @param errid Error ID.
     * @param file Which file throw an exception.
     * @param line The line of specific statement that threw the exception.
     * @param funcMsg The function threw the exception.
     * @description: Generate error message based on concrete information.
     */
    SocketException(int errid, const std::string& file, int line, const std::string& funcMsg);
    
    // Default distrutor whithout exception.
    virtual ~SocketException();

public:
    virtual const char* what() const noexcept override;

public:
    /**
     * @author: CGL
     * @return Return id of error.
     */
    int getErrorId() const;

    /**
     * @author: CGL
     * @return Return error message of type string.
     */
    std::string getErrorMsg() const;

protected:
    int m_errid;
    std::string m_errMsg;
};

/**
 * @class _SocketUtil
 * @author: CGL
 * @description: 
 *  Extract public members and encapsulate them into util base classes.
 *  It also provides the system call after exception handling.
 */
class _SocketUtil
{
public:
    // Create sockaddr_in.
    _SocketUtil();

    // Free sockaddr_in.
    virtual ~_SocketUtil();
    
public:
    /**
     * @author: CGL
     * @return Return my socket file descriptor.
     */
    int getfd() const;

    /**
     * @author: CGL
     * @return Return sockaddr_in.
     */
    sockaddr_in getAddr_in() const;

    /**
     * @author: CGL
     * @return Return a pointer to sockaddr_in of type sockaddr.
     */
    const sockaddr* getpAddr() const;

    /**
     * @author: CGL
     * @return Return the IP address of type string.
     */
    std::string getIpStr() const;

    /**
     * @author: CGL
     * @return Return the port of type int32.
     * @description: 
     */
    int getPort() const;

    /**
     * @author: CGL
     * @return Return my socket file descriptor.
     */
    operator int() const;

protected:
    /** A series of exception-handled system calls. */
    
    int _socket (int domain, int type, int protocol);
    void _bind (int fd, const struct sockaddr* addr, unsigned int len);
    void _listen (int fd, int n);
    void _connect (int fd, const struct sockaddr* addr, unsigned int len);
    int _accept (int fd, struct sockaddr* addr, unsigned int* addr_len);

    bool _read(int fd, void* buf, size_t n, int flags);
    bool _write(int fd, const void* buf, size_t n, int flags);
    void _close(int fd);

    int _epoll_create (int size);
    int _epoll_create1 (int flags);
    int _epoll_wait(int epfd, epoll_event *events, int maxevents, int timeout);
    void _epoll_ctl (int epfd, int op, int fd, struct epoll_event *event);

protected:
    int m_fd;
    sockaddr_in* m_addr;
    socklen_t m_addrLen;
};

/**
 * @author: CGL
 * @description: Socket stream which connect, read and write data from buffers.
 */
class Socket : public _SocketUtil
{
public:
    /**
     * @author: CGL
     * @description: Default constructor without initialization.
     */
    Socket();

    /**
     * @author: CGL
     * @param fd Specifies the file descriptor.
     * @param addr_in Specifies the address family.
     * @description: Create a socket that specifies the file descriptor and address family.
     */
    Socket(int fd, const sockaddr_in& addr_in);

    // Default destructor with closing file descriptor.
    virtual ~Socket();

public:
    /**
     * @author: CGL
     * @param ip The IP address to connect.
     * @param port  The port to connect.
     * @description: Connect to the specified server as a client.
     */
    virtual void Connect(const std::string& ip, int port);

    /**
     * @author: CGL
     * @param obj Read data into this object.
     * @return Return success or not.
     * @description: Read data from a socket buffer to an objet.
     */
    template<class T>
    bool Read(T& obj);

    /**
     * @author: CGL
     * @param obj Write data of this object.
     * @return Return success or not.
     * @description: Write data of an object into the buffer.
     */
    template<class T>
    bool Write(const T& obj);

    /**
     * @author: CGL
     * @param dist Read data into this address.
     * @param n The number of data.
     * @return Return success or not.
     * @description: Read n pieces of data from the socket buffer.
     */
    template<class T>
    bool Read(T* dist, size_t n);

    /**
     * @author: CGL
     * @param dist Write datas.
     * @param n The number of data.
     * @return Return success or not.
     * @description: Write n pieces of data into the socket buffer.
     */
    template<class T>
    bool Write(const T* dist, size_t n);
};

/**
 * @class SingleServer
 * @author: CGL
 * @description: A basic single thread blocking server by default.
 *  Provide listen and accept method.
 */
class SingleServer : public _SocketUtil
{
public:
    SingleServer();
    virtual ~SingleServer();

public:
    /**
     * @author: CGL
     * @param port The port to listen.
     * @description: Starup to listen connection.
     */
    virtual void Listen(int port);

    /**
     * @author: CGL
     * @description: Wait for accepting the connection from clients.
     */
    virtual Socket Accept();
};

/**
 * @class EpollServer
 * @author: CGL
 * @description:
 *  The default single-threaded non-blocking server based on Epoll.
 *  Provide Acceptor and Processor callbacks. It can be extended into the Reactor model.
 */
class EpollServer : public _SocketUtil
{
public:
    /**
     * @author: CGL
     * @description: Default constructor without initialization and any exceptions.
     */
    EpollServer();

    // Auto close the file descriptor.
    virtual ~EpollServer();

public:
    /**
     * @author: CGL
     * @param port The port to listen.
     * @description:
     *  Startup the server on this port.
     *  It will run with a loop. You could set callbacks to process the events.
     */
    virtual void Run(int port);

    /**
     * @author: CGL
     * @param acceptor The callback will active when new client connect.
     * @description: Setup the acceptor callback.
     */
    void setAcceptor(bool (*acceptor)(Socket& client));

    /**
     * @author: CGL
     * @param processor The callback will active when new messages arrived except for new client.
     * @description: Setup the processor callback.
     */
    void setProcessor(bool (*processor)(Socket& client));

protected:
    // Set the file descriptor to non-blocking.
    void setnonblocking(int fd);

    // Add a file descriptor need to listen on.
    void addfd(int epfd, int fd, bool enable_et);

protected:
    bool m_running;
    int m_epfd;
    epoll_event m_events[128];      // Epoll size default = 128
    std::map<int, Socket> m_clientMap;
    bool (*m_acceptor)(Socket& client);
    bool (*m_processor)(Socket& client);
};

template<class T>
bool Socket::Read(T& obj)
{
    return _read(m_fd, &obj, sizeof(obj), 0);
}

template<class T>
bool Socket::Write(const T& obj)
{
    return _write(m_fd, &obj, sizeof(obj), 0);
}

template<class T>
bool Socket::Read(T* dist, size_t n)
{
    return _read(m_fd, dist, sizeof(T) * n, 0);
}

template<class T>
bool Socket::Write(const T* dist, size_t n)
{
    return _write(m_fd, dist, sizeof(T) * n, 0);
}


#endif // !SOCKLIB_INCLUDE_SOCKET_SERVER_H