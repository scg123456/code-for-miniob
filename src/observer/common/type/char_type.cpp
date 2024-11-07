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
#include "common/type/char_type.h"
#include "common/value.h"
#include "common/time/datetime.h"

int CharType::compare(const Value &left, const Value &right) const
{
  ASSERT(left.attr_type() == AttrType::CHARS && right.attr_type() == AttrType::CHARS, "invalid type");
  return common::compare_string(
      (void *)left.value_.pointer_value_, left.length_, (void *)right.value_.pointer_value_, right.length_);
}

int CharType::match_like(const Value &left, const Value &right) const
{
  ASSERT(left.attr_type() == AttrType::CHARS && right.attr_type() == AttrType::CHARS, "invalid type");
  return common::match_like_string(
      (void *)left.value_.pointer_value_, (void *)right.value_.pointer_value_);
}

RC CharType::set_value_from_str(Value &val, const string &data) const
{
  val.set_string(data.c_str());
  return RC::SUCCESS;
}

RC CharType::cast_to(const Value &val, AttrType type, Value &result) const
{
  switch (type) {
    case AttrType::DATES: {
      result.attr_type_ = AttrType::DATES;
      int y, m, d;
      if (3 != sscanf(val.value_.pointer_value_, "%d-%d-%d", &y, &m, &d)) {
        LOG_WARN("invalid date format: s=%s", val.value_.pointer_value_);
        return RC::INVALID_ARGUMENT;
      }
      bool check_result = common::DateTime::check_date(y, m, d);
      if (!check_result) {
        LOG_WARN("invalid date: y=%d, m=%d, d=%d", y, m, d);
        return RC::INVALID_ARGUMENT;
      }
      result.set_date(y, m, d);
    } break;
    case AttrType::INTS: {
      result.attr_type_ = AttrType::INTS;
      char* end;
      errno = 0; // 重置 errno 以检测溢出
      long int_value = strtol(val.value_.pointer_value_, &end, 10);
      
      // 检查是否有数字被解析
      if (end == val.value_.pointer_value_) {
        // 没有数字，设置结果为0
        result.set_int(0);
      } else {
        // 检查是否超出 int 范围
        if ((int_value > INT32_MAX) || (int_value < INT32_MIN)) {
          LOG_WARN("integer overflow: s=%s", val.value_.pointer_value_);
          return RC::INVALID_ARGUMENT;
        }
        result.set_int(static_cast<int>(int_value));
      }
    } break;
    case AttrType::FLOATS: {
      result.attr_type_ = AttrType::FLOATS;
      char* end;
      errno = 0; // 重置 errno 以检测溢出
      double float_value = strtod(val.value_.pointer_value_, &end);
      
      // 检查是否有数字被解析
      if (end == val.value_.pointer_value_) {
        // 没有数字，设置结果为0.0
        result.set_float(0.0);
      } else {
        // 检查是否超出 double 范围
        if (errno == ERANGE) {
          LOG_WARN("float overflow or underflow: s=%s", val.value_.pointer_value_);
          return RC::INVALID_ARGUMENT;
        }
        result.set_float(static_cast<float>(float_value));
      }
    } break;
    default: return RC::UNIMPLEMENTED;
  }
  return RC::SUCCESS;
}

int CharType::cast_cost(AttrType type)
{
  if (type == AttrType::CHARS) {
    return 0;
  }
  if (type == AttrType::DATES) {
    return 1;
  }
  if (type == AttrType::INTS) {
    return 2;
  }
  if (type == AttrType::FLOATS) {
    return 3;
  }
  return INT32_MAX;
}

RC CharType::to_string(const Value &val, string &result) const
{
  stringstream ss;
  ss << val.value_.pointer_value_;
  result = ss.str();
  return RC::SUCCESS;
}