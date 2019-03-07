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

#include "rti/recording/storage/StorageWriter.hpp"
#include "rti/recording/storage/StorageStreamWriter.hpp"
#include "rti/recording/storage/StorageDiscoveryStreamWriter.hpp"

#include <fstream>

namespace cpp_example {

/*
 * Convenience macro to forward-declare the C-style function that will be
 * called by RTI Recording Service to create your class.
 */
RTI_RECORDING_STORAGE_WRITER_CREATE_DECL(FileStorageWriter);

/*
 * This class acts as a factory for Stream Writer objects, that store data
 * samples in a text file, transforming them from dynamic data representation
 * into text.
 * This storage writer creates three files: 1) a data file with the text
 * samples; 2) a publication file, containing a subset of the information in the
 * DCPSPublication built-in discovery topic samples; and 3) an info file, that
 * only contains the starting and ending points in time where there are data
 * samples.
 */
class FileStorageWriter : public rti::recording::storage::StorageWriter {
public:
    FileStorageWriter(const rti::routing::PropertySet& properties);
    virtual ~FileStorageWriter();

    /*
     * Recording Service will call this method to create a Stream Writer object
     * associated with a user-data topic that has been discovered.
     * The property set passed as a parameter contains information about the
     * stream not provided by the stream info object. For example, Recording
     * Service will add the DDS domain ID as a property to this set.
     */
    rti::recording::storage::StorageStreamWriter * create_stream_writer(
            const rti::routing::StreamInfo& stream_info,
            const rti::routing::PropertySet& properties);

    /*
     * Recording Service will call this method to obtain a stream writer for the
     * DDS publication built-in discovery topic.
     * Note that we're not defining a participant or subscription creation
     * method. That is telling Recording Service that we're not going to store
     * samples for those two built-in topics.
     */
    rti::recording::storage::PublicationStorageWriter * create_publication_writer();

    /*
     * Recording Service will call this method to delete a previously created
     * Stream Writer (no matter if it was created with the create_stream_writer()
     * or create_discovery_stream_writer() method).
     */
    void delete_stream_writer(
            rti::recording::storage::StorageStreamWriter *writer);

private:

    std::ofstream data_file_;

    std::ofstream info_file_;

    std::ofstream pub_file_;
};

/**
 * This class implements the provided DynamicData specialization of a
 * StorageStreamWriter to transform dynamic data objects into a text
 * representation that it stores into a file. It also stores some of the
 * sample info fields (reception timestamp, valid data flag) so that this info
 * is later available for StorageStreamReaders (see file FileStorageReader.hpp)
 * to convert or replay samples within a specified time range.
 */
class FileStreamWriter :
        public rti::recording::storage::DynamicDataStorageStreamWriter {
public:
    FileStreamWriter(std::ofstream& data_file, const std::string& stream_info);

    virtual ~FileStreamWriter();

    /*
     * This method receives a collection of Dynamic Data objects that are
     * transformed into a textual representation and stored in a text file.
     * Some of the fields in the SampleInfo object are also transformed.
     */
    void store(
            const std::vector<dds::core::xtypes::DynamicData *>& sample_seq,
            const std::vector<dds::sub::SampleInfo *>& info_seq);

private:

    uint64_t stored_sample_count_;

    std::ofstream& data_file_;

    std::string stream_name_;
};

/*
 * This class is created by the FileStorageWriter factory class when the
 * DCPSPublication built-in discovery topic is detected. This example stores
 * these samples in a separate file. Unlike with the user-data samples, the
 * discovery types are represented by their specific types (in the case of
 * DCPSPublication, dds::topic::PublicationBuiltinTopicData). The API provides
 * strongly-typed classes to store the different discovery types for your
 * convenience. In the case of this class, we've extended the
 * PublicationStorageDiscoveryStreamWriter class.
 */
class PubDiscoveryFileStreamWriter :
        public rti::recording::storage::PublicationStorageWriter {
public:
    PubDiscoveryFileStreamWriter(std::ofstream& pub_file);
    ~PubDiscoveryFileStreamWriter();

    void store(
            const std::vector<dds::topic::PublicationBuiltinTopicData *>& sample_seq,
            const std::vector<dds::sub::SampleInfo *>& info_seq);

private:

    std::ofstream& pub_file_;

    uint32_t stored_sample_count_;
};

} // namespace cpp_example
