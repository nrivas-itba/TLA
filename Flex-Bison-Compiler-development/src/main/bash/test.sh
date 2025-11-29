#! /bin/bash

set -u

BASE_PATH="$(dirname "$0")/../../.."
cd "$BASE_PATH"

GREEN='\033[0;32m'
RED='\033[0;31m'
OFF='\033[0m'
STATUS=0

# Carpeta donde voy a guardar todos los BMP generados
OUTPUT_DIR="generated-bmps"
mkdir -p "$OUTPUT_DIR"
# Limpio BMPs viejos para no mezclar corridas
rm -f "$OUTPUT_DIR"/*.bmp 2>/dev/null || true

echo "Compiler should accept..."
echo ""

for test in $(ls src/test/c/accept/); do
    # Creo un archivo "marca" con la hora actual ANTES de correr el compilador
    MARKER="$(mktemp)"

    # Corro el compilador con el test
    cat "src/test/c/accept/$test" | ".build/Flex-Bison-Compiler" >/dev/null 2>&1
    RESULT="$?"

    if [ "$RESULT" == "0" ]; then
        echo -e "    $test, ${GREEN}and it does${OFF} (status $RESULT)"

        # Busco cualquier .bmp que sea más nuevo que la marca
        # (es decir, generado/modificado por este test)
        NEW_BMP="$(find . -type f -name '*.bmp' -newer "$MARKER" 2>/dev/null | head -n 1)"

        if [ -n "$NEW_BMP" ]; then
            # Copio el primer BMP nuevo encontrado con el nombre del test
            cp "$NEW_BMP" "$OUTPUT_DIR/${test}.bmp"
        fi
    else
        STATUS=1
        echo -e "    $test, ${RED}but it rejects${OFF} (status $RESULT)"
    fi

    # Borro la marca
    rm -f "$MARKER"
done
echo ""

echo "Compiler should reject..."
echo ""

for test in $(ls src/test/c/reject/); do
    cat "src/test/c/reject/$test" | ".build/Flex-Bison-Compiler" >/dev/null 2>&1
    RESULT="$?"
    if [ "$RESULT" != "0" ]; then
        echo -e "    $test, ${GREEN}and it does${OFF} (status $RESULT)"
    else
        STATUS=1
        echo -e "    $test, ${RED}but it accepts${OFF} (status $RESULT)"
    fi
done
echo ""

echo "All done."
exit $STATUS
