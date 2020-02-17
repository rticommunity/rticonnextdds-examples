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

#ifndef CONNECTOR_BASE_HPP_
#define CONNECTOR_BASE_HPP_

#include <dds/core/xtypes/DynamicData.hpp>

/*
 * Abstract Base class with implementations for connect() and disconnect()
 */
class ConnectorBase {
public:
    ConnectorBase(const std::string topicName);
    virtual ~ConnectorBase() = 0;

    /* default implementations */
    virtual auto connect() -> bool;
    virtual void disconnect();
    virtual auto connected() -> bool;

    /* must be implemented by derived connectors */
    virtual auto writeData(const dds::core::xtypes::DynamicData *sample)
            -> bool = 0;
    virtual auto
            readData(std::unique_ptr<dds::core::xtypes::DynamicData> &sample)
                    -> unsigned int = 0;

protected:
    std::string topicName_ { "" }; /* topic name */
    bool connected_ { false };
};

#endif /* CONNECTOR_BASE_HPP_ */
