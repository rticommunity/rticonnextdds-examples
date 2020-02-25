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

#ifndef RTI_RECORDING_EXAMPLES_LEVELDBWRITER_H_
#define RTI_RECORDING_EXAMPLES_LEVELDBWRITER_H_

#include <fstream>

#include <routingservice/routingservice_infrastructure.h>

#include <rti/recording/storage/StorageWriter.hpp>

#include <leveldb/db.h>

#include "Utils.hpp"


namespace rti { namespace recording { namespace examples {

/*
 * Convenience macro to forward-declare the C-style function that will be
 * called by RTI Recording Service to create your class.
 */
RTI_RECORDING_STORAGE_WRITER_CREATE_DECL(LevelDbWriter);

/*
 * A Storage Writer implementation that uses the LevelDB database engine.
 *
 * This implementation will create a LevelDB database called 'metadata.dat'
 * where the start (first timestamp) and stop (last timestamp) times of the
 * plugin's usage are stored. This information is requested by Replay when
 * defining the replay time span.
 *
 * Because LevelDB is a key-value store, we need a key unique for every sample.
 * The key type, which is then serialized using RTI DDS Code Generator's
 * methods, is defined in file 'LevelDb_RecorderTypes.idl'. The value type is
 * also defined in this file. The DCPSPublication topic used by the publication
 * stream writers is also defined in this file as a subset of the information
 * in the topic's type.
 */
class LevelDbWriter : public rti::recording::storage::StorageWriter {
public:

    /**
     * Constructor. The storage writer expects the following property to be set
     * in the passed properties: "rti.recording.examples.leveldb.working_dir".
     * The property should be set in the RTI Recording Service XML configuration
     * (see file leveldb_recorder.xml) in the <plugin><property> section.
     */
    LevelDbWriter(const rti::routing::PropertySet& properties);

    virtual ~LevelDbWriter();

    /**
     * Recording Service will call this method to create a Stream Writer object
     * associated with a user-data topic that has been discovered.
     * The property set passed as a parameter contains information about the
     * stream not provided by the stream info object. For example, Recording
     * Service will add the DDS domain ID as a property to this set.
     * For reference on how we're storing the keys and samples in the key-value
     * store, see the types 'UserDataKey' and 'UserDataValue' in the IDL file
     * associated with this example.
     */
    rti::recording::storage::StorageStreamWriter * create_stream_writer(
            const rti::routing::StreamInfo& stream_info,
            const rti::routing::PropertySet& properties);

    /**
     * Recording Service will call this method to obtain a stream writer for the
     * DDS publication built-in discovery topic.
     * Note that we're not defining a participant or subscription creation
     * method. That is telling Recording Service that we're not going to store
     * samples for those two built-in topics.
     * For reference on what information we're storing from the DCPSPublication
     * samples, check type 'ReducedDCPSPublication' in the IDL file of this
     * example.
     */
    rti::recording::storage::PublicationStorageWriter * create_publication_writer();

    /**
     * Recording Service will call this method to delete a previously created
     * Stream Writer (no matter if it was created with the create_stream_writer()
     * or create_publication_writer() method).
     */
    void delete_stream_writer(
            rti::recording::storage::StorageStreamWriter *writer);

private:

    std::string working_dir_;

    std::unique_ptr<leveldb::DB> metadata_db_;
};

/**
 * This Stream Writer implementation will create a LevelDB database with the
 * stream name and the domain ID (of the format '<stream-name>@<domain-id>' to
 * store data samples.
 */
class LevelDbStreamWriter :
        public rti::recording::storage::DynamicDataStorageStreamWriter {
public:

    LevelDbStreamWriter(
            const std::string& working_dir,
            const rti::routing::StreamInfo& stream_info,
            uint32_t domain_id);

    virtual ~LevelDbStreamWriter();

    /**
     * Write user-data to the associated LevelDB database.
     * For reference on how we're storing the keys and samples in the key-value
     * store, see the types 'UserDataKey' and 'UserDataValue' in the IDL file
     * associated with this example.
     */
    void store(
            const std::vector<dds::core::xtypes::DynamicData *>& sample_seq,
            const std::vector<dds::sub::SampleInfo *>& info_seq);

private:

    std::string db_filename_;

    UserDataKeyComparator key_comparator_;

    std::shared_ptr<leveldb::DB> data_db_;

    rti::routing::StreamInfo stream_info_;

    uint32_t domain_id_;

    std::vector<char> key_buffer_;

    std::vector<char> value_buffer_;
};

/**
 * This publication stream writer implementation will create a LevelDB database
 * called 'DCPSPublication.dat' where it will store a reduced version of the
 * full DCPSPublication topic's type, for example purposes.
 */
class PubDiscoveryLevelDbWriter :
        public rti::recording::storage::PublicationStorageWriter {
public:

    PubDiscoveryLevelDbWriter(const std::string& working_dir);

    ~PubDiscoveryLevelDbWriter();

    /**
     * Write publication data samples to the discovery database.
     * For reference on what information we're storing from the DCPSPublication
     * samples, check type 'ReducedDCPSPublication' in the IDL file of this
     * example.
     */
    void store(
            const std::vector<dds::topic::PublicationBuiltinTopicData *>& sample_seq,
            const std::vector<dds::sub::SampleInfo *>& info_seq);

private:

    std::string discovery_filename_;

    UserDataKeyComparator key_comparator_;

    std::unique_ptr<leveldb::DB> discovery_db_;

    std::vector<char> key_buffer_;

    std::vector<char> value_buffer_;
};

} } } // namespace rti::recording::examples

#endif
