// clang-format off
// DO NOT EDIT: this file is auto-generated by caf-generate-enum-strings.
// Run the target update-enum-strings if this file is out of sync.
#include <nil/actor/pec.hpp>

#include <string>

namespace nil { namespace actor {

std::string to_string(pec x) {
  switch(x) {
    default:
      return "???";
    case pec::success:
      return "success";
    case pec::trailing_character:
      return "trailing_character";
    case pec::unexpected_eof:
      return "unexpected_eof";
    case pec::unexpected_character:
      return "unexpected_character";
    case pec::timespan_overflow:
      return "timespan_overflow";
    case pec::fractional_timespan:
      return "fractional_timespan";
    case pec::too_many_characters:
      return "too_many_characters";
    case pec::invalid_escape_sequence:
      return "invalid_escape_sequence";
    case pec::unexpected_newline:
      return "unexpected_newline";
    case pec::integer_overflow:
      return "integer_overflow";
    case pec::integer_underflow:
      return "integer_underflow";
    case pec::exponent_underflow:
      return "exponent_underflow";
    case pec::exponent_overflow:
      return "exponent_overflow";
    case pec::type_mismatch:
      return "type_mismatch";
    case pec::not_an_option:
      return "not_an_option";
    case pec::invalid_argument:
      return "invalid_argument";
    case pec::missing_argument:
      return "missing_argument";
    case pec::invalid_category:
      return "invalid_category";
    case pec::invalid_field_name:
      return "invalid_field_name";
    case pec::repeated_field_name:
      return "repeated_field_name";
    case pec::missing_field:
      return "missing_field";
    case pec::invalid_range_expression:
      return "invalid_range_expression";
  };
}

}}
