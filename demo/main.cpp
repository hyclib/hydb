#include "../include/hydb.h"
using namespace hydb;

#include <string>
#include <iostream>

int main()
{
    //IDataBase* pDB = ConnectDB(DataBaseType_Mysql, "citongs.com", 3306, "app_housekeep_op", "vVUCt2pziWJ1uQfI", "app_mango", "utf8");

    //std::string ss;
    //while(true) {

    //    std::cin>>ss;
    //    if (ss == "sel")
    //    {

    //        IDataSet* pSet;
    //        if ((pSet=pDB->ExecQuery("select * from Users")) != NULL)
    //        {
    //            printf("num %d %s\r\n", (int)pSet->GetColNum(), __TIME__);
    //            pSet->Release();
    //        }
    //        else
    //        {
    //            printf("err\r\n");
    //        }
    //    }
    //}

    hydb::IDataBase* db = hydb::ConnectDB(hydb::DataBaseType_Sqlite, 0, 0, NULL, NULL, "1.db");
    u32 tick = 1;
    if (NULL != db)
    {
        hydb::IDataSet* pSet;

        // device
        char* sql = _T("Select * from device");
        pSet = db->ExecQuery(sql);
        do {
            const char* pp = pSet->GetValue(_T("device_name"));
            int i = 0;
        } while (pSet->Read());
        pSet->Release();
    }

    return 0;
}
