#include "mysqlutil.h"
#include <vector>
#include <string>
#include <iostream>

using namespace std;

int main()
{
    MysqlUtil sqlutil;

    bool isok = sqlutil.ConnectDB("127.0.0.1", 3306, "root", "123456", "tool");

    if (!isok)
    {
        cerr<<"connect server error, "<<sqlutil.GetErrmsg()<<endl;
        return 1;
    }

    int ret = sqlutil.ExecSql(" select * from harrypotter limit 20;");

    if (ret)
    {
        cout<<sqlutil.GetErrmsg()<<endl;
        return ret;
    }

    vector<vector<string> > result;

    sqlutil.FetchRows(result);

    for(int i = 0; i < result.size(); ++i)
    {
        cout<<"第"<<i+1<<"行结果:"<<endl;

        for(int j = 0; j < result[i].size(); ++j)
        {
            cout<<result[i][j]<<",";   
        }

        cout<<endl;
    }

    return 0;
}
