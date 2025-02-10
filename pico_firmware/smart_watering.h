#ifndef SMART_WATERING_H
#define SMART_WATERING_H

void setup_init(void);
void wifi_init(void);
void dht_init(void);

void debugging(const char *message, unsigned int level);

uint8_t dht_read_byte(void);
bool dht_read_response(void);
bool dht_read(volatile float *temperature, volatile float *humidity);

float get_adc_voltage(uint channel);
float valid_value_range(float range);
float map_float(float x, float in_min, float in_max, float out_min, float out_max);
float measure_distance(void);
float random_number(void);

void measure_water_flow(void);
void read_sensor_values(void);
void read_fictitious_values(void);
void turn_on_water_pump(void);
void turn_off_water_pump(void);
void analyze_irrigation_tests(void);
void analyze_irrigation_schedules(void);
void pulse_callback(uint gpio, uint32_t events);

void send_sensor_values(void);
void send_notification(const char *title, const char *message);
void send_schedule_status(unsigned int id, const char *status);
void send_test_availability(unsigned int id, bool available);
void send_water_consumption(float consumption);

void dns_resolver(void);
static err_t recv_callback(void *args, struct tcp_pcb *protocol_control_block, struct pbuf *buffer, err_t err);
static void dns_callback(const char *name, const ip4_addr_t *ipaddr, void *args);
char *http_request(ip4_addr_t *server_ip, const char *method, const char *endpoint, const char *headers, const char *body);

struct tm time_to_tm(const char *time);
struct tm datetime_to_tm(const char *datetime);
char *split_time(const char *datetime);
char *split_datetime(const char *datetime);
char *get_time(void);
char *get_datetime(void);
char *get_timeout(const char *time, unsigned int increase);
char *iso_to_datetime(const char *iso_datetime);
int compare_time(const char *time1, const char *time2);
int compare_datetime(const char *datetime1, const char *datetime2);
bool valid_datetime(const char *datetime);

#endif // SMART_WATERING_H