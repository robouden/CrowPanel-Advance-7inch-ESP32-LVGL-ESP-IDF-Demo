#include "vars.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Static variables
static char label_count_value[16] = "0";  // Buffer to store the count as string

// Getter for label_count value
const char *get_var_label_count_value() {
    return label_count_value;
}

// Setter for label_count value
void set_var_label_count_value(const char *value) {
    strncpy(label_count_value, value, sizeof(label_count_value) - 1);
    label_count_value[sizeof(label_count_value) - 1] = '\0';  // Ensure null termination
}

// Helper function to increment the counter
void increment_label_count() {
    int count = atoi(label_count_value);
    count++;
    snprintf(label_count_value, sizeof(label_count_value), "%d", count);
} 