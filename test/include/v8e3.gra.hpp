#ifndef _TEST_FILE
#define _TEST_FILE

#define TEST_FILE "v8e3.gra"

#define test_V 8
#define test_IA \
    vector<unsigned long> { 0, 2, 4, 4, 7, 9, 11, 11, 11 }
#define test_JA \
    vector<node> { 7, 2, 7, 3, 7, 5, 4, 5, 6, 6, 7 }
#define test_np \
    vector<unsigned long> { 0, 0, 1, 1, 1, 2, 2, 4 }

#define SECTION_1

#define test_dtIA \
    vector<unsigned long> { 0, 2, 3, 3, 5, 5, 6, 6, 6 }
#define test_dtJA \
    vector<unsigned long> { 7, 2, 3, 5, 4, 6 }
#define test_dtParents \
    vector<node> { 0, 1, 0, 1, 3, 3, 5, 0}

#endif