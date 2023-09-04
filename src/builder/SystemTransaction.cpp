/* System transaction to change metadata
   Copyright (C) 2018-2023 Adam Leszczynski (aleszczynski@bersler.com)

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

#include "../common/OracleColumn.h"
#include "../common/OracleTable.h"
#include "../common/RuntimeException.h"
#include "../common/SysCCol.h"
#include "../common/SysCDef.h"
#include "../common/SysCol.h"
#include "../common/SysDeferredStg.h"
#include "../common/SysECol.h"
#include "../common/SysLob.h"
#include "../common/SysLobCompPart.h"
#include "../common/SysLobFrag.h"
#include "../common/SysObj.h"
#include "../common/SysTab.h"
#include "../common/SysTabComPart.h"
#include "../common/SysTabPart.h"
#include "../common/SysTabSubPart.h"
#include "../common/SysUser.h"
#include "../metadata/Metadata.h"
#include "../metadata/Schema.h"
#include "../metadata/SchemaElement.h"
#include "Builder.h"
#include "SystemTransaction.h"

namespace OpenLogReplicator {

    SystemTransaction::SystemTransaction(Builder* newBuilder, Metadata* newMetadata) :
                ctx(newMetadata->ctx),
                builder(newBuilder),
                metadata(newMetadata),
                sysCCol(nullptr),
                sysCDef(nullptr),
                sysCol(nullptr),
                sysDeferredStg(nullptr),
                sysECol(nullptr),
                sysLob(nullptr),
                sysLobCompPart(nullptr),
                sysLobFrag(nullptr),
                sysObj(nullptr),
                sysTab(nullptr),
                sysTabComPart(nullptr),
                sysTabPart(nullptr),
                sysTabSubPart(nullptr),
                sysTs(nullptr),
                sysUser(nullptr) {
        ctx->logTrace(TRACE_SYSTEM, "begin");
    }

    SystemTransaction::~SystemTransaction() {
        if (sysCCol != nullptr) {
            delete sysCCol;
            sysCCol = nullptr;
        }

        if (sysCCol != nullptr) {
            delete sysCCol;
            sysCCol = nullptr;
        }

        if (sysCDef != nullptr) {
            delete sysCDef;
            sysCDef = nullptr;
        }

        if (sysCol != nullptr) {
            delete sysCol;
            sysCol = nullptr;
        }

        if (sysDeferredStg != nullptr) {
            delete sysDeferredStg;
            sysDeferredStg = nullptr;
        }

        if (sysECol != nullptr) {
            delete sysECol;
            sysECol = nullptr;
        }

        if (sysLob != nullptr) {
            delete sysLob;
            sysLob = nullptr;
        }

        if (sysLobCompPart != nullptr) {
            delete sysLobCompPart;
            sysLobCompPart = nullptr;
        }

        if (sysLobFrag != nullptr) {
            delete sysLobFrag;
            sysLobFrag = nullptr;
        }

        if (sysObj != nullptr) {
            delete sysObj;
            sysObj = nullptr;
        }

        if (sysTab != nullptr) {
            delete sysTab;
            sysTab = nullptr;
        }

        if (sysTabComPart != nullptr) {
            delete sysTabComPart;
            sysTabComPart = nullptr;
        }

        if (sysTabPart != nullptr) {
            delete sysTabPart;
            sysTabPart = nullptr;
        }

        if (sysTabSubPart != nullptr) {
            delete sysTabSubPart;
            sysTabSubPart = nullptr;
        }

        if (sysTs != nullptr) {
            delete sysTs;
            sysTs = nullptr;
        }

        if (sysUser != nullptr) {
            delete sysUser;
            sysUser = nullptr;
        }
    }

    void SystemTransaction::updateNumber16(int16_t& val, int16_t defVal, typeCol column, OracleTable* table, uint64_t offset) {
        if (builder->values[column][VALUE_AFTER] != nullptr && builder->lengths[column][VALUE_AFTER] > 0) {
            char* retPtr;
            if (table->columns[column]->type != 2)
                throw RuntimeException(50019, "ddl: column type mismatch for " + table->owner + "." + table->name + ": column " +
                                       table->columns[column]->name + " type found " + std::to_string(table->columns[column]->type) + " offset: " +
                                       std::to_string(offset));

            builder->parseNumber(builder->values[column][VALUE_AFTER], builder->lengths[column][VALUE_AFTER], offset);
            builder->valueBuffer[builder->valueLength] = 0;
            auto newVal = (int16_t)strtol(builder->valueBuffer, &retPtr, 10);
            if (ctx->trace & TRACE_SYSTEM)
                ctx->logTrace(TRACE_SYSTEM, "set (" + table->columns[column]->name + ": " + std::to_string(val) + " -> " +
                              std::to_string(newVal) + ")");
            val = newVal;
        } else if (builder->values[column][VALUE_AFTER] != nullptr || builder->values[column][VALUE_BEFORE] != nullptr) {
            if (ctx->trace & TRACE_SYSTEM)
                ctx->logTrace(TRACE_SYSTEM, "set (" + table->columns[column]->name + ": " + std::to_string(val) + " -> NULL)");
            val = defVal;
        }
    }

    void SystemTransaction::updateNumber16u(uint16_t& val, uint16_t defVal, typeCol column, OracleTable* table, uint64_t offset) {
        if (builder->values[column][VALUE_AFTER] != nullptr && builder->lengths[column][VALUE_AFTER] > 0) {
            char* retPtr;
            if (table->columns[column]->type != 2)
                throw RuntimeException(50019, "ddl: column type mismatch for " + table->owner + "." + table->name + ": column " +
                                       table->columns[column]->name + " type found " + std::to_string(table->columns[column]->type) + " offset: " +
                                       std::to_string(offset));

            builder->parseNumber(builder->values[column][VALUE_AFTER], builder->lengths[column][VALUE_AFTER], offset);
            builder->valueBuffer[builder->valueLength] = 0;
            if (builder->valueLength == 0 || (builder->valueLength > 0 && builder->valueBuffer[0] == '-'))
                throw RuntimeException(50020, "ddl: column type mismatch for " + table->owner + "." + table->name + ": column " +
                                       table->columns[column]->name + " value found " + builder->valueBuffer + " offset: " + std::to_string(offset));

            uint16_t newVal = strtoul(builder->valueBuffer, &retPtr, 10);
            if (ctx->trace & TRACE_SYSTEM)
                ctx->logTrace(TRACE_SYSTEM, "set (" + table->columns[column]->name + ": " + std::to_string(val) + " -> " +
                              std::to_string(newVal) + ")");
            val = newVal;
        } else if (builder->values[column][VALUE_AFTER] != nullptr || builder->values[column][VALUE_BEFORE] != nullptr) {
            if (ctx->trace & TRACE_SYSTEM)
                ctx->logTrace(TRACE_SYSTEM, "set (" + table->columns[column]->name + ": " + std::to_string(val) + " -> NULL)");
            val = defVal;
        }
    }

    void SystemTransaction::updateNumber32u(uint32_t& val, uint32_t defVal, typeCol column, OracleTable* table, uint64_t offset) {
        if (builder->values[column][VALUE_AFTER] != nullptr && builder->lengths[column][VALUE_AFTER] > 0) {
            char* retPtr;
            if (table->columns[column]->type != 2)
                throw RuntimeException(50019, "ddl: column type mismatch for " + table->owner + "." + table->name + ": column " +
                                       table->columns[column]->name + " type found " + std::to_string(table->columns[column]->type) + " offset: " +
                                       std::to_string(offset));

            builder->parseNumber(builder->values[column][VALUE_AFTER], builder->lengths[column][VALUE_AFTER], offset);
            builder->valueBuffer[builder->valueLength] = 0;
            if (builder->valueLength == 0 || (builder->valueLength > 0 && builder->valueBuffer[0] == '-'))
                throw RuntimeException(50020, "ddl: column type mismatch for " + table->owner + "." + table->name + ": column " +
                                       table->columns[column]->name + " value found " + builder->valueBuffer + " offset: " + std::to_string(offset));

            uint32_t newVal = strtoul(builder->valueBuffer, &retPtr, 10);
            if (ctx->trace & TRACE_SYSTEM)
                ctx->logTrace(TRACE_SYSTEM, "set (" + table->columns[column]->name + ": " + std::to_string(val) + " -> " +
                              std::to_string(newVal) + ")");
            val = newVal;
        } else if (builder->values[column][VALUE_AFTER] != nullptr || builder->values[column][VALUE_BEFORE] != nullptr) {
            if (ctx->trace & TRACE_SYSTEM)
                ctx->logTrace(TRACE_SYSTEM, "set (" + table->columns[column]->name + ": " + std::to_string(val) + " -> NULL)");
            val = defVal;
        }
    }

    void SystemTransaction::updateNumber64(int64_t& val, int64_t defVal, typeCol column, OracleTable* table, uint64_t offset) {
        if (builder->values[column][VALUE_AFTER] != nullptr && builder->lengths[column][VALUE_AFTER] > 0) {
            char* retPtr;
            if (table->columns[column]->type != 2)
                throw RuntimeException(50019, "ddl: column type mismatch for " + table->owner + "." + table->name + ": column " +
                                       table->columns[column]->name + " type found " + std::to_string(table->columns[column]->type) + " offset: " +
                                       std::to_string(offset));

            builder->parseNumber(builder->values[column][VALUE_AFTER], builder->lengths[column][VALUE_AFTER], offset);
            builder->valueBuffer[builder->valueLength] = 0;
            if (builder->valueLength == 0)
                throw RuntimeException(50020, "ddl: column type mismatch for " + table->owner + "." + table->name + ": column " +
                                       table->columns[column]->name + " value found " + builder->valueBuffer + " offset: " + std::to_string(offset));

            int64_t newVal = strtol(builder->valueBuffer, &retPtr, 10);
            if (ctx->trace & TRACE_SYSTEM)
                ctx->logTrace(TRACE_SYSTEM, "set (" + table->columns[column]->name + ": " + std::to_string(val) + " -> " +
                              std::to_string(newVal) + ")");
            val = newVal;
        } else if (builder->values[column][VALUE_AFTER] != nullptr || builder->values[column][VALUE_BEFORE] != nullptr) {
            if (ctx->trace & TRACE_SYSTEM)
                ctx->logTrace(TRACE_SYSTEM, "set (" + table->columns[column]->name + ": " + std::to_string(val) + " -> NULL)");
            val = defVal;
        }
    }

    void SystemTransaction::updateNumber64u(uint64_t& val, uint64_t defVal, typeCol column, OracleTable* table, uint64_t offset) {
        if (builder->values[column][VALUE_AFTER] != nullptr && builder->lengths[column][VALUE_AFTER] > 0) {
            char* retPtr;
            if (table->columns[column]->type != 2)
                throw RuntimeException(50019, "ddl: column type mismatch for " + table->owner + "." + table->name + ": column " +
                                       table->columns[column]->name + " type found " + std::to_string(table->columns[column]->type) + " offset: " +
                                       std::to_string(offset));

            builder->parseNumber(builder->values[column][VALUE_AFTER], builder->lengths[column][VALUE_AFTER], offset);
            builder->valueBuffer[builder->valueLength] = 0;
            if (builder->valueLength == 0 || (builder->valueLength > 0 && builder->valueBuffer[0] == '-'))
                throw RuntimeException(50020, "ddl: column type mismatch for " + table->owner + "." + table->name + ": column " +
                                       table->columns[column]->name + " value found " + builder->valueBuffer + " offset: " + std::to_string(offset));

            uint64_t newVal = strtoul(builder->valueBuffer, &retPtr, 10);
            if (ctx->trace & TRACE_SYSTEM)
                ctx->logTrace(TRACE_SYSTEM, "set (" + table->columns[column]->name + ": " + std::to_string(val) + " -> " +
                              std::to_string(newVal) + ")");
            val = newVal;
        } else if (builder->values[column][VALUE_AFTER] != nullptr || builder->values[column][VALUE_BEFORE] != nullptr) {
            if (ctx->trace & TRACE_SYSTEM)
                ctx->logTrace(TRACE_SYSTEM, "set (" + table->columns[column]->name + ": " + std::to_string(val) + " -> NULL)");
            val = defVal;
        }
    }

    void SystemTransaction::updateNumberXu(typeIntX& val, typeCol column, OracleTable* table, uint64_t offset) {
        if (builder->values[column][VALUE_AFTER] != nullptr && builder->lengths[column][VALUE_AFTER] > 0) {
            if (table->columns[column]->type != 2)
                throw RuntimeException(50019, "ddl: column type mismatch for " + table->owner + "." + table->name + ": column " +
                                       table->columns[column]->name + " type found " + std::to_string(table->columns[column]->type) + " offset: " +
                                       std::to_string(offset));

            builder->parseNumber(builder->values[column][VALUE_AFTER], builder->lengths[column][VALUE_AFTER], offset);
            builder->valueBuffer[builder->valueLength] = 0;
            if (builder->valueLength == 0 || (builder->valueLength > 0 && builder->valueBuffer[0] == '-'))
                throw RuntimeException(50020, "ddl: column type mismatch for " + table->owner + "." + table->name + ": column " +
                                       table->columns[column]->name + " value found " + builder->valueBuffer + " offset: " + std::to_string(offset));

            typeIntX newVal(0);
            std::string err;
            newVal.setStr(builder->valueBuffer, builder->valueLength, err);
            if (err != "")
                ctx->error(50021, err.c_str());

            if (ctx->trace & TRACE_SYSTEM)
                ctx->logTrace(TRACE_SYSTEM, "set (" + table->columns[column]->name + ": " + val.toString() + " -> " + newVal.toString() + ")");
            val = newVal;
        } else if (builder->values[column][VALUE_AFTER] != nullptr || builder->values[column][VALUE_BEFORE] != nullptr) {
            if (ctx->trace & TRACE_SYSTEM)
                ctx->logTrace(TRACE_SYSTEM, "set (" + table->columns[column]->name + ": " + val.toString() + " -> NULL)");
            val.set(0, 0);
        }
    }

    void SystemTransaction::updateString(std::string& val, uint64_t maxLength, typeCol column, OracleTable* table, uint64_t offset) {
        if (builder->values[column][VALUE_AFTER] != nullptr && builder->lengths[column][VALUE_AFTER] > 0) {
            if (table->columns[column]->type != SYS_COL_TYPE_VARCHAR && table->columns[column]->type != SYS_COL_TYPE_CHAR)
                throw RuntimeException(50019, "ddl: column type mismatch for " + table->owner + "." + table->name + ": column " +
                                       table->columns[column]->name + " type found " + std::to_string(table->columns[column]->type) + " offset: " +
                                       std::to_string(offset));

            builder->parseString(builder->values[column][VALUE_AFTER], builder->lengths[column][VALUE_AFTER],
                                 table->columns[column]->charsetId, offset, false, false, false, true);
            std::string newVal(builder->valueBuffer, builder->valueLength);
            if (builder->valueLength > maxLength)
                throw RuntimeException(50020, "ddl: value too long for " + table->owner + "." + table->name + ": column " +
                                       table->columns[column]->name + ", length " + std::to_string(builder->valueLength) + " offset: " + std::to_string(offset));

            if (ctx->trace & TRACE_SYSTEM)
                ctx->logTrace(TRACE_SYSTEM, "set (" + table->columns[column]->name + ": '" + val + "' -> '" + newVal + "')");
            val = newVal;
        } else if (builder->values[column][VALUE_AFTER] != nullptr || builder->values[column][VALUE_BEFORE] != nullptr) {
            if (ctx->trace & TRACE_SYSTEM)
                ctx->logTrace(TRACE_SYSTEM, "set (" + table->columns[column]->name + ": '" + val + "' -> NULL)");
            val.assign("");
        }
    }

    void SystemTransaction::processInsertCCol(OracleTable* table, typeRowId& rowId, uint64_t offset) {
        SysCCol* sysCCol2 = metadata->schema->dictSysCColFind(rowId);
        if (sysCCol2 != nullptr) {
            if (!FLAG(REDO_FLAGS_ADAPTIVE_SCHEMA))
                throw RuntimeException(50022, "ddl: duplicate SYS.CCOL$: (rowid: " + rowId.toString() + ") for insert at offset: " +
                                       std::to_string(offset));
            metadata->schema->dictSysCColDrop(sysCCol2);
            delete sysCCol2;
        }
        sysCCol = new SysCCol(rowId, 0, 0, 0, 0, 0);

        uint64_t baseMax = builder->valuesMax >> 6;
        for (uint64_t base = 0; base <= baseMax; ++base) {
            auto column = static_cast<typeCol>(base << 6);
            for (uint64_t mask = 1; mask != 0; mask <<= 1, ++column) {
                if (builder->valuesSet[base] < mask)
                    break;
                if ((builder->valuesSet[base] & mask) == 0)
                    continue;

                if (table->columns[column]->name == "CON#") {
                    updateNumber32u(sysCCol->con, 0, column, table, offset);
                } else if (table->columns[column]->name == "INTCOL#") {
                    updateNumber16(sysCCol->intCol, 0, column, table, offset);
                } else if (table->columns[column]->name == "OBJ#") {
                    updateNumber32u(sysCCol->obj, 0, column, table, offset);
                } else if (table->columns[column]->name == "SPARE1") {
                    updateNumberXu(sysCCol->spare1, column, table, offset);
                }
            }
        }

        metadata->schema->dictSysCColAdd(sysCCol);
        sysCCol = nullptr;
    }

    void SystemTransaction::processInsertCDef(OracleTable* table, typeRowId& rowId, uint64_t offset) {
        SysCDef* sysCDef2 = metadata->schema->dictSysCDefFind(rowId);
        if (sysCDef2 != nullptr) {
            if (!FLAG(REDO_FLAGS_ADAPTIVE_SCHEMA))
                throw RuntimeException(50022, "ddl: duplicate SYS.DEF$: (rowid: " + rowId.toString() + ") for insert at offset: " +
                                       std::to_string(offset));
            metadata->schema->dictSysCDefDrop(sysCDef2);
            delete sysCDef2;
        }
        sysCDef = new SysCDef(rowId, 0, 0, 0);

        uint64_t baseMax = builder->valuesMax >> 6;
        for (uint64_t base = 0; base <= baseMax; ++base) {
            auto column = static_cast<typeCol>(base << 6);
            for (uint64_t mask = 1; mask != 0; mask <<= 1, ++column) {
                if (builder->valuesSet[base] < mask)
                    break;
                if ((builder->valuesSet[base] & mask) == 0)
                    continue;

                if (table->columns[column]->name == "CON#") {
                    updateNumber32u(sysCDef->con, 0, column, table, offset);
                } else if (table->columns[column]->name == "OBJ#") {
                    updateNumber32u(sysCDef->obj, 0, column, table, offset);
                } else if (table->columns[column]->name == "TYPE#") {
                    updateNumber16u(sysCDef->type, 0, column, table, offset);
                }
            }
        }

        metadata->schema->dictSysCDefAdd(sysCDef);
        sysCDef = nullptr;
    }

    void SystemTransaction::processInsertCol(OracleTable* table, typeRowId& rowId, uint64_t offset) {
        SysCol* sysCol2 = metadata->schema->dictSysColFind(rowId);
        if (sysCol2 != nullptr) {
            if (!FLAG(REDO_FLAGS_ADAPTIVE_SCHEMA))
                throw RuntimeException(50022, "ddl: duplicate SYS.COL$: (rowid: " + rowId.toString() + ") for insert at offset: " +
                                       std::to_string(offset));
            metadata->schema->dictSysColDrop(sysCol2);
            delete sysCol2;
        }
        sysCol = new SysCol(rowId, 0, 0, 0, 0, "", 0, 0, -1, -1,
                            0, 0, 0, 0, 0);

        uint64_t baseMax = builder->valuesMax >> 6;
        for (uint64_t base = 0; base <= baseMax; ++base) {
            auto column = static_cast<typeCol>(base << 6);
            for (uint64_t mask = 1; mask != 0; mask <<= 1, ++column) {
                if (builder->valuesSet[base] < mask)
                    break;
                if ((builder->valuesSet[base] & mask) == 0)
                    continue;

                if (table->columns[column]->name == "OBJ#") {
                    updateNumber32u(sysCol->obj, 0, column, table, offset);
                } else if (table->columns[column]->name == "COL#") {
                    updateNumber16(sysCol->col, 0, column, table, offset);
                } else if (table->columns[column]->name == "SEGCOL#") {
                    updateNumber16(sysCol->segCol, 0, column, table, offset);
                } else if (table->columns[column]->name == "INTCOL#") {
                    updateNumber16(sysCol->intCol, 0, column, table, offset);
                } else if (table->columns[column]->name == "NAME") {
                    updateString(sysCol->name, SYS_COL_NAME_LENGTH, column, table, offset);
                } else if (table->columns[column]->name == "TYPE#") {
                    updateNumber16u(sysCol->type, 0, column, table, offset);
                } else if (table->columns[column]->name == "LENGTH") {
                    updateNumber64u(sysCol->length, 0, column, table, offset);
                } else if (table->columns[column]->name == "PRECISION#") {
                    updateNumber64(sysCol->precision, -1, column, table, offset);
                } else if (table->columns[column]->name == "SCALE") {
                    updateNumber64(sysCol->scale, -1, column, table, offset);
                } else if (table->columns[column]->name == "CHARSETFORM") {
                    updateNumber64u(sysCol->charsetForm, 0, column, table, offset);
                } else if (table->columns[column]->name == "CHARSETID") {
                    updateNumber64u(sysCol->charsetId, 0, column, table, offset);
                } else if (table->columns[column]->name == "NULL$") {
                    updateNumber64(sysCol->null_, 0, column, table, offset);
                } else if (table->columns[column]->name == "PROPERTY") {
                    updateNumberXu(sysCol->property, column, table, offset);
                }
            }
        }

        metadata->schema->dictSysColAdd(sysCol);
        sysCol = nullptr;
    }

    void SystemTransaction::processInsertDeferredStg(OracleTable* table, typeRowId& rowId, uint64_t offset) {
        SysDeferredStg* sysDeferredStg2 = metadata->schema->dictSysDeferredStgFind(rowId);
        if (sysDeferredStg2 != nullptr) {
            if (!FLAG(REDO_FLAGS_ADAPTIVE_SCHEMA))
                throw RuntimeException(50022, "ddl: duplicate SYS.DEFERRED_STG$: (rowid: " + rowId.toString() + ") for insert at offset: " +
                                       std::to_string(offset));
            metadata->schema->dictSysDeferredStgDrop(sysDeferredStg2);
            delete sysDeferredStg2;
        }
        sysDeferredStg = new SysDeferredStg(rowId, 0, 0, 0);

        uint64_t baseMax = builder->valuesMax >> 6;
        for (uint64_t base = 0; base <= baseMax; ++base) {
            auto column = static_cast<typeCol>(base << 6);
            for (uint64_t mask = 1; mask != 0; mask <<= 1, ++column) {
                if (builder->valuesSet[base] < mask)
                    break;
                if ((builder->valuesSet[base] & mask) == 0)
                    continue;

                if (table->columns[column]->name == "OBJ#") {
                    updateNumber32u(sysDeferredStg->obj, 0, column, table, offset);
                } else if (table->columns[column]->name == "FLAGS_STG") {
                    updateNumberXu(sysDeferredStg->flagsStg, column, table, offset);
                }
            }
        }

        metadata->schema->dictSysDeferredStgAdd(sysDeferredStg);
        sysDeferredStg = nullptr;
    }

    void SystemTransaction::processInsertECol(OracleTable* table, typeRowId& rowId, uint64_t offset) {
        SysECol* sysECol2 = metadata->schema->dictSysEColFind(rowId);
        if (sysECol2 != nullptr) {
            if (!FLAG(REDO_FLAGS_ADAPTIVE_SCHEMA))
                throw RuntimeException(50022, "ddl: duplicate SYS.ECOL$: (rowid: " + rowId.toString() + ") for insert at offset: " +
                                       std::to_string(offset));
            metadata->schema->dictSysEColDrop(sysECol2);
            delete sysECol2;
        }
        sysECol = new SysECol(rowId, 0, 0, -1);

        uint64_t baseMax = builder->valuesMax >> 6;
        for (uint64_t base = 0; base <= baseMax; ++base) {
            auto column = static_cast<typeCol>(base << 6);
            for (uint64_t mask = 1; mask != 0; mask <<= 1, ++column) {
                if (builder->valuesSet[base] < mask)
                    break;
                if ((builder->valuesSet[base] & mask) == 0)
                    continue;

                if (table->columns[column]->name == "TABOBJ#") {
                    updateNumber32u(sysECol->tabObj, 0, column, table, offset);
                } else if (table->columns[column]->name == "COLNUM") {
                    updateNumber16(sysECol->colNum, 0, column, table, offset);
                } else if (table->columns[column]->name == "GUARD_ID") {
                    updateNumber16(sysECol->guardId, -1, column, table, offset);
                }
            }
        }

        metadata->schema->dictSysEColAdd(sysECol);
        sysECol = nullptr;
    }

    void SystemTransaction::processInsertLob(OracleTable* table, typeRowId& rowId, uint64_t offset) {
        SysLob* sysLob2 = metadata->schema->dictSysLobFind(rowId);
        if (sysLob2 != nullptr) {
            if (!FLAG(REDO_FLAGS_ADAPTIVE_SCHEMA))
                throw RuntimeException(50022, "ddl: duplicate SYS.LOB$: (rowid: " + rowId.toString() + ") for insert at offset: " +
                                       std::to_string(offset));
            metadata->schema->dictSysLobDrop(sysLob2);
            delete sysLob2;
        }
        sysLob = new SysLob(rowId, 0, 0, 0, 0, 0);

        uint64_t baseMax = builder->valuesMax >> 6;
        for (uint64_t base = 0; base <= baseMax; ++base) {
            auto column = static_cast<typeCol>(base << 6);
            for (uint64_t mask = 1; mask != 0; mask <<= 1, ++column) {
                if (builder->valuesSet[base] < mask)
                    break;
                if ((builder->valuesSet[base] & mask) == 0)
                    continue;

                if (table->columns[column]->name == "OBJ#") {
                    updateNumber32u(sysLob->obj, 0, column, table, offset);
                } else if (table->columns[column]->name == "COL#") {
                    updateNumber16(sysLob->col, 0, column, table, offset);
                } else if (table->columns[column]->name == "INTCOL#") {
                    updateNumber16(sysLob->intCol, 0, column, table, offset);
                } else if (table->columns[column]->name == "LOBJ#") {
                    updateNumber32u(sysLob->lObj, 0, column, table, offset);
                } else if (table->columns[column]->name == "TS#") {
                    updateNumber32u(sysLob->ts, 0, column, table, offset);
                }
            }
        }

        metadata->schema->dictSysLobAdd(sysLob);
        sysLob = nullptr;
    }

    void SystemTransaction::processInsertLobCompPart(OracleTable* table, typeRowId& rowId, uint64_t offset) {
        SysLobCompPart* sysLobCompPart2 = metadata->schema->dictSysLobCompPartFind(rowId);
        if (sysLobCompPart2 != nullptr) {
            if (!FLAG(REDO_FLAGS_ADAPTIVE_SCHEMA))
                throw RuntimeException(50022, "ddl: duplicate SYS.LOBCOMPPART$: (rowid: " + rowId.toString() + ") for insert at offset: " +
                                       std::to_string(offset));
            metadata->schema->dictSysLobCompPartDrop(sysLobCompPart2);
            delete sysLobCompPart2;
        }
        sysLobCompPart = new SysLobCompPart(rowId, 0, 0);

        uint64_t baseMax = builder->valuesMax >> 6;
        for (uint64_t base = 0; base <= baseMax; ++base) {
            auto column = static_cast<typeCol>(base << 6);
            for (uint64_t mask = 1; mask != 0; mask <<= 1, ++column) {
                if (builder->valuesSet[base] < mask)
                    break;
                if ((builder->valuesSet[base] & mask) == 0)
                    continue;

                if (table->columns[column]->name == "PARTOBJ#") {
                    updateNumber32u(sysLobCompPart->partObj, 0, column, table, offset);
                } else if (table->columns[column]->name == "LOBJ#") {
                    updateNumber32u(sysLobCompPart->lObj, 0, column, table, offset);
                }
            }
        }

        metadata->schema->dictSysLobCompPartAdd(sysLobCompPart);
        sysLobCompPart = nullptr;
    }

    void SystemTransaction::processInsertLobFrag(OracleTable* table, typeRowId& rowId, uint64_t offset) {
        SysLobFrag* sysLobFrag2 = metadata->schema->dictSysLobFragFind(rowId);
        if (sysLobFrag2 != nullptr) {
            if (!FLAG(REDO_FLAGS_ADAPTIVE_SCHEMA))
                throw RuntimeException(50022, "ddl: duplicate SYS.LOBFRAG$: (rowid: " + rowId.toString() + ") for insert at offset: " +
                                       std::to_string(offset));
            metadata->schema->dictSysLobFragDrop(sysLobFrag2);
            delete sysLobFrag2;
        }
        sysLobFrag = new SysLobFrag(rowId, 0, 0, 0);

        uint64_t baseMax = builder->valuesMax >> 6;
        for (uint64_t base = 0; base <= baseMax; ++base) {
            auto column = static_cast<typeCol>(base << 6);
            for (uint64_t mask = 1; mask != 0; mask <<= 1, ++column) {
                if (builder->valuesSet[base] < mask)
                    break;
                if ((builder->valuesSet[base] & mask) == 0)
                    continue;

                if (table->columns[column]->name == "FRAGOBJ#") {
                    updateNumber32u(sysLobFrag->fragObj, 0, column, table, offset);
                } else if (table->columns[column]->name == "PARENTOBJ#") {
                    updateNumber32u(sysLobFrag->parentObj, 0, column, table, offset);
                } else if (table->columns[column]->name == "TS#") {
                    updateNumber32u(sysLobFrag->ts, 0, column, table, offset);
                }
            }
        }

        metadata->schema->dictSysLobFragAdd(sysLobFrag);
        sysLobFrag = nullptr;
    }

    void SystemTransaction::processInsertObj(OracleTable* table, typeRowId& rowId, uint64_t offset) {
        SysObj* sysObj2 = metadata->schema->dictSysObjFind(rowId);
        if (sysObj2 != nullptr) {
            if (!FLAG(REDO_FLAGS_ADAPTIVE_SCHEMA))
                throw RuntimeException(50022, "ddl: duplicate SYS.OBJ$: (rowid: " + rowId.toString() + ") for insert at offset: " +
                                       std::to_string(offset));
            metadata->schema->dictSysObjDrop(sysObj2);
            delete sysObj2;
        }
        sysObj = new SysObj(rowId, 0, 0, 0, 0, "", 0, 0, false);

        uint64_t baseMax = builder->valuesMax >> 6;
        for (uint64_t base = 0; base <= baseMax; ++base) {
            auto column = static_cast<typeCol>(base << 6);
            for (uint64_t mask = 1; mask != 0; mask <<= 1, ++column) {
                if (builder->valuesSet[base] < mask)
                    break;
                if ((builder->valuesSet[base] & mask) == 0)
                    continue;

                if (table->columns[column]->name == "OWNER#") {
                    updateNumber32u(sysObj->owner, 0, column, table, offset);
                } else if (table->columns[column]->name == "OBJ#") {
                    updateNumber32u(sysObj->obj, 0, column, table, offset);
                } else if (table->columns[column]->name == "DATAOBJ#") {
                    updateNumber32u(sysObj->dataObj, 0, column, table, offset);
                } else if (table->columns[column]->name == "NAME") {
                    updateString(sysObj->name, SYS_OBJ_NAME_LENGTH, column, table, offset);
                } else if (table->columns[column]->name == "TYPE#") {
                    updateNumber16u(sysObj->type, 0, column, table, offset);
                } else if (table->columns[column]->name == "FLAGS") {
                    updateNumberXu(sysObj->flags, column, table, offset);
                }
            }
        }

        metadata->schema->dictSysObjAdd(sysObj);
        sysObj = nullptr;
    }

    void SystemTransaction::processInsertTab(OracleTable* table, typeRowId& rowId, uint64_t offset) {
        SysTab* sysTab2 = metadata->schema->dictSysTabFind(rowId);
        if (sysTab2 != nullptr) {
            if (!FLAG(REDO_FLAGS_ADAPTIVE_SCHEMA))
                throw RuntimeException(50022, "ddl: duplicate SYS.TAB$: (rowid: " + rowId.toString() + ") for insert at offset: " +
                                       std::to_string(offset));
            metadata->schema->dictSysTabDrop(sysTab2);
            delete sysTab2;
        }
        sysTab = new SysTab(rowId, 0, 0, 0, 0, 0, 0, 0, 0);

        uint64_t baseMax = builder->valuesMax >> 6;
        for (uint64_t base = 0; base <= baseMax; ++base) {
            auto column = static_cast<typeCol>(base << 6);
            for (uint64_t mask = 1; mask != 0; mask <<= 1, ++column) {
                if (builder->valuesSet[base] < mask)
                    break;
                if ((builder->valuesSet[base] & mask) == 0)
                    continue;

                if (table->columns[column]->name == "OBJ#") {
                    updateNumber32u(sysTab->obj, 0, column, table, offset);
                } else if (table->columns[column]->name == "DATAOBJ#") {
                    updateNumber32u(sysTab->dataObj, 0, column, table, offset);
                } else if (table->columns[column]->name == "TS#") {
                    updateNumber32u(sysTab->ts, 0, column, table, offset);
                } else if (table->columns[column]->name == "CLUCOLS") {
                    updateNumber16(sysTab->cluCols, 0, column, table, offset);
                } else if (table->columns[column]->name == "FLAGS") {
                    updateNumberXu(sysTab->flags, column, table, offset);
                } else if (table->columns[column]->name == "PROPERTY") {
                    updateNumberXu(sysTab->property, column, table, offset);
                }
            }
        }

        metadata->schema->dictSysTabAdd(sysTab);
        sysTab = nullptr;
    }

    void SystemTransaction::processInsertTabComPart(OracleTable* table, typeRowId& rowId, uint64_t offset) {
        SysTabComPart* sysTabComPart2 = metadata->schema->dictSysTabComPartFind(rowId);
        if (sysTabComPart2 != nullptr) {
            if (!FLAG(REDO_FLAGS_ADAPTIVE_SCHEMA))
                throw RuntimeException(50022, "ddl: duplicate SYS.TABCOMPART$: (rowid: " + rowId.toString() + ") for insert at offset: " +
                                       std::to_string(offset));
            metadata->schema->dictSysTabComPartDrop(sysTabComPart2);
            delete sysTabComPart2;
        }
        sysTabComPart = new SysTabComPart(rowId, 0, 0, 0);

        uint64_t baseMax = builder->valuesMax >> 6;
        for (uint64_t base = 0; base <= baseMax; ++base) {
            auto column = static_cast<typeCol>(base << 6);
            for (uint64_t mask = 1; mask != 0; mask <<= 1, ++column) {
                if (builder->valuesSet[base] < mask)
                    break;
                if ((builder->valuesSet[base] & mask) == 0)
                    continue;

                if (table->columns[column]->name == "OBJ#") {
                    updateNumber32u(sysTabComPart->obj, 0, column, table, offset);
                } else if (table->columns[column]->name == "DATAOBJ#") {
                    updateNumber32u(sysTabComPart->dataObj, 0, column, table, offset);
                } else if (table->columns[column]->name == "BO#") {
                    updateNumber32u(sysTabComPart->bo, 0, column, table, offset);
                }
            }
        }

        metadata->schema->dictSysTabComPartAdd(sysTabComPart);
        sysTabComPart = nullptr;
    }

    void SystemTransaction::processInsertTabPart(OracleTable* table, typeRowId& rowId, uint64_t offset) {
        SysTabPart* sysTabPart2 = metadata->schema->dictSysTabPartFind(rowId);
        if (sysTabPart2 != nullptr) {
            if (!FLAG(REDO_FLAGS_ADAPTIVE_SCHEMA))
                throw RuntimeException(50022, "ddl: duplicate SYS.TABPART$: (rowid: " + rowId.toString() + ") for insert at offset: " +
                                       std::to_string(offset));
            metadata->schema->dictSysTabPartDrop(sysTabPart2);
            delete sysTabPart2;
        }
        sysTabPart = new SysTabPart(rowId, 0, 0, 0);

        uint64_t baseMax = builder->valuesMax >> 6;
        for (uint64_t base = 0; base <= baseMax; ++base) {
            auto column = static_cast<typeCol>(base << 6);
            for (uint64_t mask = 1; mask != 0; mask <<= 1, ++column) {
                if (builder->valuesSet[base] < mask)
                    break;
                if ((builder->valuesSet[base] & mask) == 0)
                    continue;

                if (table->columns[column]->name == "OBJ#") {
                    updateNumber32u(sysTabPart->obj, 0, column, table, offset);
                } else if (table->columns[column]->name == "DATAOBJ#") {
                    updateNumber32u(sysTabPart->dataObj, 0, column, table, offset);
                } else if (table->columns[column]->name == "BO#") {
                    updateNumber32u(sysTabPart->bo, 0, column, table, offset);
                }
            }
        }

        metadata->schema->dictSysTabPartAdd(sysTabPart);
        sysTabPart = nullptr;
    }

    void SystemTransaction::processInsertTabSubPart(OracleTable* table, typeRowId& rowId, uint64_t offset) {
        SysTabSubPart* sysTabSubPart2 = metadata->schema->dictSysTabSubPartFind(rowId);
        if (sysTabSubPart2 != nullptr) {
            if (!FLAG(REDO_FLAGS_ADAPTIVE_SCHEMA))
                throw RuntimeException(50022, "ddl: duplicate SYS.TABSUBPART$: (rowid: " + rowId.toString() + ") for insert at offset: " +
                                       std::to_string(offset));
            metadata->schema->dictSysTabSubPartDrop(sysTabSubPart2);
            delete sysTabSubPart2;
        }
        sysTabSubPart = new SysTabSubPart(rowId, 0, 0, 0);

        uint64_t baseMax = builder->valuesMax >> 6;
        for (uint64_t base = 0; base <= baseMax; ++base) {
            auto column = static_cast<typeCol>(base << 6);
            for (uint64_t mask = 1; mask != 0; mask <<= 1, ++column) {
                if (builder->valuesSet[base] < mask)
                    break;
                if ((builder->valuesSet[base] & mask) == 0)
                    continue;

                if (table->columns[column]->name == "OBJ#") {
                    updateNumber32u(sysTabSubPart->obj, 0, column, table, offset);
                } else if (table->columns[column]->name == "DATAOBJ#") {
                    updateNumber32u(sysTabSubPart->dataObj, 0, column, table, offset);
                } else if (table->columns[column]->name == "POBJ#") {
                    updateNumber32u(sysTabSubPart->pObj, 0, column, table, offset);
                }
            }
        }

        metadata->schema->dictSysTabSubPartAdd(sysTabSubPart);
        sysTabSubPart = nullptr;
    }

    void SystemTransaction::processInsertTs(OracleTable* table, typeRowId& rowId, uint64_t offset) {
        SysTs* sysTs2 = metadata->schema->dictSysTsFind(rowId);
        if (sysTs2 != nullptr) {
            if (!FLAG(REDO_FLAGS_ADAPTIVE_SCHEMA))
                throw RuntimeException(50022, "ddl: duplicate SYS.TS$: (rowid: " + rowId.toString() + ") for insert at offset: " +
                                       std::to_string(offset));
            metadata->schema->dictSysTsDrop(sysTs2);
            delete sysTs2;
        }
        sysTs = new SysTs(rowId, 0, "", 0);

        uint64_t baseMax = builder->valuesMax >> 6;
        for (uint64_t base = 0; base <= baseMax; ++base) {
            auto column = static_cast<typeCol>(base << 6);
            for (uint64_t mask = 1; mask != 0; mask <<= 1, ++column) {
                if (builder->valuesSet[base] < mask)
                    break;
                if ((builder->valuesSet[base] & mask) == 0)
                    continue;

                if (table->columns[column]->name == "TS#") {
                    updateNumber32u(sysTs->ts, 0, column, table, offset);
                } else if (table->columns[column]->name == "NAME") {
                    updateString(sysTs->name, SYS_TS_NAME_LENGTH, column, table, offset);
                } else if (table->columns[column]->name == "BLOCKSIZE") {
                    updateNumber32u(sysTs->blockSize, 0, column, table, offset);
                }
            }
        }

        metadata->schema->dictSysTsAdd(sysTs);
        sysTs = nullptr;
    }

    void SystemTransaction::processInsertUser(OracleTable* table, typeRowId& rowId, uint64_t offset) {
        SysUser* sysUser2 = metadata->schema->dictSysUserFind(rowId);
        if (sysUser2 != nullptr) {
            if (!FLAG(REDO_FLAGS_ADAPTIVE_SCHEMA))
                throw RuntimeException(50022, "ddl: duplicate SYS.USER$: (rowid: " + rowId.toString() + ") for insert at offset: " +
                                       std::to_string(offset));
            metadata->schema->dictSysUserDrop(sysUser2);
            delete sysUser2;
        }
        sysUser = new SysUser(rowId, 0, "", 0, 0, false);

        uint64_t baseMax = builder->valuesMax >> 6;
        for (uint64_t base = 0; base <= baseMax; ++base) {
            auto column = static_cast<typeCol>(base << 6);
            for (uint64_t mask = 1; mask != 0; mask <<= 1, ++column) {
                if (builder->valuesSet[base] < mask)
                    break;
                if ((builder->valuesSet[base] & mask) == 0)
                    continue;

                if (table->columns[column]->name == "USER#") {
                    updateNumber32u(sysUser->user, 0, column, table, offset);
                } else if (table->columns[column]->name == "NAME") {
                    updateString(sysUser->name, SYS_USER_NAME_LENGTH, column, table, offset);
                } else if (table->columns[column]->name == "SPARE1") {
                    updateNumberXu(sysUser->spare1, column, table, offset);
                }
            }
        }

        metadata->schema->dictSysUserAdd(sysUser);
        sysUser = nullptr;
    }

    void SystemTransaction::processInsert(OracleTable* table, typeDataObj dataObj, typeDba bdba, typeSlot slot, uint64_t offset) {
        typeRowId rowId(dataObj, bdba, slot);
        char str[19];
        rowId.toString(str);
        if (ctx->trace & TRACE_SYSTEM)
            ctx->logTrace(TRACE_SYSTEM, "insert table (name: " + table->owner + "." + table->name + ", rowid: " + rowId.toString() + ")");

        switch (table->systemTable) {
            case TABLE_SYS_CCOL:
                processInsertCCol(table, rowId, offset);
                break;

            case TABLE_SYS_CDEF:
                processInsertCDef(table, rowId, offset);
                break;

            case TABLE_SYS_COL:
                processInsertCol(table, rowId, offset);
                break;

            case TABLE_SYS_DEFERRED_STG:
                processInsertDeferredStg(table, rowId, offset);
                break;

            case TABLE_SYS_ECOL:
                processInsertECol(table, rowId, offset);
                break;

            case TABLE_SYS_LOB:
                processInsertLob(table, rowId, offset);
                break;

            case TABLE_SYS_LOB_COMP_PART:
                processInsertLobCompPart(table, rowId, offset);
                break;

            case TABLE_SYS_LOB_FRAG:
                processInsertLobFrag(table, rowId, offset);
                break;

            case TABLE_SYS_OBJ:
                processInsertObj(table, rowId, offset);
                break;

            case TABLE_SYS_TAB:
                processInsertTab(table, rowId, offset);
                break;

            case TABLE_SYS_TABCOMPART:
                processInsertTabComPart(table, rowId, offset);
                break;

            case TABLE_SYS_TABPART:
                processInsertTabPart(table, rowId, offset);
                break;

            case TABLE_SYS_TABSUBPART:
                processInsertTabSubPart(table, rowId, offset);
                break;

            case TABLE_SYS_TS:
                processInsertTs(table, rowId, offset);
                break;

            case TABLE_SYS_USER:
                processInsertUser(table, rowId, offset);
                break;
        }
    }

    void SystemTransaction::processUpdateCCol(OracleTable* table, typeRowId& rowId, uint64_t offset) {
        sysCCol = metadata->schema->dictSysCColFind(rowId);
        if (sysCCol != nullptr) {
            metadata->schema->dictSysCColDrop(sysCCol);
        } else {
            if (!FLAG(REDO_FLAGS_ADAPTIVE_SCHEMA)) {
                if (ctx->trace & TRACE_SYSTEM)
                    ctx->logTrace(TRACE_SYSTEM, "missing SYS.CCOL$: (rowid: " + rowId.toString() + ") for update");
                return;
            }
            sysCCol = new SysCCol(rowId, 0, 0, 0, 0, 0);
        }

        uint64_t baseMax = builder->valuesMax >> 6;
        for (uint64_t base = 0; base <= baseMax; ++base) {
            auto column = static_cast<typeCol>(base << 6);
            for (uint64_t mask = 1; mask != 0; mask <<= 1, ++column) {
                if (builder->valuesSet[base] < mask)
                    break;
                if ((builder->valuesSet[base] & mask) == 0)
                    continue;

                if (table->columns[column]->name == "CON#") {
                    updateNumber32u(sysCCol->con, 0, column, table, offset);
                } else if (table->columns[column]->name == "INTCOL#") {
                    updateNumber16(sysCCol->intCol, 0, column, table, offset);
                } else if (table->columns[column]->name == "OBJ#") {
                    updateNumber32u(sysCCol->obj, 0, column, table, offset);
                } else if (table->columns[column]->name == "SPARE1") {
                    updateNumberXu(sysCCol->spare1, column, table, offset);
                }
            }
        }

        metadata->schema->dictSysCColAdd(sysCCol);
        sysCCol = nullptr;
    }

    void SystemTransaction::processUpdateCDef(OracleTable* table, typeRowId& rowId, uint64_t offset) {
        sysCDef = metadata->schema->dictSysCDefFind(rowId);
        if (sysCDef != nullptr) {
            metadata->schema->dictSysCDefDrop(sysCDef);
        } else {
            if (!FLAG(REDO_FLAGS_ADAPTIVE_SCHEMA)) {
                if (ctx->trace & TRACE_SYSTEM)
                    ctx->logTrace(TRACE_SYSTEM, "missing SYS.CDEF$: (rowid: " + rowId.toString() + ") for update");
                return;
            }
            sysCDef = new SysCDef(rowId, 0, 0, 0);
        }

        uint64_t baseMax = builder->valuesMax >> 6;
        for (uint64_t base = 0; base <= baseMax; ++base) {
            auto column = static_cast<typeCol>(base << 6);
            for (uint64_t mask = 1; mask != 0; mask <<= 1, ++column) {
                if (builder->valuesSet[base] < mask)
                    break;
                if ((builder->valuesSet[base] & mask) == 0)
                    continue;

                if (table->columns[column]->name == "CON#") {
                    updateNumber32u(sysCDef->con, 0, column, table, offset);
                } else if (table->columns[column]->name == "OBJ#") {
                    updateNumber32u(sysCDef->obj, 0, column, table, offset);
                } else if (table->columns[column]->name == "TYPE#") {
                    updateNumber16u(sysCDef->type, 0, column, table, offset);
                }
            }
        }

        metadata->schema->dictSysCDefAdd(sysCDef);
        sysCDef = nullptr;
    }

    void SystemTransaction::processUpdateCol(OracleTable* table, typeRowId& rowId, uint64_t offset) {
        sysCol = metadata->schema->dictSysColFind(rowId);
        if (sysCol != nullptr) {
            metadata->schema->dictSysColDrop(sysCol);
        } else {
            if (!FLAG(REDO_FLAGS_ADAPTIVE_SCHEMA)) {
                if (ctx->trace & TRACE_SYSTEM)
                    ctx->logTrace(TRACE_SYSTEM, "missing SYS.COL$: (rowid: " + rowId.toString() + ") for update");
                return;
            }
            sysCol = new SysCol(rowId, 0, 0, 0, 0, "", 0, 0, -1, -1,
                                0, 0, 0, 0, 0);
        }

        uint64_t baseMax = builder->valuesMax >> 6;
        for (uint64_t base = 0; base <= baseMax; ++base) {
            auto column = static_cast<typeCol>(base << 6);
            for (uint64_t mask = 1; mask != 0; mask <<= 1, ++column) {
                if (builder->valuesSet[base] < mask)
                    break;
                if ((builder->valuesSet[base] & mask) == 0)
                    continue;

                if (table->columns[column]->name == "OBJ#") {
                    updateNumber32u(sysCol->obj, 0, column, table, offset);
                } else if (table->columns[column]->name == "COL#") {
                    updateNumber16(sysCol->col, 0, column, table, offset);
                } else if (table->columns[column]->name == "SEGCOL#") {
                    updateNumber16(sysCol->segCol, 0, column, table, offset);
                } else if (table->columns[column]->name == "INTCOL#") {
                    updateNumber16(sysCol->intCol, 0, column, table, offset);
                } else if (table->columns[column]->name == "NAME") {
                    updateString(sysCol->name, SYS_COL_NAME_LENGTH, column, table, offset);
                } else if (table->columns[column]->name == "TYPE#") {
                    updateNumber16u(sysCol->type, 0, column, table, offset);
                } else if (table->columns[column]->name == "LENGTH") {
                    updateNumber64u(sysCol->length, 0, column, table, offset);
                } else if (table->columns[column]->name == "PRECISION#") {
                    updateNumber64(sysCol->precision, -1, column, table, offset);
                } else if (table->columns[column]->name == "SCALE") {
                    updateNumber64(sysCol->scale, -1, column, table, offset);
                } else if (table->columns[column]->name == "CHARSETFORM") {
                    updateNumber64u(sysCol->charsetForm, 0, column, table, offset);
                } else if (table->columns[column]->name == "CHARSETID") {
                    updateNumber64u(sysCol->charsetId, 0, column, table, offset);
                } else if (table->columns[column]->name == "NULL$") {
                    updateNumber64(sysCol->null_, 0, column, table, offset);
                } else if (table->columns[column]->name == "PROPERTY") {
                    updateNumberXu(sysCol->property, column, table, offset);
                }
            }
        }

        metadata->schema->dictSysColAdd(sysCol);
        sysCol = nullptr;
    }

    void SystemTransaction::processUpdateDeferredStg(OracleTable* table, typeRowId& rowId, uint64_t offset) {
        sysDeferredStg = metadata->schema->dictSysDeferredStgFind(rowId);
        if (sysDeferredStg != nullptr) {
            metadata->schema->dictSysDeferredStgDrop(sysDeferredStg);
        } else {
            if (!FLAG(REDO_FLAGS_ADAPTIVE_SCHEMA)) {
                if (ctx->trace & TRACE_SYSTEM)
                    ctx->logTrace(TRACE_SYSTEM, "missing SYS.DEFERRED_STG$: (rowid: " + rowId.toString() + ") for update");
                return;
            }
            sysDeferredStg = new SysDeferredStg(rowId, 0, 0, 0);
        }

        uint64_t baseMax = builder->valuesMax >> 6;
        for (uint64_t base = 0; base <= baseMax; ++base) {
            auto column = static_cast<typeCol>(base << 6);
            for (uint64_t mask = 1; mask != 0; mask <<= 1, ++column) {
                if (builder->valuesSet[base] < mask)
                    break;
                if ((builder->valuesSet[base] & mask) == 0)
                    continue;

                if (table->columns[column]->name == "OBJ#") {
                    updateNumber32u(sysDeferredStg->obj, 0, column, table, offset);
                } else if (table->columns[column]->name == "FLAGS_STG") {
                    updateNumberXu(sysDeferredStg->flagsStg, column, table, offset);
                }
            }
        }

        metadata->schema->dictSysDeferredStgAdd(sysDeferredStg);
        sysDeferredStg = nullptr;
    }

    void SystemTransaction::processUpdateECol(OracleTable* table, typeRowId& rowId, uint64_t offset) {
        sysECol = metadata->schema->dictSysEColFind(rowId);
        if (sysECol != nullptr) {
            metadata->schema->dictSysEColDrop(sysECol);
        } else {
            if (!FLAG(REDO_FLAGS_ADAPTIVE_SCHEMA)) {
                if (ctx->trace & TRACE_SYSTEM)
                    ctx->logTrace(TRACE_SYSTEM, "missing SYS.ECOL$: (rowid: " + rowId.toString() + ") for update");
                return;
            }
            sysECol = new SysECol(rowId, 0, 0, -1);
        }

        uint64_t baseMax = builder->valuesMax >> 6;
        for (uint64_t base = 0; base <= baseMax; ++base) {
            auto column = static_cast<typeCol>(base << 6);
            for (uint64_t mask = 1; mask != 0; mask <<= 1, ++column) {
                if (builder->valuesSet[base] < mask)
                    break;
                if ((builder->valuesSet[base] & mask) == 0)
                    continue;

                if (table->columns[column]->name == "TABOBJ#") {
                    updateNumber32u(sysECol->tabObj, 0, column, table, offset);
                } else if (table->columns[column]->name == "COLNUM") {
                    updateNumber16(sysECol->colNum, 0, column, table, offset);
                } else if (table->columns[column]->name == "GUARD_ID") {
                    updateNumber16(sysECol->guardId, -1, column, table, offset);
                }
            }
        }

        metadata->schema->dictSysEColAdd(sysECol);
        sysECol = nullptr;
    }

    void SystemTransaction::processUpdateLob(OracleTable* table, typeRowId& rowId, uint64_t offset) {
        sysLob = metadata->schema->dictSysLobFind(rowId);
        if (sysLob != nullptr) {
            metadata->schema->dictSysLobDrop(sysLob);
        } else {
            if (!FLAG(REDO_FLAGS_ADAPTIVE_SCHEMA)) {
                if (ctx->trace & TRACE_SYSTEM)
                    ctx->logTrace(TRACE_SYSTEM, "missing SYS.LOB$: (rowid: " + rowId.toString() + ") for update");
                return;
            }
            sysLob = new SysLob(rowId, 0, 0, 0, 0, 0);
        }

        uint64_t baseMax = builder->valuesMax >> 6;
        for (uint64_t base = 0; base <= baseMax; ++base) {
            auto column = static_cast<typeCol>(base << 6);
            for (uint64_t mask = 1; mask != 0; mask <<= 1, ++column) {
                if (builder->valuesSet[base] < mask)
                    break;
                if ((builder->valuesSet[base] & mask) == 0)
                    continue;

                if (table->columns[column]->name == "OBJ#") {
                    updateNumber32u(sysLob->obj, 0, column, table, offset);
                } else if (table->columns[column]->name == "COL#") {
                    updateNumber16(sysLob->col, 0, column, table, offset);
                } else if (table->columns[column]->name == "INTCOL#") {
                    updateNumber16(sysLob->intCol, 0, column, table, offset);
                } else if (table->columns[column]->name == "LOBJ#") {
                    updateNumber32u(sysLob->lObj, 0, column, table, offset);
                } else if (table->columns[column]->name == "TS#") {
                    updateNumber32u(sysLob->ts,0, column, table, offset);
                }
            }
        }

        metadata->schema->dictSysLobAdd(sysLob);
        sysLob = nullptr;
    }

    void SystemTransaction::processUpdateLobCompPart(OracleTable* table, typeRowId& rowId, uint64_t offset) {
        sysLobCompPart = metadata->schema->dictSysLobCompPartFind(rowId);
        if (sysLobCompPart != nullptr) {
            metadata->schema->dictSysLobCompPartDrop(sysLobCompPart);
        } else {
            if (!FLAG(REDO_FLAGS_ADAPTIVE_SCHEMA)) {
                if (ctx->trace & TRACE_SYSTEM)
                    ctx->logTrace(TRACE_SYSTEM, "missing SYS.LOBCOMPPART$: (rowid: " + rowId.toString() + ") for update");
                return;
            }
            sysLobCompPart = new SysLobCompPart(rowId, 0, 0);
        }

        uint64_t baseMax = builder->valuesMax >> 6;
        for (uint64_t base = 0; base <= baseMax; ++base) {
            auto column = static_cast<typeCol>(base << 6);
            for (uint64_t mask = 1; mask != 0; mask <<= 1, ++column) {
                if (builder->valuesSet[base] < mask)
                    break;
                if ((builder->valuesSet[base] & mask) == 0)
                    continue;

                if (table->columns[column]->name == "PARTOBJ#") {
                    updateNumber32u(sysLobCompPart->partObj, 0, column, table, offset);
                } else if (table->columns[column]->name == "LOBJ#") {
                    updateNumber32u(sysLobCompPart->lObj, 0, column, table, offset);
                }
            }
        }

        metadata->schema->dictSysLobCompPartAdd(sysLobCompPart);
        sysLobCompPart = nullptr;
    }

    void SystemTransaction::processUpdateLobFrag(OracleTable* table, typeRowId& rowId, uint64_t offset) {
        sysLobFrag = metadata->schema->dictSysLobFragFind(rowId);
        if (sysLobFrag != nullptr) {
            metadata->schema->dictSysLobFragDrop(sysLobFrag);
        } else {
            if (!FLAG(REDO_FLAGS_ADAPTIVE_SCHEMA)) {
                if (ctx->trace & TRACE_SYSTEM)
                    ctx->logTrace(TRACE_SYSTEM, "missing SYS.LOBFRAG$: (rowid: " + rowId.toString() + ") for update");
                return;
            }
             sysLobFrag = new SysLobFrag(rowId, 0, 0, 0);
        }

        uint64_t baseMax = builder->valuesMax >> 6;
        for (uint64_t base = 0; base <= baseMax; ++base) {
            auto column = static_cast<typeCol>(base << 6);
            for (uint64_t mask = 1; mask != 0; mask <<= 1, ++column) {
                if (builder->valuesSet[base] < mask)
                    break;
                if ((builder->valuesSet[base] & mask) == 0)
                    continue;

                if (table->columns[column]->name == "FRAGOBJ#") {
                    updateNumber32u(sysLobFrag->fragObj, 0, column, table, offset);
                } else if (table->columns[column]->name == "PARENTOBJ#") {
                    updateNumber32u(sysLobFrag->parentObj, 0, column, table, offset);
                } else if (table->columns[column]->name == "TS#") {
                    updateNumber32u(sysLobFrag->ts, 0, column, table, offset);
                }
            }
        }

        metadata->schema->dictSysLobFragAdd(sysLobFrag);
        sysLobFrag = nullptr;
    }

    void SystemTransaction::processUpdateObj(OracleTable* table, typeRowId& rowId, uint64_t offset) {
        sysObj = metadata->schema->dictSysObjFind(rowId);
        if (sysObj != nullptr) {
            metadata->schema->dictSysObjDrop(sysObj);
        } else {
            if (!FLAG(REDO_FLAGS_ADAPTIVE_SCHEMA)) {
                if (ctx->trace & TRACE_SYSTEM)
                    ctx->logTrace(TRACE_SYSTEM, "missing SYS.OBJ$: (rowid: " + rowId.toString() + ") for update");
                return;
            }
            sysObj = new SysObj(rowId, 0, 0, 0, 0, "", 0, 0, false);
        }

        uint64_t baseMax = builder->valuesMax >> 6;
        for (uint64_t base = 0; base <= baseMax; ++base) {
            auto column = static_cast<typeCol>(base << 6);
            for (uint64_t mask = 1; mask != 0; mask <<= 1, ++column) {
                if (builder->valuesSet[base] < mask)
                    break;
                if ((builder->valuesSet[base] & mask) == 0)
                    continue;

                if (table->columns[column]->name == "OWNER#") {
                    updateNumber32u(sysObj->owner, 0, column, table, offset);
                } else if (table->columns[column]->name == "OBJ#") {
                    updateNumber32u(sysObj->obj, 0, column, table, offset);
                } else if (table->columns[column]->name == "DATAOBJ#") {
                    updateNumber32u(sysObj->dataObj, 0, column, table, offset);
                } else if (table->columns[column]->name == "NAME") {
                    updateString(sysObj->name, SYS_OBJ_NAME_LENGTH, column, table, offset);
                } else if (table->columns[column]->name == "TYPE#") {
                    updateNumber16u(sysObj->type, 0, column, table, offset);
                } else if (table->columns[column]->name == "FLAGS") {
                    updateNumberXu(sysObj->flags, column, table, offset);
                }
            }
        }

        metadata->schema->dictSysObjAdd(sysObj);
        sysObj = nullptr;
    }

    void SystemTransaction::processUpdateTab(OracleTable* table, typeRowId& rowId, uint64_t offset) {
        sysTab = metadata->schema->dictSysTabFind(rowId);
        if (sysTab != nullptr) {
            metadata->schema->dictSysTabDrop(sysTab);
        } else {
            if (!FLAG(REDO_FLAGS_ADAPTIVE_SCHEMA)) {
                if (ctx->trace & TRACE_SYSTEM)
                    ctx->logTrace(TRACE_SYSTEM, "missing SYS.TAB$: (rowid: " + rowId.toString() + ") for update");
                return;
            }
            sysTab = new SysTab(rowId, 0, 0, 0, 0, 0, 0, 0, 0);
        }

        uint64_t baseMax = builder->valuesMax >> 6;
        for (uint64_t base = 0; base <= baseMax; ++base) {
            auto column = static_cast<typeCol>(base << 6);
            for (uint64_t mask = 1; mask != 0; mask <<= 1, ++column) {
                if (builder->valuesSet[base] < mask)
                    break;
                if ((builder->valuesSet[base] & mask) == 0)
                    continue;

                if (table->columns[column]->name == "OBJ#") {
                    updateNumber32u(sysTab->obj, 0, column, table, offset);
                } else if (table->columns[column]->name == "DATAOBJ#") {
                    updateNumber32u(sysTab->dataObj, 0, column, table, offset);
                } else if (table->columns[column]->name == "TS#") {
                    updateNumber32u(sysTab->ts, 0, column, table, offset);
                } else if (table->columns[column]->name == "CLUCOLS") {
                    updateNumber16(sysTab->cluCols, 0, column, table, offset);
                } else if (table->columns[column]->name == "FLAGS") {
                    updateNumberXu(sysTab->flags, column, table, offset);
                } else if (table->columns[column]->name == "PROPERTY") {
                    updateNumberXu(sysTab->property, column, table, offset);
                }
            }
        }

        metadata->schema->dictSysTabAdd(sysTab);
        sysTab = nullptr;
    }

    void SystemTransaction::processUpdateTabComPart(OracleTable* table, typeRowId& rowId, uint64_t offset) {
        sysTabComPart = metadata->schema->dictSysTabComPartFind(rowId);
        if (sysTabComPart != nullptr) {
            metadata->schema->dictSysTabComPartDrop(sysTabComPart);
        } else {
            if (!FLAG(REDO_FLAGS_ADAPTIVE_SCHEMA)) {
                if (ctx->trace & TRACE_SYSTEM)
                    ctx->logTrace(TRACE_SYSTEM, "missing SYS.TABCOMPART$: (rowid: " + rowId.toString() + ") for update");
                return;
            }
            sysTabComPart = new SysTabComPart(rowId, 0, 0, 0);
        }

        uint64_t baseMax = builder->valuesMax >> 6;
        for (uint64_t base = 0; base <= baseMax; ++base) {
            auto column = static_cast<typeCol>(base << 6);
            for (uint64_t mask = 1; mask != 0; mask <<= 1, ++column) {
                if (builder->valuesSet[base] < mask)
                    break;
                if ((builder->valuesSet[base] & mask) == 0)
                    continue;

                if (table->columns[column]->name == "OBJ#") {
                    updateNumber32u(sysTabComPart->obj, 0, column, table, offset);
                } else if (table->columns[column]->name == "DATAOBJ#") {
                    updateNumber32u(sysTabComPart->dataObj, 0, column, table, offset);
                } else if (table->columns[column]->name == "BO#") {
                    updateNumber32u(sysTabComPart->bo, 0, column, table, offset);
                }
            }
        }

        metadata->schema->dictSysTabComPartAdd(sysTabComPart);
        sysTabComPart = nullptr;
    }

    void SystemTransaction::processUpdateTabPart(OracleTable* table, typeRowId& rowId, uint64_t offset) {
        sysTabPart = metadata->schema->dictSysTabPartFind(rowId);
        if (sysTabPart != nullptr) {
            metadata->schema->dictSysTabPartDrop(sysTabPart);
        } else {
            if (!FLAG(REDO_FLAGS_ADAPTIVE_SCHEMA)) {
                if (ctx->trace & TRACE_SYSTEM)
                    ctx->logTrace(TRACE_SYSTEM, "missing SYS.TABPART$: (rowid: " + rowId.toString() + ") for update");
                return;
            }
            sysTabPart = new SysTabPart(rowId, 0, 0, 0);
        }

        uint64_t baseMax = builder->valuesMax >> 6;
        for (uint64_t base = 0; base <= baseMax; ++base) {
            auto column = static_cast<typeCol>(base << 6);
            for (uint64_t mask = 1; mask != 0; mask <<= 1, ++column) {
                if (builder->valuesSet[base] < mask)
                    break;
                if ((builder->valuesSet[base] & mask) == 0)
                    continue;

                if (table->columns[column]->name == "OBJ#") {
                    updateNumber32u(sysTabPart->obj, 0, column, table, offset);
                } else if (table->columns[column]->name == "DATAOBJ#") {
                    updateNumber32u(sysTabPart->dataObj, 0, column, table, offset);
                } else if (table->columns[column]->name == "BO#") {
                    updateNumber32u(sysTabPart->bo, 0, column, table, offset);
                }
            }
        }
        metadata->schema->dictSysTabPartAdd(sysTabPart);
        sysTabPart = nullptr;
    }

    void SystemTransaction::processUpdateTabSubPart(OracleTable* table, typeRowId& rowId, uint64_t offset) {
        sysTabSubPart = metadata->schema->dictSysTabSubPartFind(rowId);
        if (sysTabSubPart != nullptr) {
            metadata->schema->dictSysTabSubPartDrop(sysTabSubPart);
        } else {
            if (!FLAG(REDO_FLAGS_ADAPTIVE_SCHEMA)) {
                if (ctx->trace & TRACE_SYSTEM)
                    ctx->logTrace(TRACE_SYSTEM, "missing SYS.TABSUBPART$: (rowid: " + rowId.toString() + ") for update");
                return;
            }
            sysTabSubPart = new SysTabSubPart(rowId, 0, 0, 0);
        }

        uint64_t baseMax = builder->valuesMax >> 6;
        for (uint64_t base = 0; base <= baseMax; ++base) {
            auto column = static_cast<typeCol>(base << 6);
            for (uint64_t mask = 1; mask != 0; mask <<= 1, ++column) {
                if (builder->valuesSet[base] < mask)
                    break;
                if ((builder->valuesSet[base] & mask) == 0)
                    continue;

                if (table->columns[column]->name == "OBJ#") {
                    updateNumber32u(sysTabSubPart->obj, 0, column, table, offset);
                } else if (table->columns[column]->name == "DATAOBJ#") {
                    updateNumber32u(sysTabSubPart->dataObj, 0, column, table, offset);
                } else if (table->columns[column]->name == "POBJ#") {
                    updateNumber32u(sysTabSubPart->pObj, 0, column, table, offset);
                }
            }
        }

        metadata->schema->dictSysTabSubPartAdd(sysTabSubPart);
        sysTabSubPart = nullptr;
    }

    void SystemTransaction::processUpdateTs(OracleTable* table, typeRowId& rowId, uint64_t offset) {
        sysTs = metadata->schema->dictSysTsFind(rowId);
        if (sysTs != nullptr) {
            metadata->schema->dictSysTsDrop(sysTs);
        } else {
            if (!FLAG(REDO_FLAGS_ADAPTIVE_SCHEMA)) {
                if (ctx->trace & TRACE_SYSTEM)
                    ctx->logTrace(TRACE_SYSTEM, "missing SYS.TS$: (rowid: " + rowId.toString() + ") for update");
                return;
            }
            sysTs = new SysTs(rowId, 0, "", 0);
        }

        uint64_t baseMax = builder->valuesMax >> 6;
        for (uint64_t base = 0; base <= baseMax; ++base) {
            auto column = static_cast<typeCol>(base << 6);
            for (uint64_t mask = 1; mask != 0; mask <<= 1, ++column) {
                if (builder->valuesSet[base] < mask)
                    break;
                if ((builder->valuesSet[base] & mask) == 0)
                    continue;

                if (table->columns[column]->name == "TS#") {
                    updateNumber32u(sysTs->ts, 0, column, table, offset);
                } else if (table->columns[column]->name == "NAME") {
                    updateString(sysTs->name, SYS_TS_NAME_LENGTH, column, table, offset);
                } else if (table->columns[column]->name == "BLOCKSIZE") {
                    updateNumber32u(sysTs->blockSize, 0, column, table, offset);
                }
            }
        }

        metadata->schema->dictSysTsAdd(sysTs);
        sysTs = nullptr;
    }

    void SystemTransaction::processUpdateUser(OracleTable* table, typeRowId& rowId, uint64_t offset) {
        sysUser = metadata->schema->dictSysUserFind(rowId);
        if (sysUser != nullptr) {
            metadata->schema->dictSysUserDrop(sysUser);
        } else {
            if (!FLAG(REDO_FLAGS_ADAPTIVE_SCHEMA)) {
                if (ctx->trace & TRACE_SYSTEM)
                    ctx->logTrace(TRACE_SYSTEM, "missing SYS.USER$: (rowid: " + rowId.toString() + ") for update");
                return;
            }
            sysUser = new SysUser(rowId, 0, "", 0, 0, false);
        }

        uint64_t baseMax = builder->valuesMax >> 6;
        for (uint64_t base = 0; base <= baseMax; ++base) {
            auto column = static_cast<typeCol>(base << 6);
            for (uint64_t mask = 1; mask != 0; mask <<= 1, ++column) {
                if (builder->valuesSet[base] < mask)
                    break;
                if ((builder->valuesSet[base] & mask) == 0)
                    continue;

                if (table->columns[column]->name == "USER#") {
                    updateNumber32u(sysUser->user, 0, column, table, offset);
                } else if (table->columns[column]->name == "NAME") {
                    updateString(sysUser->name, SYS_USER_NAME_LENGTH, column, table, offset);
                } else if (table->columns[column]->name == "SPARE1") {
                    updateNumberXu(sysUser->spare1, column, table, offset);
                }
            }
        }

        metadata->schema->dictSysUserAdd(sysUser);
        sysUser = nullptr;
    }

    void SystemTransaction::processUpdate(OracleTable* table, typeDataObj dataObj, typeDba bdba, typeSlot slot, uint64_t offset) {
        typeRowId rowId(dataObj, bdba, slot);
        char str[19];
        rowId.toString(str);
        if (ctx->trace & TRACE_SYSTEM)
            ctx->logTrace(TRACE_SYSTEM, "update table (name: " + table->owner + "." + table->name + ", rowid: " + rowId.toString() + ")");

        switch (table->systemTable) {
            case TABLE_SYS_CCOL:
                processUpdateCCol(table, rowId, offset);
                break;

            case TABLE_SYS_CDEF:
                processUpdateCDef(table, rowId, offset);
                break;

            case TABLE_SYS_COL:
                processUpdateCol(table, rowId, offset);
                break;

            case TABLE_SYS_DEFERRED_STG:
                processUpdateDeferredStg(table, rowId, offset);
                break;

            case TABLE_SYS_ECOL:
                processUpdateECol(table, rowId, offset);
                break;

            case TABLE_SYS_LOB:
                processUpdateLob(table, rowId, offset);
                break;

            case TABLE_SYS_LOB_COMP_PART:
                processUpdateLobCompPart(table, rowId, offset);
                break;

            case TABLE_SYS_LOB_FRAG:
                processUpdateLobFrag(table, rowId, offset);
                break;

            case TABLE_SYS_OBJ:
                processUpdateObj(table, rowId, offset);
                break;

            case TABLE_SYS_TAB:
                processUpdateTab(table, rowId, offset);
                break;

            case TABLE_SYS_TABCOMPART:
                processUpdateTabComPart(table, rowId, offset);
                break;

            case TABLE_SYS_TABPART:
                processUpdateTabPart(table, rowId, offset);
                break;

            case TABLE_SYS_TABSUBPART:
                processUpdateTabSubPart(table, rowId, offset);
                break;

            case TABLE_SYS_TS:
                processUpdateTs(table, rowId, offset);
                break;

            case TABLE_SYS_USER:
                processUpdateUser(table, rowId, offset);
                break;
        }
    }

    void SystemTransaction::processDeleteCCol(typeRowId& rowId, uint64_t offset __attribute__((unused))) {
        sysCCol = metadata->schema->dictSysCColFind(rowId);
        if (sysCCol == nullptr) {
            if (!FLAG(REDO_FLAGS_ADAPTIVE_SCHEMA)) {
                if (ctx->trace & TRACE_SYSTEM)
                    ctx->logTrace(TRACE_SYSTEM, "missing SYS.CCOL$: (rowid: " + rowId.toString() + ") for delete");
                return;
            }
        }

        metadata->schema->dictSysCColDrop(sysCCol);
        metadata->schema->sysCColSetTouched.erase(sysCCol);
        delete sysCCol;
        sysCCol = nullptr;
    }

    void SystemTransaction::processDeleteCDef(typeRowId& rowId, uint64_t offset __attribute__((unused))) {
        sysCDef = metadata->schema->dictSysCDefFind(rowId);
        if (sysCDef == nullptr) {
            if (!FLAG(REDO_FLAGS_ADAPTIVE_SCHEMA)) {
                if (ctx->trace & TRACE_SYSTEM)
                    ctx->logTrace(TRACE_SYSTEM, "missing SYS.CDEF$: (rowid: " + rowId.toString() + ") for delete");
                return;
            }
        }

        metadata->schema->dictSysCDefDrop(sysCDef);
        metadata->schema->sysCDefSetTouched.erase(sysCDef);
        delete sysCDef;
        sysCDef = nullptr;
    }

    void SystemTransaction::processDeleteCol(typeRowId& rowId, uint64_t offset __attribute__((unused))) {
        sysCol = metadata->schema->dictSysColFind(rowId);
        if (sysCol == nullptr) {
            if (!FLAG(REDO_FLAGS_ADAPTIVE_SCHEMA)) {
                if (ctx->trace & TRACE_SYSTEM)
                    ctx->logTrace(TRACE_SYSTEM, "missing SYS.COL$: (rowid: " + rowId.toString() + ") for delete");
                return;
            }
        }

        metadata->schema->dictSysColDrop(sysCol);
        metadata->schema->sysColSetTouched.erase(sysCol);
        delete sysCol;
        sysCol = nullptr;
    }

    void SystemTransaction::processDeleteDeferredStg(typeRowId& rowId, uint64_t offset __attribute__((unused))) {
        sysDeferredStg = metadata->schema->dictSysDeferredStgFind(rowId);
        if (sysDeferredStg == nullptr) {
            if (!FLAG(REDO_FLAGS_ADAPTIVE_SCHEMA)) {
                if (ctx->trace & TRACE_SYSTEM)
                    ctx->logTrace(TRACE_SYSTEM, "missing SYS.DEFERRED_STG$: (rowid: " + rowId.toString() + ") for delete");
                return;
            }
        }

        metadata->schema->dictSysDeferredStgDrop(sysDeferredStg);
        metadata->schema->sysDeferredStgSetTouched.erase(sysDeferredStg);
        delete sysDeferredStg;
        sysDeferredStg = nullptr;
    }

    void SystemTransaction::processDeleteECol(typeRowId& rowId, uint64_t offset __attribute__((unused))) {
        sysECol = metadata->schema->dictSysEColFind(rowId);
        if (sysECol == nullptr) {
            if (!FLAG(REDO_FLAGS_ADAPTIVE_SCHEMA)) {
                if (ctx->trace & TRACE_SYSTEM)
                    ctx->logTrace(TRACE_SYSTEM, "missing SYS.ECOL$: (rowid: " + rowId.toString() + ") for delete");
                return;
            }
        }

        metadata->schema->dictSysEColDrop(sysECol);
        metadata->schema->sysEColSetTouched.erase(sysECol);
        delete sysECol;
        sysECol = nullptr;
    }

    void SystemTransaction::processDeleteLob(typeRowId& rowId, uint64_t offset __attribute__((unused))) {
        sysLob = metadata->schema->dictSysLobFind(rowId);
        if (sysLob == nullptr) {
            if (!FLAG(REDO_FLAGS_ADAPTIVE_SCHEMA)) {
                if (ctx->trace & TRACE_SYSTEM)
                    ctx->logTrace(TRACE_SYSTEM, "missing SYS.LOB$: (rowid: " + rowId.toString() + ") for delete");
                return;
            }
        }

        metadata->schema->dictSysLobDrop(sysLob);
        metadata->schema->sysLobSetTouched.erase(sysLob);
        delete sysLob;
        sysLob = nullptr;
    }

    void SystemTransaction::processDeleteLobCompPart(typeRowId& rowId, uint64_t offset __attribute__((unused))) {
        sysLobCompPart = metadata->schema->dictSysLobCompPartFind(rowId);
        if (sysLobCompPart == nullptr) {
            if (!FLAG(REDO_FLAGS_ADAPTIVE_SCHEMA)) {
                if (ctx->trace & TRACE_SYSTEM)
                    ctx->logTrace(TRACE_SYSTEM, "missing SYS.LOBCOMPPART$: (rowid: " + rowId.toString() + ") for delete");
                return;
            }
        }

        metadata->schema->dictSysLobCompPartDrop(sysLobCompPart);
        metadata->schema->sysLobCompPartSetTouched.erase(sysLobCompPart);
        delete sysLobCompPart;
        sysLobCompPart = nullptr;
    }

    void SystemTransaction::processDeleteLobFrag(typeRowId& rowId, uint64_t offset __attribute__((unused))) {
        sysLobFrag = metadata->schema->dictSysLobFragFind(rowId);
        if (sysLobFrag == nullptr) {
            if (!FLAG(REDO_FLAGS_ADAPTIVE_SCHEMA)) {
                if (ctx->trace & TRACE_SYSTEM)
                    ctx->logTrace(TRACE_SYSTEM, "missing SYS.LOBFRAG$: (rowid: " + rowId.toString() + ") for delete");
                return;
            }
        }

        metadata->schema->dictSysLobFragDrop(sysLobFrag);
        metadata->schema->sysLobFragSetTouched.erase(sysLobFrag);
        delete sysLobFrag;
        sysLobFrag = nullptr;
    }

    void SystemTransaction::processDeleteObj(typeRowId& rowId, uint64_t offset __attribute__((unused))) {
        sysObj = metadata->schema->dictSysObjFind(rowId);
        if (sysObj == nullptr) {
            if (!FLAG(REDO_FLAGS_ADAPTIVE_SCHEMA)) {
                if (ctx->trace & TRACE_SYSTEM)
                    ctx->logTrace(TRACE_SYSTEM, "missing SYS.OBJ$: (rowid: " + rowId.toString() + ") for delete");
                return;
            }
        }

        metadata->schema->dictSysObjDrop(sysObj);
        metadata->schema->sysObjSetTouched.erase(sysObj);
        delete sysObj;
        sysObj = nullptr;
    }

    void SystemTransaction::processDeleteTab(typeRowId& rowId, uint64_t offset __attribute__((unused))) {
        sysTab = metadata->schema->dictSysTabFind(rowId);
        if (sysTab == nullptr) {
            if (!FLAG(REDO_FLAGS_ADAPTIVE_SCHEMA)) {
                if (ctx->trace & TRACE_SYSTEM)
                    ctx->logTrace(TRACE_SYSTEM, "missing SYS.TAB$: (rowid: " + rowId.toString() + ") for delete");
                return;
            }
        }

        metadata->schema->dictSysTabDrop(sysTab);
        metadata->schema->sysTabSetTouched.erase(sysTab);
        delete sysTab;
        sysTab = nullptr;
    }

    void SystemTransaction::processDeleteTabComPart(typeRowId& rowId, uint64_t offset __attribute__((unused))) {
        sysTabComPart = metadata->schema->dictSysTabComPartFind(rowId);
        if (sysTabComPart == nullptr) {
            if (!FLAG(REDO_FLAGS_ADAPTIVE_SCHEMA)) {
                if (ctx->trace & TRACE_SYSTEM)
                    ctx->logTrace(TRACE_SYSTEM, "missing SYS.TABCOMPART$: (rowid: " + rowId.toString() + ") for delete");
                return;
            }
        }

        metadata->schema->dictSysTabComPartDrop(sysTabComPart);
        metadata->schema->sysTabComPartSetTouched.erase(sysTabComPart);
        delete sysTabComPart;
        sysTabComPart = nullptr;
    }

    void SystemTransaction::processDeleteTabPart(typeRowId& rowId, uint64_t offset __attribute__((unused))) {
        sysTabPart = metadata->schema->dictSysTabPartFind(rowId);
        if (sysTabPart == nullptr) {
            if (!FLAG(REDO_FLAGS_ADAPTIVE_SCHEMA)) {
                if (ctx->trace & TRACE_SYSTEM)
                    ctx->logTrace(TRACE_SYSTEM, "missing SYS.TABPART$: (rowid: " + rowId.toString() + ") for delete");
                return;
            }
        }

        metadata->schema->dictSysTabPartDrop(sysTabPart);
        metadata->schema->sysTabPartSetTouched.erase(sysTabPart);
        delete sysTabPart;
        sysTabPart = nullptr;
    }

    void SystemTransaction::processDeleteTabSubPart(typeRowId& rowId, uint64_t offset __attribute__((unused))) {
        sysTabSubPart = metadata->schema->dictSysTabSubPartFind(rowId);
        if (sysTabSubPart == nullptr) {
            if (!FLAG(REDO_FLAGS_ADAPTIVE_SCHEMA)) {
                if (ctx->trace & TRACE_SYSTEM)
                    ctx->logTrace(TRACE_SYSTEM, "missing SYS.TABSUBPART$: (rowid: " + rowId.toString() + ") for delete");
                return;
            }
        }

        metadata->schema->dictSysTabSubPartDrop(sysTabSubPart);
        metadata->schema->sysTabSubPartSetTouched.erase(sysTabSubPart);
        delete sysTabSubPart;
        sysTabSubPart = nullptr;
    }

    void SystemTransaction::processDeleteTs(typeRowId& rowId, uint64_t offset __attribute__((unused))) {
        sysTs = metadata->schema->dictSysTsFind(rowId);
        if (sysTs == nullptr) {
            if (!FLAG(REDO_FLAGS_ADAPTIVE_SCHEMA)) {
                if (ctx->trace & TRACE_SYSTEM)
                    ctx->logTrace(TRACE_SYSTEM, "missing SYS.TS$: (rowid: " + rowId.toString() + ") for delete");
                return;
            }
        }

        metadata->schema->dictSysTsDrop(sysTs);
        delete sysTs;
        sysTs = nullptr;
    }

    void SystemTransaction::processDeleteUser(typeRowId& rowId, uint64_t offset __attribute__((unused))) {
        sysUser = metadata->schema->dictSysUserFind(rowId);
        if (sysUser == nullptr) {
            if (!FLAG(REDO_FLAGS_ADAPTIVE_SCHEMA)) {
                if (ctx->trace & TRACE_SYSTEM)
                    ctx->logTrace(TRACE_SYSTEM, "missing SYS.USER$: (rowid: " + rowId.toString() + ") for delete");
                return;
            }
        }

        metadata->schema->dictSysUserDrop(sysUser);
        metadata->schema->sysUserSetTouched.erase(sysUser);
        delete sysUser;
        sysUser = nullptr;
    }

    void SystemTransaction::processDelete(OracleTable* table, typeDataObj dataObj, typeDba bdba, typeSlot slot, uint64_t offset) {
            typeRowId rowId(dataObj, bdba, slot);
        char str[19];
        rowId.toString(str);
        if (ctx->trace & TRACE_SYSTEM)
            ctx->logTrace(TRACE_SYSTEM, "delete table (name: " + table->owner + "." + table->name + ", rowid: " + rowId.toString() + ")");

        switch (table->systemTable) {
            case TABLE_SYS_CCOL:
                processDeleteCCol(rowId, offset);
                break;
            case TABLE_SYS_CDEF:
                processDeleteCDef(rowId, offset);
                break;
            case TABLE_SYS_COL:
                processDeleteCol(rowId, offset);
                break;
            case TABLE_SYS_DEFERRED_STG:
                processDeleteDeferredStg(rowId, offset);
                break;
            case TABLE_SYS_ECOL:
                processDeleteECol(rowId, offset);
                break;
            case TABLE_SYS_LOB:
                processDeleteLob(rowId, offset);
                break;
            case TABLE_SYS_LOB_COMP_PART:
                processDeleteLobCompPart(rowId, offset);
                break;
            case TABLE_SYS_LOB_FRAG:
                processDeleteLobFrag(rowId, offset);
                break;
            case TABLE_SYS_OBJ:
                processDeleteObj(rowId, offset);
                break;
            case TABLE_SYS_TAB:
                processDeleteTab(rowId, offset);
                break;
            case TABLE_SYS_TABCOMPART:
                processDeleteTabComPart(rowId, offset);
                break;
            case TABLE_SYS_TABPART:
                processDeleteTabPart(rowId, offset);
                break;
            case TABLE_SYS_TABSUBPART:
                processDeleteTabSubPart(rowId, offset);
                break;
            case TABLE_SYS_TS:
                processDeleteTs(rowId, offset);
                break;
            case TABLE_SYS_USER:
                processDeleteUser(rowId, offset);
                break;
        }
    }

    void SystemTransaction::commit(typeScn scn) {
        if (ctx->trace & TRACE_SYSTEM)
            ctx->logTrace(TRACE_SYSTEM, "commit");

        if (!metadata->schema->touched)
            return;

        std::list<std::string> msgsDropped;
        std::list<std::string> msgsUpdated;
        metadata->schema->scn = scn;
        metadata->schema->dropUnusedMetadata(metadata->users, msgsDropped);

        for (SchemaElement* element: metadata->schemaElements)
            metadata->schema->buildMaps(element->owner, element->table, element->keys, element->keysStr, element->options,
                                        msgsUpdated, metadata->suppLogDbPrimary, metadata->suppLogDbAll,
                                        metadata->defaultCharacterMapId, metadata->defaultCharacterNcharMapId);
        metadata->schema->resetTouched();

        for (const auto& msg: msgsDropped) {
            ctx->info(0, "dropped metadata: " + msg);
        }
        for (const auto& msg: msgsUpdated) {
            ctx->info(0, "updated metadata: " + msg);
        }
    }
}
