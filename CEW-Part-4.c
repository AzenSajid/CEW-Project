#!/bin/bash

CITY=$1  # Accept city name as the first argument

if [ -z "$CITY" ]; then
    echo "Usage: $0 <city_name>"
    exit 1
fi

# Compile the program
make clean
make

for num in {1..5}
do
    ./weather_app "$CITY"
    sleep 5
done

make clean
