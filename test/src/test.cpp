/*
 * @FilePath: /simtochat/test/src/test.cpp
 * @Author: CGL
 * @Date: 2021-05-13 20:39:02
 * @LastEditors: CGL
 * @LastEditTime: 2021-05-13 22:37:37
 * @Description: 
 */
#include "MySQLConnector.h"

#include <iostream>
using namespace std;

int main()
{
    MySQLConnector connector;
    connector.Setup("127.0.0.1", "root", "12321", "test");
    try
    {
        connector.Connect();
        connector.Excute("insert into test (name) values (\"小红\")");
        MySQLResultSet rs = connector.ExcuteQuery("select * from test");
        cout << rs.getFiledsNum() << ", " << rs.getRowsNum() << endl;
        while (rs.NextRow())
        {
            cout << rs[0] << " " << rs[1] << endl;
        }
        
    }
    catch(const MySQLException& e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}