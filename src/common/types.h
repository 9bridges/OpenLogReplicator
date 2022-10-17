/* Definition of types and macros
   Copyright (C) 2018-2022 Adam Leszczynski (aleszczynski@bersler.com)

This file is part of OpenLogReplicator.

OpenLogReplicator is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as published
by the Free Software Foundation; either version 3, or (at your option)
any later version.

OpenLogReplicator is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
Public License for more details.

You should have received a copy of the GNU General Public License
along with OpenLogReplicator; see the file LICENSE;  If not see
<http://www.gnu.org/licenses/>.  */

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "../../config.h"

#ifndef TYPES_H_
#define TYPES_H_

#define ERROR(__x)                              {std::stringstream __s; time_t __now = time(nullptr); tm __nowTm = *localtime(&__now); char __str[50]; strftime(__str, sizeof(__str), "%F %T", &__nowTm); __s << __str << " [ERROR] " << __x << std::endl; std::cerr << __s.str(); }

typedef uint32_t typeResetlogs;
typedef uint32_t typeActivation;
typedef uint16_t typeSum;
typedef uint16_t typeOp1;
typedef uint32_t typeOp2;
typedef int16_t typeConId;
typedef uint64_t typeUba;
typedef uint32_t typeSeq;
typedef uint64_t typeScn;
typedef uint16_t typeSubScn;
typedef uint16_t typeSlt;
typedef uint32_t typeSqn;
typedef uint8_t typeRci;
typedef int16_t typeUsn;
typedef uint64_t typeXidMap;
typedef uint16_t typeAfn;
typedef uint32_t typeDba;
typedef uint16_t typeSlot;
typedef uint32_t typeBlk;
typedef uint32_t typeObj;
typedef uint32_t typeDataObj;
typedef uint64_t typeObj2;
typedef int16_t typeCol;
typedef uint16_t typeType;
typedef uint32_t typeCon;
typedef uint32_t typeTs;
typedef uint32_t typeUser;
typedef uint8_t  typeOptions;
typedef uint16_t typeField;

typedef uint16_t typeUnicode16;
typedef uint32_t typeUnicode32;
typedef uint64_t typeUnicode;

#define CONFIG_SCHEMA_VERSION                   "0.9.50"
#define CHECKPOINT_FILE_MAX_SIZE                1024
#define CONFIG_FILE_MAX_SIZE                    1048576
#define CHECKPOINT_SCHEMA_FILE_MAX_SIZE         2147483648
#define ZERO_SEQ                                ((typeSeq)0xFFFFFFFF)
#define ZERO_SCN                                ((typeScn)0xFFFFFFFFFFFFFFFF)
#define ZERO_BLK                                ((typeBlk)0xFFFFFFFF)
#define MEMORY_ALIGNMENT                        512
#define MAX_PATH_LENGTH                         2048
#define MAX_NO_COLUMNS                          1000
#define MAX_TRANSACTIONS_LIMIT                  1048576
#define MAX_RECORDS_IN_LWN                      1048576

#define MESSAGE_FORMAT_DEFAULT                  0
#define MESSAGE_FORMAT_FULL                     1
#define MESSAGE_FORMAT_ADD_SEQUENCES            2
// JSON only:
#define MESSAGE_FORMAT_SKIP_BEGIN               4
#define MESSAGE_FORMAT_SKIP_COMMIT              8

#define TIMESTAMP_FORMAT_UNIX                   0
#define TIMESTAMP_FORMAT_ISO8601                1
#define TIMESTAMP_FORMAT_ALL_PAYLOADS           2

#define CHAR_FORMAT_UTF8                        0
#define CHAR_FORMAT_NOMAPPING                   1
#define CHAR_FORMAT_HEX                         2

#define SCN_FORMAT_NUMERIC                      0
#define SCN_FORMAT_HEX                          1
#define SCN_FORMAT_ALL_PAYLOADS                 2

#define RID_FORMAT_SKIP                         0
#define RID_FORMAT_DEFAULT                      1

#define XID_FORMAT_TEXT_HEX                     0
#define XID_FORMAT_TEXT_DEC                     1
#define XID_FORMAT_NUMERIC                      2

#define UNKNOWN_FORMAT_QUESTION_MARK            0
#define UNKNOWN_FORMAT_DUMP                     1

#define SCHEMA_FORMAT_NAME                      0
#define SCHEMA_FORMAT_FULL                      1
#define SCHEMA_FORMAT_REPEATED                  2
#define SCHEMA_FORMAT_OBJ                       4

#define UNKNOWN_TYPE_HIDE                       0
#define UNKNOWN_TYPE_SHOW                       1

// Default, only changed columns for update, or PK
#define COLUMN_FORMAT_CHANGED                   0
// Show full nulls from insert & delete
#define COLUMN_FORMAT_FULL_INS_DEC              1
// Show all from redo
#define COLUMN_FORMAT_FULL_UPD                  2

#define TRANSACTION_INSERT                      1
#define TRANSACTION_DELETE                      2
#define TRANSACTION_UPDATE                      3

#define VALUE_BEFORE                            0
#define VALUE_AFTER                             1
#define VALUE_BEFORE_SUPP                       2
#define VALUE_AFTER_SUPP                        3

#define OPTIONS_DEBUG_TABLE                     1
#define OPTIONS_SYSTEM_TABLE                    2

#define TABLE_SYS_CCOL                          1
#define TABLE_SYS_CDEF                          2
#define TABLE_SYS_COL                           3
#define TABLE_SYS_DEFERRED_STG                  4
#define TABLE_SYS_ECOL                          5
#define TABLE_SYS_LOB                           6
#define TABLE_SYS_LOB_COMP_PART                 7
#define TABLE_SYS_LOB_FRAG                      8
#define TABLE_SYS_OBJ                           9
#define TABLE_SYS_TAB                           10
#define TABLE_SYS_TABPART                       11
#define TABLE_SYS_TABCOMPART                    12
#define TABLE_SYS_TABSUBPART                    13
#define TABLE_SYS_TS                            14
#define TABLE_SYS_USER                          15

#define BLOCK(__uba)                            ((uint32_t)((__uba)&0xFFFFFFFF))
#define SEQUENCE(__uba)                         ((uint16_t)((((uint64_t)(__uba))>>32)&0xFFFF))
#define RECORD(__uba)                           ((uint8_t)((((uint64_t)(__uba))>>48)&0xFF))
#define PRINTUBA(__uba)                         "0x"<<std::setfill('0')<<std::setw(8)<<std::hex<<BLOCK(__uba)<<"."<<std::setfill('0')<<std::setw(4)<<std::hex<<SEQUENCE(__uba)<<"."<<std::setfill('0')<<std::setw(2)<<std::hex<<(uint32_t)RECORD(__uba)

#define SCN(__scn1,__scn2)                      ((((uint64_t)(__scn1))<<32)|(__scn2))
#define PRINTSCN48(__scn)                       "0x"<<std::setfill('0')<<std::setw(4)<<std::hex<<((uint32_t)((__scn)>>32)&0xFFFF)<<"."<<std::setw(8)<<((__scn)&0xFFFFFFFF)
#define PRINTSCN64(__scn)                       "0x"<<std::setfill('0')<<std::setw(16)<<std::hex<<(__scn)

#define JSON_PARAMETER_LENGTH   256
#define JSON_BROKERS_LENGTH     4096
#define JSON_TOPIC_LENGTH       256

#define JSON_USERNAME_LENGTH    128
#define JSON_PASSWORD_LENGTH    128
#define JSON_SERVER_LENGTH      4096
#define JSON_KEY_LENGTH         4096
#define JSON_XID_LIST_LENGTH    1048576

#define FB_N                    0x01
#define FB_P                    0x02
#define FB_L                    0x04
#define FB_F                    0x08
#define FB_D                    0x10
#define FB_H                    0x20
#define FB_C                    0x40
#define FB_K                    0x80

#define OP_IUR                  0x01
#define OP_IRP                  0x02
#define OP_DRP                  0x03
#define OP_LKR                  0x04
#define OP_URP                  0x05
#define OP_ORP                  0x06
#define OP_MFC                  0x07
#define OP_CFA                  0x08
#define OP_CKI                  0x09
#define OP_SKL                  0x0A
#define OP_QMI                  0x0B
#define OP_QMD                  0x0C
#define OP_DSC                  0x0E
#define OP_LMN                  0x10
#define OP_LLB                  0x11
#define OP_019                  0x13
#define OP_SHK                  0x14
#define OP_021                  0x15
#define OP_CMP                  0x16
#define OP_DCU                  0x17
#define OP_MRK                  0x18
#define OP_ROWDEPENDENCIES      0x40

#define VCONTEXT_LENGTH         30
#define VPARAMETER_LENGTH       4000
#define VPROPERTY_LENGTH        4000

#endif
