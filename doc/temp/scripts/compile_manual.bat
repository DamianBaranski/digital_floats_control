@echo off
setlocal enabledelayedexpansion

REM Compile LaTeX Manual for Digital Floats Controller
REM This script compiles the manual.tex file into a PDF document

REM Get script directory and set paths dynamically
set "SCRIPT_DIR=%~dp0"
set "DOC_DIR=%SCRIPT_DIR%.."
set "OUTPUT_DIR=%DOC_DIR%\..\output"

REM Create output directory if it doesn't exist
if not exist "%OUTPUT_DIR%" mkdir "%OUTPUT_DIR%"

REM Navigate to document directory (temp folder)
cd "%DOC_DIR%" || (
    echo Error: Document directory not found!
    exit /b 1
)

echo ====================================================
echo      Compiling Digital Floats Controller Manual     
echo ====================================================

REM Run XeLaTeX compilation (first pass)
echo Running first XeLaTeX pass...
xelatex -interaction=nonstopmode -output-directory="..\output" manual.tex

REM Run XeLaTeX again for table of contents and cross-references
echo Running second XeLaTeX pass for references...
xelatex -interaction=nonstopmode -output-directory="..\output" manual.tex

REM Final pass to ensure all references are updated
echo Running final XeLaTeX pass...
xelatex -interaction=nonstopmode -output-directory="..\output" manual.tex

REM Check if compilation was successful
if exist "..\output\manual.pdf" (
    echo ====================================================
    echo Compilation successful! PDF created at: doc\output\manual.pdf
    echo ====================================================
) else (
    echo ====================================================
    echo Error: Compilation failed! Check the log files for errors.
    echo ====================================================
    exit /b 1
)

REM Return to original directory
cd "%SCRIPT_DIR%\..\..\.." 