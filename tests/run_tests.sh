#!/bin/bash
#
# Test runner for php2wasm
# Runs all tests and compares output with expected results
#

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
WASM_BINARY="../dist/php.wasm"
TEST_DIR="$(dirname "$0")"
OUTPUT_DIR="$TEST_DIR/output"
EXPECTED_DIR="$TEST_DIR/expected"

# Create output directory
mkdir -p "$OUTPUT_DIR"

# Test counter
TESTS_PASSED=0
TESTS_FAILED=0
TOTAL_TESTS=0

# Print colored output
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[PASS]${NC} $1"
}

print_error() {
    echo -e "${RED}[FAIL]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

# Check if WASM binary exists
check_wasm_binary() {
    if [[ ! -f "$WASM_BINARY" ]]; then
        print_error "WASM binary not found: $WASM_BINARY"
        print_info "Please run 'make release' first"
        exit 1
    fi
}

# Run a single test
run_test() {
    local test_file="$1"
    local test_name=$(basename "$test_file" .php)
    local output_file="$OUTPUT_DIR/${test_name}.out"
    local expected_file="$EXPECTED_DIR/${test_name}.txt"
    
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    
    print_info "Running test: $test_name"
    
    # Run the test
    if wasmtime run --dir=. "$WASM_BINARY" -- "$test_file" > "$output_file" 2>&1; then
        # Check if expected output exists
        if [[ -f "$expected_file" ]]; then
            # Compare output with expected
            if diff -q "$output_file" "$expected_file" > /dev/null; then
                print_success "$test_name - Output matches expected"
                TESTS_PASSED=$((TESTS_PASSED + 1))
            else
                print_error "$test_name - Output does not match expected"
                print_info "Expected:"
                cat "$expected_file"
                print_info "Actual:"
                cat "$output_file"
                TESTS_FAILED=$((TESTS_FAILED + 1))
            fi
        else
            print_warning "$test_name - No expected output file, test passed"
            TESTS_PASSED=$((TESTS_PASSED + 1))
        fi
    else
        print_error "$test_name - Test execution failed"
        print_info "Output:"
        cat "$output_file"
        TESTS_FAILED=$((TESTS_FAILED + 1))
    fi
}

# Run all tests
run_all_tests() {
    print_info "Starting test suite..."
    print_info "WASM binary: $WASM_BINARY"
    print_info "Test directory: $TEST_DIR"
    print_info "Output directory: $OUTPUT_DIR"
    echo
    
    # Find all PHP test files
    local test_files=($(find "$TEST_DIR" -name "test_*.php" -type f))
    
    if [[ ${#test_files[@]} -eq 0 ]]; then
        print_error "No test files found"
        exit 1
    fi
    
    print_info "Found ${#test_files[@]} test files"
    echo
    
    # Run each test
    for test_file in "${test_files[@]}"; do
        run_test "$test_file"
        echo
    done
}

# Generate test report
generate_report() {
    echo "=========================================="
    echo "Test Report"
    echo "=========================================="
    echo "Total tests: $TOTAL_TESTS"
    echo "Passed: $TESTS_PASSED"
    echo "Failed: $TESTS_FAILED"
    echo "Success rate: $(( (TESTS_PASSED * 100) / TOTAL_TESTS ))%"
    echo "=========================================="
    
    if [[ $TESTS_FAILED -eq 0 ]]; then
        print_success "All tests passed!"
        exit 0
    else
        print_error "$TESTS_FAILED test(s) failed"
        exit 1
    fi
}

# Main execution
main() {
    check_wasm_binary
    run_all_tests
    generate_report
}

# Run main function
main "$@"
