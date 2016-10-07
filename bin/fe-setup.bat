@echo off
setlocal EnableDelayedExpansion
setlocal EnableExtensions

echo Setting up F.A.T.E...
echo The FE_PATH environment variable will be set to your current directory.
set /p agreed="Do you agree ? (y/n)"
if agreed NEQ Y (
    set /p fe_dir="Please specify the path to the F.A.T.E SDK/repo."
) else (
    set fe_dir "%cd%"
)
setx FE_PATH "%fe_dir%"
rem XXX: This keeps adding to the PATH.
setx PATH "%FE_PATH%;%path%;"
!java! -jar closure.jar
