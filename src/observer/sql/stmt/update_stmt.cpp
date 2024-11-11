/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by Wangyunlai on 2022/5/22.
//

#include "sql/stmt/update_stmt.h"
#include "sql/stmt/filter_stmt.h"
#include "common/log/log.h"
#include "storage/db/db.h"
#include "storage/table/table.h"
#include "sql/parser/expression_binder.h"

UpdateStmt::UpdateStmt(Table *table, std::string attribute_name, const Value value, FilterStmt *filter_stmt)
    : table_(table), attribute_name_(attribute_name), value_(value), filter_stmt_(filter_stmt)
{}

RC UpdateStmt::create(Db *db, UpdateSqlNode &update, Stmt *&stmt)
{
  const char *table_name = update.relation_name.c_str();
  if (nullptr == db || nullptr == table_name || (update.value.length() == 0 && update.value.attr_type() != AttrType::NULLS)) {
    LOG_WARN("invalid argument. db=%p, table_name=%p, value_type=%d", db, table_name, update.value.attr_type());
    return RC::INVALID_ARGUMENT;
  }

  BinderContext binder_context;

  // check whether the table exists
  Table *table = db->find_table(table_name);
  if (nullptr == table) {
    LOG_WARN("no such table. db=%s, table_name=%s", db->name(), table_name);
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }

  // check the field
  const FieldMeta *field = table->table_meta().field(update.attribute_name.c_str());
  if (nullptr == field) {
    LOG_WARN("schema mismatch. field name =%s", update.attribute_name);
    return RC::SCHEMA_FIELD_MISSING;
  }

  binder_context.add_table(table);
  std::unordered_map<std::string, Table *> table_map;
  table_map.insert(std::pair<std::string, Table *>(std::string(table_name), table));
  ExpressionBinder expression_binder(binder_context);

  FilterStmt *filter_stmt = nullptr;
  RC          rc          = FilterStmt::create(db, expression_binder, std::move(update.conditions), filter_stmt);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to create filter statement. rc=%d:%s", rc, strrc(rc));
    return rc;
  }

  stmt = new UpdateStmt(table, update.attribute_name, update.value, filter_stmt);
  return RC::SUCCESS;
}
