#! /bin/bash

set -euo pipefail

BASE_PATH="$(dirname "$0")/../../.."
cd "$BASE_PATH"

INPUT="$1"
shift 1

BASENAME=$(basename "$INPUT")

# Pasamos OUTPUT_NAME como argumento al compilador
cat "$INPUT" | ".build/Flex-Bison-Compiler" "${BASENAME}.bmp" "$@"
