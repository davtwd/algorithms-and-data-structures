#include <Cli/lib/dsh/dst.h>

namespace dst {

    std::vector<std::unique_ptr<ds::utils::Analyzer>> createAnalyzers() {
        std::vector<std::unique_ptr<ds::utils::Analyzer>> analyzers;
        analyzers.emplace_back(std::make_unique<ds::utils::ListsAnalyzer>());
        return analyzers;
    }

    void runTests(const std::vector<std::unique_ptr<ds::tests::Test>>& tests) {
        for (const std::unique_ptr<ds::tests::Test>& test : tests) {
            test->run();
        }
    }

    void printResults(const std::vector<std::unique_ptr<ds::tests::Test>>& tests) {
        for (const std::unique_ptr<ds::tests::Test>& test : tests) {
            ds::tests::console_print_results(*test, ds::tests::ConsoleOutputType::NoLeaf);
        }
    }

    void runAnalyzers(const std::vector<std::unique_ptr<ds::utils::Analyzer>>& analyzers) {
        for (const std::unique_ptr<ds::utils::Analyzer>& analyzer : analyzers) {
            analyzer->analyze();
        }
    }

    void setAnalyzersParameters(
        const std::vector<std::unique_ptr<ds::utils::Analyzer>>& analyzers,
        const std::string& path
    ) {
        for (const std::unique_ptr<ds::utils::Analyzer>& analyzer : analyzers) {
            analyzer->setOutputDirectory(path);
            analyzer->setReplicationCount(100);
            analyzer->setStepSize(10'000);
            analyzer->setStepCount(10);
        }
    }

} // namespace datastructures tests