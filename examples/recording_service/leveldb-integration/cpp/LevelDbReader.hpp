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

#ifndef RTI_RECORDING_EXAMPLES_LEVELDBREADER_H_
#define RTI_RECORDING_EXAMPLES_LEVELDBREADER_H_

#include <fstream>

#include <routingservice/routingservice_infrastructure.h>

#include <rti/recording/storage/StorageReader.hpp>

#include <leveldb/db.h>

#include "Utils.hpp"


namespace rti { namespace recording { namespace examples {

/*
 * Convenience macro to forward-declare the C-style function that will be
 * called by RTI Recording Service to create your class.
 */
RTI_RECORDING_STORAGE_READER_CREATE_DECL(LevelDbReader);

/**
 * A Storage Reader implementation that uses the LevelDB database engine.
 *
 * This implementation integrates with the LevelDbWriter class and expects the
 * same DBs to be found in the specified working directory:
 * - a 'metadata.dat' database, that contains the start and stop time of the
 *   service. This will be used by the stream info reader to provide this info
 *   to Replay when asked for.
 * - a 'DCPSPublication.dat database, storing the (reduced) DCPSPublication
 *   samples. The samples are expected to be stored with type 'UserDataKey' as
 *   the key and 'ReducedDCPSPublication' as the value. See the IDL file
 *   (LevelDb_RecorderTypes.idl) in the example for more details.
 * - one '<stream-name>@<domain-id>' database per recorded user-data stream. The
 *   database will contain samples, with the key being of type 'UserDataKey' and
 *   the value being of type 'UserDataValue'. For more information, check the
 *   IDL file (LevelDb_RecorderTypes.idl) provided with this example.
 */
class LevelDbReader : public rti::recording::storage::StorageReader {
public:

    /**
     * Constructor. The storage writer expects the following property to be set
     * in the passed properties: "rti.recording.examples.leveldb.working_dir".
     * The property should be set in the RTI Recording Service XML configuration
     * (see files leveldb_replay.xml or leveldb_converter.xml) in the
     * <plugin><property> section.
     */
    LevelDbReader(const rti::routing::PropertySet& properties);

    ~LevelDbReader();

    /**
     * Recording Service will call this method to create a Stream Info Reader
     * object that can read the stored DCPSPublication samples and build
     * StreamInfo objects out of them, and pass them to the processing engine.
     * This method expects the time range information to be passed as properties
     * in the property set parameter. This will be provided by Recording Service
     * automatically.
     */
    rti::recording::storage::StorageStreamInfoReader * create_stream_info_reader(
            const rti::routing::PropertySet& properties);

    void delete_stream_info_reader(
            rti::recording::storage::StorageStreamInfoReader *stream_info_reader);

    /**
     * Recording Service will call this method to create a Stream Reader object
     * that can read the stored user-data samples associated with a StreamInfo
     * parameter.
     * This method expects the time range information to be passed as properties
     * in the property set parameter, as well as the associated DDS Domain ID.
     * This will be provided by Recording Service automatically.
     */
    rti::recording::storage::StorageStreamReader * create_stream_reader(
            const rti::routing::StreamInfo& stream_info,
            const rti::routing::PropertySet& properties);

    void delete_stream_reader(
            rti::recording::storage::StorageStreamReader *stream_reader);
private:

    std::string working_dir_;

};

/*
 * This Stream Reader implementation is able to read LevelDB user-data databases
 * created by the 'LevelDbStreamWriter' class. The name of the database is
 * expected to be of the form '<stream-name>@<domain-id>'.
 */
class LevelDbStreamReader :
        public rti::recording::storage::DynamicDataStorageStreamReader {
public:

    LevelDbStreamReader(
            const std::string& working_dir,
            const rti::routing::StreamInfo& stream_info,
            uint32_t domain_id,
            int64_t start_timestamp,
            int64_t end_timestamp);

    virtual ~LevelDbStreamReader();

    /*
     * Implementation of the read operation. It should interpret the selector
     * state object that expresses the specific needs of Replay/Converter about
     * the data to be provided (data not read before vs data of any kind, lower
     * and upper time limits, etc).
     */
    virtual void read(
            std::vector<dds::core::xtypes::DynamicData *>& sample_seq,
            std::vector<dds::sub::SampleInfo *>& info_seq,
            const rti::recording::storage::SelectorState& selector);

    /*
     * The return loan operation should free any resources allocated by the
     * read() operation.
     */
    virtual void return_loan(
            std::vector<dds::core::xtypes::DynamicData *>& sample_seq,
            std::vector<dds::sub::SampleInfo *>& info_seq);

    /*
     * This method should flag Replay/Converter that all data related to the
     * data stream has been read and that we're ready for termination.
     */
    virtual bool finished();

    virtual void reset();

private:

    std::string db_filename_;

    rti::routing::StreamInfo stream_info_;

    dds::core::xtypes::DynamicType type_;

    std::unique_ptr<leveldb::DB> data_db_;

    UserDataKeyComparator key_comparator_;

    std::unique_ptr<leveldb::Iterator> db_iterator_;

    std::vector<char> key_buffer_;

    std::vector<char> value_buffer_;

    std::vector<std::shared_ptr<dds::core::xtypes::DynamicData>> loaned_samples_;

    std::vector<std::shared_ptr<dds::sub::SampleInfo>> loaned_infos_;

    int64_t start_timestamp_;

    int64_t end_timestamp_;

    int64_t current_timestamp_;

    bool finished_;

};

/*
 * This stream info reader implementation is able to read a Level DB discovery
 * database recorded with the LevelDbWriter implementation, called
 * 'DCPSPublication.dat'.
 */
class LevelDbStreamInfoReader :
        public rti::recording::storage::StorageStreamInfoReader {
public:

    LevelDbStreamInfoReader(
            const std::string& working_dir,
            int64_t start_timestamp,
            int64_t end_timestamp);

    ~LevelDbStreamInfoReader();

    /*
     * Implementation of the read operation. It should interpret the selector
     * state object that expresses the specific needs of Replay/Converter about
     * the data to be provided (data not read before vs data of any kind, lower
     * and upper time limits, etc).
     */
    virtual void read(
            std::vector<rti::routing::StreamInfo *>& sample_seq,
            const rti::recording::storage::SelectorState& selector);

    /*
     * The return loan operation should free any resources allocated by the
     * read() operation.
     */
    virtual void return_loan(
            std::vector<rti::routing::StreamInfo *>& sample_seq);

    /*
     * An int64-represented time-stamp (in nanoseconds) representing the
     * starting point in time where recorded data exists, or when the service
     * started executing.
     */
    virtual int64_t service_start_time();

    /*
     * An int64-represented time-stamp (in nanoseconds) representing the
     * final point in time where recorded data exists, or when the service
     * finished executing.
     */
    virtual int64_t service_stop_time();

    virtual bool finished();

    virtual void reset();

private:

    std::unique_ptr<leveldb::DB> metadata_db_;

    int64_t service_start_time_;

    int64_t service_end_time_;

    int64_t start_timestamp_;

    int64_t end_timestamp_;

    int64_t current_timestamp_;

    std::string discovery_filename_;

    std::unique_ptr<leveldb::DB> discovery_db_;

    UserDataKeyComparator key_comparator_;

    std::unique_ptr<leveldb::Iterator> db_iterator_;

    std::vector<char> key_buffer_;

    std::vector<char> value_buffer_;

    std::vector<std::shared_ptr<rti::routing::StreamInfo>> loaned_stream_infos_;

    bool finished_;

};

} } } // namespace rti::recording::examples

#endif
