#pragma once

#include <DataStructures/tests/root.h>
#include <DataStructures/complexities/list_analyzer.h>

#ifndef ANALYZER_OUTPUT
#define ANALYZER_OUTPUT "."
#endif

namespace dst {

    std::vector<std::unique_ptr<ds::utils::Analyzer>> createAnalyzers();
    void runTests(const std::vector<std::unique_ptr<ds::tests::Test>>& tests);
    void printResults(const std::vector<std::unique_ptr<ds::tests::Test>>& tests);
    void runAnalyzers(const std::vector<std::unique_ptr<ds::utils::Analyzer>>& analyzers);
    void setAnalyzersParameters(const std::vector<std::unique_ptr<ds::utils::Analyzer>>& analyzers, const std::string& path);

} // namespace datastructures tests