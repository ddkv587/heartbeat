#ifndef DEF_DB_HPP
#define DEF_DB_HPP

#include <sqlite3.h>

#include "common.hpp"

namespace HeartBeat
{
    class IDBInterface
    {
    public:
        enum ESQLCommand
        {
            SQL_CREATE = 0,
            SQL_INSERT,
            SQL_UPDATE,
            SQL_DELETE,
            SQL_READ
        };
    };

    class CSqlLiteDB final
    {
    private:
        static const ::std::string s_strSQLCreate;
        static const ::std::string s_strSQLInsert;
        static const ::std::string s_strSQLUpdate;
        static const ::std::string s_strSQLDelete;
        static const ::std::string s_strSQLRead;

    public:
        bool initialize()
        {
            sqlite3_prepare_v2( m_pDB, s_strSQLCreate.c_str(), -1, &m_stmtCreate, NULL );
            sqlite3_prepare_v2( m_pDB, s_strSQLInsert.c_str(), -1, &m_stmtInsert, NULL );
            sqlite3_prepare_v2( m_pDB, s_strSQLUpdate.c_str(), -1, &m_stmtUpdate, NULL );
            sqlite3_prepare_v2( m_pDB, s_strSQLDelete.c_str(), -1, &m_stmtDelete, NULL );
            sqlite3_prepare_v2( m_pDB, s_strSQLRead.c_str(), -1, &m_stmtRead, NULL );

            //  Optional, but will most likely increase performance.
            sqlite3_exec( m_pDB, "BEGIN TRANSACTION", 0, 0, 0 );    
        }

        void uninitialize()
        {
            sqlite3_finalize( m_stmtCreate );
            sqlite3_finalize( m_stmtInsert );
            sqlite3_finalize( m_stmtUpdate );
            sqlite3_finalize( m_stmtDelete );
            sqlite3_finalize( m_stmtRead );
        }

    protected:
        CSqlLiteDB( ::std::string strDBPath )
        {
            if ( SQLITE_OK != sqlite3_open( strDBPath.c_str(), &m_pDB ) ) {
                // open db error

                return;
            }

            initialize();
        }

        ~CSqlLiteDB()
        {
            uninitialize();

            sqlite3_close( m_pDB );     
        }
    
    private:
        bool execute( IDBInterface::ESQLCommand cmd, const ::std::vector<::std::string>& aryArgs )
        {
            switch ( cmd )
            {
            case IDBInterface::SQL_CREATE:
                return sqlCreate( aryArgs );
            case IDBInterface::SQL_INSERT:
                return sqlInsert( aryArgs );
            case IDBInterface::SQL_UPDATE:
                return sqlUpdate( aryArgs );
            case IDBInterface::SQL_DELETE:
                return sqlDelete( aryArgs );
            case IDBInterface::SQL_READ:
                return false;
            }
        }

        bool execute( IDBInterface::ESQLCommand cmd, const ::std::vector<::std::string>& aryArgs, ::std::vector<sqlite3_value *>& result )
        {
            switch ( cmd )
            {
            case IDBInterface::SQL_CREATE:
            case IDBInterface::SQL_INSERT:
            case IDBInterface::SQL_UPDATE:
            case IDBInterface::SQL_DELETE:
                return execute( cmd, aryArgs );
            case IDBInterface::SQL_READ:
                return sqlRead( aryArgs, result );
            }
        }

        bool sqlCreate( const ::std::vector<::std::string>& aryArgs )
        {
            if ( aryArgs.size() < 2 ) return false;

            // bind
            sqlite3_bind_text( m_stmtCreate, 1, aryArgs[0].c_str(), aryArgs[0].size() + 1, nullptr );
            sqlite3_bind_text( m_stmtCreate, 2, aryArgs[1].c_str(), aryArgs[1].size() + 1, nullptr );

            // evaluate
            sqlite3_step( m_stmtCreate );

            // unbind
            sqlite3_clear_bindings( m_stmtCreate );

            // reset
            sqlite3_reset( m_stmtCreate );
        }

        bool sqlInsert( const ::std::vector<::std::string>& aryArgs )
        {
            if ( aryArgs.size() < 3 ) return false;

            // bind
            sqlite3_bind_text( m_stmtInsert, 1, aryArgs[0].c_str(), aryArgs[0].size() + 1, nullptr );
            sqlite3_bind_text( m_stmtInsert, 2, aryArgs[1].c_str(), aryArgs[1].size() + 1, nullptr );
            sqlite3_bind_text( m_stmtInsert, 2, aryArgs[2].c_str(), aryArgs[2].size() + 1, nullptr );

            // evaluate
            if ( SQLITE_DONE != sqlite3_step( m_stmtInsert ) ) {
                // error
                uninitialize();
                sqlite3_close( m_pDB );

                return false;
            }

            // unbind
            sqlite3_clear_bindings( m_stmtInsert );

            // reset
            sqlite3_reset( m_stmtInsert );
        }

        bool sqlUpdate( const ::std::vector<::std::string>& aryArgs )
        {
            if ( aryArgs.size() < 3 ) return false;

            // bind
            sqlite3_bind_text( m_stmtUpdate, 1, aryArgs[0].c_str(), aryArgs[0].size() + 1, nullptr );
            sqlite3_bind_text( m_stmtUpdate, 2, aryArgs[1].c_str(), aryArgs[1].size() + 1, nullptr );
            sqlite3_bind_text( m_stmtUpdate, 2, aryArgs[2].c_str(), aryArgs[2].size() + 1, nullptr );

            // evaluate
            if ( SQLITE_DONE != sqlite3_step( m_stmtUpdate ) ) {
                // error
                uninitialize();
                sqlite3_close( m_pDB );

                return false;
            }

            // unbind
            sqlite3_clear_bindings( m_stmtUpdate );

            // reset
            sqlite3_reset( m_stmtUpdate );
        }

        bool sqlDelete( const ::std::vector<::std::string>& aryArgs )
        {
            if ( aryArgs.size() < 2 ) return false;

            // bind
            sqlite3_bind_text( m_stmtDelete, 1, aryArgs[0].c_str(), aryArgs[0].size() + 1, nullptr );
            sqlite3_bind_text( m_stmtDelete, 2, aryArgs[1].c_str(), aryArgs[1].size() + 1, nullptr );

            // evaluate
            if ( SQLITE_DONE != sqlite3_step( m_stmtDelete ) ) {
                // error
                uninitialize();
                sqlite3_close( m_pDB );

                return false;
            }

            // unbind
            sqlite3_clear_bindings( m_stmtDelete );

            // reset
            sqlite3_reset( m_stmtDelete );
        }

        bool sqlRead( const ::std::vector<::std::string>& aryArgs, ::std::vector<sqlite3_value *>& result )
        {
            if ( aryArgs.size() < 3 ) return false;

            // bind
            sqlite3_bind_text( m_stmtRead, 1, aryArgs[0].c_str(), aryArgs[0].size() + 1, nullptr );
            sqlite3_bind_text( m_stmtRead, 2, aryArgs[1].c_str(), aryArgs[1].size() + 1, nullptr );
            sqlite3_bind_text( m_stmtRead, 2, aryArgs[2].c_str(), aryArgs[2].size() + 1, nullptr );

            // evaluate
            int ret;
            while ( SQLITE_DONE == ( ret = sqlite3_step( m_stmtRead ) ) ) {
                //if ( ret !=  )
                ;
            }

            // unbind
            sqlite3_clear_bindings( m_stmtRead );

            // reset
            sqlite3_reset( m_stmtRead );
        }

        
    private:
        sqlite3*        m_pDB;

        sqlite3_stmt*   m_stmtCreate;
        sqlite3_stmt*   m_stmtInsert;
        sqlite3_stmt*   m_stmtUpdate;
        sqlite3_stmt*   m_stmtDelete;
        sqlite3_stmt*   m_stmtRead;
    };
}
#endif