//
// Copyright 2019 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#ifndef ZETASQL_TOOLS_EXECUTE_QUERY_EXECUTE_QUERY_WRITER_H_
#define ZETASQL_TOOLS_EXECUTE_QUERY_EXECUTE_QUERY_WRITER_H_

#include <iosfwd>
#include <memory>
#include <ostream>

#include "zetasql/public/evaluator_table_iterator.h"
#include "zetasql/resolved_ast/resolved_node.h"
#include "absl/status/status.h"
#include "absl/strings/string_view.h"

namespace zetasql {

class ExecuteQueryWriter {
 public:
  virtual ~ExecuteQueryWriter() = default;

  virtual absl::Status parsed(absl::string_view parse_debug_string) {
    return WriteOperationString("parsed", parse_debug_string);
  }
  virtual absl::Status unparsed(absl::string_view unparse_string) {
    return WriteOperationString("unparsed", unparse_string);
  }

  virtual absl::Status resolved(const ResolvedNode& ast) {
    return absl::UnimplementedError(
        "ExecuteQueryWriter::resolved is not implemented");
  }
  virtual absl::Status unanalyze(absl::string_view unanalyze_string) {
    return WriteOperationString("unanalyze", unanalyze_string);
  }

  virtual absl::Status explained(const ResolvedNode& ast,
                                 absl::string_view explain) {
    return absl::UnimplementedError(
        "ExecuteQueryWriter::explained is not implemented");
  }
  virtual absl::Status executed(const ResolvedNode& ast,
                                std::unique_ptr<EvaluatorTableIterator> iter) {
    return absl::UnimplementedError(
        "ExecuteQueryWriter::executed is not implemented");
  }
  virtual absl::Status ExecutedExpression(const ResolvedNode& ast,
                                          const Value& value) {
    return absl::UnimplementedError(
        "ExecuteQueryWriter::executed is not implemented");
  }

 protected:
  virtual absl::Status WriteOperationString(absl::string_view operation_name,
                                            absl::string_view str) {
    return absl::UnimplementedError(
        absl::StrCat("ExecuteQueryWriter does not implement ", operation_name));
  }
};

// Writes a human-readable representation of the query result to an output
// stream.
class ExecuteQueryStreamWriter : public ExecuteQueryWriter {
 public:
  explicit ExecuteQueryStreamWriter(std::ostream&);
  ExecuteQueryStreamWriter(const ExecuteQueryStreamWriter&) = delete;
  ExecuteQueryStreamWriter& operator=(const ExecuteQueryStreamWriter&) = delete;

  absl::Status resolved(const ResolvedNode& ast) override;
  absl::Status explained(const ResolvedNode& ast,
                         absl::string_view explain) override;
  absl::Status executed(const ResolvedNode& ast,
                        std::unique_ptr<EvaluatorTableIterator> iter) override;
  absl::Status ExecutedExpression(const ResolvedNode& ast,
                                  const Value& value) override;

 protected:
  absl::Status WriteOperationString(absl::string_view operation_name,
                                    absl::string_view str) override {
    stream_ << str << std::endl;
    return absl::OkStatus();
  }

 private:
  std::ostream& stream_;
};

}  // namespace zetasql

#endif  // ZETASQL_TOOLS_EXECUTE_QUERY_EXECUTE_QUERY_WRITER_H_
