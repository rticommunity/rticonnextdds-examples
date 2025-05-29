file(READ ${INPUT_FILE} CONTENTS)

# Find the positions of the start and end tags
string(FIND "${CONTENTS}" "<not_after>" START_INDEX)
string(FIND "${CONTENTS}" "</not_after>" END_INDEX)

if(START_INDEX EQUAL -1 OR END_INDEX EQUAL -1)
    message(FATAL_ERROR "Tags not found in the input file")
endif()

# Compute the new contents
string(LENGTH <not_after> NOT_AFTER_LENGTH)
math(EXPR START_INDEX "${START_INDEX} + ${NOT_AFTER_LENGTH}")
string(SUBSTRING "${CONTENTS}" 0 ${START_INDEX} BEFORE_START)
string(SUBSTRING "${CONTENTS}" ${END_INDEX} -1 AFTER_END)

# Replace with the current date + 1 minute
string(TIMESTAMP current_epoch "%s" UTC)
MATH(EXPR expiring_epoch "(${current_epoch} + 60)")

# SOURCE_DATE_EPOCH allows the date and time to be set externally by an exported
# environment variable. If the SOURCE_DATE_EPOCH environment variable is set,
# the string(TIMESTAMP [...]) cmake command will return its value instead of the
# current time.
# Backup.
if (DEFINED ENV{SOURCE_DATE_EPOCH})
    set(_old_source_date_epoch ENV{SOURCE_DATE_EPOCH})
endif()
#
# Set new value.
set(ENV{SOURCE_DATE_EPOCH} ${expiring_epoch})
#
# Get the timestamp that we want.
string(TIMESTAMP expiring_date "%Y-%m-%dT%H:%M:%S" UTC)
#
# Revert old value.
if (DEFINED ${_old_source_date_epoch})
    set(ENV{SOURCE_DATE_EPOCH} ${_old_source_date_epoch})
else()
    unset(ENV{SOURCE_DATE_EPOCH})
endif()

file(WRITE ${OUTPUT_FILE} "${BEFORE_START}${expiring_date}${AFTER_END}")