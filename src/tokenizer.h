#pragma once

#include <vector>

struct tokenizer {
    char* field_0;
    int field_4;
    bool field_8;
    char field_9;
    char field_A;
    char field_B;

    std::vector<char*> field_C;

    decltype(field_C)::iterator field_1C;

    tokenizer(bool a2);

    ~tokenizer();

    char* get_token();

    void setup_current_iterator();

    int parse(char* a2, const char* a3);

    void clear();
};

extern void tokenizer_unit_test();
