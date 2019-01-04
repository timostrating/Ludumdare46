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

echo "// Generated main test file" > $OUT_PATH/main.test.c
echo "#include <stdio.h>" >> $OUT_PATH/main.test.c
echo "#include \"tonc.h\"" >> $OUT_PATH/main.test.c
echo "#include \"minunit.h\"" >> $OUT_PATH/main.test.c
echo "int tests_run = 0;" >> $OUT_PATH/main.test.c
echo "$TEST_DEFINES" >> $OUT_PATH/main.test.c
echo "char* allTests()" >> $OUT_PATH/main.test.c
echo "{" >> $OUT_PATH/main.test.c
echo "  $TEST_CALLS" >> $OUT_PATH/main.test.c
echo "  return 0;" >> $OUT_PATH/main.test.c
echo "}" >> $OUT_PATH/main.test.c
echo "int main()" >> $OUT_PATH/main.test.c
echo "{" >> $OUT_PATH/main.test.c
echo "  REG_DISPCNT= DCNT_MODE0 | DCNT_BG0;" >> $OUT_PATH/main.test.c
echo "  tte_init_se_default(0, BG_CBB(0)|BG_SBB(31));" >> $OUT_PATH/main.test.c
echo "  tte_init_con();" >> $OUT_PATH/main.test.c
echo "  tte_printf(\"-GBA MinUnit Tester-\n\n\");" >> $OUT_PATH/main.test.c
echo "  tte_printf(\"Running tests...\n\");" >> $OUT_PATH/main.test.c
echo "  char* result = allTests();" >> $OUT_PATH/main.test.c
echo "  if (result != 0)" >> $OUT_PATH/main.test.c
echo "    tte_printf(\"FAIL: %s\n\", result);" >> $OUT_PATH/main.test.c
echo "  else" >> $OUT_PATH/main.test.c
echo "    tte_printf(\"ALL %d TESTS PASSED!\n\", tests_run);" >> $OUT_PATH/main.test.c
echo "  while(1);" >> $OUT_PATH/main.test.c
echo "}" >> $OUT_PATH/main.test.c

echo "[INFO] - Wrote main test file to $OUT_PATH!"
