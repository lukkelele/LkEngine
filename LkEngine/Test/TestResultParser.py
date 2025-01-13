#--------------------------------------------------------------------------
# [LkEngine] TestResultParser.py
# 
# Parse and summarize test Results.
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
    print(f"Parsing test results", flush=True)
    bYamlFileParsed = False
    try:
        # Load the YAML file.
        with open(Filepath, "r") as File:
            Data = yaml.safe_load(File)

        # Extract the test results.
        Tests = Data.get(f"{TestSuite}", [])
        Total = len(Tests)
        print(f"Found {Total} tests for {TestSuite}", flush=True)
        if Total == 0:
            return 1

        PassedTests = []
        FailedTests = []

        for Test in Tests:
            for TestName, Result in Test.items():
                if Result == 1:
                    PassedTests.append(TestName)
                else:
                    FailedTests.append(TestName)

        Failed = len(FailedTests)
        Passed = len(PassedTests)

        ResultSummary = {
            "Total": Total,
            "Passed": {
                "Count": Passed,
                "Tests": PassedTests
            },
            "Failed": {
                "Count": Failed,
                "Tests": FailedTests
            }
        }

        print(f"Test Results: {ResultSummary['Passed']['Count']}/{Total} passed ({100 * float(Passed/Total):.1f}%)", flush=True)
        bYamlFileParsed = True

        # Save summary to a JSON file.
        ResultDir = Path(Filepath).parent
        with open(f"{ResultDir}/{SummaryFileName}", "w") as SummaryFile:
            json.dump(ResultSummary, SummaryFile, indent=4)
            print(f"Dumped test Results at: {SummaryFile}", flush=True)

        # Generate badge used on the README.
        # * All tests OK:   Green
        # * >50% tests OK:  Yellow
        # * <=50% tests OK: Red
        BadgeData = {
            "schemaVersion": 1,
            "label": f"{TestSuite}",
            "message": f"{Passed}/{Total}",
            "color": (
                "success" if Failed == 0 else 
                "warning" if Passed / Total >= 0.50 else "critical"
            )
        }

        print(f"Badge Data: {BadgeData}", flush=True)
        BadgePath = ResultDir / BadgeFileName
        with open(BadgePath, "w") as BadgeFile:
            json.dump(BadgeData, BadgeFile, indent=4)
            print(f"Badge JSON created at: {BadgePath}", flush=True)

        return 0

    except Exception as e:
        if not bYamlFileParsed:
            print(f"[TestResultParser] Parsing of YAML file failed\n * File: {Filepath}\n * {e}", flush=True)
        else:
            print(f"[TestResultParser] Failed to save test summary\n * {e}", flush=True)
        return 1


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print(f"[TestResultParser] Error occured, number of least required arguments are 2 but got {len(sys.argv)}", flush=True)
        sys.exit(1)

    TestResultFile = sys.argv[1]
    print(f"Results File: {TestResultFile}", flush=True)

    # Extract the text between '-' and '.yaml'.
    Match = re.match(r"^.*-(.*?)\.yaml$", os.path.basename(TestResultFile))
    if not Match:
        print(f"Failed to extract test suite from filename: {TestResultFile}")
        sys.exit(1)

    TestSuite = Match.group(1)
    #SummaryFileName = f"{TestSuite}-Summary.json"
    #BadgeFileName = f"{TestSuite}-Badge.json"
    SummaryFileName = f"Summary.json"
    BadgeFileName = f"Badge.json"
    print(f"TestSuite: {TestSuite}", flush=True)
    print(f"Badge File: {BadgeFileName}", flush=True)

    ExitCode = ParseTestResults(TestResultFile)
    sys.exit(ExitCode)
