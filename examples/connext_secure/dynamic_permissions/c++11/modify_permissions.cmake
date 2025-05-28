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
string(TIMESTAMP CURRENT_DATE "%Y-%m-%d %H:%M:%S")
file(WRITE ${OUTPUT_FILE} "${BEFORE_START}${CURRENT_DATE}\n${AFTER_END}")