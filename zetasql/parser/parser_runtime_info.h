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

#ifndef ZETASQL_PARSER_PARSER_RUNTIME_INFO_H_
#define ZETASQL_PARSER_PARSER_RUNTIME_INFO_H_

#include <cstdint>

#include "zetasql/common/timer_util.h"
#include "zetasql/public/language_options.h"
#include "zetasql/public/options.pb.h"
#include "zetasql/public/proto/logging.pb.h"
#include "absl/base/attributes.h"

namespace zetasql {

inline ExecutionStats::ParserVariant GetPrimaryParser(
    const LanguageOptions& language_options) {
  return language_options.LanguageFeatureEnabled(FEATURE_TEXTMAPPER_PARSER)
             ? ExecutionStats::PARSER_TEXTMAPPER
             : ExecutionStats::PARSER_BISON;
}

inline ExecutionStats::ParserVariant GetShadowParser(
    const LanguageOptions& language_options) {
  if (!language_options.LanguageFeatureEnabled(FEATURE_SHADOW_PARSING)) {
    return ExecutionStats::PARSER_UNSPECIFIED;
  }
  return language_options.LanguageFeatureEnabled(FEATURE_TEXTMAPPER_PARSER)
             ? ExecutionStats::PARSER_BISON
             : ExecutionStats::PARSER_TEXTMAPPER;
}

class ParserRuntimeInfo {
 public:
  explicit ParserRuntimeInfo(const LanguageOptions& language_options)
  {}
  // Used only for analyzer_output's compatibility. "
  ABSL_DEPRECATED("Use the constructor that takes a LanguageOptions.")
  ParserRuntimeInfo()
  {}

  internal::TimedValue& parser_timed_value() { return parser_timed_value_; }
  const internal::TimedValue& parser_timed_value() const {
    return parser_timed_value_;
  }

  void AccumulateAll(const ParserRuntimeInfo& rhs) {
    parser_timed_value_.Accumulate(rhs.parser_timed_value_);

    num_lexical_tokens_ += rhs.num_lexical_tokens_;
  }

  void add_lexical_tokens(int64_t tokens) { num_lexical_tokens_ += tokens; }

  int64_t num_lexical_tokens() const { return num_lexical_tokens_; }

  AnalyzerLogEntry log_entry() const {
    AnalyzerLogEntry entry;
    entry.set_num_lexical_tokens(num_lexical_tokens());

    auto add_timing = [&](AnalyzerLogEntry::LoggedOperationCategory op,
                          const internal::TimedValue& time,
                          const ExecutionStats::ParserVariant parser_variant) {
      if (!time.HasAnyRecordedTiming()) return;
      auto& stage = *entry.add_execution_stats_by_op();
      stage.set_key(op);
      *stage.mutable_value() = time.ToExecutionStatsProto();
    };
    add_timing(AnalyzerLogEntry::PARSER, parser_timed_value(),
               ExecutionStats::PARSER_BISON);
    return entry;
  }

 private:

  internal::TimedValue parser_timed_value_;
  int64_t num_lexical_tokens_ = 0;
};

}  // namespace zetasql
#endif  // ZETASQL_PARSER_PARSER_RUNTIME_INFO_H_
