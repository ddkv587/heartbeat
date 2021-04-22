#include "db.hpp"

namespace HeartBeat
{
    const ::std::string IDBInterface::s_strSQLCreate = "CREATE TABLE %table_name% ( %column_list% )";
    const ::std::string IDBInterface::s_strSQLInsert = "INSERT INTO %table_name% ( %column_list% ) VALUES( %value_list% );";
    const ::std::string IDBInterface::s_strSQLUpdate = "UPDATE %table_name% SET %set_list% WHERE %where_list%";
    const ::std::string IDBInterface::s_strSQLDelete = "DELETE FROM %table_name% WHERE %where_list%";
    const ::std::string IDBInterface::s_strSQLRead   = "SELECT %column_list% FROM %table_name% WHERE %where_list%";
}