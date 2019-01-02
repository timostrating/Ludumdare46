#!/usr/bin/env bash

find_all_test_files() {
    echo $(find $APP_PATH -type f -name '*.test.c')
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
OUT_PATH=$APP_PATH
if [ "$2" != "" ]; then
    OUT_PATH=$2
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
echo "// Generated main file"$'\r'
echo "#include <stdio.h>"$'\r'
echo "#include \"../../lib/libtonc/include/tonc.h\""$'\r'
echo "#include \"../../lib/minunit/minunit.h\""$'\r'
echo "$TEST_DEFINES"$'\r'
echo "char* allTests()"$'\r'
echo "{"$'\r'
echo "$TEST_CALLS"$'\r'
echo "return 0;"$'\r'
echo "}"$'\r'
echo "int main()"$'\r'
echo "{"$'\r'
echo "tte_init_se_default(0, BG_CBB(0)|BG_SBB(31));"$'\r'
echo "tte_init_con();"$'\r'
echo "char* result = allTests();"$'\r'
echo "if (result != 0)"$'\r'
echo "tte_printf(\"%s\n\", result);"$'\r'
echo "else"$'\r'
echo "tte_printf(\"ALL TESTS PASSED\n\");"$'\r'
echo "while(1);"$'\r'
echo "}"$'\r') > $OUT_PATH/main.test.c
echo "[INFO] - Wrote main test file to $OUT_PATH!"
