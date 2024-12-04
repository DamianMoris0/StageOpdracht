#include "unity.h"

#define TESTRAIL_PASS "1"
#define TESTRAIL_FAIL "5"

#define TESTRAIL_BASE_URL "https://brightest.testrail.com/index.php?/api/v2/"
#define TESTRAIL_USERNAME "delivery@brightest.be"
#define TESTRAIL_API_KEY "Wy6ze50e.e9PQa.s3TLs-qN2GVvRKIskaySlofAzy"

void insertTestStatusToTestrail(struct UNITY_STORAGE_T unity, char* st);
void add_test_result(int run_id, int case_id, char *status, char *comment);