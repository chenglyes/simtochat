#include "Socket.h"

#include <sys/socket.h>
#include <sys/select.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SOCKET_UTIL_EXCEPTION(errid, msg) if((msg)) throw SocketException(errid, __FILE__, __LINE__, #msg)

SocketException::SocketException()
    : m_errid(0), m_errMsg("ERROR: Exception.")
{

}

SocketException::SocketException(const std::string& msg)
    : m_errid(0), m_errMsg(msg)
{

}

SocketException::SocketException(int errid, const std::string& msg)
    : m_errid(errid)
{
    char buffer[1024] = {0};
    std::string em = msg;
    if (msg == "") em = "Exception";
    sprintf(buffer, "ERROR(%d): %s.", m_errid, em.c_str());
    m_errMsg = buffer;
}

SocketException::SocketException(int errid, const std::string& file, int line, const std::string& funcMsg)
    : m_errid(errid)
{
    char buffer[1024] = {0};
    sprintf(buffer, "ERROR(%d): Exception(%s) in file(%s) at line(%d).",
        m_errid, funcMsg.c_str(), file.c_str(), line);
    m_errMsg = buffer;
}

SocketException::~SocketException()
{

}

const char* SocketException::what() const noexcept
{
    return m_errMsg.c_str();
}

int SocketException::getErrorId() const
{
    return m_errid;
}

std::string SocketException::getErrorMsg() const
{
    return m_errMsg;
}

_SocketUtil::_SocketUtil()
    : m_fd(0), m_addr(nullptr)
{
    m_addr = new sockaddr_in{0};
    m_addrLen = sizeof(*m_addr);
}

_SocketUtil::~_SocketUtil()
{
    if (m_addr) delete m_addr;
}

int _SocketUtil::getfd() const
{
    return m_fd;
}

sockaddr_in _SocketUtil::getAddr_in() const
{
    return *m_addr;
}

const sockaddr* _SocketUtil::getpAddr() const
{
    return (sockaddr*)m_addr;
}

std::string _SocketUtil::getIpStr() const
{
    return inet_ntoa(m_addr->sin_addr);
}

int _SocketUtil::getPort() const
{
    return ntohs(m_addr->sin_port);
}

_SocketUtil::operator int() const
{
    return m_fd;
}

int _SocketUtil::_socket (int domain, int type, int protocol)
{
    int rst;
    SOCKET_UTIL_EXCEPTION(0, -1 == (rst = socket(domain, type, protocol)));
    return rst;
}

void _SocketUtil::_bind (int fd, const struct sockaddr* addr, socklen_t len)
{
    SOCKET_UTIL_EXCEPTION(0, -1 == bind(fd, addr, len));
}

void _SocketUtil::_listen (int fd, int n)
{
    SOCKET_UTIL_EXCEPTION(0, -1 == listen(fd, n));
}

void _SocketUtil::_connect (int fd, const struct sockaddr* addr, socklen_t len)
{
    SOCKET_UTIL_EXCEPTION(0, -1 == connect(fd, addr, len));
}

int _SocketUtil::_accept (int fd, struct sockaddr* addr, socklen_t* addr_len)
{
    int rst;
    SOCKET_UTIL_EXCEPTION(0, -1 == (rst = accept(fd, addr, addr_len)));
    return rst;
}

bool _SocketUtil::_read(int fd, void* buf, size_t n, int flags)
{
    int sz = 0;
    SOCKET_UTIL_EXCEPTION(0, -1 == (sz = recv(fd, buf, n, flags)));
    return sz > 0;
}

bool _SocketUtil::_write(int fd, const void* buf, size_t n, int flags)
{
    int sz = 0;
    SOCKET_UTIL_EXCEPTION(0, -1 == (sz = send(fd, buf, n, flags)));
    return sz > 0;
}

void _SocketUtil::_close(int fd)
{
    SOCKET_UTIL_EXCEPTION(0, -1 == close(fd));
}

int _SocketUtil::_epoll_create (int size)
{
    int rst;
    if (rst == -1) SOCKET_UTIL_EXCEPTION(0, -1 == (rst = epoll_create(size)));
    return rst;
}

int _SocketUtil::_epoll_create1 (int flags)
{
    int rst;
    SOCKET_UTIL_EXCEPTION(0, -1 == (epoll_create1(flags)));
    return rst;
}

void _SocketUtil::_epoll_ctl (int epfd, int op, int fd, struct epoll_event *event)
{
    SOCKET_UTIL_EXCEPTION(0, -1 == epoll_ctl(epfd, op, fd, event));
}

Socket::Socket()
    : _SocketUtil()
{

}

Socket::~Socket()
{
    close(m_fd);
}

void Socket::Connect(const std::string& ip, int port)
{
    m_fd = _socket(AF_INET, SOCK_STREAM, 0);
    m_addr->sin_family = AF_INET;
    m_addr->sin_port = htons(port);
    m_addr->sin_addr.s_addr = inet_addr(ip.c_str());
    _connect(m_fd, getpAddr(), m_addrLen);
}

SingleServer::SingleServer()
    : _SocketUtil()
{

}

SingleServer::~SingleServer()
{
    close(m_fd);
}

void SingleServer::Listen(int port)
{
    m_fd = _socket(AF_INET, SOCK_STREAM, 0);
    m_addr->sin_family = AF_INET;
    m_addr->sin_port = htons(port);
    m_addr->sin_addr.s_addr = INADDR_ANY;
    _bind(m_fd, getpAddr(), m_addrLen);
    _listen(m_fd, INT32_MAX);
}

Socket SingleServer::Accept()
{
    Socket client;
    client.m_fd = _accept(m_fd, (sockaddr*)client.m_addr, &client.m_addrLen);
    return client;
}
