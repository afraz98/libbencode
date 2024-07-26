#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "bencode.h"

void test_integer() {
    const char* input = "i42e";
    string in = { input, 0 };
    bencode_value *val = parse_object_str(&in);
    assert(val != NULL);
    assert(val->type == B_ENCODED_INTEGER);
    assert(val->integer == 42);
    free_bencode_value(val);
    printf("test_integer passed.\n");
}

void test_string() {
    const char* input = "5:hello";
    string in = { input, 0 };
    bencode_value *val = parse_object_str(&in);
    assert(val != NULL);
    assert(val->type == B_ENCODED_STRING);
    assert(val->string.strlen == 5);
    assert(strncmp(val->string.string, "hello", 5) == 0);
    free_bencode_value(val);
    printf("test_string passed.\n");
}

void test_list() {
    const char* input = "li42e5:helloe";
    string in = { input, 0 };
    bencode_value *val = parse_object_str(&in);
    assert(val != NULL);
    assert(val->type == B_ENCODED_LIST);
    assert(val->list.llen == 2);
    assert(val->list.list_val[0]->type == B_ENCODED_INTEGER);
    assert(val->list.list_val[0]->integer == 42);
    assert(val->list.list_val[1]->type == B_ENCODED_STRING);
    assert(val->list.list_val[1]->string.strlen == 5);
    assert(strncmp(val->list.list_val[1]->string.string, "hello", 5) == 0);
    free_bencode_value(val);
    printf("test_list passed.\n");
}

void test_dict() {
    const char* input = "d3:keyi42ee";
    string in = { input, 0 };
    bencode_value *val = parse_object_str(&in);
    assert(val != NULL);
    assert(val->type == B_ENCODED_DICTIONARY);
    assert(val->dict.dlen == 1);
    assert(val->dict.keys[0]->type == B_ENCODED_STRING);
    assert(val->dict.keys[0]->string.strlen == 3);
    assert(strncmp(val->dict.keys[0]->string.string, "key", 3) == 0);
    assert(val->dict.values[0]->type == B_ENCODED_INTEGER);
    assert(val->dict.values[0]->integer == 42);
    free_bencode_value(val);
    printf("test_dict passed.\n");
}

void test_bencode_encode() {
    // Test encoding an integer
    bencode_value int_val = { .type = B_ENCODED_INTEGER, .integer = 42 };
    char output[256];
    size_t offset = 0;
    bencode_encode(&int_val, output, &offset);
    assert(strcmp(output, "i42e") == 0);
    printf("test_bencode_encode integer passed.\n");

    // Test encoding a string
    bencode_value str_val = { .type = B_ENCODED_STRING, .string = { .string = "hello", .strlen = 5 } };
    offset = 0;
    bencode_encode(&str_val, output, &offset);
    assert(strcmp(output, "5:hello") == 0);
    printf("test_bencode_encode string passed.\n");

    // Test encoding a list
    bencode_value *list_values[2];
    list_values[0] = &int_val;
    list_values[1] = &str_val;
    bencode_value list_val = { .type = B_ENCODED_LIST, .list = { .list_val = list_values, .llen = 2 } };
    offset = 0;
    bencode_encode(&list_val, output, &offset);
    assert(strcmp(output, "li42e5:helloe") == 0);
    printf("test_bencode_encode list passed.\n");

    // Test encoding a dictionary
    bencode_value *dict_keys[1];
    bencode_value *dict_values[1];
    dict_keys[0] = &str_val;  // key "hello"
    dict_values[0] = &int_val;  // value 42
    bencode_value dict_val = { .type = B_ENCODED_DICTIONARY, .dict = { .keys = dict_keys, .values = dict_values, .dlen = 1 } };
    offset = 0;
    bencode_encode(&dict_val, output, &offset);
    assert(strcmp(output, "d5:helloi42ee") == 0);
    printf("test_bencode_encode dictionary passed.\n");
}

int main() {
    test_integer();
    test_string();
    test_list();
    test_dict();
    test_bencode_encode();

    printf("All tests passed.\n");
    return 0;
}