#! /bin/bash

set -euo pipefail

BASE_PATH="$(dirname "$0")/../../.."
cd "$BASE_PATH"

# Check if input file is provided
if [ $# -eq 0 ]; then
    echo "Usage: $0 <input.bsim> [output.txt]"
    echo "       echo 'expression' | $0 (for calculator mode)"
    exit 1
fi

INPUT="$1"

# If input file has .bsim extension, generate .txt output
if [[ "$INPUT" == *.bsim ]]; then
    if [ $# -ge 2 ]; then
        OUTPUT="$2"
    else
        OUTPUT="${INPUT%.bsim}.txt"
    fi
    
    echo "Compiling BoardSim file: $INPUT -> $OUTPUT"
    cat "$INPUT" | ".build/Flex-Bison-Compiler" > "$OUTPUT"
    echo "Output written to $OUTPUT"
else
    # Legacy calculator mode - direct output to stdout
    shift 1
    cat "$INPUT" | ".build/Flex-Bison-Compiler" "$@"
fi
