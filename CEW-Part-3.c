#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include "functions.h"

// Callback function for cURL
size_t WriteCallback(void *contents, size_t size, size_t nmemb, char *userp) {
    size_t total_size = size * nmemb;
    strncat(userp, contents, total_size < 4096 ? total_size : 4096 - strlen(userp) - 1);
    return total_size;
}

int append_to_history(const char *raw_data_file, const char *history_file) {
    FILE *raw_data_fp = fopen(raw_data_file, "r");
    if (!raw_data_fp) {
        perror("Error opening raw data file");
        return 0;
    }

    FILE *history_fp = fopen(history_file, "a");
    if (!history_fp) {
        perror("Error opening history file");
        fclose(raw_data_fp);
        return 0;
    }

    char buffer[4096];
    size_t bytes_read = fread(buffer, 1, sizeof(buffer) - 1, raw_data_fp);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0'; // Null-terminate the buffer
        fprintf(history_fp, "%s\n", buffer); // Append to history file with a newline
    }

    fclose(raw_data_fp);
    fclose(history_fp);

    return 1;
}
// Format city name (capitalize first letter, lowercase others)
void format_city_name(char *city) {
    if (city[0] >= 'a' && city[0] <= 'z') {
        city[0] = city[0] - 'a' + 'A';
    }
    for (int i = 1; city[i] != '\0'; i++) {
        if (city[i] >= 'A' && city[i] <= 'Z') {
            city[i] = city[i] - 'A' + 'a';
        }
    }
}

// Fetch weather data
int fetch_weather_data(const char *api_key, const char *city, const char *filename) {
    CURL *curl;
    CURLcode res;
    char url[256];
    char response[4096] = {0};

    snprintf(url, sizeof(url), "https://api.openweathermap.org/data/2.5/weather?q=%s&appid=%s&units=metric", city, api_key);

    curl = curl_easy_init();
    if (!curl) return 0;

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);

    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) return 0;

    FILE *file = fopen(filename, "w");
    if (!file) return 0;

    fprintf(file, "%s", response);
    fclose(file);

    return 1;
}

// Parse weather data
int parse_weather_data(const char *filename, WeatherData *weather) {
    FILE *file = fopen(filename, "r");
    if (!file) return 0;

    char buffer[4096];
    fread(buffer, 1, sizeof(buffer), file);
    fclose(file);

    cJSON *json = cJSON_Parse(buffer);
    if (!json) return 0;

    cJSON *main = cJSON_GetObjectItem(json, "main");
    cJSON *weather_array = cJSON_GetObjectItem(json, "weather");

    if (main) {
        weather->temperature = cJSON_GetObjectItem(main, "temp")->valuedouble;
    }

    if (cJSON_IsArray(weather_array)) {
        cJSON *weather_obj = cJSON_GetArrayItem(weather_array, 0);
        strncpy(weather->description, cJSON_GetObjectItem(weather_obj, "description")->valuestring, sizeof(weather->description) - 1);
    }

    cJSON_Delete(json);
    return 1;
}

// Save processed data
void save_processed_data(const char *filename, WeatherData *weather) {
    FILE *file = fopen(filename, "a");
    if (!file) return;

    fprintf(file, "%.2f°C, %s\n", weather->temperature, weather->description);
    fclose(file);
}

// Check if average calculation is needed
int is_average_calculation_time(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) return 0;

    int count = 0;
    char line[128];
    while (fgets(line, sizeof(line), file)) {
        count++;
    }
    fclose(file);

    return (count % 24 == 0);
}

// Calculate average temperature
float calculate_average_temperature(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) return -1;

    int count = 0;
    float sum = 0;
    char line[128];

    while (fgets(line, sizeof(line), file)) {
        float temp;
        sscanf(line, "%f", &temp);
        sum += temp;
        count++;
    }
    fclose(file);

    return (count > 0) ? sum / count : -1;
}

// Save average temperature
void save_average_temperature(const char *filename, float avg_temp) {
    FILE *file = fopen(filename, "a");
    if (!file) return;

    fprintf(file, "Average Temperature: %.2f°C\n", avg_temp);
    fclose(file);
}
