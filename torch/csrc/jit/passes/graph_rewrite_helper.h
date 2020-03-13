#pragma once

#include <torch/csrc/jit/ir/ir.h>
#include <torch/csrc/jit/ir/irparser.h>

namespace torch {
namespace jit {
namespace graph_rewrite_helper {

struct ConvBNParameters {
  at::Tensor conv_w;
  at::Tensor conv_b;
  at::Tensor bn_rm;
  at::Tensor bn_rv;
  double bn_eps = 0.0;
  at::Tensor bn_w;
  at::Tensor bn_b;
};

/**
 * Given the current weight and bias tensors of a Conv2d module and parameters
 * of the BatchNorm2d module we're folding with, compute the updated values
 * for the weight and bias.
 *
 * The function is basically copied from torch/nn/utils/fusion.py
 */
std::tuple<at::Tensor, at::Tensor>
  computeUpdatedConvWeightAndBias(const ConvBNParameters& p);

std::string getFuncName(Value* func_value);
Value* getValue(
    const std::string& name,
    const std::unordered_map<const Value*, Value*>& match_vmap,
    const std::unordered_map<std::string, Value*>& vmap);
c10::optional<IValue> getIValue(
    const std::string& name,
    const std::unordered_map<const Value*, Value*>& match_vmap,
    const std::unordered_map<std::string, Value*>& vmap);
void replaceConvolutionWithConv2d(std::shared_ptr<Graph>& graph);

// This struct contains a compiled IR pattens slated for use in the
// findPatternMatches function. The struct encapsulates the common
// information from parseIR that is used in conjunction with the
// pattern matching facility. A const instance of this struct can
// also be stored away to cache the compiled IR pattern and reduce
// runtime cost
struct PatternInfo {
  std::string pattern_string;
  std::unique_ptr<Graph> pattern_graph;
  std::unordered_map<std::string, Value*> vmap;

  static PatternInfo parse_from_str(std::string pattern_string) {
    PatternInfo rv{
        std::move(pattern_string), std::make_unique<Graph>(), decltype(vmap){}};
    parseIR(rv.pattern_string, rv.pattern_graph.get(), rv.vmap);
    return rv;
  }
};

} // namespace graph_rewrite_helper
} // namespace jit
} // namespace torch
