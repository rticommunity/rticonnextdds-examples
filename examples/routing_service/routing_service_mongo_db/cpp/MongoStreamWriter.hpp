/*
 * (c) 2019 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 *
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the Software.  Licensee has the right to distribute object form
 * only for use with RTI products.  The Software is provided "as is", with no
 * warranty of any type, including any warranty for fitness for any purpose.
 * RTI is under no obligation to maintain or support the Software.  RTI shall
 * not be liable for any incidental or consequential damages arising out of the
 * use or inability to use the software.
 */

#ifndef MONGO_STREAMWRITER_HPP
#define MONGO_STREAMWRITER_HPP

#include <iostream>
#include <fstream>

#include <rti/routing/adapter/AdapterPlugin.hpp>
#include <rti/routing/adapter/StreamWriter.hpp>
#include <mongocxx/database.hpp>

#include "MongoConnection.hpp"

namespace rti { namespace community { namespace examples {

class MongoStreamWriter : public rti::routing::adapter::DynamicDataStreamWriter {
    
public:
    MongoStreamWriter(
            MongoConnection& connection,
            const rti::routing::StreamInfo& stream_info,
            const std::string& db_name,
            const rti::routing::PropertySet &properties);

    ~MongoStreamWriter();


    int write(
            const std::vector<dds::core::xtypes::DynamicData *> &samples,
            const std::vector<dds::sub::SampleInfo *> &infos) override final;

private:
    mongocxx::database database();
    
private:
    std::string db_name_;
    std::string stream_name_;
    MongoConnection& connection_;
};

}}}  // namespace rti::community::examples

#endif
