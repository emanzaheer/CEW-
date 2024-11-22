#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>
#include <curl/curl.h>
#include <unistd.h>

struct MemoryStruct {
    char *memory;
    size_t size;
};

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
char *makeHttpRequest(const char *url, const char *apiKey, const char *location, long *httpCode);
void processWeatherData(const char *response);

int main() {
    const char *API_URL = "http://api.weatherapi.com/v1/current.json";
    const char *API_KEY = "3d9e7a6fceae4eb98d540834231812";
    const char *LOCATION = "karachi";

    while (1) {
        long httpCode;
        char *response = makeHttpRequest(API_URL, API_KEY, LOCATION, &httpCode);

        if (httpCode == 200) {
            // Parse the JSON response using cJSON
            cJSON *root = cJSON_Parse(response);
            if (root) {
                // Extract the data
                cJSON *location = cJSON_GetObjectItem(root, "location");
                cJSON *current = cJSON_GetObjectItem(root, "current");

                const char *locationName = cJSON_GetObjectItem(location, "name")->valuestring;
                const char *region = cJSON_GetObjectItem(location, "region")->valuestring;
                const char *country = cJSON_GetObjectItem(location, "country")->valuestring;
                double latitude = cJSON_GetObjectItem(location, "lat")->valuedouble;
                double longitude = cJSON_GetObjectItem(location, "lon")->valuedouble;
                double temperatureC = cJSON_GetObjectItem(current, "temp_c")->valuedouble;
                double windSpeedKph = cJSON_GetObjectItem(current, "wind_kph")->valuedouble;
                double visibilityKm = cJSON_GetObjectItem(current, "vis_km")->valuedouble;
                const char *condition = cJSON_GetObjectItem(cJSON_GetObjectItem(current, "condition"), "text")->valuestring;

                // Print the information to the console
                printf("Location: %s, %s, %s\n", locationName, region, country);
                printf("Latitude: %.6f\n", latitude);
                printf("Longitude: %.6f\n", longitude);
                printf("Temperature: %.2f°C\n", temperatureC);
                printf("Wind Speed: %.2f km/h\n", windSpeedKph);
                printf("Visibility: %.2f km\n", visibilityKm);
                printf("Condition: %s\n", condition);

                // Save the information to a text file in append mode
                FILE *file = fopen("raw_data.txt", "a");
                if (file != NULL) {
                    fprintf(file,"\n");
                    fprintf(file, "Location: %s, %s, %s\n", locationName, region, country);
                    fprintf(file, "Latitude: %.6f\n", latitude);
                    fprintf(file, "Longitude: %.6f\n", longitude);
                    fprintf(file, "Temperature: %.2f°C\n", temperatureC);
                    fprintf(file, "Wind Speed: %.2f km/h\n", windSpeedKph);
                    fprintf(file, "Visibility: %.2f km\n", visibilityKm);
                    fprintf(file,"\n");
                    fclose(file);
                    printf("Weather information appended to 'raw_data.txt'\n");
                } else {
                    fprintf(stderr, "Failed to open the file for writing\n");
                }

                // Free the JSON object
                cJSON_Delete(root);
            } else {
                fprintf(stderr, "Failed to parse JSON\n");
            }
        } else {
            fprintf(stderr, "HTTP request failed with status code: %ld\n", httpCode);
        }

        free(response); // Free the API response
        sleep(20);
    }
    return 0;
}

void processWeatherData(const char *response) {
    // You can put your data processing logic here if needed
    // This function is called after each API response is received
}

char *makeHttpRequest(const char *url, const char *apiKey, const char *location, long *httpCode) {
    char apiUrl[256];
    snprintf(apiUrl, sizeof(apiUrl), "%s?key=%s&q=%s", url, apiKey, location);

    CURL *curl = curl_easy_init();
    struct MemoryStruct chunk;
    chunk.memory = malloc(1);
    chunk.size = 0;

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, apiUrl);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
        // Exclude headers from the response
        curl_easy_setopt(curl, CURLOPT_HEADER, 0L);

        CURLcode res = curl_easy_perform(curl);
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, httpCode);
        curl_easy_cleanup(curl);
    } else {
        fprintf(stderr, "Failed to initialize CURL\n");
        *httpCode = -1;
    }

    if (*httpCode != 200) {
        fprintf(stderr, "HTTP request failed with status code: %ld\n", *httpCode);
        free(chunk.memory);
        return NULL;
    }

    return strdup(chunk.memory);
}

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    // Allocate memory for the new content
    char *temp = realloc(mem->memory, mem->size + realsize + 1);
    if (temp == NULL) {
        fprintf(stderr, "Failed to allocate memory\n");
        return 0;
    }

    mem->memory = temp;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = '\0';

    return realsize;
}
