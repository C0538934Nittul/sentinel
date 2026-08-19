/**
 * @file main.cpp
 * @brief Command-line entry point for sentinel-analyzer.
 * @component analyzer (executable)
 * @status COMPLETE. This is plumbing (argument parsing, I/O, exit-code mapping), not assessed
 *         logic -- the actual analysis happens inside the stubbed ThreatAnalyzer::analyze() and
 *         the stubbed DetectionRule subclasses this program calls into.
 *
 * Usage:
 *   sentinel-analyzer --events <path> --config <path> [--out <path>] [--log <path>] [--pretty]
 *   sentinel-analyzer --stdin --config <path>
 *   sentinel-analyzer --help | --version
 *
 * Exit codes:
 *   0  success
 *   2  bad command-line arguments
 *   3  file error (could not open events/config/output/log file)
 *   4  parse or validation error (malformed JSON, schema violation)
 *   5  unexpected/internal error
 *
 * On any error (exit codes 2-5), a single structured JSON object is written to stderr:
 *   { "error": { "code": "<EXIT_CODE_NAME>", "message": "<human-readable message>" } }
 * so a calling process (the API's analyzerService) can parse it directly instead of scraping
 * free-form text.
 */

#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "sentinel/ConfigReader.hpp"
#include "sentinel/EventReader.hpp"
#include "sentinel/IncidentResult.hpp"
#include "sentinel/Logger.hpp"
#include "sentinel/RuleFactory.hpp"
#include "sentinel/SentinelException.hpp"
#include "sentinel/ThreatAnalyzer.hpp"

namespace {

constexpr const char* kVersion = "0.1.0-dev";

constexpr int kExitOk = 0;
constexpr int kExitBadArgs = 2;
constexpr int kExitFileError = 3;
constexpr int kExitParseValidationError = 4;
constexpr int kExitUnexpectedError = 5;

struct CliOptions {
    std::optional<std::string> eventsPath;
    std::optional<std::string> configPath;
    std::optional<std::string> outPath;
    std::string logPath = "sentinel-analyzer.log";
    bool useStdin = false;
    bool pretty = false;
    bool showHelp = false;
    bool showVersion = false;
};

void printUsage(std::ostream& out) {
    out << "sentinel-analyzer -- Sentinel security-event analysis engine\n\n"
        << "Usage:\n"
        << "  sentinel-analyzer --events <path> --config <path> [--out <path>] "
           "[--log <path>] [--pretty]\n"
        << "  sentinel-analyzer --stdin --config <path>\n"
        << "  sentinel-analyzer --help | --version\n";
}

void writeErrorJson(const std::string& code, const std::string& message) {
    nlohmann::json errorJson = {{"error", {{"code", code}, {"message", message}}}};
    std::cerr << errorJson.dump() << std::endl;
}

/**
 * @brief Parse argv into CliOptions.
 * @throws std::invalid_argument on unknown flags, missing values, or an invalid combination
 *         (e.g. neither --events nor --stdin given, or both given).
 */
CliOptions parseArgs(int argc, char** argv) {
    CliOptions options;
    std::vector<std::string> args(argv + 1, argv + argc);

    auto requireValue = [&](std::size_t index, const std::string& flag) -> const std::string& {
        if (index + 1 >= args.size()) {
            throw std::invalid_argument("missing value for " + flag);
        }
        return args[index + 1];
    };

    for (std::size_t i = 0; i < args.size(); ++i) {
        const std::string& arg = args[i];
        if (arg == "--help") {
            options.showHelp = true;
        } else if (arg == "--version") {
            options.showVersion = true;
        } else if (arg == "--stdin") {
            options.useStdin = true;
        } else if (arg == "--pretty") {
            options.pretty = true;
        } else if (arg == "--events") {
            options.eventsPath = requireValue(i, arg);
            ++i;
        } else if (arg == "--config") {
            options.configPath = requireValue(i, arg);
            ++i;
        } else if (arg == "--out") {
            options.outPath = requireValue(i, arg);
            ++i;
        } else if (arg == "--log") {
            options.logPath = requireValue(i, arg);
            ++i;
        } else {
            throw std::invalid_argument("unknown argument: " + arg);
        }
    }

    if (options.showHelp || options.showVersion) {
        return options;
    }

    if (!options.configPath.has_value()) {
        throw std::invalid_argument("--config is required");
    }
    if (options.useStdin && options.eventsPath.has_value()) {
        throw std::invalid_argument("--stdin and --events are mutually exclusive");
    }
    if (!options.useStdin && !options.eventsPath.has_value()) {
        throw std::invalid_argument("one of --events <path> or --stdin is required");
    }

    return options;
}

}  // namespace

int main(int argc, char** argv) {
    CliOptions options;
    try {
        options = parseArgs(argc, argv);
    } catch (const std::invalid_argument& ex) {
        writeErrorJson("BAD_ARGUMENTS", ex.what());
        printUsage(std::cerr);
        return kExitBadArgs;
    }

    if (options.showHelp) {
        printUsage(std::cout);
        return kExitOk;
    }
    if (options.showVersion) {
        std::cout << "sentinel-analyzer " << kVersion << '\n';
        return kExitOk;
    }

    try {
        sentinel::Logger logger(options.logPath);
        logger.info("sentinel-analyzer starting, version " + std::string(kVersion));

        const sentinel::AnalysisConfig config = sentinel::ConfigReader::readFromFile(*options.configPath);
        logger.info("loaded config from " + *options.configPath);

        std::vector<sentinel::SecurityEvent> events;
        if (options.useStdin) {
            events = sentinel::EventReader::readFromStream(std::cin);
            logger.info("loaded events from stdin");
        } else {
            events = sentinel::EventReader::readFromFile(*options.eventsPath);
            logger.info("loaded events from " + *options.eventsPath);
        }

        auto rules = sentinel::RuleFactory::buildRules(config);
        sentinel::ThreatAnalyzer analyzer;
        for (auto& rule : rules) {
            analyzer.addRule(std::move(rule));
        }
        logger.info("registered " + std::to_string(analyzer.ruleCount()) + " rule(s)");

        const std::vector<sentinel::IncidentResult> incidents = analyzer.analyze(events);
        logger.info("analysis produced " + std::to_string(incidents.size()) + " incident(s)");

        nlohmann::json resultJson = nlohmann::json::array();
        for (const auto& incident : incidents) {
            resultJson.push_back(incident.toJson());
        }
        nlohmann::json output = {{"incidents", resultJson}};

        const int indent = options.pretty ? 2 : -1;
        if (options.outPath.has_value()) {
            std::ofstream outFile(*options.outPath);
            if (!outFile.is_open()) {
                throw sentinel::FileError("could not open output file: " + *options.outPath);
            }
            outFile << output.dump(indent) << '\n';
        } else {
            std::cout << output.dump(indent) << '\n';
        }

        logger.info("sentinel-analyzer finished successfully");
        return kExitOk;

    } catch (const sentinel::FileError& ex) {
        writeErrorJson("FILE_ERROR", ex.what());
        return kExitFileError;
    } catch (const sentinel::ParseError& ex) {
        writeErrorJson("PARSE_ERROR", ex.what());
        return kExitParseValidationError;
    } catch (const sentinel::ValidationError& ex) {
        writeErrorJson("VALIDATION_ERROR", ex.what());
        return kExitParseValidationError;
    } catch (const sentinel::ConfigError& ex) {
        writeErrorJson("CONFIG_ERROR", ex.what());
        return kExitParseValidationError;
    } catch (const sentinel::SentinelException& ex) {
        writeErrorJson("SENTINEL_ERROR", ex.what());
        return kExitUnexpectedError;
    } catch (const std::exception& ex) {
        writeErrorJson("UNEXPECTED_ERROR", ex.what());
        return kExitUnexpectedError;
    } catch (...) {
        writeErrorJson("UNEXPECTED_ERROR", "unknown non-standard exception");
        return kExitUnexpectedError;
    }
}
