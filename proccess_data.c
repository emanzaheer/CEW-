#include <stdio.h>
#include <string.h>

void calculateMean(FILE *rawDataFile, FILE *processedDataFile);

int main() {
    // Open the raw data file for reading
    FILE *rawDataFile = fopen("raw_data.txt", "r");

    if (rawDataFile != NULL) {
        // Open the processed data file for writing
        FILE *processedDataFile = fopen("processed_data.txt", "w");

        if (processedDataFile != NULL) {
            // Calculate the mean and save processed data
            calculateMean(rawDataFile, processedDataFile);

            // Close the processed data file
            fclose(processedDataFile);
        } else {
            fprintf(stderr, "Failed to open the file 'processed_data.txt' for writing\n");
        }

        // Close the raw data file
        fclose(rawDataFile);
    } else {
        fprintf(stderr, "Failed to open the file 'raw_data.txt' for reading\n");
    }

    return 0;
}

void calculateMean(FILE *rawDataFile, FILE *processedDataFile) {
    double temperatureC, windSpeedKph, visibilityKm;
    double temperatureSum = 0.0;
    double windSpeedSum = 0.0;
    double visibilitySum = 0.0;
    int count = 0;

    char line[256];

    // Skip the Location line
    while (fgets(line, sizeof(line), rawDataFile) != NULL && !strstr(line, "Latitude:"));

    // Read each line from rawDataFile
    while (fgets(line, sizeof(line), rawDataFile) != NULL) {
        // Attempt to read Temperature, Wind Speed, and Visibility
        if (sscanf(line, "Temperature: %lf", &temperatureC) == 1 &&
            fgets(line, sizeof(line), rawDataFile) != NULL &&
            sscanf(line, "Wind Speed: %lf", &windSpeedKph) == 1 &&
            fgets(line, sizeof(line), rawDataFile) != NULL &&
            sscanf(line, "Visibility: %lf", &visibilityKm) == 1) {
            // Calculate sum
            temperatureSum += temperatureC;
            windSpeedSum += windSpeedKph;
            visibilitySum += visibilityKm;
            count++;
        }
    }
    if (count > 0) {
        // Calculate mean values
        double meanTemperature = temperatureSum / count;
        double meanWindSpeed = windSpeedSum / count;
        double meanVisibility = visibilitySum / count;

        // Save mean values to processed data file
        fprintf(processedDataFile, "Mean_Temperature: %.2f\n", meanTemperature);
        fprintf(processedDataFile, "Mean_Wind_Speed: %.2f\n", meanWindSpeed);
        fprintf(processedDataFile, "Mean_Visibility: %.2f\n", meanVisibility);

        // Optionally, perform other processing or output to console
        printf("Mean Temperature: %.2f\n", meanTemperature);
        printf("Mean Wind Speed: %.2f\n", meanWindSpeed);
        printf("Mean Visibility: %.2f\n", meanVisibility);
    } else {
        fprintf(stderr, "No valid data found in the raw data file\n");
    }
}