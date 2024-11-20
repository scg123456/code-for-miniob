/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include "common/lang/comparator.h"
#include "common/log/log.h"
#include "common/type/text_type.h"
#include "common/value.h"
#include "common/time/datetime.h"

// int TextType::compare(const Value &left, const Value &right) const
// {
//   ASSERT(left.attr_type() == AttrType::CHARS && right.attr_type() == AttrType::CHARS, "invalid type");
//   if (right.attr_type() == AttrType::NULLS) {
//     return INT32_MIN;
//   }
//   return common::compare_string(
//       (void *)left.value_.pointer_value_, left.length_, (void *)right.value_.pointer_value_, right.length_);
// }

// int TextType::match_like(const Value &left, const Value &right) const
// {
//   ASSERT(left.attr_type() == AttrType::CHARS && right.attr_type() == AttrType::CHARS, "invalid type");
//   return common::match_like_string(
//       (void *)left.value_.pointer_value_, (void *)right.value_.pointer_value_);
// }

RC TextType::set_value_from_str(Value &val, const string &data) const
{
  // val.set_string(data.c_str());

  // 暂时不懂这里的逻辑，待会再实现

  return RC::SUCCESS;
}

RC TextType::cast_to(const Value &val, AttrType type, Value &result) const
{
  switch (type) {
    case AttrType::CHARS: {
      result.attr_type_ = AttrType::CHARS;
      result.set_string(val.value_.pointer_value_, 4 /* 默认，暂设为4 */);
    } break;
    case AttrType::NULLS: {
      result.attr_type_ = AttrType::NULLS;
      result.set_null();
    } break;
    default: return RC::UNIMPLEMENTED;
  }
  return RC::SUCCESS;
}

int TextType::cast_cost(AttrType type)
{
  if (type == AttrType::CHARS) {
    return 0;
  }
  if (type == AttrType::NULLS) {
    return 1;
  }
  return INT32_MAX;
}

RC TextType::to_string(const Value &val, string &result) const
{
  stringstream ss;
  ss << val.value_.pointer_value_;
  result = ss.str();
  return RC::SUCCESS;
}