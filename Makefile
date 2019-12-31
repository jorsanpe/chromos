PROJECT_DIR=$(PWD)
CMAKE=/Applications/CMake.app/Contents/bin/cmake
TESTS_BUILD_FOLDER=build/tests
BUILD_FOLDER=build

all: $(BUILD_FOLDER)/build.ninja
	cd $(BUILD_FOLDER) && ninja $(VERBOSE) protock

clean: 
	$(RM) -r build
	$(RM) -r run_*_tests.sh
	$(RM) -r CMakeFiles
	$(RM) CMakeCache.txt
	$(RM) *.ninja

%/build.ninja: $(shell find . -type f -name 'CMakeLists.txt')
	mkdir -p $*
	cd $* && $(CMAKE) -G Ninja $(CMAKE_VERBOSE) $(PROJECT_DIR)

unit: $(TESTS_BUILD_FOLDER)/build.ninja
	ninja $(VERBOSE) -C $(TESTS_BUILD_FOLDER) unit && chmod +x run_unit_tests.sh
	./run_unit_tests.sh

integration: $(TESTS_BUILD_FOLDER)/build.ninja
	ninja $(VERBOSE) -C $(TESTS_BUILD_FOLDER) integration && chmod +x run_integration_tests.sh
	./run_integration_tests.sh
	
test_%: $(TESTS_BUILD_FOLDER)/build.ninja
	ninja $(VERBOSE) -C $(TESTS_BUILD_FOLDER) $@

.PHONY: clean all

