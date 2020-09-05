#ifndef DEF_DB_HPP__
#define DEF_DB_HPP__

#include <sqlite3.h>

#include "common.hpp"
#include "config.hpp"

#define ENV_TABLE_NAME      "%table_name%"
#define ENV_COLUMN_LIST     "%column_list%"
#define ENV_VALUE_LIST      "%value_list%"
#define ENV_SET_LIST        "%set_list%"
#define ENV_WHERE_LIST      "%where_list%"

static constexpr size_t  SIZE_OF_TABLE_NAME     = sizeof( ENV_TABLE_NAME ) - 1;
static constexpr size_t  SIZE_OF_COLUMN_LIST    = sizeof( ENV_COLUMN_LIST ) - 1;
static constexpr size_t  SIZE_OF_VALUE_LIST     = sizeof( ENV_VALUE_LIST ) - 1;
static constexpr size_t  SIZE_OF_SET_LIST       = sizeof( ENV_SET_LIST ) - 1;
static constexpr size_t  SIZE_OF_WHERE_LIST     = sizeof( ENV_WHERE_LIST ) - 1;

#define FORMAT_TABLE_NAME( target, strSrc )     target.replace( target.find( ENV_TABLE_NAME ), SIZE_OF_TABLE_NAME, strSrc )
#define FORMAT_COLUMN_LIST( target, strSrc )    target.replace( target.find( ENV_COLUMN_LIST ), SIZE_OF_COLUMN_LIST, strSrc )
#define FORMAT_VALUE_LIST( target, strSrc )     target.replace( target.find( ENV_VALUE_LIST ), SIZE_OF_VALUE_LIST, strSrc )
#define FORMAT_SET_LIST( target, strSrc )       target.replace( target.find( ENV_SET_LIST ), SIZE_OF_SET_LIST, strSrc )
#define FORMAT_WHERE_LIST( target, strSrc )     target.replace( target.find( ENV_WHERE_LIST ), SIZE_OF_WHERE_LIST, strSrc )

namespace HeartBeat
{
    class IDBInterface
    {
    public:
        enum ENUM_VALUE_TYPE
        {
            VT_INT,
            VT_INT64,
            VT_TEXT,
        };

        struct tagValue {
            ENUM_VALUE_TYPE type;

            union 
            {
                int             iValue;
                int64_t         i64Value;
                ::std::string   strValue;
            }; 

            explicit tagValue()
                : type( VT_INT )
                , iValue( -1 )
            {
                ;
            }

            explicit tagValue( int value )
                : type( VT_INT )
                , iValue( value )
            {
                ;
            }

            explicit tagValue( int64_t value )
                : type( VT_INT64 )
                , i64Value( value )
            {
                ;
            }

            explicit tagValue( const ::std::string& value )
                : type( VT_TEXT )
                , strValue( value )
            {
                ;
            }

            tagValue( const tagValue& value )
                : type( VT_TEXT )
                , strValue( "" )
            {
                type = value.type;

                switch(type) {
                case VT_INT:
                    iValue = value.iValue;
                    break;
                case VT_INT64:
                    i64Value = value.i64Value;
                    break;
                case VT_TEXT:
                    strValue = value.strValue;
                    break;  
                }
            }

            tagValue( const tagValue&& value )
            {
                ::std::cout << "tagValue move costruct" << ::std::endl;
                
                type = value.type;

                switch(type) {
                case VT_INT:
                ::std::cout << "tagValue move costruct1" << ::std::endl;
                    iValue = value.iValue;
                    break;
                case VT_INT64:
                ::std::cout << "tagValue move costruct2" << ::std::endl;
                    i64Value = value.i64Value;
                    break;
                case VT_TEXT:
                ::std::cout << "tagValue move costruct3" << ::std::endl;
                    strValue = value.strValue;
                    break;  
                }
            }

            ~tagValue()
            {
                ;
            }
        };

    protected:
        static const ::std::string s_strSQLCreate;
        static const ::std::string s_strSQLInsert;
        static const ::std::string s_strSQLUpdate;
        static const ::std::string s_strSQLDelete;
        static const ::std::string s_strSQLRead;

    public:
        IDBInterface()
        {
            ;
        }

        virtual ~IDBInterface()
        {
            ;
        }

        virtual bool initialize() = 0;
        virtual void uninitialize() = 0;

        virtual bool dbiCreate( const ::std::vector<::std::string>& aryArgs ) = 0;
        virtual bool dbiInsert( int key, const ::std::vector<tagValue>& aryArgs ) = 0;
        virtual bool dbiUpdate( int key, const ::std::vector<tagValue>& aryArgs ) = 0;
        virtual bool dbiDelete( int key, const ::std::vector<tagValue>& aryArgs ) = 0;
        virtual bool dbiRead( int key, const ::std::vector<tagValue>& aryArgs, ::std::vector<sqlite3_value *>& result, int& iCol, int& iRow ) = 0;  
    };

    class CSqlLiteDB : public IDBInterface
    {
    public:

    public:
        explicit CSqlLiteDB( ::std::string strDBPath )
            : m_bInitialized( false )
            , m_pDB( NULL )
        {
            if ( SQLITE_OK != sqlite3_open_v2( strDBPath.c_str(), &m_pDB, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL ) ) {
                // open db error

                return;
            }

            //initialize();
        }

        virtual ~CSqlLiteDB()
        {
            uninitialize();

            ::std::cout << "close db" << ::std::endl;

            sqlite3_close( m_pDB );  
        }

        bool initialize()
        {
            //  Optional, but will most likely increase performance.
            sqlite3_exec( m_pDB, "BEGIN TRANSACTION", 0, 0, 0 );

            m_bInitialized = true;
            
            return true;
        }

        bool initialize( ::std::string strTableName, \
                            const ::std::vector<::std::string>& columnList = {} )
        {
            if ( m_bInitialized )   return true;
            if ( strTableName.empty() || 0 == columnList.size() ) return false;

            ::std::vector<::std::string> aryArgs;
            
            aryArgs.clear();
            aryArgs.emplace_back( strTableName );

            ::std::string strColumn = columnList[0];
            for ( auto it = columnList.begin() + 1; it != columnList.end(); ++it ) {
                strColumn = strColumn + "," + (*it);
            }

            aryArgs.emplace_back( strColumn );

            if ( !sqlCreate( aryArgs ) ) {
                ::std::cout << "sqlCreate error" << ::std::endl;

                return false;
            }

            //  Optional, but will most likely increase performance.
            sqlite3_exec( m_pDB, "BEGIN TRANSACTION", 0, 0, 0 );

            m_bInitialized = true;

            return true;
        }

        void uninitialize()
        {
            if ( !m_bInitialized )  return;

            for ( auto it = m_aryInsertStmt.begin(); it != m_aryInsertStmt.end(); ++it ) {
                sqlite3_finalize( *it );
            }
            m_aryInsertStmt.clear();

            for ( auto it = m_aryUpdateStmt.begin(); it != m_aryUpdateStmt.end(); ++it ) {
                sqlite3_finalize( *it );
            }
            m_aryUpdateStmt.clear();

            for ( auto it = m_aryDeleteStmt.begin(); it != m_aryDeleteStmt.end(); ++it ) {
                sqlite3_finalize( *it );
            }
            m_aryDeleteStmt.clear();

            for ( auto it = m_aryReadStmt.begin(); it != m_aryReadStmt.end(); ++it ) {
                sqlite3_finalize( *it );
            }
            m_aryReadStmt.clear();

            m_bInitialized = false;
        }

        virtual bool dbiCreate( const ::std::vector<::std::string>& aryArgs )
        {
            if ( !m_bInitialized )  return false;

            return sqlCreate( aryArgs );
        }

        virtual bool dbiInsert( int key, const ::std::vector<tagValue>& aryArgs )
        {
            if ( !m_bInitialized || m_aryInsertStmt.size() <= key )  return false;

            return sqlInsert( m_aryInsertStmt[key], aryArgs );
        }

        virtual bool dbiUpdate( int key, const ::std::vector<tagValue>& aryArgs )
        {
            if ( !m_bInitialized || m_aryUpdateStmt.size() <= key )  return false;

            return sqlUpdate( m_aryUpdateStmt[key], aryArgs );
        }

        virtual bool dbiDelete( int key, const ::std::vector<tagValue>& aryArgs )
        {
            if ( !m_bInitialized || m_aryDeleteStmt.size() <= key )  return false;

            return sqlDelete( m_aryDeleteStmt[key], aryArgs );
        }

        virtual bool dbiRead( int key, const ::std::vector<tagValue>& aryArgs, ::std::vector<sqlite3_value *>& result, int& iCol, int& iRow )
        {
            if ( !m_bInitialized || m_aryReadStmt.size() <= key )  return false;

            return sqlRead( m_aryReadStmt[key], aryArgs, result, iCol, iRow );
        }

        int registerInsertStmt( ::std::string strTableName, \
                                    const ::std::vector<::std::string>& columnList = {}, \
                                    const ::std::vector<::std::string>& valueList = {} )
        {
            if ( strTableName.empty() || 0 == columnList.size() ) return -1;

            ::std::string strColumnList = columnList[0];
            ::std::string strValueList = "?";
            for ( auto it = columnList.begin() + 1; it != columnList.end(); ++it ) {
                strColumnList = strColumnList + "," + (*it);
                strValueList = strValueList + ",?";
            }

            ::std::string strSqlCMD = s_strSQLInsert;
            FORMAT_TABLE_NAME( strSqlCMD, strTableName );
            FORMAT_COLUMN_LIST( strSqlCMD, strColumnList );
            FORMAT_VALUE_LIST( strSqlCMD, strValueList );

            sqlite3_stmt* stmt = NULL;
            assert( SQLITE_OK == sqlite3_prepare_v2( m_pDB, strSqlCMD.c_str(), -1, &stmt, NULL ) );
            m_aryInsertStmt.emplace_back( stmt );

            return ( m_aryInsertStmt.size() - 1 );
        }

        int registerUpdateStmt( ::std::string strTableName, \
                                    const ::std::vector<::std::string>& setList = {}, \
                                    const ::std::vector<::std::string>& whereList = {} )
        {
            if ( strTableName.empty() || 0 == setList.size() || 0 == whereList.size() ) return -1;

            ::std::string strSetList = setList[0] + " = ?";
            for ( auto it = setList.begin() + 1; it != setList.end(); ++it ) {
                strSetList = strSetList + ", " + (*it) + " = ?";
            }

            ::std::string strWhereList = whereList[0] + " = ?";
            for ( auto it = whereList.begin() + 1; it != whereList.end(); ++it ) {
                strWhereList = strWhereList + " AND " + (*it) + " = ?";
            }

            ::std::string strSqlCMD = s_strSQLUpdate;
            FORMAT_TABLE_NAME( strSqlCMD, strTableName );
            FORMAT_SET_LIST( strSqlCMD, strSetList );
            FORMAT_WHERE_LIST( strSqlCMD, strWhereList );
            
            sqlite3_stmt* stmt = NULL;        
            assert( SQLITE_OK == sqlite3_prepare_v2( m_pDB, strSqlCMD.c_str(), -1, &stmt, NULL ) );
            m_aryUpdateStmt.emplace_back( stmt );

            return ( m_aryUpdateStmt.size() - 1 );
        }

        int registerDeleteStmt( ::std::string strTableName, \
                                    const ::std::vector<::std::string>& whereList = {} )
        {
            if ( strTableName.empty() || 0 == whereList.size() ) return -1;

            ::std::string strWhereList  = whereList[0] + "=?";
            for ( auto it = whereList.begin() + 1; it != whereList.end(); ++it ) {
                strWhereList = strWhereList + " AND " + (*it) + "=?";   
            }

            ::std::string strSqlCMD = s_strSQLDelete;
            FORMAT_TABLE_NAME( strSqlCMD, strTableName );
            FORMAT_WHERE_LIST( strSqlCMD, strWhereList );

            sqlite3_stmt* stmt = NULL;
            assert( SQLITE_OK == sqlite3_prepare_v2( m_pDB, strSqlCMD.c_str(), -1, &stmt, NULL ) );
            m_aryDeleteStmt.emplace_back( stmt );

            return ( m_aryDeleteStmt.size() - 1 );
        }

        int registerReadStmt( ::std::string strTableName, \
                                    const ::std::vector<::std::string>& columnList = {}, \
                                    const ::std::vector<::std::string>& whereList = {} )
        {
            if ( strTableName.empty() || 0 == columnList.size() || 0 == whereList.size() ) return -1;

            ::std::string strColumnList    = columnList[0];
            for ( auto it = columnList.begin() + 1; it != columnList.end(); ++it ) {
                strColumnList = strColumnList + "," + (*it);    
            }

            ::std::string strWhereList  = whereList[0] + "=?";
            for ( auto it = whereList.begin() + 1; it != whereList.end(); ++it ) {
                strWhereList = strWhereList + " AND " + (*it) + "=?";    
            }

            ::std::string strSqlCMD = s_strSQLRead;
            FORMAT_TABLE_NAME( strSqlCMD, strTableName );
            FORMAT_COLUMN_LIST( strSqlCMD, strColumnList );
            FORMAT_WHERE_LIST( strSqlCMD, strWhereList );

            sqlite3_stmt* stmt = NULL;
            assert( SQLITE_OK == sqlite3_prepare_v2( m_pDB, strSqlCMD.c_str(), -1, &stmt, NULL ) );
            m_aryReadStmt.emplace_back( stmt );

            return ( m_aryReadStmt.size() - 1 );
        }

    protected:
        int execute( const ::std::string& strSqlCMD )
        {
            if ( !m_pDB || strSqlCMD.empty() )  return false;

            int ret = 0;
            sqlite3_stmt* stmtCustom = NULL;

            assert( SQLITE_OK == sqlite3_prepare_v2( m_pDB, strSqlCMD.c_str(), -1, &stmtCustom, NULL ) );
            ret = sqlite3_step( stmtCustom );
            assert( SQLITE_OK == sqlite3_finalize( stmtCustom ) );

            return ret;
        }

        int execute( const ::std::string& strSqlCMD, ::std::vector<sqlite3_value *>& result, int& iCol, int& iRow  )
        {
            return 0;
        }
    
    private:
        bool checkTableExist( const ::std::string& strTableName )
        {
            ::std::string strSqlCMD = "SELECT name FROM sqlite_master WHERE type='table' AND name='" + strTableName + "'";

            return ( SQLITE_ROW == execute( strSqlCMD ) );
        }

        bool sqlCreate( const ::std::vector<::std::string>& aryArgs )
        {
            if ( aryArgs.size() < 2 ) return false;
            if ( checkTableExist( aryArgs[0] ) ) return true;

            // format string
            ::std::string strSqlCMD = s_strSQLCreate;
            FORMAT_TABLE_NAME( strSqlCMD, aryArgs[0] );
            FORMAT_COLUMN_LIST( strSqlCMD, aryArgs[1] );

            return ( SQLITE_DONE ==  execute( strSqlCMD ) );
        }

        bool sqlInsert( sqlite3_stmt* stmt, const ::std::vector<tagValue>& aryArgs )
        {
            if ( !stmt || 0 == aryArgs.size() ) return false;

            // bind value
            for ( int i=0; i < aryArgs.size(); ++i ) {
                switch ( aryArgs[i].type )
                {
                case VT_INT:
                    sqlite3_bind_int( stmt, i + 1, aryArgs[i].iValue );
                    break;
                case VT_INT64:
                    sqlite3_bind_int64( stmt, i + 1, aryArgs[i].i64Value );
                    break;
                case VT_TEXT:
                    sqlite3_bind_text( stmt, i + 1, aryArgs[i].strValue.c_str(), -1, SQLITE_TRANSIENT );
                    break;
                }      
            }

            // evaluate
            if ( SQLITE_DONE != sqlite3_step( stmt ) ) {
                // error
                uninitialize();
                sqlite3_close( m_pDB );

                return false;
            }

            // unbind
            sqlite3_clear_bindings( stmt );

            // reset
            sqlite3_reset( stmt );

            return true;
        }

        bool sqlUpdate( sqlite3_stmt* stmt, const ::std::vector<tagValue>& aryArgs )
        {
            if ( !stmt || 0 == aryArgs.size() ) return false;

            // bind value
            for ( int i=0; i < aryArgs.size(); ++i ) {
                switch ( aryArgs[i].type )
                {
                case VT_INT:
                    sqlite3_bind_int( stmt, i + 1, aryArgs[i].iValue );
                    break;
                case VT_INT64:
                    sqlite3_bind_int64( stmt, i + 1, aryArgs[i].i64Value );
                    break;
                case VT_TEXT:
                    sqlite3_bind_text( stmt, i + 1, aryArgs[i].strValue.c_str(), -1, SQLITE_TRANSIENT );
                    break;
                }  
            }

            // evaluate
            if ( SQLITE_DONE != sqlite3_step( stmt ) ) {
                // error
                uninitialize();
                sqlite3_close( m_pDB );

                return false;
            }

            // unbind
            sqlite3_clear_bindings( stmt );

            // reset
            sqlite3_reset( stmt );

            return true;
        }

        bool sqlDelete( sqlite3_stmt* stmt, const ::std::vector<tagValue>& aryArgs )
        {
            if ( !stmt || 0 == aryArgs.size() ) return false;

            // bind value
            for ( int i=0; i < aryArgs.size(); ++i ) {
                switch ( aryArgs[i].type )
                {
                case VT_INT:
                    sqlite3_bind_int( stmt, i + 1, aryArgs[i].iValue );
                    break;
                case VT_INT64:
                    sqlite3_bind_int64( stmt, i + 1, aryArgs[i].i64Value );
                    break;
                case VT_TEXT:
                    sqlite3_bind_text( stmt, i + 1, aryArgs[i].strValue.c_str(), -1, SQLITE_TRANSIENT );
                    break;
                }
            }

            // evaluate
            if ( SQLITE_DONE != sqlite3_step( stmt ) ) {
                // error
                uninitialize();
                sqlite3_close( m_pDB );

                return false;
            }

            // unbind
            sqlite3_clear_bindings( stmt );

            // reset
            sqlite3_reset( stmt );

            return true;
        }

        bool sqlRead( sqlite3_stmt* stmt, const ::std::vector<tagValue>& aryArgs, ::std::vector<sqlite3_value *>& result, int& iCol, int& iRow )
        {
            if ( !stmt || 0 == aryArgs.size() ) return false;

            // bind value
            for ( int i=0; i < aryArgs.size(); ++i ) {
                switch ( aryArgs[i].type )
                {
                case VT_INT:
                    sqlite3_bind_int( stmt, i + 1, aryArgs[i].iValue );
                    break;
                case VT_INT64:
                    sqlite3_bind_int64( stmt, i + 1, aryArgs[i].i64Value );
                    break;
                case VT_TEXT:
                    sqlite3_bind_text( stmt, i + 1, aryArgs[i].strValue.c_str(), -1, SQLITE_TRANSIENT );
                    break;
                }
            }

            // evaluate
            int ret     = 0;
            bool bLoop  = true;
            iCol = 0;
            iRow = 0;
            while ( bLoop && ( SQLITE_DONE != ( ret = sqlite3_step( stmt ) ) ) ) {
                switch ( ret ) 
                {
                case SQLITE_BUSY:
                    // try again
                    break;
                case SQLITE_ROW:
                    //result
                    iCol = sqlite3_column_count( stmt );
                    for ( int i = 0; i < iCol; ++i ) {
                        result.push_back( sqlite3_column_value( stmt, i ) );
                    }
                    ++iRow;
                    break;
                case SQLITE_ERROR:
                    result.clear();
                    iCol = 0;
                    iRow = 0;
                    ::std::cout << "sqlite3_step error " << sqlite3_errmsg( m_pDB ) << ::std::endl;
                    bLoop = false;
                    break;
                case SQLITE_MISUSE:
                    result.clear();
                    iCol = 0;
                    iRow = 0;
                    ::std::cout << "sqlite3_step error with misuse" << ::std::endl;
                    bLoop = false;
                    break;
                }
            }

            // unbind
            sqlite3_clear_bindings( stmt );

            // reset
            sqlite3_reset( stmt );

            return bLoop;
        }
  
    private:
        bool                                        m_bInitialized;

        sqlite3*                                    m_pDB;

        ::std::vector< sqlite3_stmt* >              m_aryInsertStmt;
        ::std::vector< sqlite3_stmt* >              m_aryUpdateStmt;
        ::std::vector< sqlite3_stmt* >              m_aryDeleteStmt;
        ::std::vector< sqlite3_stmt* >              m_aryReadStmt;
    };
}
#endif