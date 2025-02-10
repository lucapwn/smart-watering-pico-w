#include <string.h>
#include <stdio.h>
#include <regex.h>
#include <time.h>

#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "pico/multicore.h"

#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/adc.h"

#include "lwip/dns.h"
#include "lwip/tcp.h"

#include "cJSON.h"
#include "ds3231.h"
#include "smart_watering.h"

#define WIFI_SSID        "YOUR_WIFI_NAME"
#define WIFI_PASSWORD    "YOUR_WIFI_PASSWORD"

#define SERVER_HOSTNAME  "192.168.2.64" // Seu IP ou servidor, exemplo: "example.com".
#define SERVER_PORT      80

#define DJANGO_API_TOKEN   "93a079be-561a-437a-9706-747968e48a53"

#define DEBUG_WIFI      true
#define DEBUG_SENSOR    true
#define DEBUG_REQUEST   true
#define DEBUG_RESPONSE  true  // Ative ou desative mensagens de depuração.

#define DS3231_SDA_GPIO    0
#define DS3231_SCL_GPIO    1

#define DHT11_GPIO        16
#define RELAY_GPIO        17
#define YFS201_GPIO       18
#define HCSR04_ECHO_GPIO  19
#define HCSR04_TRIG_GPIO  20

#define FC28_GPIO_ADC     28  // ADC 2
#define YL83_GPIO_ADC     27  // ADC 1
#define TEMT6000_GPIO_ADC 26  // ADC 0

#define ADC_0_CHANNEL      0
#define ADC_1_CHANNEL      1
#define ADC_2_CHANNEL      2

#define REQUEST_BUFFER_LENGTH  2048
#define SIMPLE_BUFFER_LENGTH    255
#define DATETIME_BUFFER_LENGTH   20
#define TIME_BUFFER_LENGTH        9

#define WIFI_DEBUGGING_LEVEL     1
#define SENSOR_DEBUGGING_LEVEL   2
#define REQUEST_DEBUGGING_LEVEL  3
#define RESPONSE_DEBUGGING_LEVEL 4

#define WIFI_TIMEOUT_DELAY   10000

#define MAXIMUM_ADC_VALUE          4095.0f  // 12 bits do ADC.
#define LUX_CALIBRATION_FACTOR     1000.0f
#define FLOW_CALIBRATION_FACTOR     450.0f
#define SPEED_CALIBRATION_FACTOR   0.0343f  // 343 m/s (velocidade média do som).
#define PICO_ADC_VOLTAGE_REFERENCE    3.3f

#define REQUEST_CONTENT_TYPE    "application/x-www-form-urlencoded"
#define REQUEST_USER_AGENT      "Raspberry Pi Pico W"
#define REQUEST_CONNECTION_TYPE "close"

#define GET_REQUEST_HEADERS     "Host: "          SERVER_HOSTNAME         "\r\n" \
                                "User-Agent: "    REQUEST_USER_AGENT      "\r\n" \
                                "Connection: "    REQUEST_CONNECTION_TYPE

#define POST_REQUEST_HEADERS    "Host: "          SERVER_HOSTNAME         "\r\n" \
                                "User-Agent: "    REQUEST_USER_AGENT      "\r\n" \
                                "Content-Type: "  REQUEST_CONTENT_TYPE    "\r\n" \
                                "Authorization: " DJANGO_API_TOKEN        "\r\n" \
                                "Connection: "    REQUEST_CONNECTION_TYPE

// Defina sua data e hora aqui.

ds3231_data_t ds3231_data = {
    .seconds = 0,
    .minutes = 30,
    .hours   = 9,
    .day     = 1,
    .date    = 9,
    .month   = 2,
    .year    = 25,
    .century = 1,
    .am_pm   = false
};

ds3231_t ds3231;

volatile float humidity             = 0.0f;
volatile float rain_level           = 0.0f;
volatile float luminosity           = 0.0f;
volatile float water_flow           = 0.0f;
volatile float water_level          = 0.0f;
volatile float temperature          = 0.0f;
volatile float soil_moisture        = 0.0f;
volatile float total_water_consumed = 0.0f;

volatile bool dns_resolved          = false;
volatile bool response_available    = false;
volatile bool water_pump_turned_on  = false;

volatile uint32_t pulse_count = 0;

char request_response[REQUEST_BUFFER_LENGTH];

ip4_addr_t server_address;

void debugging(const char *message, unsigned int level) {
    const char *prefix = NULL;

    if (level == WIFI_DEBUGGING_LEVEL && DEBUG_WIFI) {
        prefix = "Wi-Fi   ";
    } else if (level == SENSOR_DEBUGGING_LEVEL && DEBUG_SENSOR) {
        prefix = "Sensor  ";
    } else if (level == REQUEST_DEBUGGING_LEVEL && DEBUG_REQUEST) {
        prefix = "Request ";
    } else if (level == RESPONSE_DEBUGGING_LEVEL && DEBUG_RESPONSE) {
        prefix = "Response";
    }
    
    if (prefix) {
        printf("[%s]  %s  %s\n", get_datetime(), prefix, message);
    }
}

void setup_init(void) {
    stdio_init_all();

    gpio_init(DHT11_GPIO);
    gpio_init(RELAY_GPIO);
    gpio_init(YFS201_GPIO);
    gpio_init(DS3231_SDA_GPIO);
    gpio_init(DS3231_SCL_GPIO);
    gpio_init(HCSR04_ECHO_GPIO);
    gpio_init(HCSR04_TRIG_GPIO);

    gpio_set_dir(RELAY_GPIO, GPIO_OUT);
    gpio_set_dir(YFS201_GPIO, GPIO_IN);
    gpio_set_dir(HCSR04_ECHO_GPIO, GPIO_IN);
    gpio_set_dir(HCSR04_TRIG_GPIO, GPIO_OUT);

    gpio_set_function(DS3231_SDA_GPIO, GPIO_FUNC_I2C);
    gpio_set_function(DS3231_SCL_GPIO, GPIO_FUNC_I2C);

    gpio_pull_up(DS3231_SDA_GPIO);
    gpio_pull_up(DS3231_SCL_GPIO);

    ds3231_init(&ds3231, i2c_default, DS3231_DEVICE_ADRESS, AT24C32_EEPROM_ADRESS_0);
    i2c_init(ds3231.i2c, 400 * 1000);

    adc_init();

    adc_gpio_init(FC28_GPIO_ADC);
    adc_gpio_init(YL83_GPIO_ADC);
    adc_gpio_init(TEMT6000_GPIO_ADC);

    gpio_set_irq_enabled_with_callback(YFS201_GPIO, GPIO_IRQ_EDGE_RISE, true, pulse_callback);

    multicore_launch_core1(measure_water_flow);

    srand(time(NULL));
}

void wifi_init(void) {
    if (cyw43_arch_init()) {
        debugging("Erro ao inicializar Wi-Fi.", WIFI_DEBUGGING_LEVEL);
        return;
    }

    cyw43_arch_enable_sta_mode();
    debugging("Conectando ao Wi-Fi.", WIFI_DEBUGGING_LEVEL);

    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, WIFI_TIMEOUT_DELAY)) {
        debugging("Falha ao conectar ao Wi-Fi.", WIFI_DEBUGGING_LEVEL);
        return;
    }

    debugging("Wi-Fi conectado!", WIFI_DEBUGGING_LEVEL);
}

float valid_value_range(float range) {
    return (range < 0.0f) ? 0.0f : (range > 100.0f) ? 100.0f : range;
}

float map_float(float x, float in_min, float in_max, float out_min, float out_max) {
    float result = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;

    return (result > 0.0f) ? result : 0.0f;
}

float get_adc_voltage(uint channel) {
    adc_select_input(channel);

    uint16_t raw_value = adc_read();
    float voltage = raw_value * PICO_ADC_VOLTAGE_REFERENCE / MAXIMUM_ADC_VALUE;

    return voltage;
}

void dht_init(void) {
    gpio_set_dir(DHT11_GPIO, GPIO_OUT);
    gpio_put(DHT11_GPIO, false);
    sleep_ms(18);
    gpio_put(DHT11_GPIO, true);
    sleep_us(30);
    gpio_set_dir(DHT11_GPIO, GPIO_IN);
}

bool dht_read_response(void) {
    uint32_t count = 0;

    while (gpio_get(DHT11_GPIO)) {
        if (++count > 1000) {
            return false;
        }
    }

    while (!gpio_get(DHT11_GPIO));
    while (gpio_get(DHT11_GPIO));

    return true;
}

uint8_t dht_read_byte(void) {
    uint8_t value = 0;

    for (int i = 0; i < 8; i++) {
        while (!gpio_get(DHT11_GPIO));
        sleep_us(30);

        if (gpio_get(DHT11_GPIO)) {
            value = (value << 1) | 1;
        } else {
            value = (value << 1);
        }

        while (gpio_get(DHT11_GPIO));
    }

    return value;
}

bool dht_read(volatile float *temperature, volatile float *humidity) {
    dht_init();

    if (!dht_read_response()) {
        return false;
    }
    
    uint8_t rh_int = dht_read_byte();
    uint8_t rh_dec = dht_read_byte();
    uint8_t temp_int = dht_read_byte();
    uint8_t temp_dec = dht_read_byte();
    uint8_t checksum = dht_read_byte();

    if (checksum != (rh_int + rh_dec + temp_int + temp_dec)) {
        return false;
    }

    *humidity = rh_int + (rh_dec / 10.0f);
    *temperature = temp_int + (temp_dec / 10.0f);

    return true;
}

void send_sensor_values(void) {
    char post_request_body[SIMPLE_BUFFER_LENGTH];

    snprintf(post_request_body, sizeof(post_request_body),
        "rain_level=%.2f&soil_moisture=%.2f&humidity=%.2f&temperature=%.2f&water_flow=%.2f&water_level=%.2f&luminosity=%.2f",
        rain_level,
        soil_moisture,
        humidity,
        temperature,
        water_flow,
        water_level,
        luminosity
    );

    http_request(&server_address, "POST", "/api/sensors/", POST_REQUEST_HEADERS, post_request_body);
}

void send_notification(const char *title, const char *message) {
    char post_request_body[SIMPLE_BUFFER_LENGTH];

    snprintf(post_request_body, sizeof(post_request_body), "title=%s&message=%s", title, message);

    http_request(&server_address, "POST", "/api/notifications/", POST_REQUEST_HEADERS, post_request_body);
}

void send_schedule_status(unsigned int id, const char *status) {
    char post_request_body[SIMPLE_BUFFER_LENGTH];

    snprintf(post_request_body, sizeof(post_request_body), "id=%u&status=%s", id, status);

    http_request(&server_address, "POST", "/api/change-status/", POST_REQUEST_HEADERS, post_request_body);
}

void send_test_availability(unsigned int id, bool available) {
    char post_request_body[SIMPLE_BUFFER_LENGTH];

    snprintf(post_request_body, sizeof(post_request_body), "id=%u&available=%d", id, available);

    http_request(&server_address, "POST", "/api/change-availability/", POST_REQUEST_HEADERS, post_request_body);
}

void send_water_consumption(float consumption) {
    char post_request_body[SIMPLE_BUFFER_LENGTH];

    snprintf(post_request_body, sizeof(post_request_body), "consumption=%.2f", consumption);

    http_request(&server_address, "POST", "/api/water-consumption/", POST_REQUEST_HEADERS, post_request_body);
}

void pulse_callback(uint gpio, uint32_t events) {
    pulse_count++;
}

void measure_water_flow(void) {
    while (true) {
        water_flow = (pulse_count * 60.0f) / FLOW_CALIBRATION_FACTOR;
        total_water_consumed += (pulse_count / FLOW_CALIBRATION_FACTOR);
        pulse_count = 0;
        sleep_ms(1000);
    }
}

static void dns_callback(const char *name, const ip4_addr_t *ipaddr, void *args) {
    if (!ipaddr) {
        debugging("Falha na resolução de DNS.", REQUEST_DEBUGGING_LEVEL);
        return;
    }

    char buffer[SIMPLE_BUFFER_LENGTH];

    server_address = *ipaddr;
    dns_resolved = true;

    snprintf(buffer, sizeof(buffer), "O endereço IP de %s é %s.", name, ip4addr_ntoa(ipaddr));

    debugging(buffer, REQUEST_DEBUGGING_LEVEL);
}

void dns_resolver(void) {
    char buffer[SIMPLE_BUFFER_LENGTH];

    snprintf(buffer, sizeof(buffer), "Resolvendo DNS para %s.", SERVER_HOSTNAME);

    debugging(buffer, REQUEST_DEBUGGING_LEVEL);

    err_t dns_result = dns_gethostbyname(SERVER_HOSTNAME, &server_address, dns_callback, NULL);

    if (dns_result == ERR_OK) {
        dns_callback(SERVER_HOSTNAME, &server_address, NULL);
    } else if (dns_result == ERR_INPROGRESS) {
        debugging("Resolução de DNS em andamento.", REQUEST_DEBUGGING_LEVEL);
    } else {
        debugging("Falha imediata na resolução de DNS.", REQUEST_DEBUGGING_LEVEL);
    }

    while (!dns_resolved) {
        cyw43_arch_poll();
        sleep_ms(100);
    }
}

static err_t recv_callback(void *args, struct tcp_pcb *protocol_control_block, struct pbuf *buffer, err_t err) {
    if (!buffer) {
        debugging("Conexão fechada pelo servidor.", REQUEST_DEBUGGING_LEVEL);
        tcp_close(protocol_control_block);
        return ERR_OK;
    }

    char *response = (char *)buffer->payload;
    char *body_start = strstr(response, "\r\n\r\n");

    if (body_start) {
        body_start += 4;

        snprintf(request_response, sizeof(request_response), "%s", body_start);
        response_available = true;
    }

    pbuf_free(buffer);

    return ERR_OK;
}

char *http_request(ip4_addr_t *server_ip, const char *method, const char *endpoint, const char *headers, const char *body) {
    response_available = false;
    
    dns_resolver();

    int length;
    char request[REQUEST_BUFFER_LENGTH];

    if (!strcmp(method, "GET")) {
        length = snprintf(request, sizeof(request), "%s %s HTTP/1.1\r\n%s\r\n\r\n", 
                    method,
                    endpoint,
                    headers
                );
    } else if (!strcmp(method, "POST")) {
        length = snprintf(request, sizeof(request), "%s %s HTTP/1.1\r\n%s\r\nContent-Length: %zu\r\n\r\n%s",
                    method,
                    endpoint,
                    headers,
                    strlen(body),
                    body
                );
    } else {
        debugging("Método de requisição não suportado.", REQUEST_DEBUGGING_LEVEL);
        return NULL;
    }

    struct tcp_pcb *protocol_control_block = tcp_new();

    if (!protocol_control_block) {
        debugging("Erro ao criar o bloco de controle de protocolo.", REQUEST_DEBUGGING_LEVEL);
        return NULL;
    }

    if (tcp_connect(protocol_control_block, server_ip, SERVER_PORT, NULL) != ERR_OK) {
        debugging("Erro ao conectar ao servidor.", REQUEST_DEBUGGING_LEVEL);
        tcp_abort(protocol_control_block);
        return NULL;
    }

    tcp_recv(protocol_control_block, recv_callback);

    if (tcp_write(protocol_control_block, request, length, TCP_WRITE_FLAG_COPY) != ERR_OK) {
        debugging("Erro ao enviar requisição HTTP.", REQUEST_DEBUGGING_LEVEL);
        tcp_abort(protocol_control_block);
        return NULL;
    }

    tcp_output(protocol_control_block);

    while (!response_available) {
        cyw43_arch_poll();
        sleep_ms(100);
    }

    debugging(request_response, RESPONSE_DEBUGGING_LEVEL);

    return request_response;
}

float measure_distance(void) {
    gpio_put(HCSR04_TRIG_GPIO, true);
    sleep_us(10);
    gpio_put(HCSR04_TRIG_GPIO, false);

    while (!gpio_get(HCSR04_ECHO_GPIO));
    absolute_time_t start = get_absolute_time();
    while (gpio_get(HCSR04_ECHO_GPIO));
    absolute_time_t end = get_absolute_time();

    int64_t pulse_duration = absolute_time_diff_us(start, end);

    return (pulse_duration * SPEED_CALIBRATION_FACTOR) / 2.0f;
}

struct tm time_to_tm(const char *time) {
    struct tm tm_struct;
    memset(&tm_struct, '\0', sizeof(struct tm));

    sscanf(time, "%d:%d:%d", 
        &tm_struct.tm_hour, 
        &tm_struct.tm_min, 
        &tm_struct.tm_sec
    );

    return tm_struct;
}

int compare_time(const char *time1, const char *time2) {
    struct tm tm1 = time_to_tm(time1);
    struct tm tm2 = time_to_tm(time2);

    time_t t1 = mktime(&tm1);
    time_t t2 = mktime(&tm2);

    if (t1 == t2) {
        return 0;
    }
    
    return (t1 < t2) ? -1 : 1;
}

struct tm datetime_to_tm(const char *datetime) {
    struct tm tm_struct;
    memset(&tm_struct, '\0', sizeof(struct tm));

    sscanf(datetime, "%d/%d/%d %d:%d:%d", 
        &tm_struct.tm_mday, 
        &tm_struct.tm_mon, 
        &tm_struct.tm_year, 
        &tm_struct.tm_hour, 
        &tm_struct.tm_min, 
        &tm_struct.tm_sec
    );

    tm_struct.tm_mon -= 1;
    tm_struct.tm_year -= 1900;

    return tm_struct;
}

int compare_datetime(const char *datetime1, const char *datetime2) {
    struct tm tm1 = datetime_to_tm(datetime1);
    struct tm tm2 = datetime_to_tm(datetime2);

    time_t t1 = mktime(&tm1);
    time_t t2 = mktime(&tm2);

    if (t1 == t2) {
        return 0;
    }

    return (t1 < t2) ? -1 : 1;
}

float random_number(void) {
    return 1.0 + ((float)rand() / RAND_MAX) * (100.0 - 1.0);
}

void read_fictitious_values(void) {
    char buffer[SIMPLE_BUFFER_LENGTH];

    water_flow           = random_number();
    total_water_consumed = random_number();
    temperature          = random_number();
    humidity             = random_number();
    water_level          = random_number();
    luminosity           = random_number();
    rain_level           = random_number();
    soil_moisture        = random_number();

    snprintf(buffer, sizeof(buffer), "Fluxo de Água: %.2f L/min", water_flow);
    debugging(buffer, SENSOR_DEBUGGING_LEVEL);

    snprintf(buffer, sizeof(buffer), "Total Consumido: %.2f L", total_water_consumed);
    debugging(buffer, SENSOR_DEBUGGING_LEVEL);

    snprintf(buffer, sizeof(buffer), "Temperatura: %.2f °C", temperature);
    debugging(buffer, SENSOR_DEBUGGING_LEVEL);

    snprintf(buffer, sizeof(buffer), "Umidade do Ar: %.2f%%", humidity);
    debugging(buffer, SENSOR_DEBUGGING_LEVEL);

    snprintf(buffer, sizeof(buffer), "Nível de Água: %.2f cm", water_level);
    debugging(buffer, SENSOR_DEBUGGING_LEVEL);

    snprintf(buffer, sizeof(buffer), "Luminosidade: %.2f lx", luminosity);
    debugging(buffer, SENSOR_DEBUGGING_LEVEL);

    snprintf(buffer, sizeof(buffer), "Nível de Chuva: %.2f%%", rain_level);
    debugging(buffer, SENSOR_DEBUGGING_LEVEL);

    snprintf(buffer, sizeof(buffer), "Umidade do Solo: %.2f%%", soil_moisture);
    debugging(buffer, SENSOR_DEBUGGING_LEVEL);
}

void read_sensor_values(void) {
    char buffer[SIMPLE_BUFFER_LENGTH];

    snprintf(buffer, sizeof(buffer), "Fluxo de Água: %.2f L/min", water_flow);
    debugging(buffer, SENSOR_DEBUGGING_LEVEL);

    snprintf(buffer, sizeof(buffer), "Total Consumido: %.2f L", total_water_consumed);
    debugging(buffer, SENSOR_DEBUGGING_LEVEL);

    if (dht_read(&temperature, &humidity)) {
        snprintf(buffer, sizeof(buffer), "Temperatura: %.2f °C", temperature);
        debugging(buffer, SENSOR_DEBUGGING_LEVEL);

        snprintf(buffer, sizeof(buffer), "Umidade do Ar: %.2f%%", humidity);
        debugging(buffer, SENSOR_DEBUGGING_LEVEL);
    } else {
        debugging("Erro ao ler DHT11.", SENSOR_DEBUGGING_LEVEL);
    }

    water_level = measure_distance();
    snprintf(buffer, sizeof(buffer), "Nível de Água: %.2f cm", water_level);
    debugging(buffer, SENSOR_DEBUGGING_LEVEL);

    float adc0_voltage = get_adc_voltage(ADC_0_CHANNEL);
    luminosity = adc0_voltage * LUX_CALIBRATION_FACTOR;
    snprintf(buffer, sizeof(buffer), "Luminosidade: %.2f lx", luminosity);
    debugging(buffer, SENSOR_DEBUGGING_LEVEL);

    float adc1_voltage = get_adc_voltage(ADC_1_CHANNEL);
    float adc1_map_value = map_float(adc1_voltage, 2.06f, PICO_ADC_VOLTAGE_REFERENCE, 100.0f, 0.0f);
    rain_level = valid_value_range(adc1_map_value);
    snprintf(buffer, sizeof(buffer), "Nível de Chuva: %.2f%%", rain_level);
    debugging(buffer, SENSOR_DEBUGGING_LEVEL);

    float adc2_voltage = get_adc_voltage(ADC_2_CHANNEL);
    float adc2_map_value = map_float(adc2_voltage, 1.5f, PICO_ADC_VOLTAGE_REFERENCE, 100.0f, 0.0f);
    soil_moisture = valid_value_range(adc2_map_value);
    snprintf(buffer, sizeof(buffer), "Umidade do Solo: %.2f%%", soil_moisture);
    debugging(buffer, SENSOR_DEBUGGING_LEVEL);
}

bool valid_datetime(const char *datetime) {
    regex_t regex;
    
    regcomp(&regex, "^(0[1-9]|[12][0-9]|3[01])/(0[1-9]|1[0-2])/([0-9]{4}) (2[0-3]|[01][0-9]):([0-5][0-9]):([0-5][0-9])$", REG_EXTENDED);
    bool result = regexec(&regex, datetime, 0, NULL, 0);
    regfree(&regex);
    
    return !result;
}

char *split_time(const char *datetime) {
    static char buffer[TIME_BUFFER_LENGTH];

    strncpy(buffer, datetime + 11, TIME_BUFFER_LENGTH - 1);
    buffer[TIME_BUFFER_LENGTH - 1] = '\0';

    return buffer;
}

char *split_datetime(const char *datetime) {
    static char buffer[DATETIME_BUFFER_LENGTH];

    strncpy(buffer, datetime, DATETIME_BUFFER_LENGTH - 1);
    buffer[DATETIME_BUFFER_LENGTH - 1] = '\0';

    return buffer;
}

char *get_time(void) {
    static char buffer[TIME_BUFFER_LENGTH];

    if (ds3231_read_current_time(&ds3231, &ds3231_data)) {
        debugging("Nenhum dado recebido do DS3231.", SENSOR_DEBUGGING_LEVEL);
        return NULL;
    }

    snprintf(buffer, sizeof(buffer), "%02u:%02u:%02u",
        ds3231_data.hours,
        ds3231_data.minutes,
        ds3231_data.seconds
    );
    
    return buffer;
}

char *get_datetime(void) {
    static char buffer[DATETIME_BUFFER_LENGTH];

    if (ds3231_read_current_time(&ds3231, &ds3231_data)) {
        debugging("Nenhum dado recebido do DS3231.", SENSOR_DEBUGGING_LEVEL);
        return NULL;
    }

    snprintf(buffer, sizeof(buffer), "%02u/%02u/20%02u %02u:%02u:%02u",
        ds3231_data.date,
        ds3231_data.month,
        ds3231_data.year,
        ds3231_data.hours,
        ds3231_data.minutes,
        ds3231_data.seconds
    );

    return buffer;
}

char *get_timeout(const char *time, unsigned int increase) {
    int hour, minute, second;
    static char timeout[TIME_BUFFER_LENGTH];
    
    if (sscanf(time, "%d:%d:%d", &hour, &minute, &second) != 3) {
        snprintf(timeout, sizeof(timeout), "00:00:00");
        return timeout;
    }
    
    second += increase;
    minute += second / 60;
    second %= 60;
    hour += minute / 60;
    minute %= 60;
    hour %= 24;
    
    snprintf(timeout, sizeof(timeout), "%02d:%02d:%02d", hour, minute, second);

    return timeout;
}

char *iso_to_datetime(const char *iso_datetime) {
    int year, month, day, hour, minute, second;
    static char datetime[DATETIME_BUFFER_LENGTH];

    if (sscanf(iso_datetime, "%4d-%2d-%2dT%2d:%2d:%2d", &year, &month, &day, &hour, &minute, &second) == 6) {
        snprintf(datetime, sizeof(datetime), "%02d/%02d/%04d %02d:%02d:%02d", day, month, year, hour, minute, second);
        return datetime;
    }

    debugging("Data e hora em formato inválido.", SENSOR_DEBUGGING_LEVEL);

    return NULL;
}

void turn_on_water_pump(void) {
    gpio_put(RELAY_GPIO, true);
    water_pump_turned_on = true;
    debugging("Bomba de água ligada.", SENSOR_DEBUGGING_LEVEL);
}

void turn_off_water_pump(void) {
    gpio_put(RELAY_GPIO, false);
    water_pump_turned_on = false;
    debugging("Bomba de água desligada.", SENSOR_DEBUGGING_LEVEL);
}

void analyze_irrigation_tests(void) {
    char *irrigation_tests = http_request(&server_address, "GET", "/api/irrigation-tests/?filter=last_available", GET_REQUEST_HEADERS, NULL);

    if (!irrigation_tests) return;

    cJSON *json = cJSON_Parse(irrigation_tests);

    if (!json || !cJSON_IsArray(json) || !cJSON_GetArraySize(json)) {
        cJSON_Delete(json);
        return;
    }

    cJSON *first_item = cJSON_GetArrayItem(json, 0);

    if (!first_item) {
        cJSON_Delete(json);
        return;
    }

    cJSON *pk = cJSON_GetObjectItem(first_item, "pk");
    cJSON *fields = cJSON_GetObjectItem(first_item, "fields");

    if (!pk || !fields) {
        cJSON_Delete(json);
        return;
    }

    cJSON *irrigation_time = cJSON_GetObjectItem(fields, "irrigation_time");
    cJSON *created_at = cJSON_GetObjectItem(fields, "created_at");

    if (!irrigation_time || !cJSON_IsNumber(irrigation_time) || !created_at || !cJSON_IsString(created_at)) {
        cJSON_Delete(json);
        return;
    }

    unsigned int id = pk->valueint;
    unsigned int irrigation_duration = irrigation_time->valueint;
    char *created_at_str = iso_to_datetime(created_at->valuestring);

    char *created_at_time = split_time(created_at_str);
    char *timeout = get_timeout(created_at_time, irrigation_duration);
    char *time_now = get_time();

    if (compare_time(time_now, created_at_time) >= 0 && compare_time(time_now, timeout) < 0) {
        if (!water_pump_turned_on) {
            total_water_consumed = 0.0f;
        }

        turn_on_water_pump();
    } else if (compare_time(time_now, timeout) >= 0) {
        turn_off_water_pump();

        send_test_availability(id, false);
        send_water_consumption(total_water_consumed);
    }

    cJSON_Delete(json);
}

void analyze_irrigation_schedules(void) {
    char *irrigation_schedules = http_request(&server_address, "GET", "/api/irrigation-schedules/?status=not_irrigated", GET_REQUEST_HEADERS, NULL);

    if (!irrigation_schedules) return;

    cJSON *json = cJSON_Parse(irrigation_schedules);

    if (!json || !cJSON_IsArray(json) || !cJSON_GetArraySize(json)) {
        cJSON_Delete(json);
        return;
    }

    for (size_t i = 0; i < cJSON_GetArraySize(json); i++) {
        cJSON *item = cJSON_GetArrayItem(json, i);

        if (!item) continue;

        cJSON *pk = cJSON_GetObjectItem(item, "pk");
        cJSON *fields = cJSON_GetObjectItem(item, "fields");

        char *time_now = get_time();
        char *datetime_now = get_datetime();

        if (!pk || !fields) continue;

        unsigned int id = pk->valueint;
        cJSON *irrigation_type = cJSON_GetObjectItem(fields, "irrigation_type");

        if (!irrigation_type) continue;

        char *irrigation_type_str = irrigation_type->valuestring;

        if (!strcmp(irrigation_type_str, "day")) {
            // Irrigação por dia e horários específicos

            cJSON *datetime_on = cJSON_GetObjectItem(fields, "datetime_on");
            cJSON *datetime_off = cJSON_GetObjectItem(fields, "datetime_off");

            if (!datetime_on || !datetime_off) continue;

            char *datetime_on_str = iso_to_datetime(datetime_on->valuestring);
            char *datetime_off_str = iso_to_datetime(datetime_off->valuestring);

            if (compare_datetime(datetime_now, datetime_on_str) >= 0 && compare_datetime(datetime_now, datetime_off_str) < 0) {
                if (!water_pump_turned_on) {
                    total_water_consumed = 0.0f;

                    turn_on_water_pump();
                    send_schedule_status(id, "irrigating");

                    char *title = "Agenda de Irrigação";
                    char message[SIMPLE_BUFFER_LENGTH];
                    
                    snprintf(message, sizeof(message), "O sistema de irrigação foi ligado às %s e será desligado às %s.", datetime_on_str, datetime_off_str);
                    send_notification(title, message);
                }
            } else if (compare_datetime(datetime_now, datetime_off_str) >= 0) {
                turn_off_water_pump();
                send_schedule_status(id, "irrigated");
                send_water_consumption(total_water_consumed);
            }
        } else if (!strcmp(irrigation_type_str, "time")) {
            // Irrigação por horários específicos
            
            cJSON *irrigation_period = cJSON_GetObjectItem(fields, "irrigation_period");
            cJSON *time_on = cJSON_GetObjectItem(fields, "time_on");
            cJSON *time_off = cJSON_GetObjectItem(fields, "time_off");

            if (!irrigation_period || !time_on || !time_off) continue;

            char *irrigation_period_str = irrigation_period->valuestring;
            char *time_on_str = time_on->valuestring;
            char *time_off_str = time_off->valuestring;

            if (compare_time(time_now, time_on_str) >= 0 && compare_time(time_now, time_off_str) < 0) {
                if (!water_pump_turned_on) {
                    total_water_consumed = 0.0f;

                    turn_on_water_pump();
                    send_schedule_status(id, "irrigating");

                    char *title = "Agenda de Irrigação";
                    char message[SIMPLE_BUFFER_LENGTH];

                    snprintf(message, sizeof(message), "O sistema de irrigação foi ligado às %s e será desligado às %s.", time_on_str, time_off_str);
                    send_notification(title, message);
                }
            } else if (compare_time(time_now, time_off_str) >= 0) {
                turn_off_water_pump();
                send_schedule_status(id, !strcmp(irrigation_period_str, "unique") ? "irrigated" : "scheduled");
                send_water_consumption(total_water_consumed);
            }
        } else if (!strcmp(irrigation_type_str, "humidity")) {
            // Irrigação por intervalos de umidade do solo específicos

            cJSON *humidity_on = cJSON_GetObjectItem(fields, "humidity_on");
            cJSON *humidity_off = cJSON_GetObjectItem(fields, "humidity_off");
            cJSON *irrigation_period = cJSON_GetObjectItem(fields, "irrigation_period");
            cJSON *night_irrigation = cJSON_GetObjectItem(fields, "night_irrigation");

            if (!humidity_on || !humidity_off || !irrigation_period || !night_irrigation) continue;

            unsigned int humidity_on_value = humidity_on->valueint;
            unsigned int humidity_off_value = humidity_off->valueint;
            char *irrigation_period_str = irrigation_period->valuestring;
            bool night_irrigation_value = night_irrigation->valueint;

            if (soil_moisture >= humidity_on_value && soil_moisture < humidity_off_value) {
                if (night_irrigation_value || (compare_time(time_now, "05:00:00") >= 0 && compare_time(time_now, "18:00:00") <= 0)) {
                    if (!water_pump_turned_on) {
                        total_water_consumed = 0.0f;

                        turn_on_water_pump();
                        send_schedule_status(id, "irrigating");

                        char *title = "Agenda de Irrigação";
                        char message[SIMPLE_BUFFER_LENGTH];

                        snprintf(message, sizeof(message), "O sistema de irrigação foi ligado às %s ao detectar a umidade de solo com o valor de %u%% e será desligado ao atingir %u%%.", time_now, humidity_on_value, humidity_off_value);
                        send_notification(title, message);
                    }
                }
            } else if (soil_moisture >= humidity_off_value && water_pump_turned_on) {
                turn_off_water_pump();
                send_schedule_status(id, !strcmp(irrigation_period_str, "unique") ? "irrigated" : "scheduled");
                send_water_consumption(total_water_consumed);
            }
        } else if (!strcmp(irrigation_type_str, "flow")) {
            // Irrigação pelo fluxo de água em litros específicos

            cJSON *irrigation_period = cJSON_GetObjectItem(fields, "irrigation_period");
            cJSON *water_flow = cJSON_GetObjectItem(fields, "water_flow");
            cJSON *time_on = cJSON_GetObjectItem(fields, "time_on");

            if (!irrigation_period || !water_flow || !time_on) continue;

            char *irrigation_period_str = irrigation_period->valuestring;
            float water_flow_value = water_flow->valuedouble;
            char *time_on_str = time_on->valuestring;

            unsigned int timeout_in_seconds = 5;
            char *timeout = get_timeout(time_on_str, timeout_in_seconds);

            if (compare_time(time_now, time_on_str) >= 0 && compare_time(time_now, timeout) < 0) {
                if (!water_pump_turned_on) {
                    total_water_consumed = 0.0f;

                    turn_on_water_pump();
                    send_schedule_status(id, "irrigating");

                    char *title = "Agenda de Irrigação";
                    char message[SIMPLE_BUFFER_LENGTH];

                    snprintf(message, sizeof(message), "O sistema de irrigação foi ligado às %s e será desligado após irrigar %.2f litros de água.", time_on_str, water_flow_value);
                    send_notification(title, message);
                }
            } else if (total_water_consumed >= water_flow_value && water_pump_turned_on) {
                turn_off_water_pump();
                send_schedule_status(id, !strcmp(irrigation_period_str, "unique") ? "irrigated" : "scheduled");
                send_water_consumption(total_water_consumed);
            }
        }
    }

    cJSON_Delete(json);
}

int main(int argc, char *argv[]) {
    setup_init();
    wifi_init();

    /* Execute esta função uma única vez e, em seguida, recompile o firmware novamente com a função comentada,
       para salvar a data e hora local na variável "ds3231_data". */
    
    // ds3231_configure_time(&ds3231, &ds3231_data);

    while (true) {
        cyw43_arch_poll();

        /* Se caso não tiver algum dos sensores, use dados fictícios através dessa função. */
        // read_fictitious_values();

        read_sensor_values();
        analyze_irrigation_tests();
        analyze_irrigation_schedules();
        send_sensor_values();

        sleep_ms(3000);
    }

    cyw43_arch_deinit();

    return EXIT_SUCCESS;
}