# Digital Floats Controller Manual - Split Structure

This directory contains the Digital Floats Controller manual split into multiple files for easier maintenance and management.

## Document Structure

The manual has been split into the following files:

1. `main.tex` - The main file that includes all other files and defines the document structure
2. `frontmatter.tex` - Contains thank you letter, document control, contact information, and other front matter
3. `chapter01.tex` - General Information
4. `chapter02.tex` - System Features
5. `chapter03.tex` - Installation
6. `chapter04.tex` - Operation
7. `chapter05.tex` - Integration with Other Systems
8. `chapter06.tex` - Maintenance
9. `chapter07.tex` - Troubleshooting
10. `chapter08.tex` - Configuration
11. `chapter09.tex` - Compliance and Certification
12. `chapter10.tex` - Parts and Ordering
13. `chapter11.tex` - Warranty and Support
14. `chapter12.tex` - Appendices

## How to Compile

To compile the full manual:

1. Ensure all chapter files are present in the same directory as `main.tex`
2. Run the LaTeX compiler on `main.tex`:
   ```
   xelatex main.tex
   ```
3. Generate the table of contents:
   ```
   xelatex main.tex
   ```
4. Open the resulting `main.pdf` file to view the complete manual

## Notes for Editors

- The original `manual.tex` file has been preserved for reference
- Each chapter file begins with `\chapter{Chapter Title}` and contains all sections and content for that chapter
- The frontmatter contains all content that appears before the first numbered chapter
- The main.tex file contains all the preamble code, package imports, and style definitions
- To add a new chapter, create a new chapter file and add it to the list of `\include` statements in `main.tex`
- Images should be placed in an `images/` folder at the same level as the TeX files

## Missing Information

Several sections throughout the manual are marked with `{\color{missingred}[Required: description]}` to indicate where additional information needs to be added. These should be completed before finalizing the document.

## Compilation Requirements

- XeLaTeX is required (not pdfLaTeX) for proper font rendering
- The Arial font must be installed on the system
- All packages used in the preamble must be installed 