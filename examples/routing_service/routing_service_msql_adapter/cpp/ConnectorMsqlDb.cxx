/*
 * (c) 2020 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 *
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the Software.  Licensee has the right to distribute object form
 * only for use with RTI products.  The Software is provided "as is", with no
 * warranty of any type, including any warranty for fitness for any purpose.
 * RTI is under no obligation to maintain or support the Software.  RTI shall
 * not be liable for any incidental or consequential damages arising out of the
 * use or inability to use the software.
 */

#include "ConnectorMsqlDb.hpp"
#include "RsLog.hpp"
#include <sqlext.h>

#define RTI_RS_LOG_ARGS "ConnectorMsqlDb"
#define SQL_BUFF_LEN 1024
#define TIMEOUT 5

using namespace rti::routing;
using namespace dds::core::xtypes;

ConnectorMsqlDb::ConnectorMsqlDb(
        const std::string &topic_name,
        const PropertySet &properties)
        : ConnectorBase(topic_name)
{
    set_connection_info(properties);
    connect();
}

ConnectorMsqlDb::~ConnectorMsqlDb()
{
    disconnect();
}

/*
 * Connect to the MSQL database.
 * Return true if connection succeeds, otherwise false
 */
auto ConnectorMsqlDb::connect() -> bool
{
    RTI_RS_LOG_FN(connect_db);

    SQLRETURN ret_code;

    /* Allocates the environment handle */
    ret_code = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &sql_env_handle);
    if (ret_code != SQL_SUCCESS && ret_code != SQL_SUCCESS_WITH_INFO) {
        disconnect();
        throw dds::core::Error(
                "SQL Error - Allocating the environment handle failed");
    }

    /* Set the ODBC version environment attribute */
    ret_code = SQLSetEnvAttr(
            sql_env_handle,
            SQL_ATTR_ODBC_VERSION,
            (SQLPOINTER) SQL_OV_ODBC3,
            0);
    if (ret_code != SQL_SUCCESS && ret_code != SQL_SUCCESS_WITH_INFO) {
        disconnect();
        throw dds::core::Error(
                "SQL Error - Setting the ODBC version environment attribute");
    }

    /* Allocates the connection handle */
    ret_code = SQLAllocHandle(SQL_HANDLE_DBC, sql_env_handle, &sql_conn_handle);
    if (ret_code != SQL_SUCCESS && ret_code != SQL_SUCCESS_WITH_INFO) {
        disconnect();
        throw dds::core::Error(
                "SQL Error - Allocating SQL database connection failed");
    }

    /* Sets login timeout to 5 seconds */
    ret_code = SQLSetConnectAttr(
            sql_conn_handle,
            SQL_LOGIN_TIMEOUT,
            (SQLPOINTER) TIMEOUT,
            0);
    if (ret_code != SQL_SUCCESS && ret_code != SQL_SUCCESS_WITH_INFO) {
        disconnect();
        throw dds::core::Error("SQL Error - Setting connection timeout failed");
    }

    SQLCHAR retconstring[SQL_BUFF_LEN];
    /* connect to SQL Server */
    ret_code = SQLDriverConnect(
            sql_conn_handle,
            NULL,
            (SQLCHAR *) connection_info.data(),
            SQL_NTS,
            retconstring,
            SQL_BUFF_LEN,
            NULL,
            SQL_DRIVER_NOPROMPT);

    if (ret_code == SQL_ERROR) {
        msql_show_error(SQL_HANDLE_DBC, sql_conn_handle);
        disconnect();
        throw dds::core::Error("SQL Error - Connection to SQL Server failed");
    }

    RTI_RS_LOG(connection_info.data());
    RTI_RS_LOG("SQL connection to database successful");
    connected_ = true;

    ret_code =
            SQLAllocHandle(SQL_HANDLE_STMT, sql_conn_handle, &sql_stmt_handle);

    if (ret_code != SQL_SUCCESS && ret_code != SQL_SUCCESS_WITH_INFO) {
        disconnect();
        throw dds::core::Error(
                "SQL Error - Allocating the statement handle failed");
    }
    return connected_;
}

/*
 * Free up resources and set connected_ to false
 */
void ConnectorMsqlDb::disconnect()
{
    RTI_RS_LOG_FN(disconnect_db);

    SQLFreeHandle(SQL_HANDLE_STMT, sql_stmt_handle);
    SQLDisconnect(sql_conn_handle);
    SQLFreeHandle(SQL_HANDLE_DBC, sql_conn_handle);
    SQLFreeHandle(SQL_HANDLE_ENV, sql_env_handle);

    connected_ = false;
}

/*
 * Set connection information and form connection string
 */
void ConnectorMsqlDb::set_connection_info(const PropertySet &properties)
{
    RTI_RS_LOG_FN(set_connection_info);

    auto it = properties.find("msql.db.servername");
    if (it != properties.end()) {
        server = it->second;
    } else {
        throw dds::core::Error("Server name not found in properties");
    }

    it = properties.find("msql.db.dbname");
    if (it != properties.end()) {
        db_name = it->second;
    } else {
        throw dds::core::Error("Database name not found in properties");
    }

    it = properties.find("msql.db.tablename");
    if (it != properties.end()) {
        table_name = it->second;
    } else {
        throw dds::core::Error("Table name not found in properties");
    }

    it = properties.find("msql.db.username");
    if (it != properties.end()) {
        user_name = it->second;
    } else {
        throw dds::core::Error("Username name not found in properties");
    }

    it = properties.find("msql.db.password");
    if (it != properties.end()) {
        password = it->second;
    } else {
        throw dds::core::Error("Password name not found in properties");
    }

    connection_info = "DRIVER={SQL Server}; SERVER=" + server + "; DATABASE="
            + db_name + "; UID=" + user_name + "; PWD=" + password + ";";
}

/*
 * Utility method to provide MSQL error information
 */
void ConnectorMsqlDb::msql_show_error(
        unsigned int handle_type,
        const SQLHANDLE &handle)
{
    SQLCHAR sql_state[SQL_BUFF_LEN];
    SQLCHAR message[SQL_BUFF_LEN];

    if (SQL_SUCCESS
        == SQLGetDiagRec(
                handle_type,
                handle,
                1,
                sql_state,
                NULL,
                message,
                SQL_BUFF_LEN,
                NULL)) {
        std::cout << "Message: " << message << "\nSQLSTATE: " << sql_state
                  << std::endl;
    }
}

/*
 * Write the data to the database
 * return true on success, otherwise failure
 */
auto ConnectorMsqlDb::write_data(const DynamicData &sample) -> bool
{
    bool success = true;

    /* write to end position */
    std::ostringstream cmd(std::ostringstream::ate);

    /* write the data to the database*/
    cmd << "INSERT INTO " << db_name << ".dbo." << table_name
        << "(topicname, color, x, y, shapesize, angle, fillkind) VALUES ('"
        << topic_name_ << "','" << sample.value<std::string>("color") << "',"
        << sample.value<int32_t>("x") << "," << sample.value<int32_t>("y")
        << "," << sample.value<int32_t>("shapesize") << ","
        << sample.value<float>("angle") << ","
        << sample.value<int32_t>("fillKind") << ")";

    /* write cmd to stdout */
    /* std::cout << cmd.str(); */

    SQLRETURN retcode = SQLExecDirect(
            sql_stmt_handle,
            (SQLCHAR *) cmd.str().c_str(),
            SQL_NTS);
    if (retcode == SQL_ERROR) {
        msql_show_error(SQL_HANDLE_STMT, sql_stmt_handle);
        success = false;
    }

    return success;
}

/*
 * returns the number of records read from the database
 */
auto ConnectorMsqlDb::read_data(std::unique_ptr<DynamicData> &sample)
        -> unsigned int
{
    unsigned int count = 0; /* number of records read */

    /* write to end position */
    std::ostringstream cmd(std::ostringstream::ate);

    /* get one record from the database at the offset specified using ascending
     * order */
    cmd << "SELECT * FROM " << table_name << " WHERE topicname = '"
        << topic_name_ << "' ORDER BY id ASC OFFSET " << offset_
        << " ROWS FETCH FIRST 1 ROWS ONLY";

    /* write cmd to stdout */
    /* std::cout << cmd.str() << std::endl; */

    if (SQL_SUCCESS
        != SQLExecDirect(
                sql_stmt_handle,
                (SQLCHAR *) cmd.str().c_str(),
                SQL_NTS)) {
        msql_show_error(SQL_HANDLE_STMT, sql_stmt_handle);
        return count;
    }

    while (SQLFetch(sql_stmt_handle) == SQL_SUCCESS) {
        /* start at column 3 since column 1 is an auto incremented id and column
         * 2 is the topicname which we already know */
        char color[128];
        int value = 0;
        float angle = 0.0;

        SQLGetData(sql_stmt_handle, 3, SQL_C_CHAR, &color, 128, NULL);
        sample->value("color", std::string(color));
        SQLGetData(
                sql_stmt_handle,
                4,
                SQL_C_DEFAULT,
                &value,
                sizeof(value),
                NULL);
        sample->value("x", value);
        SQLGetData(
                sql_stmt_handle,
                5,
                SQL_C_DEFAULT,
                &value,
                sizeof(value),
                NULL);
        sample->value("y", value);
        SQLGetData(
                sql_stmt_handle,
                6,
                SQL_C_DEFAULT,
                &value,
                sizeof(value),
                NULL);
        sample->value("shapesize", value);
        SQLGetData(
                sql_stmt_handle,
                7,
                SQL_C_DEFAULT,
                &angle,
                sizeof(angle),
                NULL);
        sample->value("angle", angle);
        SQLGetData(
                sql_stmt_handle,
                8,
                SQL_C_DEFAULT,
                &value,
                sizeof(value),
                NULL);
        sample->value("fillKind", value);

        count++;
        offset_++;
    }

    /* Close Cursor before next iteration starts */
    if (SQL_SUCCESS != SQLCloseCursor(sql_stmt_handle)) {
        msql_show_error(SQL_HANDLE_STMT, sql_stmt_handle);
        return count;
    }

    return count;
}
