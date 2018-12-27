#!/usr/bin/env bash

find_all_test_files() {
    echo $(find ../../source/app -type f -name '*.test.c')
}

find_all_test_names() {
    TEST_FILES=$@
    for test_file in $TEST_FILES; do
        function_line=$(grep "^char *" $test_file)
        function_definition=$(echo "$function_line" | cut -d ' ' -f 3)
	function_name=$(echo "$function_definition" | rev | cut -c 3- | rev)
        echo "$function_name"
    done
}

make_test_defines() {
    TEST_NAMES=$@
    for test_name in $TEST_NAMES; do
        echo "char* $test_name();"
    done
}

make_test_calls() {
    TEST_NAMES=$@
    for test_name in $TEST_NAMES; do
        echo "mu_run_test($test_name);"
    done
}

APP_PATH=../../source/app
if [ "$1" != "" ]; then
    APP_PATH=$1
fi
echo "[INFO] - Searching $APP_PATH..."
TEST_FILES=$(find_all_test_files)
TEST_NAMES=$(find_all_test_names $TEST_FILES)
echo "[INFO] - Found tests:"
echo $TEST_NAMES

echo "[INFO] - Building main test file..."
TEST_DEFINES=$(make_test_defines $TEST_NAMES)
TEST_CALLS=$(make_test_calls "$TEST_NAMES")

echo $(
echo "void print(char* text);"
echo "$TEST_DEFINES"
echo "char* allTests()"
echo "{"
echo "$TEST_CALLS"
echo "return 0;"
echo "}"
echo "int main()"
echo "{"
echo "char* result = allTests();"
echo "if (result != 0)"
echo "printf(\"%s\n\", result);"
echo "else"
echo "printf(\"ALL TESTS PASSED\n\");"
echo "while(1);"
echo "}") > main.c

