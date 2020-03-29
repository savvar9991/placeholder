// clang-format off
// DO NOT EDIT: this file is auto-generated by caf-generate-enum-strings.
// Run the target update-enum-strings if this file is out of sync.
#include <nil/actor/intrusive/task_result.hpp>

#include <string>

namespace nil { namespace actor {
namespace intrusive {

std::string to_string(task_result x) {
  switch(x) {
    default:
      return "???";
    case task_result::resume:
      return "resume";
    case task_result::skip:
      return "skip";
    case task_result::stop:
      return "stop";
    case task_result::stop_all:
      return "stop_all";
  };
}

} // namespace intrusive
}}
