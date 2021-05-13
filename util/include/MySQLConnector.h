/*
 * @FilePath: /simtochat/util/include/MySQLConnector.h
 * @Author: CGL
 * @Date: 2021-05-03 11:27:24
 * @LastEditors: CGL
 * @LastEditTime: 2021-05-13 20:27:52
 * @Description:
 *  Define tools to connect to the MySQL database.
 */
#ifndef UTIL_INCLUDE_MYSQL_CONNECTOR_H
#define UTIL_INCLUDE_MYSQL_CONNECTOR_H

#include <string>
#include <exception>

struct MYSQL;
struct MYSQL_RES;

/**
 * @author: CGL
 * @class MySQLException
 * @description: Describe the error encountered while connecting to the MySQL database.
 */
class MySQLException : public std::exception
{
public:
    MySQLException();
    virtual ~MySQLException();

public:
    virtual const char* what() const noexcept override;
    
};

/**
 * @author: CGL
 * @class MySQLResultSet
 * @description: A collection of results from a query using MySQLConnector.
 */
class MySQLResultSet
{
public:
    MySQLResultSet(MYSQL_RES* mysql_res);
    virtual ~MySQLResultSet();

public:
    /**
     * @author: CGL
     * @return Return false if there is no next row.
     * @description: Move the cursor down to point to the next row of result data.
     */
    bool NextRow();

    /**
     * @author: CGL
     * @param cols Specifies the column to be fetched.
     * @return Return the result string address of the specified column for the current row.
     * @description: Gets the data for a column of the current row, returned as a string address.
     */
    char* getData(unsigned int cols) const;

    /**
     * @author: CGL
     * @param cols Specifies the column to be fetched.
     * @return Return the result string address of the specified column for the current row.
     * @description: Get the data for a column of the current row, returned as a string address.
     */
    char* operator[](unsigned int cols) const;
    
    /**
     * @author: CGL
     * @description: Release the result resources.
     */
    void Release();

    /**
     * @author: CGL
     * @return Return the number of result rows.
     * @description: Get the number of result rows.
     */
    unsigned int getRowsNum() const;

    /**
     * @author: CGL
     * @return Return the number of result columns.
     * @description: Get the number of result columns.
     */
    unsigned int getFiledsNum() const;

protected:
    MYSQL_RES* m_res;
    char** m_rows;
};

/**
 * @author: CGL
 * @class MySQLConnector
 * @description: A utility class for connecting to a MySQL database.
 */
class MySQLConnector
{
public:
    // Default constructor without initialize and connection.
    MySQLConnector();

    // Automatically close the connection and release resources.
    virtual ~MySQLConnector();

public:
    /**
     * @author: CGL
     * @param serverIp The IP address of the host to connect to.
     * @param username MySQL database user.
     * @param password The password for your user.
     * @param dbname The name of the database to access.
     * @param port The port of the host. It should be default if the host is local.
     * @description:
     *  Set the connection configurations and 
     *  initialize the connection if it not already connected.
     */
    virtual void Setup(
        const std::string& serverIp,
        const std::string& username,
        const std::string& password,
        const std::string& dbname,
        unsigned int port = 0
    );
    
    /**
     * @author: CGL
     * @description: Connect to the specified database.
     */
    virtual void Connect();

    /**
     * @author: CGL
     * @description: Disconnect from the database.
     */
    virtual void Close();

    /**
     * @author: CGL
     * @param sql The SQL statement to execute.
     * @return Return the number of affected rows.
     * @description: Execute an SQL statement without any security checks
     *  and return the number of affected rows.
     */
    virtual uint64_t Excute(const std::string& sql);
    
    /**
     * @author: CGL
     * @param query The SQL statement to execute.
     * @return Return a set of query results.
     * @description: Execute a query SQL statement without any security checks
     *  and return a set of query results.
     * @attention Please call the 'Release()' method to release the result set after you use it.
     */
    virtual MySQLResultSet ExcuteQuery(const std::string& query);

protected:
    // Check if it is initialized before initializing it.
    void _SafeInit();

    // Check if it is closed before closing it.
    void _SafeClose();

protected:
    /**
     * @author: CGL
     * @struct MySQLConfig
     * @description: Describe the specific MySQL connection properties.
     */
    struct MySQLConfig
    {
        // The IP address of the host to connect to.
        std::string serverIp;
        
        // MySQL database user.
        std::string username;

        // The password for the user.
        std::string password;

        // The name of the database to access.
        std::string dbname;

        // The port of the host. It should be 0 if the host is local.
        unsigned int port = 0;
    };
    
    MySQLConfig m_config;
    MYSQL* m_mysql;
    MYSQL_RES* m_result;

    bool m_ready;   // check mark
    bool m_init;    // check mark
};


#endif // !UTIL_INCLUDE_MYSQL_CONNECTOR_H