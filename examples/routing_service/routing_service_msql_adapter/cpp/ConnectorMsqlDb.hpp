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

#ifndef CONNECTOR_MSQLDB_HPP_
#define CONNECTOR_MSQLDB_HPP_

#include "ConnectorBase.hpp"
#include <dds/core/xtypes/DynamicData.hpp>
#include <rti/routing/PropertySet.hpp>
#include <sqltypes.h>
#include <windows.h>

/*
 * MSQL implementation of a connector
 */
class ConnectorMsqlDb : public ConnectorBase {
public:
    ConnectorMsqlDb(
            const std::string &topic_name,
            const rti::routing::PropertySet &properties);
    virtual ~ConnectorMsqlDb();

    auto connect() -> bool;
    void disconnect();

    /* write data to the database*/
    auto write_data(const dds::core::xtypes::DynamicData &sample) -> bool;

    /* read data from the database */
    auto read_data(std::unique_ptr<dds::core::xtypes::DynamicData> &sample)
            -> unsigned int;

private:
    void set_connection_info(const rti::routing::PropertySet &properties);
    void msql_show_error(unsigned int handle_type, const SQLHANDLE &handle);

    SQLHANDLE sql_env_handle_ { nullptr };
    SQLHANDLE sql_conn_handle_ { nullptr };
    SQLHANDLE sql_stmt_handle_ { nullptr };

    std::string server_ { "" };
    std::string db_name_ { "" };
    std::string table_name_ { "" };
    std::string user_name_ { "" };
    std::string password_ { "" };
    std::string connection_info_ { "" };

    unsigned int offset_ { 0 }; /* database offset */
};

#endif /* CONNECTOR_MSQLDB_HPP_ */
