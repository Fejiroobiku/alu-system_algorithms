@echo off
echo Binary Trees Runner
echo.

if "%1"=="1" (
    echo Compiling Task 1...
    gcc -mconsole -I. 0-binary_tree_node.c 1-binary_tree_insert_left.c 1-main.c -o 1.exe
    if %errorlevel% equ 0 (
        echo Running...
        1.exe
    )
) else if "%1"=="2" (
    echo Compiling Task 2...
    gcc -mconsole -I. 0-binary_tree_node.c 2-binary_tree_insert_right.c 2-main.c -o 2.exe
    if %errorlevel% equ 0 (
        echo Running...
        2.exe
    )
) else if "%1"=="all" (
    call %0 1
    echo.
    call %0 2
) else (
    echo Usage: run.bat [1|2|all]
    echo.
    echo   1 - Compile and run Task 1
    echo   2 - Compile and run Task 2
    echo   all - Run both tasks
)