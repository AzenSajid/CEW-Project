#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functions.h"

int main(int argc, char *argv[]) {
    system("clear");

    // Ensure a city is provided via command-line argument
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <City_Name>\n", argv[0]);
        return 1;
    }

    const char *api_key = "ce18c3b2bf0093492bf3b15733e685ca";
    char city[100];
    strncpy(city, argv[1], sizeof(city) - 1);
    city[sizeof(city) - 1] = '\0';  // Ensure null-terminated string

    // Format the city name (capitalize the first letter, lowercase others)
    format_city_name(city);

    // Fetch and save raw weather data
    char raw_data_filename[] = "raw_data.json";
    if (!fetch_weather_data(api_key, city, raw_data_filename)) {
        fprintf(stderr, "Failed to fetch weather data for city: %s\n", city);
        return 1;
    }

    // Append raw data to history file
    const char *history_filename = "weather_history.json";
    if (!append_to_history(raw_data_filename, history_filename)) {
        fprintf(stderr, "Failed to append weather data to history file.\n");
        return 1;
    }

    // Parse the weather data from the raw JSON file and save processed data
    WeatherData weather;
    if (!parse_weather_data(raw_data_filename, &weather)) {
        fprintf(stderr, "Failed to parse weather data.\n");
        return 1;
    }

    save_processed_data("processed_data.txt", &weather);

    // Print current weather data
    printf("\nCurrent Weather for %s:\n", city);
    printf("----------------------------\n");
    printf("Temperature: %.2f°C\n", weather.temperature);
    printf("Condition: %s\n", weather.description);
    printf("----------------------------\n");

    // Temperature alerts
    if (weather.temperature > 30) {
        printf("ALERT: High Temperature! %.2f°C\n", weather.temperature);
        system("zenity --warning --text='High Temperature!' --title='Warning'");

    }
    if (weather.temperature < 10) {
        printf("ALERT: Low Temperature! %.2f°C\n", weather.temperature);
        system("zenity --warning --text='LOW Temperature!' --title='Warning'");
    }

    // Check if it's time to calculate the average temperature (every 24 readings)
    if (is_average_calculation_time("processed_data.txt")) {
        float avg_temp = calculate_average_temperature("processed_data.txt");
        if (avg_temp != -1) {
            printf("\nAverage Temperature for the last 24 readings: %.2f°C\n", avg_temp);
            save_average_temperature("average_temperatures.txt", avg_temp);
        }
    }

    return 0;
}
