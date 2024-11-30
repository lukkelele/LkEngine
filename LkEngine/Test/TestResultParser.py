#--------------------------------------------------------------------------
# [LkEngine] TestResultParser.py
#
#--------------------------------------------------------------------------
import yaml
import json
import sys

def ParseTestResults(Filepath):
    try:
        # Load the YAML file.
        with open(Filepath, "r") as file:
            data = yaml.safe_load(file)

        # Extract the test results.
        tests = data.get('AutomationTest', [])
        total = len(tests)
        passed = 0

        for test in tests:
            for test_name, result in test.items():
                if result == 1:
                    passed += 1

        failed = total - passed
        results_summary = {
            "total": total,
            "passed": passed,
            "failed": failed
        }

        # Save summary to a JSON file.
        with open(f"{Filepath}-Summary.json", "w") as summary_file:
            json.dump(results_summary, summary_file)

        print(f"Test Results: {passed}/{total} passed")
        return 0 if failed == 0 else 1

    except Exception as e:
        print(f"[TestResultParser] Error parsing the YAML file: {e}")
        return 1


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print(f"[TestResultParser] Error occured, passed {len(sys.argv)} arguments, required to be at least 2")
        sys.exit(1)

    results_file = sys.argv[1]
    exit_code = ParseTestResults(results_file)
    sys.exit(exit_code)
