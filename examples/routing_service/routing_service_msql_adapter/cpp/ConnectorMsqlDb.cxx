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
        const std::string topicName,
        const PropertySet &properties)
        : ConnectorBase(topicName)
{
    setConnectionInfo(properties);
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

    SQLRETURN retcode;

    /* Allocates the environment handle */
    retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &sqlEnvHandle);
    if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
        RTI_RS_ERROR("SQL Error - Allocating the environment handle failed");
        disconnect();
        return connected_;
    }

    /* Set the ODBC version environment attribute */
    retcode = SQLSetEnvAttr(
            sqlEnvHandle,
            SQL_ATTR_ODBC_VERSION,
            (SQLPOINTER) SQL_OV_ODBC3,
            0);
    if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
        RTI_RS_ERROR(
                "SQL Error - Setting the ODBC version environment attribute");
        disconnect();
        return connected_;
    }

    /* Allocates the connection handle */
    retcode = SQLAllocHandle(SQL_HANDLE_DBC, sqlEnvHandle, &sqlConnHandle);
    if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
        RTI_RS_ERROR("SQL Error - Allocating SQL database connection failed");
        disconnect();
        return connected_;
    }

    /* Sets login timeout to 5 seconds */
    retcode = SQLSetConnectAttr(
            sqlConnHandle,
            SQL_LOGIN_TIMEOUT,
            (SQLPOINTER) TIMEOUT,
            0);
    if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
        RTI_RS_ERROR("SQL Error - Setting connection timeout failed");
        disconnect();
        return connected_;
    }

    SQLCHAR retconstring[SQL_BUFF_LEN];
    /* connect to SQL Server */
    retcode = SQLDriverConnect(
            sqlConnHandle,
            NULL,
            (SQLCHAR *) connectionInfo.data(),
            SQL_NTS,
            retconstring,
            SQL_BUFF_LEN,
            NULL,
            SQL_DRIVER_NOPROMPT);

    if (retcode == SQL_ERROR) {
        msqlShowError(SQL_HANDLE_DBC, sqlConnHandle);
        disconnect();
        return connected_;
    } else {
        RTI_RS_LOG(connectionInfo.data());
        RTI_RS_LOG("SQL connection to database successful");
        connected_ = true;
    }

    retcode = SQLAllocHandle(SQL_HANDLE_STMT, sqlConnHandle, &sqlStmtHandle);

    if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
        RTI_RS_ERROR("SQL Error - Allocating the statement handle failed");
        disconnect();
        return connected_;
    }
    return connected_;
}

/*
 * Free up resources and set connected_ to false
 */
void ConnectorMsqlDb::disconnect()
{
    RTI_RS_LOG_FN(disconnect_db);

    SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
    SQLDisconnect(sqlConnHandle);
    SQLFreeHandle(SQL_HANDLE_DBC, sqlConnHandle);
    SQLFreeHandle(SQL_HANDLE_ENV, sqlEnvHandle);

    connected_ = false;
}

/*
 * Set connection information and form connection string
 */
void ConnectorMsqlDb::setConnectionInfo(const PropertySet &properties)
{
    RTI_RS_LOG_FN(setConnectionInfo);

    auto it = properties.find("msql.db.servername");
    if (it != properties.end()) {
        server = it->second;
    } else {
        RTI_RS_ERROR("Server name not found in properties");
        return;
    }

    it = properties.find("msql.db.dbname");
    if (it != properties.end()) {
        dbName = it->second;
    } else {
        RTI_RS_ERROR("Database name not found in properties");
        return;
    }

    it = properties.find("msql.db.tablename");
    if (it != properties.end()) {
        tableName = it->second;
    } else {
        RTI_RS_ERROR("Table name not found in properties");
        return;
    }

    it = properties.find("msql.db.username");
    if (it != properties.end()) {
        userName = it->second;
    } else {
        RTI_RS_ERROR("Username name not found in properties");
        return;
    }

    it = properties.find("msql.db.password");
    if (it != properties.end()) {
        password = it->second;
    } else {
        RTI_RS_ERROR("Password name not found in properties");
        return;
    }

    connectionInfo = "DRIVER={SQL Server}; SERVER=" + server + "; DATABASE="
            + dbName + "; UID=" + userName + "; PWD=" + password + ";";
}

/*
 * Utility method to provide MSQL error information
 */
void ConnectorMsqlDb::msqlShowError(
        unsigned int handletype,
        const SQLHANDLE &handle)
{
    SQLCHAR sqlstate[SQL_BUFF_LEN];
    SQLCHAR message[SQL_BUFF_LEN];

    if (SQL_SUCCESS
        == SQLGetDiagRec(
                handletype,
                handle,
                1,
                sqlstate,
                NULL,
                message,
                SQL_BUFF_LEN,
                NULL)) {
        std::cout << "Message: " << message << "\nSQLSTATE: " << sqlstate
                  << std::endl;
    }
}

/*
 * Write the data to the database
 * return true on success, otherwise failure
 */
auto ConnectorMsqlDb::writeData(const DynamicData *sample) -> bool
{
    bool success = true;

    /* write to end position */
    std::ostringstream cmd(std::ostringstream::ate);

    /* write the data to the database*/
    cmd << "INSERT INTO " << dbName << ".dbo." << tableName
        << "(topicname, color, x, y, shapesize, angle, fillkind) VALUES ('"
        << topicName_ << "','" << sample->value<std::string>("color") << "',"
        << sample->value<int32_t>("x") << "," << sample->value<int32_t>("y")
        << "," << sample->value<int32_t>("shapesize") << ","
        << sample->value<float>("angle") << ","
        << sample->value<int32_t>("fillKind") << ")";

    /* write cmd to stdout */
    /* std::cout << cmd.str(); */

    SQLRETURN retcode = SQLExecDirect(
            sqlStmtHandle,
            (SQLCHAR *) cmd.str().c_str(),
            SQL_NTS);
    if (retcode == SQL_ERROR) {
        msqlShowError(SQL_HANDLE_STMT, sqlStmtHandle);
        success = false;
    }

    return success;
}

/*
 * returns the number of records read from the database
 */
auto ConnectorMsqlDb::readData(std::unique_ptr<DynamicData> &sample)
        -> unsigned int
{
    unsigned int count = 0; /* number of records read */

    /* write to end position */
    std::ostringstream cmd(std::ostringstream::ate);  

    /* get one record from the database at the offset specified using ascending
     * order */
    cmd << "SELECT * FROM " << tableName << " WHERE topicname = '" << topicName_
        << "' ORDER BY id ASC OFFSET " << offset_
        << " ROWS FETCH FIRST 1 ROWS ONLY";

    /* write cmd to stdout */
    /* std::cout << cmd.str() << std::endl; */

    if (SQL_SUCCESS
        != SQLExecDirect(
                sqlStmtHandle,
                (SQLCHAR *) cmd.str().c_str(),
                SQL_NTS)) {
        msqlShowError(SQL_HANDLE_STMT, sqlStmtHandle);
        return count;
    }

    while (SQLFetch(sqlStmtHandle) == SQL_SUCCESS) {
        /* start at column 3 since column 1 is an auto incremented id and column
         * 2 is the topicname which we already know */
        char color[128];
        int value = 0;
        float angle = 0.0;

        SQLGetData(sqlStmtHandle, 3, SQL_C_DEFAULT, &color, 128, NULL);
        sample->value("color", std::string(color));
        SQLGetData(
                sqlStmtHandle,
                4,
                SQL_C_DEFAULT,
                &value,
                sizeof(value),
                NULL);
        sample->value("x", value);
        SQLGetData(
                sqlStmtHandle,
                5,
                SQL_C_DEFAULT,
                &value,
                sizeof(value),
                NULL);
        sample->value("y", value);
        SQLGetData(
                sqlStmtHandle,
                6,
                SQL_C_DEFAULT,
                &value,
                sizeof(value),
                NULL);
        sample->value("shapesize", value);
        SQLGetData(
                sqlStmtHandle,
                7,
                SQL_C_DEFAULT,
                &angle,
                sizeof(angle),
                NULL);
        sample->value("angle", angle);
        SQLGetData(
                sqlStmtHandle,
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
    if (SQL_SUCCESS != SQLCloseCursor(sqlStmtHandle)) {
        msqlShowError(SQL_HANDLE_STMT, sqlStmtHandle);
        return count;
    }

    return count;
}
