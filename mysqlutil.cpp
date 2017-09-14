#include "mysqlutil.h"
#include <cstring>
#include <string>

using namespace std;

MysqlUtil::MysqlUtil()
{
    memset(&m_hMySql, 0, sizeof(MYSQL));
    m_result = NULL;
    m_bIsOpen = false;
}

MysqlUtil::~MysqlUtil()
{
}

bool MysqlUtil::ConnectDB(const char * ip, unsigned port, const char * username, const char * passwd, const char * dbname)
{
	if (m_bIsOpen)
	{
		return true;
	}

	if (!mysql_init(&m_hMySql))      //初始化数据库连接对象
	{
		return false;
	}

	//实时连接
	if (!mysql_real_connect(&m_hMySql, ip, username, passwd, dbname, port, NULL, 0 | CLIENT_MULTI_STATEMENTS | CLIENT_MULTI_RESULTS))
	{
        errmsg = string("connect to db server error. reason=") + mysql_error(&m_hMySql);
		return false;  //连接失败
	}						
	else
	{
		m_bIsOpen = true;  //连接成功

		//设置连接编码
		(void)mysql_set_character_set(&m_hMySql,"utf8");
	}

    return true;
}

void MysqlUtil::CloseDB()
{
	mysql_close(&m_hMySql);

	m_bIsOpen = false;
}

int MysqlUtil::ExecSql(const char * sql)
{
	if (!m_bIsOpen)
	{
		errmsg = "didn't connect to db server";
		return connect_db_error;
	}

	if (NULL == sql)
	{
		errmsg = "exec sql cannot be empty";
		return sql_param_error;
	}

	int n = 0;

	try
	{
		n = strlen(sql);
	}
	catch (...)
	{
		errmsg = "get sql length error";
		return sql_param_error;
	}

	if (0 == n)
	{
		errmsg = "sql length can not be zero";
		return sql_param_error;
	}

	const char *pDst = sql;
	int i;

	for (i = 0; i < n;i++, pDst++)
	{
		if (*pDst != ' ')
			break;
	}

	n -= i;

	int nRet = mysql_real_query(&m_hMySql, pDst, n);

	if (0 != nRet)
	{
		errmsg = string("Error:") + mysql_error(&m_hMySql);
		return exec_error;
	}

	m_result = mysql_store_result(&m_hMySql);

	if (mysql_field_count(&m_hMySql) > 0 && NULL == m_result)
	{
		errmsg = string("Error:") + mysql_error(&m_hMySql);
		return store_result_error;
	}

	return 0;
}

int MysqlUtil::FetchRows(std::vector<std::vector<std::string> > & rows)
{
	rows.clear();

	int num_fields = mysql_field_count(&m_hMySql);

	if (num_fields == 0)
	{
		//上次执行应返回空结果集
		return 0;
	}

	//正常返回结果
	int i = 0;

	while (NULL != (m_row = mysql_fetch_row(m_result)))
	{
		//输出结果集中的每个字段
        rows.resize(i+1);

		for (int j = 0; j < num_fields; ++j)
		{
            //处理NULL的情况
            if (NULL == m_row[j])
            {
                rows[i].push_back("NULL");
            }
            else
            {
			    rows[i].push_back(m_row[j]);
            }
		}

		++i;
	}

	mysql_free_result(m_result);

	return 0;
}
