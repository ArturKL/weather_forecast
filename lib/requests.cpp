#include "requests.h"
#include <iostream>

namespace Requests {

json handle_response(const cpr::Response& r) {
	json response;
	if (r.status_code != 200) {
		if (r.text.empty()) {
			return {{"error", std::to_string(r.status_code) + " " + r.error.message + " " + r.reason}};
		}
		response = json::parse(r.text);
		if (response.contains("error")) {
			return {{"error", response["error"]}};
		}
		return {{"error", response["message"]}};
	}
	response = json::parse(r.text);
	return response;
}

json CityRequest::RequestCoords() {
	cpr::Response r = cpr::Get(city_coordinates_url,
	                           header,
	                           cpr::Parameters{{"name", city_name},
											   {"country", country_iso2}});

	json response = handle_response(r);
	if (response.contains("error")) {
		return response;
	}
	if (response.empty()) {
		return json::parse("{\"error\": \"Couldn't find city " + city_name + ", " + country_iso2 + "\"}");
	}
	return response[0];
}

json CityRequest::RequestTimezone() {
	cpr::Response r = cpr::Get(city_timezone_url,
	                           header,
	                           cpr::Parameters{{"city", city_name},
	                                           {"country", country_iso2}});

	json response = handle_response(r);
	return response;
}

json CityRequest::RequestCountryName() {
	if (country_iso2.empty()) {
		RequestCoords();
	}
	cpr::Response r = cpr::Get(country_url,
	                           header,
	                           cpr::Parameters{{"name", country_iso2}});

	json response = handle_response(r);
	if (response.contains("error")) {
		return response;
	}

	if (response.empty()) {
		return json::parse("{\"error\": \"Couldn't find country " + country_iso2 + "\"}");
	}
	return response[0];
}

json WeatherRequest::RequestWeather() {
	cpr::Parameters parameters;
	parameters.Add(city_longitude);
	parameters.Add(city_latitude);
	parameters.Add(timezone);
	parameters.Add({"forecast_days", std::to_string(forecast_days)});
	parameters.Add(ExtendedParameter{"hourly", hourly_params.begin(), hourly_params.end()});
	cpr::Response r = cpr::Get(url, parameters);
	return json::parse(r.text);
}

} // namespace Requests