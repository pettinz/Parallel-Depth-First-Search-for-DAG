#ifndef _TEST_FILE
#define _TEST_FILE

#define TEST_FILE "quer.gra"

#define TEST_READ

#define test_V 7
#define test_IA \
    vector<unsigned long> { 0, 2, 4, 6, 6, 7, 8, 8 }
#define test_JA \
    vector<node> { 1, 2, 3, 4, 4, 5, 6, 6 }
#define test_np \
    vector<unsigned long> { 0, 1, 1, 1, 2, 1, 2 }

#define SECTION_1

#define test_dtIA \
    vector<unsigned long> { 0, 2, 4, 5, 5, 6, 6, 6 }
#define test_dtJA \
    vector<unsigned long> { 1, 2, 3, 4, 5, 6 }
#define test_dtParents \
    vector<node> { 0, 0, 0, 1, 1, 2, 4 }

#define SECTION_2

#define test_subgraph \
    vector<unsigned long> { 7, 4, 2, 1, 2, 1, 1 }
#define test_presum \
    vector<unsigned long> { 0, 0, 4, 0, 1, 0, 0 }

#define SECTION_3

#define test_preorder \
    vector<unsigned long> { 0, 1, 5, 2, 3, 6, 4 }
#define test_postorder \
    vector<unsigned long> { 6, 3, 5, 0, 2, 4, 1 }

#endif