#ifndef METRICS_H
#define METRICS_H


void metrics_add_request(double time);
void metrics_print(void);
double get_time_ms(void);

#endif