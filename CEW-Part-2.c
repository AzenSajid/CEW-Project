#ifndef FUNCTIONS_H
#define FUNCTIONS_H

typedef struct {
    float temperature;
    char description[100];
} WeatherData;

int append_to_history(const char *raw_data_file, const char *history_file);
void format_city_name(char *city);
int fetch_weather_data(const char *api_key, const char *city, const char *filename);
int parse_weather_data(const char *filename, WeatherData *weather);
void save_processed_data(const char *filename, WeatherData *weather);
int is_average_calculation_time(const char *filename);
float calculate_average_temperature(const char *filename);
void save_average_temperature(const char *filename, float avg_temp);

#endif
