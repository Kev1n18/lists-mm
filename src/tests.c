#include "unity/unity.h"

#ifdef _WIN32
#else
#include <unistd.h>
#endif

#include "list_mm.h"

ListMM list;
#define LIST_FILE_NAME "tests.lst"

Element data[7] = {
    {.value = 1, .id = "a"},
    {.value = 2, .id = "b"},
    {.value = 3, .id = "c"},
    {.value = 4, .id = "d"},
    {.value = 5, .id = "e"},
    {.value = 6, .id = "f"},
    {.value = 7, .id = "g"}};

void delete_list_file() {
#ifdef _WIN32
    _unlink(LIST_FILE_NAME);
#else
    unlink(LIST_FILE_NAME);
#endif
}

bool equal_elements(Element* first, Element* second) {
    return first->value == second->value;
}

void setUp(void) {
    delete_list_file();
    list = list_create(LIST_FILE_NAME);
}

void tearDown(void) {
    if (list != NULL) {
        list_destroy(list);
    }
    delete_list_file();
}

void test_list_is_empty() {
    TEST_ASSERT(list_is_empty(list));
}

void test_create_with_existing_file() {
    ListMM other_list = list_create(LIST_FILE_NAME);
    TEST_ASSERT_NULL(other_list);
}

void test_open_existing_file() {
    list_close(list);
    list = NULL;
    ListMM other_list = list_open(LIST_FILE_NAME);
    TEST_ASSERT_NOT_NULL(other_list);
    list_destroy(other_list);
}

void test_open_non_existing_file() {
    ListMM other_list = list_open("no file");
    TEST_ASSERT_NULL(other_list);
}

void test_list_size() {
    TEST_ASSERT_EQUAL(0, list_size(list));
    list_insert_first(list, &data[0]);
    TEST_ASSERT_EQUAL(1, list_size(list));
    list_insert_first(list, &data[1]);
    TEST_ASSERT_EQUAL(2, list_size(list));
}

void test_insert_get_first() {
    list_insert_first(list, &data[0]);
    Element first_element = list_get_first(list);
    TEST_ASSERT_EQUAL(1, first_element.value);
}

void test_insert_get_last() {
    list_insert_last(list, &data[0]);
    Element last_element = list_get_last(list);
    TEST_ASSERT_EQUAL(1, last_element.value);
    list_insert_last(list, &data[1]);
    last_element = list_get_last(list);
    TEST_ASSERT_EQUAL(data[1].value, last_element.value);
}

void test_get() {
    list_insert_last(list, &data[0]);
    list_insert_last(list, &data[1]);
    TEST_ASSERT_EQUAL(data[0].value, list_get(list, 0).value);
    TEST_ASSERT_EQUAL(data[1].value, list_get(list, 1).value);
}

void test_insert() {
    list_insert(list, &data[0], 0);
    TEST_ASSERT_EQUAL(data[0].value, list_get(list, 0).value);
    list_insert_last(list, &data[1]);
    list_insert_last(list, &data[3]);
    list_insert(list, &data[2], 2);
    Element element = list_get(list, 2);
    TEST_ASSERT_EQUAL(data[2].value, element.value);
    list_insert(list, &data[4], 4);
    TEST_ASSERT_EQUAL(data[4].value, list_get(list, 4).value);
    list_insert(list, &data[6], 0);
    TEST_ASSERT_EQUAL(data[6].value, list_get(list, 0).value);
    TEST_ASSERT_EQUAL(data[2].value, list_get(list, 3).value);
    TEST_ASSERT_EQUAL(data[4].value, list_get(list, 5).value);
}

void test_find() {
    list_insert_last(list, &data[0]);
    list_insert_last(list, &data[1]);
    list_insert_last(list, &data[2]);
    list_insert_last(list, &data[3]);
    list_insert_last(list, &data[4]);
    TEST_ASSERT_EQUAL(-1, list_find(list, equal_elements, &data[5]));
    TEST_ASSERT_EQUAL(0, list_find(list, equal_elements, &data[0]));
    TEST_ASSERT_EQUAL(1, list_find(list, equal_elements, &data[1]));
    TEST_ASSERT_EQUAL(2, list_find(list, equal_elements, &data[2]));
    TEST_ASSERT_EQUAL(3, list_find(list, equal_elements, &data[3]));
    TEST_ASSERT_EQUAL(4, list_find(list, equal_elements, &data[4]));
}

void test_remove_first() {
    list_insert_last(list, &data[0]);
    list_insert_last(list, &data[1]);
    list_insert_last(list, &data[2]);
    list_remove_first(list);
    TEST_ASSERT_EQUAL(data[1].value, list_get(list, 0).value);
    list_remove_first(list);
    TEST_ASSERT_EQUAL(data[2].value, list_get(list, 0).value);
}

void test_remove_last() {
    list_insert_last(list, &data[0]);
    list_insert_last(list, &data[1]);
    list_insert_last(list, &data[2]);
    TEST_ASSERT_EQUAL(3, list_remove_last(list).value);
    TEST_ASSERT_EQUAL(2, list_get_last(list).value);
    TEST_ASSERT_EQUAL(2, list_remove_last(list).value);
    TEST_ASSERT_EQUAL(1, list_get_last(list).value);
}

void test_remove(){
    list_insert_last(list, &data[0]); //0
    list_insert_last(list, &data[1]);
    list_insert_last(list, &data[2]); 
    list_insert_last(list, &data[3]); //2
    list_insert_last(list, &data[4]);
    list_insert_last(list, &data[5]); //4
    TEST_ASSERT_EQUAL(1, list_remove(list, 0).value);
    TEST_ASSERT_EQUAL(6, list_remove(list, 4).value);
    TEST_ASSERT_EQUAL(4, list_remove(list, 2).value);
    TEST_ASSERT_EQUAL(3, list_remove(list, 1).value);
}

void test_make_empty() {
    list_insert_last(list, &data[0]);
    list_insert_last(list, &data[1]);
    list_insert_last(list, &data[2]);
    list_insert_last(list, &data[3]);
    list_insert_last(list, &data[4]);
    list_insert_last(list, &data[5]);
    TEST_ASSERT_EQUAL(6, list_size(list));
    list_make_empty(list);
    TEST_ASSERT_EQUAL(0, list_size(list));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_create_with_existing_file);
    RUN_TEST(test_open_existing_file);
    RUN_TEST(test_open_non_existing_file);
    RUN_TEST(test_list_is_empty);
    RUN_TEST(test_list_size);
    RUN_TEST(test_insert_get_first);
    RUN_TEST(test_insert_get_last);
    RUN_TEST(test_get);
    RUN_TEST(test_insert);
    RUN_TEST(test_find);
    RUN_TEST(test_remove_first);
    RUN_TEST(test_remove_last);
    RUN_TEST(test_remove);
    RUN_TEST(test_make_empty);
    return UNITY_END();
}