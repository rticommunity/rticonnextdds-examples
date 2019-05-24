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

#ifndef RTI_RECORDER_UTILS_PRINTFORMATCSV_HPP_
#define RTI_RECORDER_UTILS_PRINTFORMATCSV_HPP_

#include <fstream>
#include <iostream>
#include <list>

#include "dds_c/dds_c_printformat.h"
#include "dds/core/xtypes/DynamicType.hpp"
#include "dds/core/xtypes/MemberType.hpp"

#include "PrintFormatWrapper.hpp"

namespace rti { namespace recorder { namespace utils {

class PrintFormatCsvProperty {
public:
    explicit PrintFormatCsvProperty();

    PrintFormatCsvProperty(const PrintFormatCsvProperty& other);
    
    const std::string& empty_member_value_representation() const;
    bool enum_as_string() const;

    void empty_member_value_representation(const std::string& value);
    void enum_as_string(bool the_enum_as_string);

private:
    std::string empty_member_value_rep_;
    bool enum_as_string_;
    
};

class PrintFormatCsv
{

public:

    class ColumnInfo;
    class ColumnInfo {
    public:
        typedef std::list<ColumnInfo>::const_iterator iterator;

        ColumnInfo(
                const std::string& name,
                const dds::core::xtypes::DynamicType& type);
        
        ColumnInfo(const ColumnInfo& other);


        ColumnInfo();

        ColumnInfo& add_child(const ColumnInfo& info);

        const ColumnInfo& parent() const;

        iterator first_child() const;

        const std::list<ColumnInfo>& children() const;

        const dds::core::xtypes::TypeKind& type_kind() const;

        const std::string& name() const;

        bool has_parent() const;

        bool is_collection() const;

        friend std::ostream& operator<<(
                std::ostream& os,
                const ColumnInfo& info);
        
    private:
        const ColumnInfo *parent_;
        std::string name_;
        dds::core::xtypes::TypeKind type_kind_;
        std::list<ColumnInfo> children_;
    };
    
public:
    typedef ColumnInfo::iterator Cursor;
    typedef std::list<Cursor> CursorStack;

    static const std::string& COLUMN_SEPARATOR_DEFAULT();
    static const std::string& EMPTY_MEMBER_VALUE_REPRESENTATION_DEFAULT();
    static const PrintFormatCsvProperty& PROPERTY_DEFAULT();

    PrintFormatCsv(
            const PrintFormatCsvProperty& property,
            const dds::core::xtypes::DynamicType& type,
            std::ofstream& output_file);

    const PrintFormatCsvProperty& property() const;
    
    DDS_PrintFormat* native();

    static PrintFormatCsv& from_native(DDS_PrintFormat *native);

    std::ofstream& output_file();

    void reset_member();    
    
    ColumnInfo& column_info();

    Cursor& cursor();

    CursorStack& cursor_stack();

    void push_cursor();

    void pop_cursor();

    void skip_cursor_siblings(RTIXMLSaveContext* save_context);
    void skip_cursor(RTIXMLSaveContext* save_context);
    void position_cursor(
            const std::string& member_name,
            RTIXMLSaveContext *save_context);

private:
    void build_column_info(
            ColumnInfo& current_info,
            const std::string& member_name,
            const dds::core::xtypes::DynamicType& member_type);
    
    void print_type_header(
            const ColumnInfo& current_info,
            std::ostringstream& string_stream);

    static void skip_cursor_columns(
            PrintFormatCsv::Cursor& cursor,
            RTIXMLSaveContext* save_context,
            const std::string& empty_member_value_rep);

    void initialize_native();
    

private:
    PrintFormatWrapper<PrintFormatCsv> format_wrapper_;
    const PrintFormatCsvProperty& property_;
    dds::core::xtypes::DynamicType type_;
    std::ofstream& output_file_;
    ColumnInfo column_info_;
    CursorStack cursor_stack_;
};

} } }

#endif
