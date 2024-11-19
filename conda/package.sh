#!/bin/bash
set -euo pipefail

usage()
{
  cat << EOF

Builds the gadgetron conda package.

Usage: $0
EOF
}

output_path="$(dirname "$0")/build_pkg"

# Build up channel directives
channels=(
  nvidia/label/cuda-12.3.0
  ismrmrd
  gadgetron
  conda-forge
  bioconda
  defaults
)

channel_directives=$(printf -- "-c %s " "${channels[@]}")

python3 validate_versions.py

mkdir -p "$output_path"
bash -c "conda build --no-anaconda-upload --output-folder $output_path $channel_directives $(dirname "$0")"
