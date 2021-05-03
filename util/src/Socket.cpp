/*
 * @FilePath: /simtochat/util/src/Socket.cpp
 * @Author: CGL
 * @Date: 2021-05-03 15:40:39
 * @LastEditors: CGL
 * @LastEditTime: 2021-05-03 15:40:45
 * @Description: 
 */
#include "Socket.h"

#include <sys/socket.h>
#include <sys/select.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

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
    SOCKET_UTIL_EXCEPTION(0, -1 == (rst = epoll_create(size)));
    return rst;
}

int _SocketUtil::_epoll_create1 (int flags)
{
    int rst;
    SOCKET_UTIL_EXCEPTION(0, -1 == (rst = epoll_create1(flags)));
    return rst;
}

int _SocketUtil::_epoll_wait(int epfd, epoll_event *events, int maxevents, int timeout)
{
    int count;
    SOCKET_UTIL_EXCEPTION(0, -1 == (count = epoll_wait(epfd, events, maxevents, timeout)));
    return count;
}

void _SocketUtil::_epoll_ctl (int epfd, int op, int fd, struct epoll_event *event)
{
    SOCKET_UTIL_EXCEPTION(0, -1 == epoll_ctl(epfd, op, fd, event));
}

Socket::Socket()
    : _SocketUtil()
{

}

Socket::Socket(int fd, const sockaddr_in& addr_in)
    : _SocketUtil()
{
    m_fd = fd;
    memcpy(m_addr, &addr_in, m_addrLen);
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
    sockaddr_in addrClient = {0};
    socklen_t addrLen = sizeof(addrClient);
    int sockfd = _accept(m_fd, (sockaddr*)&addrClient, &addrLen);
    return Socket(sockfd, addrClient);
}

EpollServer::EpollServer()
    : m_running(true), m_epfd(0), m_events{0}, m_acceptor(nullptr), m_processor(nullptr)
{

}

EpollServer::~EpollServer()
{
    close(m_epfd);
    close(m_fd);
}

void EpollServer::Run(int port)
{
    m_fd = _socket(AF_INET, SOCK_STREAM, 0);
    m_addr->sin_family = AF_INET;
    m_addr->sin_port = htons(port);
    m_addr->sin_addr.s_addr = INADDR_ANY;
    _bind(m_fd, getpAddr(), m_addrLen);
    _listen(m_fd, INT32_MAX);

    m_epfd = _epoll_create(128);
    addfd(m_epfd, m_fd, true);

    while (m_running)
    {
        int count = epoll_wait(m_epfd, m_events, 128, -1);
        for (size_t i = 0; i < count; i++)
        {
            int sockfd = m_events[i].data.fd;
            if (sockfd == m_fd)
            {
                sockaddr_in addrClient;
                socklen_t addrLen = sizeof(addrClient);
                int clientfd = _accept(m_fd, (sockaddr*)&addrClient, &addrLen);
                Socket client(clientfd, addrClient);
                if (!m_acceptor) continue;
                if (m_acceptor(client))
                {
                    addfd(m_epfd, clientfd, true);
                    m_clientMap[clientfd] = client;
                }
            }
            else
            {
                if (!m_processor) continue;
                if (!m_processor(m_clientMap[sockfd]))
                {
                    m_clientMap.erase(sockfd);
                }
            }
        }
    }
}

void EpollServer::setAcceptor(bool (*acceptor)(Socket& client))
{
    m_acceptor = acceptor;
}

void EpollServer::setProcessor(bool (*processor)(Socket& client))
{
    m_processor = processor;
}

void EpollServer::setnonblocking(int fd)
{
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0) | O_NONBLOCK);
}

void EpollServer::addfd(int epfd, int fd, bool enable_et)
{
    epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLIN;
    if (enable_et) ev.events = EPOLLIN | EPOLLET;
    _epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
    setnonblocking(fd);
}