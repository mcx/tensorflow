/* Copyright 2024 The OpenXLA Authors.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#ifndef XLA_SERVICE_GPU_TRANSFORMS_COLLECTIVES_GPU_COLLECTIVE_COMBINER_UTILS_H_
#define XLA_SERVICE_GPU_TRANSFORMS_COLLECTIVES_GPU_COLLECTIVE_COMBINER_UTILS_H_

#include "absl/status/status.h"
#include "xla/hlo/ir/hlo_instruction.h"
#include "xla/hlo/ir/hlo_module.h"
#include "xla/hlo/ir/hlo_schedule.h"

namespace xla::gpu {

// Adds information that `instr` has been pipelined to the
// `CollectiveBackendInfo`. It is up to the caller to decide when to invoke
// this.
absl::Status AppendPipelinedInstruction(HloInstruction* instr,
                                        HloInstruction* new_while_instr);

// Returns true if `instr` is a pipelined collective instruction.
bool IsPipelinedCollective(const HloInstruction& instr);

// Returns true if module contains any pipelined instruction. False otherwise.
bool ContainsPipelinedInstruction(const HloModule& module);

}  // namespace xla::gpu

#endif  // XLA_SERVICE_GPU_TRANSFORMS_COLLECTIVES_GPU_COLLECTIVE_COMBINER_UTILS_H_
