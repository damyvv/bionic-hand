#!/bin/bash
#
# Robot Hand Test Suite & Coverage Report Generator
# 
# This script configures, builds, tests, and generates a coverage report
# for the robot_hand module.
#
# Usage: ./run_robot_hand_coverage.sh [options]
# Options:
#   --help              Show this help message
#   --clean             Clean build artifacts before running
#   --view              Open the coverage report in a browser (if available)
#   --gcovr-arg <arg>   Pass an extra argument to gcovr (repeatable)
#   --                  Forward all remaining arguments to gcovr

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build/host-coverage"
COVERAGE_JSON="${BUILD_DIR}/robot_hand_src_coverage.json"
CLEAN_BUILD=false
VIEW_REPORT=false
GCOVR_ARGS=("--exclude-unreachable-branches")

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --help)
            echo "Robot Hand Test Suite & Coverage Report Generator"
            echo ""
            echo "Usage: $0 [options]"
            echo "Options:"
            echo "  --help              Show this help message"
            echo "  --clean             Clean build artifacts before running"
            echo "  --view              Open the coverage report in a browser"
            echo "  --gcovr-arg <arg>   Pass an extra argument to gcovr (repeatable)"
            echo "  --                  Forward all remaining arguments to gcovr"
            echo ""
            exit 0
            ;;
        --clean)
            CLEAN_BUILD=true
            shift
            ;;
        --view)
            VIEW_REPORT=true
            shift
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

echo "=========================================="
echo "Robot Hand Test & Coverage Report"
echo "=========================================="
echo ""

# Clean if requested
if [ "$CLEAN_BUILD" = true ]; then
    echo "[1/6] Cleaning build artifacts..."
    rm -rf "$BUILD_DIR"
    echo "      ✓ Build directory cleaned"
    echo ""
fi

# Configure
echo "[2/6] Configuring CMake with coverage support..."
cmake -S "$SCRIPT_DIR" -B "$BUILD_DIR" \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_C_FLAGS='-O0 -g --coverage' \
    -DCMAKE_CXX_FLAGS='-O0 -g --coverage' \
    -DCMAKE_EXE_LINKER_FLAGS='--coverage' \
    -DCMAKE_SHARED_LINKER_FLAGS='--coverage' \
    > /dev/null 2>&1
echo "      ✓ CMake configured"
echo ""

# Build
echo "[3/6] Building robot_hand tests..."
cmake --build "$BUILD_DIR" --target runRobotHandTests -j$(nproc) > /dev/null 2>&1
echo "      ✓ Tests built"
echo ""

# Clean old coverage data
echo "[4/6] Clearing old coverage data..."
find "$BUILD_DIR" -name '*.gcda' -delete 2>/dev/null || true
echo "      ✓ Coverage data cleared"
echo ""

# Run tests
echo "[5/6] Running robot_hand tests..."
ctest --test-dir "$BUILD_DIR" \
    -R "(Pca9685|Servo|Finger|Hand|CommandLine|SerialInput|SerialOutput|SerialLineSource)" \
    --output-on-failure \
    2>&1 | tail -20
echo ""

# Generate coverage report
echo "[6/6] Generating coverage report..."
python3 -m gcovr -r "$SCRIPT_DIR" "$BUILD_DIR" \
    --filter '^src/robot_hand/' \
    --exclude '^src/robot_hand/main\.cpp$' \
    --txt \
    --txt-summary \
    --json "$COVERAGE_JSON" \
    "${GCOVR_ARGS[@]}" \
    2>&1 | tail -30
echo ""

# Summary
echo "=========================================="
echo "Coverage Report Summary"
echo "=========================================="
echo ""
echo "Full JSON report: $COVERAGE_JSON"
echo ""

# Extract and display key metrics from JSON
if command -v python3 &> /dev/null; then
    python3 - "$COVERAGE_JSON" << 'PYTHON_SCRIPT'
import json
import sys

try:
    with open(sys.argv[1], 'r') as f:
        data = json.load(f)
        if 'summary' in data:
            summary = data['summary']
            print(f"Lines:      {summary.get('line_percent', 'N/A'):.1f}% ({summary.get('lines_covered', 0)}/{summary.get('lines_total', 0)})")
            print(f"Functions:  {summary.get('function_percent', 'N/A'):.1f}% ({summary.get('functions_covered', 0)}/{summary.get('functions_total', 0)})")
            print(f"Branches:   {summary.get('branch_percent', 'N/A'):.1f}% ({summary.get('branches_covered', 0)}/{summary.get('branches_total', 0)})")
except Exception as e:
    print(f"Note: Could not parse coverage summary: {e}")
PYTHON_SCRIPT
fi

echo ""
echo "View coverage text report:"
python3 -m gcovr -r "$SCRIPT_DIR" "$BUILD_DIR" \
    --filter '^src/robot_hand/' \
    --exclude '^src/robot_hand/main\.cpp$' \
    --txt \
    "${GCOVR_ARGS[@]}"
echo ""
echo "✓ Coverage report generation complete!"
echo ""

# Optionally open in browser
if [ "$VIEW_REPORT" = true ]; then
    echo "Attempting to open coverage report..."
    mkdir -p ./.tmp
    python3 -m gcovr -r "$SCRIPT_DIR" "$BUILD_DIR" \
        --filter '^src/robot_hand/' \
        --exclude '^src/robot_hand/main\.cpp$' \
        --html-details ./.tmp/robot_hand_coverage.html \
        "${GCOVR_ARGS[@]}"
    
    echo "./.tmp/robot_hand_coverage.html"
fi
