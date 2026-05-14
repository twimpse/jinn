#!/bin/sh
#
# Seed file generator for build system.
# Creates/updates .seed file and src/seed.h with SHA512 hash.
#

set -e  # Exit on error

# Get build root (parent of tools directory)
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_ROOT="$(dirname "$SCRIPT_DIR")"
cd "$BUILD_ROOT"

SEED_FILE=".seed"
SEED_HEADER="src/seed.h"

# Ensure src directory exists
mkdir -p src

if [ ! -f "$SEED_FILE" ]; then
    # Step 1: Create empty .seed file
    touch "$SEED_FILE"
    
    # Step 2: MD5 sum of empty file, write to temp file
    TEMP_MD5=$(mktemp)
    md5sum "$SEED_FILE" | cut -d' ' -f1 > "$TEMP_MD5"
    
    # Step 3: SHA512 sum of temp file (containing MD5), write to .seed
    sha512sum "$TEMP_MD5" | cut -d' ' -f1 > "$SEED_FILE"
    
    # Clean up
    rm -f "$TEMP_MD5"
else
    # Step: SHA512 sum of existing .seed, write to temp then move
    TEMP_SHA=$(mktemp)
    sha512sum "$SEED_FILE" | cut -d' ' -f1 > "$TEMP_SHA"
    mv "$TEMP_SHA" "$SEED_FILE"
fi

# Read final SHA512 from .seed
FINAL_SEED="$(cat "$SEED_FILE")"

# Update src/seed.h
cat > "$SEED_HEADER" << EOF
#ifndef SEED_H
#define SEED_H

#define SEED "$FINAL_SEED"

#endif /* SEED_H */
EOF

echo "Updated $SEED_HEADER with SEED = $FINAL_SEED"
