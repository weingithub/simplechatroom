#ifndef MYSQL_UTIL_H
#define MYSQL_UTIL_H

#include <mysql.h>
#include <string>
#include <iostream>
#include <vector>

class MysqlUtil
{
public:
    enum
    {
		connect_db_error = 1,
		sql_param_error,
		exec_error = 1,
		store_result_error,
        max_len = 50,
    };

    MysqlUtil();

    ~MysqlUtil();

    bool ConnectDB(const char * ip, unsigned port, const char * username, const char * passwd, const char * dbname);

	void CloseDB();

	int ExecSql(const char * sql);

	const std::string & GetErrmsg() const {return errmsg;}

	int FetchRows(std::vector<std::vector<std::string> > & rows);
protected: 
    MYSQL m_hMySql;                        //数据库连接句柄
    bool  m_bIsOpen;                         //数据库打开标志   
    MYSQL_RES *m_result;                //sql语句操作结果集
    MYSQL_ROW m_row;                    //记录
    MYSQL_FIELD *m_filds;
    
	std::string errmsg;
};


#endif //MYSQL_UTIL_H
