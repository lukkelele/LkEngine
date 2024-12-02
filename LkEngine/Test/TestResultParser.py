#--------------------------------------------------------------------------
# [LkEngine] TestResultParser.py
# 
# Parse and summarize test results.
# Generates a shield.io badge that is used to display the test report in
# the README.
#
# Usage:
#        TestResultParser.py <TEST_RESULTS>
#--------------------------------------------------------------------------
import yaml
import json
import os
import sys
import re
from pathlib import Path


def ParseTestResults(Filepath):
    bYamlFileParsed = False
    try:
        # Load the YAML file.
        with open(Filepath, "r") as file:
            data = yaml.safe_load(file)

        # Extract the test results.
        tests = data.get(f"{TestSuite}", [])
        total = len(tests)
        if total == 0:
            print(f"Found 0 tests for {TestSuite}")
            return 1

        passed_tests = []
        failed_tests = []

        for test in tests:
            for test_name, result in test.items():
                if result == 1:
                    passed_tests.append(test_name)
                else:
                    failed_tests.append(test_name)

        failed = len(failed_tests)
        passed = len(passed_tests)

        result_summary = {
            "Total": total,
            "Passed": {
                "Count": passed,
                "Tests": passed_tests
            },
            "Failed": {
                "Count": failed,
                "Tests": failed_tests
            }
        }

        print(f"Test Results: {result_summary['Passed']['Count']}/{total} passed ({100 * float(passed/total):.1f}%)")
        bYamlFileParsed = True

        # Save summary to a JSON file.
        ResultDir = Path(Filepath).parent
        with open(f"{ResultDir}/{SummaryFile}", "w") as summary_file:
            json.dump(result_summary, summary_file, indent=4)
            print(f"Dumped test results at: {summary_file}")

        # Generate badge used on the README.
        # * All tests OK:   Green
        # * >50% tests OK:  Yellow
        # * <=50% tests OK: Red
        BadgeData = {
            "schemaVersion": 1,
            "label": f"{TestSuite}",
            "message": f"{passed}/{total}",
            "color": (
                "success" if failed == 0 else 
                "warning" if passed / total >= 0.50 else
                "critical"
            )
        }

        print(f"Badge Data: {BadgeData}")
        BadgePath = ResultDir / BadgeFile
        with open(BadgePath, "w") as badge_file:
            json.dump(BadgeData, badge_file, indent=4)
            print(f"Badge JSON created at: {BadgePath}")

        return 0

    except Exception as e:
        if not bYamlFileParsed:
            print(f"[TestResultParser] Parsing of YAML file failed\n * File: {Filepath}\n * {e}")
        else:
            print(f"[TestResultParser] Failed to save test summary\n * {e}")
        return 1


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print(f"[TestResultParser] Error occured, number of least required arguments are 2 but got {len(sys.argv)}")
        sys.exit(1)

    results_file = sys.argv[1]

    # Extract the text between '-' and '.yaml'.
    match = re.match(r"^.*-(.*?)\.yaml$", os.path.basename(results_file))
    if not match:
        print(f"Failed to extract test suite from filename: {results_file}")
        sys.exit(1)

    TestSuite = match.group(1)
    SummaryFile = f"{TestSuite}-Summary.json"
    BadgeFile = f"{TestSuite}-Badge.json"
    print(f"Results File: {results_file}")
    print(f"TestSuite: {TestSuite}")
    print(f"Badge File: {BadgeFile}")

    exit_code = ParseTestResults(results_file)
    sys.exit(exit_code)
