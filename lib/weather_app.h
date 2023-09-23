#include "requests.h"
#include <codecvt>
#include <fstream>
#include <windows.h>
#include <chrono>

namespace weatherapp {
using namespace Requests;

std::wstring string_to_wstring(const std::string& string) {
	// Get the required buffer size for the conversion
	int bufferSize = MultiByteToWideChar(CP_UTF8, 0, string.c_str(), -1, nullptr, 0);

	// Allocate buffer
	wchar_t* buffer = new wchar_t[bufferSize];

	// Do the actual conversion
	MultiByteToWideChar(CP_UTF8, 0, string.c_str(), -1, buffer, bufferSize);

	std::wstring result(buffer);
	delete[] buffer;

	return result;
}

// Class for the one page (one city)
class WeatherPage {
  class Day {
   private:
	const int MORNING = 6;
	const int STEP = 6;

	std::string date;
	json
		forecast; // Contains four objects for every time of day. Each object contains hourly params for corresponding hour
	json hourly_params = {
		"weathercode",
		"temperature_2m",
		"apparent_temperature",
		"windspeed_10m",
		"winddirection_10m",
		"visibility",
		"precipitation",
		"precipitation_probability"
	};

	std::wstring get_ascii_art_from_weathercode(int weathercode, int line_number);

	std::wstring weathercode_to_text(int weathercode, int max_width);

	std::wstring convert_day_format(const std::string& date);

	std::wstring GetTemperatureString(int temperature, int apparent_temperature);

	std::wstring GetWindString(int wind_direction, int wind_speed);

	std::wstring GetPrecipitationString(float precipitation, int probability);

   public:
	Day(json& weather_forecast, int day_number) {
		date = weather_forecast["hourly"]["time"][day_number * 24];
		for (int i = 0; i < 4; i++) {
			forecast[i] = json::object();
			for (const std::string& param : hourly_params) {
				forecast[i][param] = weather_forecast["hourly"][param][day_number * 24 + MORNING + STEP * i];
			}
		}
	}

	void draw_day();
  };

 private:
  WeatherRequest weather_request;
  json location_info;
  std::string city_name;
  std::string country_name;
  std::string full_country_name;
  const int MAX_FORECAST_DAYS = 16;
  int forecast_days = 7;
  int update_frequency;
  json forecast;
  std::vector<Day> day_cards;
  std::chrono::time_point<std::chrono::steady_clock> last_update_time;
  bool page_is_valid = true;
  std::wstring error_message;

  json configure_location();
 public:
  WeatherPage(const std::string& city_name,
              const std::string& country_name,
              int forecast_days,
              int update_frequency)
	  : city_name(city_name),
	    country_name(country_name),
	    forecast_days(forecast_days),
		update_frequency(update_frequency) {
	  location_info = configure_location();
	  if (location_info.contains("error")) {
		  page_is_valid = false;
		  error_message = string_to_wstring(location_info["error"]);
	  }
	  if (page_is_valid) {
		  weather_request = WeatherRequest(location_info);
	  }
	  if (forecast_days > 16) {
		  this->forecast_days = 16;
	  }
	  update_forecast();
  }

  void update_forecast();

  void display_page();

  void increase_forecast_days();

  void decrease_forecast_days();

  bool check_time_for_update();
};


class WeatherApp {
 private:
  std::vector<WeatherPage> pages;
  json config;
  int forecast_days = 3;
  int update_frequency = 5;
  bool error_has_occurred = false;
  std::wstring error_message;
  int page_amount;
  int current_page = 0;
  bool page_changed = true;

  const std::wstring ascii_error = L" /$$$$$$$$ /$$$$$$$  /$$$$$$$   /$$$$$$  /$$$$$$$ \n"
                                   L"| $$_____/| $$__  $$| $$__  $$ /$$__  $$| $$__  $$\n"
                                   L"| $$      | $$  \\ $$| $$  \\ $$| $$  \\ $$| $$  \\ $$\n"
                                   L"| $$$$$   | $$$$$$$/| $$$$$$$/| $$  | $$| $$$$$$$/\n"
                                   L"| $$__/   | $$__  $$| $$__  $$| $$  | $$| $$__  $$\n"
                                   L"| $$      | $$  \\ $$| $$  \\ $$| $$  | $$| $$  \\ $$\n"
                                   L"| $$$$$$$$| $$  | $$| $$  | $$|  $$$$$$/| $$  | $$\n"
                                   L"|________/|__/  |__/|__/  |__/ \\______/ |__/  |__/\n";

  void ParseConfig(const std::string& config_path) {
	  std::ifstream f(config_path);
	  if (!f.good()) {
		  SaveError(L"Couldn't open or read config file!");
	  }
	  try {
		  config = json::parse(f);
	  } catch (const json::parse_error& error) {
		  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		  SaveError(L"Couldn't parse json config! " + converter.from_bytes(error.what()));
		  return;
	  }
	  if (config.contains("forecast_days") &&
		  1 < config["forecast_days"] && config["forecast_days"] < 17) {
		  forecast_days = config["forecast_days"];
	  }
	  if (config.contains("update_frequency") && 0 < config["update_frequency"] && config["update_frequency"] < 61) {
		  update_frequency = config["update_frequency"];
	  }
	  if (!config.contains("cities")) {
		  SaveError(L"Config must contain a member \"cities\" "
		            L"e.g. \"cities\" : [[\"Kazan\", \"RU\"], [\"Paris\", \"FR\"]]");
		  return;
	  }
	  if (config["cities"].empty()) {
		  SaveError(L"The list of cities is empty!");
		  return;
	  }
	  for (const auto& city : config["cities"]) {
		  if (!city.is_array() || city.size() != 2 ||
			  city.size() == 2 && !(city[0].is_string() && city[1].is_string())) {
			  SaveError(L"Incorrect city format in config. Should be [city_name, countr_iso2_code] "
			            L"e.g. \"cities\" : [[\"Kazan\", \"RU\"], [\"Paris\", \"FR\"]]");
			  return;
		  }
		  std::string city_name = city[0];
		  std::string country_code = city[1];
		  pages.push_back(WeatherPage(city_name, country_code, forecast_days, update_frequency));
	  }
  }

  // TODO: rename all methods to CamelCase
  void SaveError(const std::wstring& message) {
	  error_has_occurred = true;
	  error_message = message;
  }

  bool ProcessInputs() {
	  page_changed = true;
	  if (GetAsyncKeyState(VK_ESCAPE)) {
		  return false;
	  }
	  if (GetAsyncKeyState('N') == 1) {
		  current_page = std::max(0, (current_page + 1) % page_amount);
		  return true;
	  }
	  if (GetAsyncKeyState('P') == 1) {
		  current_page = (page_amount + current_page - 1) % page_amount;
		  return true;
	  }
	  if (GetAsyncKeyState(VK_OEM_PLUS) == 1) {
		  pages[current_page].increase_forecast_days();
		  return true;
	  }
	  if (GetAsyncKeyState(VK_OEM_MINUS) == 1) {
		  pages[current_page].decrease_forecast_days();
		  return true;
	  }
	  page_changed = false;
	  return true;
  }

 public:
  WeatherApp(const std::string& config_path) {
	  ParseConfig(config_path);
	  page_amount = pages.size();
  }

  void Start() {
	  if (!error_has_occurred) {
		  pages[current_page].display_page();
	  }

	  while (ProcessInputs()) {
		  if (error_has_occurred) {
			  system("cls");
			  std::wcout << ascii_error << error_message << std::endl;
		  } else {
			  if (pages[current_page].check_time_for_update()) {
				  pages[current_page].update_forecast();
				  pages[current_page].display_page();
			  }
			  if (page_changed) {
				  pages[current_page].display_page();
			  }
			  Sleep(500);
		  }
	  }
  }
};
} // namespace weatherapp

