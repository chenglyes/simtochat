/*
 * @FilePath: /simtochat/util/src/MySQLConnector.cpp
 * @Author: CGL
 * @Date: 2021-05-03 15:40:09
 * @LastEditors: CGL
 * @LastEditTime: 2021-05-13 20:28:16
 * @Description: 
 */
#include "MySQLConnector.h"
#include <mysql/mysql.h>

MySQLException::MySQLException()
{
    
}

MySQLException::~MySQLException()
{
    
}

const char* MySQLException::what() const noexcept
{
    return "An exception occurred while accessing MySQL!";
}

MySQLResultSet::MySQLResultSet(MYSQL_RES* mysql_res)
    : m_res(mysql_res), m_rows(nullptr)
{

}

MySQLResultSet::~MySQLResultSet()
{
    
}

bool MySQLResultSet::NextRow()
{
    m_rows = mysql_fetch_row(m_res);
    return m_rows != nullptr;
}

char* MySQLResultSet::getData(unsigned int cols) const
{
    return m_rows[cols];
}

char* MySQLResultSet::operator[](unsigned int cols) const
{
    return getData(cols);
}

void MySQLResultSet::Release()
{
    mysql_free_result(m_res);
}

unsigned int MySQLResultSet::getRowsNum() const
{
    return mysql_num_rows(m_res);
}

unsigned int MySQLResultSet::getFiledsNum() const
{
    return mysql_num_fields(m_res);
}

MySQLConnector::MySQLConnector()
    : m_mysql(nullptr), m_result(nullptr), m_ready(false), m_init(false)
{
    
}

MySQLConnector::~MySQLConnector()
{
    _SafeClose();
}

void MySQLConnector::Setup(
    const std::string& serverIp,
    const std::string& username,
    const std::string& password,
    const std::string& dbname,
    unsigned int port
)
{
    m_config.serverIp = serverIp;
    m_config.username = username;
    m_config.password = password;
    m_config.dbname = dbname;
    m_config.port = port;
    m_ready = false;
    _SafeInit();
}

void MySQLConnector::Connect()
{
    if (m_ready) return;
    
    _SafeInit();
    m_mysql = mysql_real_connect(
        m_mysql,
        m_config.serverIp.c_str(),
        m_config.username.c_str(),
        m_config.password.c_str(),
        m_config.dbname.c_str(),
        0, nullptr, 0
    );
    if (!m_mysql) throw MySQLException();
    m_ready = true;
}

void MySQLConnector::Close()
{
    _SafeClose();
}

uint64_t MySQLConnector::Excute(const std::string& sql)
{
    if (0 != mysql_real_query(m_mysql, sql.c_str(), sql.length()))
    {
        throw MySQLException();
    }
    return mysql_affected_rows(m_mysql);
}

MySQLResultSet MySQLConnector::ExcuteQuery(const std::string& query)
{
    if (0 != mysql_real_query(m_mysql, query.c_str(), query.length()))
    {
        throw MySQLException();
    }
    MYSQL_RES* res = mysql_store_result(m_mysql);
    return MySQLResultSet(res);
}

void MySQLConnector::_SafeInit()
{
    if (!m_init)
    {
        m_mysql = mysql_init(nullptr);
        if (!m_mysql) throw std::exception();
        m_init = true;
    }
}

void MySQLConnector::_SafeClose()
{
    mysql_free_result(m_result);
    mysql_close(m_mysql);
    m_ready = false;
    m_init = false;
}