# Digital Floats Controller – User Manual

This repository contains the LaTeX source for the Digital Floats Controller user manual, formatted according to ATA iSpec 2200 conventions.

## Requirements

To compile this document, you need:

1. A modern TeX distribution (TeXLive, MiKTeX, etc.)
2. XeLaTeX or LuaLaTeX compiler
3. Calibri font installed on your system (or modify the template to use another font)

## Font Change

The template now uses Arial font to match the modern look of the cover page. If you prefer to use IBM Plex Sans as originally specified, you can:

1. Install IBM Plex Sans font
2. Change the following line in manual.tex:
   ```latex
   \setmainfont{Arial}
   ```
   to
   ```latex
   \setmainfont{IBM Plex Sans}
   ```

## Font Installation

### Windows

1. Download IBM Plex Sans from [Google Fonts](https://fonts.google.com/specimen/IBM+Plex+Sans)
2. Extract the ZIP file
3. Select all font files, right-click, and choose "Install"

### macOS

1. Download IBM Plex Sans from [Google Fonts](https://fonts.google.com/specimen/IBM+Plex+Sans)
2. Extract the ZIP file
3. Double click each font file and click "Install Font"

### Linux

```bash
# Ubuntu/Debian
sudo apt install fonts-ibm-plex

# Fedora
sudo dnf install ibm-plex-sans-fonts
```

## Compilation

To compile the manual, use XeLaTeX or LuaLaTeX:

```bash
# Using XeLaTeX
xelatex manual.tex
xelatex manual.tex  # Run twice for correct cross-references

# OR using LuaLaTeX
lualatex manual.tex
lualatex manual.tex  # Run twice for correct cross-references
```

## Document Structure

The document follows the ATA iSpec 2200 structure with the following chapters:

1. General Information
2. Construction and Technical Characteristics
3. Equipment Installation
4. Operation
5. Integration with Other Systems
6. Technical Maintenance and Servicing
7. Troubleshooting and Diagnostics
8. Certification and Compliance
9. Spare Parts and Logistics
10. Modifications and Updates
11. Training
12. Appendices

## Notes About Numbering

The document has been configured to:
- Automatically number chapters and sections according to ATA conventions
- Properly format headers and prevent overlapping with content
- Remove duplicate numbering in section titles

## Chapter Layout

The document follows these layout guidelines:
- Each chapter starts on a right-hand (odd-numbered) page
- Chapter titles use the blue color scheme matching the cover page design
- A horizontal rule underlines each chapter title
- Chapter content begins on a new page after the title
- Empty pages are marked with "This page intentionally left blank"
- Each chapter's content starts with the first section

## Header Format

The document headers are formatted as follows:
- Left header shows "Chapter X" on the first line and the chapter title on the second line
- Right header shows "Digital Floats Controller" on the first line and "User Manual" on the second
- Headers are in italic font for a professional appearance
- The header is separated from the content by a thin rule line

## Customization

- The document uses a custom cover page image (front_page_manual.png) placed in the images/ directory
- To replace placeholder text, replace the `\lipsum[x]` commands with actual content
- Update document metadata (version, ID, date) in the title page section

## License

Copyright © Skymatik Aero. All rights reserved. 