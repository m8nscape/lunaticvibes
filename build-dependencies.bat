@echo on
set INITIAL_PATH=%CD%
set BATCH_PATH=%~dp0

REM -------- yaml-cpp --------
REM -------- Output: libyaml-cppmd[d].lib --------

echo BUILD yaml-cpp START
IF NOT EXIST %BATCH_PATH%ext\yaml-cpp\build (
	mkdir %BATCH_PATH%ext\yaml-cpp\build
	cd /d %BATCH_PATH%ext\yaml-cpp\build
	IF NOT EXIST YAML_CPP.sln (
		cmake -G "Visual Studio 15 2017" ..
	)
)
echo "Build configuration: %~1|%~2"
devenv %BATCH_PATH%ext\yaml-cpp\build\YAML_CPP.sln /build "%~1|%~2" /project "yaml-cpp static md"
echo BUILD yaml-cpp FINISH


cd /d %INITIAL_PATH%