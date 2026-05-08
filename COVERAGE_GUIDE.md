# Robot Hand Coverage Report Script

This repository includes a convenient script to run tests and generate coverage reports for the robot_hand module.

## Quick Start

```bash
./run_robot_hand_coverage.sh
```

## Usage

```bash
./run_robot_hand_coverage.sh [options]
```

### Options

- `--help` - Display help message
- `--clean` - Clean build artifacts before running (recommended for first run or after major changes)
- `--view` - Automatically open the coverage report in your default browser

## Examples

### Run coverage with clean rebuild
```bash
./run_robot_hand_coverage.sh --clean
```

### Generate and view coverage report
```bash
./run_robot_hand_coverage.sh --view
```

### Full options
```bash
./run_robot_hand_coverage.sh --clean --view
```

## What the Script Does

1. **Configures** CMake with coverage flags (`--coverage`, `-O0 -g`, etc.)
2. **Builds** the robot_hand test suite with instrumentation
3. **Clears** old coverage data files (`.gcda`)
4. **Runs** all robot_hand-related unit tests
5. **Generates** a coverage report using `gcovr`
6. **Displays** a summary showing line, function, and branch coverage percentages

## Output

- Terminal: Coverage summary and metrics
- JSON: `build/host-coverage/robot_hand_src_coverage.json` - Machine-readable coverage data
- HTML (optional): `/tmp/robot_hand_coverage.html` - Interactive coverage report (with `--view`)

## Requirements

- CMake 3.24+
- Ninja build system
- GCC with coverage support (`gcov`)
- Python 3 with `gcovr` installed
- GoogleTest (included in project)

## Interpreting Results

```
Lines:      95.2% (474 out of 498)    - Line coverage percentage
Functions:  97.6% (120 out of 123)    - Function coverage percentage
Branches:   66.5% (266 out of 400)    - Branch coverage percentage
```

Higher percentages indicate better test coverage. The goal is to maintain at least 95% line coverage.

## Notes

- First run will take longer as it needs to configure and build
- Subsequent runs reuse the build (faster, unless `--clean` is used)
- Coverage data is isolated in `build/host-coverage/` to avoid affecting other builds
- The script uses thread parallelization (`-j$(nproc)`) for faster builds
