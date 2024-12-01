#--------------------------------------------------------------------------
# [LkEngine] TestResultParser.py
# 
# Parse and summarize test results.
# Generates a shield.io badge that is used to display the test report in
# the README.
#--------------------------------------------------------------------------
import yaml
import json
import sys
from pathlib import Path

# TODO: This should be dynamic, hardcoded for now.
TestSuite = "AutomationTest"
SummaryFile = f"Summary.json"
BadgeFile = f"{TestSuite}-Badge.json"

def ParseTestResults(Filepath):
    bYamlFileParsed = False
    try:
        # Load the YAML file.
        with open(Filepath, "r") as file:
            data = yaml.safe_load(file)

        # Extract the test results.
        tests = data.get(f"{TestSuite}", [])
        total = len(tests)
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
            json.dump(result_summary, summary_file)
            print(f"Dumped test results at: {summary_file}")

        #=============================================
        # Generate badge for shields.io
        #=============================================
        badge_data = {
            "schemaVersion": 1,
            "label": "tests",
            "message": f"{passed}/{total}",
            "color": "success" if failed == 0 else "critical"
        }

        badge_path = ResultDir / BadgeFile
        with open(badge_path, "w") as badge_file:
            json.dump(badge_data, badge_file, indent=4)
            print(f"Badge JSON created at: {badge_path}")

        return 0 if failed == 0 else 1

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
    print(f"Results File: {results_file}")
    exit_code = ParseTestResults(results_file)
    sys.exit(exit_code)
