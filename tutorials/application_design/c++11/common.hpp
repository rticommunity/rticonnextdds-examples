//
// (c) 2024 Copyright, Real-Time Innovations, Inc.  All rights reserved.
//
//  RTI grants Licensee a license to use, modify, compile, and create derivative
//  works of the Software.  Licensee has the right to distribute object form
//  only for use with RTI products.  The Software is provided "as is", with no
//  warranty of any type, including any warranty for fitness for any purpose.
//  RTI is under no obligation to maintain or support the Software.  RTI shall
//  not be liable for any incidental or consequential damages arising out of the
//  use or inability to use the software.

#ifndef COMMON_HPP
#define COMMON_HPP

#include <vector>
#include <sstream>
#include <random>

#include "VehicleModeling.hpp"

using CoordSequence = rti::core::bounded_sequence<Coord, 100>;

// Coord namespace

std::string to_string(const Coord &coord)
{
    std::ostringstream ss;
    ss << "Coord(lat: " << coord.lat << ", lon: " << coord.lon << ")";
    return ss.str();
}

// namespace

namespace rti::core {  // bounded_sequence namespace

std::string to_string(const CoordSequence &route)
{
    using ::to_string;

    std::ostringstream ss;
    ss << "Route(";
    for (auto it = route.begin(); it != route.end();) {
        ss << to_string(*it);
        if (++it != route.end()) {
            ss << ", ";
        }
    }
    ss << ")";
    return ss.str();
}

}  // namespace rti::core

namespace utils {

namespace details {

static std::random_device rd;
static std::mt19937 gen { rd() };

};  // namespace details

double random_range(double min, double max)
{
    return std::uniform_real_distribution<>(min, max)(details::gen);
}

double random_stduniform()
{
    return random_range(0.0, 1.0);
}

std::string new_vin()
{
    static const std::string choices("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ");

    std::ostringstream ss;
    for (int i = 0; i < VIN_LENGTH; ++i) {
        ss << choices[random_range(0, choices.size())];
    }
    return ss.str();
}


CoordSequence create_new_route(int n = 5)
{
    CoordSequence route;
    for (int i = 0; i < n; ++i) {
        double r1 = random_stduniform() * 100.0;
        double r2 = random_stduniform() * 100.0;
        route.push_back(Coord { r1, r2 });
    }
    return route;
}

}  // namespace utils

#endif
