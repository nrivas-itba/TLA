#! /bin/bash

set -euo pipefail

BASE_PATH="$(dirname "$0")/../../.."
cd "$BASE_PATH"

INPUT="$1"
shift 1

# Extraer el nombre base del archivo (ej: "04-draws-a-line")
BASENAME=$(basename "$INPUT")
# Crear nombre de salida (ej: "04-draws-a-line.bmp")
OUTPUT_NAME="${BASENAME}.bmp"

# Pasamos OUTPUT_NAME como argumento al compilador
cat "$INPUT" | ".build/Flex-Bison-Compiler" "$OUTPUT_NAME" "$@"