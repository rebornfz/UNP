#ifndef _CONNECTTION_POOL_
#define _CONNECTTION_POOL_

#include <stdio.h>
#include <list>
#include <mysql/mysql.h>
#include <error.h>
#include <string.h>
#include <iostream>
#include <string>
#include "../lock/locker.h"
#include "../log/log.h"
using namespace std;

class connection_pool{
public:
    string m_url;
    string m_Port;
    string m_User;
    string m_PassWord;
    string m_DatabaseName;
    int m_close_log;

    MYSQL *GeetConnection();
    bool ReleaseConnection(MYSQL *conn);
    int GetFreeConn();
    void DestroyPool;

    static connection_pool *GetInstance(string url, string User, string PassWord, string DataBaseName, int Port, int MaxConn, int close_log);

    void init();

private:
    connection_pool();
    ~connection_pool();
    int m_MaxConn;
    int m_CurConn;
    int m_FreeConn;
    locker lock;
    list<MYSQL*> connList;
    sem reserve;
};

class connectionRAII{
public:
    connectionRAII(MYSQL **con, connection_pool *connPool);
    ~connectionRAII();

private:
    MYSQL *conRAII;
    connection_pool *poolRAII;
};
#endif