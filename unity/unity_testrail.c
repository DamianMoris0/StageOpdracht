#include "unity_testrail.h"
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>

void insertTestStatusToTestrail(struct UNITY_STORAGE_T unity, char* st)
{
    int run_id = 2811;
    int case_id;
    char *comment;

    if (!strcmp(unity.CurrentTestName, "test_initSensor")) {
        case_id = 53235;
    }
    else if (!strcmp(unity.CurrentTestName, "test_createJsonFromSensorData")) {
        case_id = 53236;
    }
    else if (!strcmp(unity.CurrentTestName, "test_checkDataBounds")) {
        case_id = 53234;
    }
    else if (!strcmp(unity.CurrentTestName, "test_getSensorTypeName")) {
        case_id = 53237;
    }
    else {
        printf("else\n\n");
    }

    comment = (char*)malloc((strlen(unity.CurrentTestName) + strlen(" executed successfully")) * sizeof(char) + 1);
    strcpy(comment, unity.CurrentTestName);
    strcat(comment, " executed successfully");
    
    add_test_result(run_id, case_id, st, comment);

    free(comment);
}

// Function to post test results
void add_test_result(int run_id, int case_id, char *status, char *comment) {
    CURL *curl;
    CURLcode res;
    char url[256];
    char data[512];
    struct curl_slist *headers = NULL;

    // Construct the API endpoint URL
    snprintf(url, sizeof(url), "%sadd_result_for_case/%d/%d", TESTRAIL_BASE_URL, run_id, case_id);

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