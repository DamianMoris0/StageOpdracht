#include "unity_testrail.h"
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>

void insertTestStatusToTestrail(struct UNITY_STORAGE_T unity, char* st)
{
    int runId = TESTRAIL_RUN_ID;
    int caseId;
    char* comment;
    char* execState;

    struct Tests tests[] = {
        /* test_sensor tests */
        {.testName = "test_checkDataBounds",            .caseId = 53234},
        {.testName = "test_initSensor",                 .caseId = 53235},
        {.testName = "test_createJsonFromSensorData",   .caseId = 53236},
        {.testName = "test_getSensorTypeName",          .caseId = 53237},
        /* test_mqtt tests */
        {.testName = "test_configSSL",                  .caseId = 53240},
        {.testName = "test_connectBroker",              .caseId = 53241},
        {.testName = "test_publishMessage",             .caseId = 53242},
    };

    for (int i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        if (!strcmp(unity.CurrentTestName, tests[i].testName)) {
            caseId = tests[i].caseId;
            break;
        }
    }

    if (!strcmp(st, TESTRAIL_PASS)) {
        execState = " executed successfully";
    }
    else if (!strcmp(st, TESTRAIL_FAIL)) {
        execState = " execution failed";
    }
    else {
        execState = " status unknown";
    }
    comment = (char*)malloc((strlen(unity.CurrentTestName) + strlen(execState)) * sizeof(char) + 1);
    strcpy(comment, unity.CurrentTestName);
    strcat(comment, execState);

    add_test_result(runId, caseId, st, comment);

    free(comment);
}

// Function to post test results
void add_test_result(int runId, int caseId, char *status, char *comment) {
    CURL *curl;
    CURLcode res;
    char url[256];
    char data[512];
    struct curl_slist *headers = NULL;

    // Construct the API endpoint URL
    snprintf(url, sizeof(url), "%sadd_result_for_case/%d/%d", TESTRAIL_BASE_URL, runId, caseId);

    // Construct the JSON payload
    snprintf(data, sizeof(data), "{\"status_id\": %s, \"comment\": \"%s\"}", status, comment);

    curl = curl_easy_init();
    if (curl) {
        // Set the URL
        curl_easy_setopt(curl, CURLOPT_URL, url);

        // Add authentication
        curl_easy_setopt(curl, CURLOPT_USERNAME, TESTRAIL_USERNAME);
        curl_easy_setopt(curl, CURLOPT_PASSWORD, TESTRAIL_API_KEY);

        // Set headers
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set the POST data
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);

        // Perform the request
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            printf("Test result added successfully.\n");
        }

        // Clean up
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }
}