#include "db.hpp"

namespace HeartBeat
{
    const ::std::string CSqlLiteDB::s_strSQLCreate = "CREATE TABLE ? ( ? )";
    const ::std::string CSqlLiteDB::s_strSQLInsert = "INSERT INTO ? ( ? ) VALUES( ? )";
    const ::std::string CSqlLiteDB::s_strSQLUpdate = "UPDATE ? SET ? WHERE ?";
    const ::std::string CSqlLiteDB::s_strSQLDelete = "DELETE FROM ? WHERE ?";
    const ::std::string CSqlLiteDB::s_strSQLRead   = "SELECT ? FROM ? WHERE ?";
}