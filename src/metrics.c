#include <stdio.h>
#include <sys/time.h>
#include <stdint.h>

#include "metrics.h"

typedef struct {
    uint64_t total_requests;
    double total_time_ms;
    double avg_response_time_ms;
} metrics_t;

static metrics_t g_metrics = {0};

void metrics_add_request(double time_ms) {
    g_metrics.total_requests++;
    g_metrics.total_time_ms += time_ms;
    g_metrics.avg_response_time_ms = g_metrics.total_time_ms / g_metrics.total_requests;
}

void metrics_print() {
    printf("Total Requests: %lu\n", g_metrics.total_requests);
    printf("Avg Response Time: %.2f ms\n", g_metrics.avg_response_time_ms);
}

double get_time_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0;
}