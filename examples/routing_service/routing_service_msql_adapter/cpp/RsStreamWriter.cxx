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

#include "RsStreamWriter.hpp"
#include "ConnectorMsqlDb.hpp"
#include "RsLog.hpp"

#define RTI_RS_LOG_ARGS	"RsStreamWriter"

using namespace rti::routing;
using namespace rti::routing::adapter;
using namespace dds::core::xtypes;
using namespace dds::sub;

RsStreamWriter::RsStreamWriter(const StreamInfo &streamInfo, const PropertySet& properties)
{
	connWriter_ = new ConnectorMsqlDb(streamInfo.stream_name(), properties);
}

RsStreamWriter::~RsStreamWriter()
{
	delete connWriter_;
}

auto RsStreamWriter::write(
	const std::vector<DynamicData *>& sample_seq,
	const std::vector<SampleInfo *>& info_seq) -> int
{
	RTI_RS_LOG_FN(write);

	if (connWriter_->connected() == false) {
		return 0;
	}

	unsigned int count = 0;
	if (sample_seq.size() == info_seq.size()) {
		for (int i = 0; i < sample_seq.size(); i++) {
			if (info_seq[i]->valid()) {

				/* write data to the connector writer */
				if (connWriter_->writeData(sample_seq[i])) {
					count++;
				}
			}
		}
	}
	else {
		RTI_RS_ERROR("Sample and Info sequeces different lengths");
	}

	/* return number of samples written */
	return count;
}
