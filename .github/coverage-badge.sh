#!/bin/sh
# Writes a coverage badge as SVG, used by the CI.
# Usage: coverage-badge.sh <percent> <output file>
set -e

percent="$1"
output="$2"

# Green from 80 %, yellow from 60 %, red below
if [ "$percent" -ge 80 ]; then
    color="#4c1"
elif [ "$percent" -ge 60 ]; then
    color="#dfb317"
else
    color="#e05d44"
fi

cat > "$output" <<EOF
<svg xmlns="http://www.w3.org/2000/svg" width="104" height="20" role="img" aria-label="coverage: $percent%">
  <title>coverage: $percent%</title>
  <rect width="61" height="20" fill="#555"/>
  <rect x="61" width="43" height="20" fill="$color"/>
  <g fill="#fff" text-anchor="middle" font-family="Verdana,Geneva,DejaVu Sans,sans-serif" font-size="11">
    <text x="30.5" y="14">coverage</text>
    <text x="82.5" y="14">$percent%</text>
  </g>
</svg>
EOF
