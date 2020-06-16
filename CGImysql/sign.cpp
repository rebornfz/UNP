#include <mysql/mysql.h>
#include <iostream>
#include <string>
#include <string.h>
#include <cstdio>
#include <map>
#include <fstream>
#include <sstream>
#include "../lock/locker.h"
using namespace std;

int main(int argc, char *argv[]){
    map<string, string> user;

    locker lock;
    string SQLVeriyf(argv[3]);

    if(SQLVeriyf == "2"){
        string sql_user(argv[4]);
        string sql_passwd(argv[5]);
        string sql_name(argv[6]);

        MYSQL *con = NULL;
        con = mysql_init(con);
        
        if(con == NULL){
            cout << "Error:" << mysql_error(con);
            exit(1);
        }
        con = mysql_real_connect(con, "localhost", sql_user.c_str(), sql_passwd.c_str(), sql_name.c_str(), 3306, NULL, 0);

        if(con == NULL){
            cout << "Error" << mysql_error(con);
            exit(1);
        }

        if(mysql_query(con, "SELECT username, passwd FROM user")){
            cout << "select error:" << mysql_error(con);
            return -1;
        }

        MYSQL_RES *result = mysql_store_result(con);
        int num_fields = mysql_num_fields(result);
        MYSQL_FILED *fields = mysql_fetch_fields(result);

        while(MYSQL_ROW row = mysql_fetch_row(result)){
            string temp1(row[0]);
            string temp2(row[1]);
            users[temp1] = temp2;
        }

        string name(argv[1]);
        const char *namep = name.c_str();
        string passwd(argv[2]);
        const char *passwdp = passwd.c_str();
        char flag = *argv[0];

        char *sql_insert = (char *)malloc(sizeof(char) * 200);
        strcpy(sql_insert, "INSERT INTO user(username, passwd) VALUES(");
        strcat(sql_insert, "'");
        strcat(sql_insert, namep);
        strcat(sql_insert, "', '");
        strcat(sql_insert, passwdp);
        strcat(sql_insert, "')");

        if(flag == '3'){//注册
            if(users.find(name) == users.end()){
                lock.lock();
                int res = mysql_query(con, sql_insert);
                lock.unlock();

                if(!res) printf("1\n");
                else
                {
                    printf("0\n");
                }
                
            }
            else
            {
                printf("0\n");
            }
            
        }
        else if(flag == '2'){
            if(users.find(name) != users.end() && users[name] == passwd){
                printf("1\n");
            }
            else{
                printf("0\n");
            }

        }
        else
        {
            printf("0\n");
        }
        mysql_free_result(result);
        
    }
    else{
        ifstream out(argv[2]);
        string linestr;
        while(getline(out, linestr)){
            string str;
            stringstream id_passed(linestr);

            getline(id_passed, str, ' ');
            string temp1(str);

            getline(id_passwd, str, ' ');
            string temp2(str);
            users[temp1] = temp2;
        }
        out.close();

        string name(argv[0]);
        const char *namep = name.c_str();
        string passwd(argv[1]);
        const char *passwdp = passwd.c_str();

        if(users.find(name) != users.end() && useers[name] == passwd){
            printf("1\n");
        }
        else{
            printf("0\n");
        }
    }
}
