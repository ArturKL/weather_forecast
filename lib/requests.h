#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

#include <iostream>
#include <string>

namespace Requests {
using json = nlohmann::json;

json handle_response(const cpr::Response& r);

struct ExtendedParameter : cpr::Parameter {
 private:
  // Needed to concat strings to pass string arrays to cpr::Parameters as there is no way to provide string array
  template<typename Iter>
  std::string join(Iter begin, Iter end, std::string const& separator) {
	  std::string result;
	  while (begin != end) {
		  if (!result.empty()) {
			  result += separator;
		  }
		  result += *begin++;
	  }
	  return result;
  }
 public:
  using cpr::Parameter::Parameter;

  template<typename Iter>
  ExtendedParameter(const std::string& p_key, Iter begin, Iter end)
	  : cpr::Parameter(p_key, join(begin, end, ",")) {};

  ExtendedParameter() : cpr::Parameter("", "") {};
};

class CityRequest {
 private:
  std::string city_name;
  std::string country_iso2;
  const cpr::Url city_coordinates_url = cpr::Url{"https://api.api-ninjas.com/v1/city"};
  const cpr::Url city_timezone_url = cpr::Url{"https://api.api-ninjas.com/v1/timezone"};
  const cpr::Url country_url = cpr::Url{"https://api.api-ninjas.com/v1/country"};
  const cpr::Header header = cpr::Header{{"X-Api-Key", "GA2fO3IeI9FCCvdpj1xTcVMAUC2IJRxuYw26FebZ"}};

 public:
  explicit CityRequest(const std::string& city_name, const std::string& country_name) : city_name(city_name),
                                                                                        country_iso2(country_name) {};

  json RequestCoords();
  json RequestTimezone();
  json RequestCountryName();
};

class WeatherRequest {
 private:
  cpr::Url url = cpr::Url{"https://api.open-meteo.com/v1/forecast"};
  int forecast_days = 16; // Should be <= 16
  ExtendedParameter city_latitude;
  ExtendedParameter city_longitude;
  ExtendedParameter timezone;
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
  ExtendedParameter hourly{"hourly", hourly_params.begin(), hourly_params.end()};

 public:
  WeatherRequest() = default;

  WeatherRequest(const json& parameters) {
	  city_latitude = {"latitude", parameters["latitude"]};
	  city_longitude = {"longitude", parameters["longitude"]};
	  timezone = {"timezone", parameters["timezone"]};
  }

  json RequestWeather();

  void update_forecast_days();
};
} // namespace Requests