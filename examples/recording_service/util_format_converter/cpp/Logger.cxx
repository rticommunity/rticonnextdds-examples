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

#include "Logger.hpp"

std::ostream& operator<< (
        std::ostream& out,
        const rti::config::Verbosity& verbosity)
{
    switch (verbosity.underlying()) {
    case rti::config::Verbosity::SILENT:
        out << 0;
        break;

    case rti::config::Verbosity::EXCEPTION:
        out << 1;
        break;

    case rti::config::Verbosity::WARNING:
        out << 2;
        break;

    case rti::config::Verbosity::STATUS_LOCAL:
        out << 3;
        break;

    case rti::config::Verbosity::STATUS_REMOTE:
        out << 4;
        break;

    default:
        out << 5;
    }

    return out;
}

Logger::Logger() :
        _verbosity(rti::config::Verbosity::EXCEPTION)
{

}

Logger& Logger::instance()
{
    static Logger instance;
    return instance;
}

void Logger::verbosity(rti::config::Verbosity& verbosity)
{
    instance()._verbosity = verbosity;
}

rti::config::Verbosity Logger::verbosity() const
{
    return instance()._verbosity;
}


