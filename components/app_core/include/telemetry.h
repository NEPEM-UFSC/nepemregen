#pragma once

#include <string>

class Telemetry {
public:
  static void init();
  static void log_event(const char *trigger, const char *action, float temp,
                        float hum_ar, float hum_solo, bool lag_ativo);

private:
  static void write_to_csv(const std::string &line);
  static bool http_available;
  static void send_http(const std::string &data);
};
