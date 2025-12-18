#!/bin/bash

# Define colors
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

echo "--- Running Smoke Tests ---"

# Test 1: Milestone 3 (LRU)
if ./build/paging_sim_m3 > /dev/null; then
    echo -e "${GREEN}[PASS] Milestone 3 runs successfully.${NC}"
else
    echo -e "${RED}[FAIL] Milestone 3 crashed!${NC}"
    exit 1
fi

# Test 2: Milestone 4 (Multi-Level)
if ./build/paging_sim_m4 > /dev/null; then
    echo -e "${GREEN}[PASS] Milestone 4 runs successfully.${NC}"
else
    echo -e "${RED}[FAIL] Milestone 4 crashed!${NC}"
    exit 1
fi

echo "--- All Tests Passed ---"
