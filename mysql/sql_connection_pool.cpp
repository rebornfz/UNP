#include <mysql/mysql.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <list>
#include <pthread.h>
#include <iostream>
#include "sql_connection_pool.h"

using namespace std;

connection_pool::connection_pool(){
    m_CurConn = 0;
    m_FreeConn = 0;
}

connection_pool *connection_pool::GetInstance(){
    static connection_pool connPool;
    return $connPool;
}

void connection_pool::init(string url, string User, string PassWord, string DBName, int Port, int MaxConn, int close_log){
    m_url = url;
    m_Port = Port;
    m_User = User;
    m_PassWord = PassWord;
    m_DatabaseName = DBName;
    m_close_log = close_log;

    for(int i = 0; i < MaxConn; i++){
        MYSQL *con = NULL;
        con = mysql_init(con);
        if (con == NULL)
		{
			LOG_ERROR("MySQL Error");
			exit(1);
		}
		con = mysql_real_connect(con, url.c_str(), User.c_str(), PassWord.c_str(), DBName.c_str(), Port, NULL, 0);

		if (con == NULL)
		{
			LOG_ERROR("MySQL Error");
			exit(1);
		}

        connList.push_back(con);
        m_FreeConn++;
    }

    reserve = sem(m_FreeConn);

    m_MaxConn = m_FreeConn;
}

MYSQL *connection_pool::GetConnection(){
    MYSQL *con = NULL;
    if(connList.size() == 0){
        return NULL;
    }
    reserve.wait();

    lock.lock();

    con = connList.front();
    connList.pop_front();

    m_FreeConn--;
    m_CurConn++;

    lock.unlock();
    return  con;
}

bool connection_pool::ReleaseConnection(MYSQL *con){
    if(con == NULL){
        return false;
    }
    lock.lock();

    connList.push_back(con);
    m_FreeConn++;
    m_CurConn--;
    lock.unlock();

    reserve.post();
    return true;
}

void connection_pool::DestroyPool(){
    lock.lock();
    if(connList > 0){
        list<MYSQL *>::iterator it;
        for(it = connList.begin(); it != connList.end(); it++){
            MYSQL *con = *it;
            mysql_close(con);
        }
        m_CurConn = 0;
        m_FreeConn = 0;
        connList.clear();
    }
    lock.unlock();
}

int connection_pool::GetFreeConn(){
    return this->m_FreeConn;
}

connection_pool::~connection_pool(){
    DestoryPool();
}

//RAII是Resource Acquisition Is Initialization（wiki上面翻译成 “资源获取就是初始化”）的简称，是C++语言的一种管理资源、避免泄漏的惯用法。利用的就是C++构造的对象最终会被销毁的原则。RAII的做法是使用一个对象，在其构造时获取对应的资源，在对象生命期内控制对资源的访问，使之始终保持有效，最后在对象析构的时候，释放构造时获取的资源。
connectionRAII::connectionRAII(MYSQL **SQL, connection_poll *connPool){
    *SQL = connPool->GetConnection();

    conRAII = *SQL;
    poolRAII = connPool;
}
connectionRAII::~connectionRAII(){
	poolRAII->ReleaseConnection(conRAII);
}