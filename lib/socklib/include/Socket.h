#ifndef SIMTOCHAT_INCLUDE_SOCKET_SERVER_H
#define SIMTOCHAT_INCLUDE_SOCKET_SERVER_H

#include <exception>
#include <string>

struct sockaddr;
struct sockaddr_in;
typedef unsigned int socklen_t;

class SocketException : std::exception
{
public:
    SocketException();
    SocketException(const std::string& msg);
    SocketException(int errid, const std::string& msg = "");
    SocketException(int errid, const std::string& file, int line, const std::string& funcMsg);
    virtual ~SocketException();

public:
    virtual const char* what() const noexcept override;

public:
    inline int getErrorId() const;
    std::string getErrorMsg() const;

protected:
    int m_errid;
    std::string m_errMsg;
};

class _SocketUtil
{
public:
    _SocketUtil();
    virtual ~_SocketUtil();
public:
    inline int getfd() const;
    inline sockaddr_in getAddr_in() const;
    inline const sockaddr* getpAddr() const;
    inline std::string getIpStr() const;
    inline int getPort() const;

    inline operator int() const;

protected:
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
    void _epoll_ctl (int epfd, int op, int fd, struct epoll_event *event);

protected:
    int m_fd;
    sockaddr_in* m_addr;
    socklen_t m_addrLen;
};

class Socket : public _SocketUtil
{
public:
    Socket();
    virtual ~Socket();

public:
    virtual void Connect(const std::string& ip, int port);

    template<class T>
    bool Read(T& obj);

    template<class T>
    bool Write(const T& obj);

    template<class T>
    bool Read(T* dist, size_t n);

    template<class T>
    bool Write(const T* dist, size_t n);

protected:
    friend class SingleServer;
};

class SingleServer : public _SocketUtil
{
public:
    SingleServer();
    virtual ~SingleServer();

public:
    virtual void Listen(int port);
    virtual Socket Accept();
};

class MultiThreadServer : public _SocketUtil
{
public:
    MultiThreadServer();
    virtual ~MultiThreadServer();

public:
    virtual void Run();

protected:
    static void* UserThread(void* aarg);

protected:
    bool m_running;

};

#include <sys/socket.h>

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


#endif // !SIMTOCHAT_INCLUDE_SOCKET_SERVER_H
