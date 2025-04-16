#!/bin/bash

# Compile LaTeX Manual for Digital Floats Controller
# This script compiles the manual.tex file into a PDF document

# Set directory variables
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
DOC_DIR="$(dirname "$SCRIPT_DIR")"  # Parent directory of scripts (temp folder)
OUTPUT_DIR="$(dirname "$DOC_DIR")/output"  # doc/output directory

# Create output directory if it doesn't exist
mkdir -p "$OUTPUT_DIR"

# Navigate to document directory (temp folder)
cd "$DOC_DIR" || { echo "Error: Document directory not found!"; exit 1; }

echo "===================================================="
echo "     Compiling Digital Floats Controller Manual     "
echo "===================================================="

# Run XeLaTeX compilation (first pass)
echo "Running first XeLaTeX pass..."
xelatex -interaction=nonstopmode -output-directory="../output" manual.tex

# Run XeLaTeX again for table of contents and cross-references
echo "Running second XeLaTeX pass for references..."
xelatex -interaction=nonstopmode -output-directory="../output" manual.tex

# Final pass to ensure all references are updated
echo "Running final XeLaTeX pass..."
xelatex -interaction=nonstopmode -output-directory="../output" manual.tex

# Check if compilation was successful
if [ -f "../output/manual.pdf" ]; then
    echo "===================================================="
    echo "Compilation successful! PDF created at: doc/output/manual.pdf"
    echo "===================================================="
else
    echo "===================================================="
    echo "Error: Compilation failed! Check the log files for errors."
    echo "===================================================="
    exit 1
fi

# Return to original directory
cd - > /dev/null 