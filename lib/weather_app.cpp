#include "weather_app.h"
#include <ctime>
#include <sstream>

namespace weatherapp {
using namespace Requests;

json WeatherPage::configure_location() {
	std::wcout << L"Updating city information..." << std::endl;
	CityRequest city_request(city_name, country_name);
	json location_info;
	json coords_response = city_request.RequestCoords();
	if (coords_response.contains("error")) {
		return coords_response;
	}
	location_info["latitude"] = to_string(coords_response["latitude"]);
	location_info["longitude"] = to_string(coords_response["latitude"]);

	json timezone_response = city_request.RequestTimezone();
	if (timezone_response.contains("error")) {
		return timezone_response;
	}
	location_info["timezone"] = timezone_response["timezone"];

	// City API response contains iso2 name (two letter name) of country. Make a request to get the full country name
	json country_response = city_request.RequestCountryName();
	if (country_response.contains("error")) {
		return country_response;
	}
	full_country_name = country_response["name"];
	return location_info;
}

void WeatherPage::update_forecast() {
	if (!page_is_valid) {
		return;
	}
	std::wcout << L"Updateing forecast..." << std::endl;
	forecast = weather_request.RequestWeather();
	day_cards.clear();
	for (int i = 0; i < MAX_FORECAST_DAYS; i++) {
		day_cards.push_back(Day(forecast, i));
	}
	last_update_time = std::chrono::steady_clock::now();
}

void WeatherPage::display_page() {
	system("cls");
	if (!page_is_valid) {
		std::wcout << L" _____                    \n"
		               "|  ___|                   \n"
		               "| |__ _ __ _ __ ___  _ __ \n"
		               "|  __| '__| '__/ _ \\| '__|\n"
		               "| |__| |  | | | (_) | |   \n"
		               "\\____/_|  |_|  \\___/|_|   \n"
		               "                          \n";
		std::wcout << error_message << std::endl;
		return;
	}
	std::wcout << L"Weather Forecast: "
	           << string_to_wstring(city_name) << L", " << string_to_wstring(full_country_name) << std::endl;

	for (int i = 0; i < forecast_days; i++) {
		day_cards[i].draw_day();
	}

}

void WeatherPage::increase_forecast_days() {
	if (forecast_days == 15) {
		return;
	}
	forecast_days++;
}

void WeatherPage::decrease_forecast_days() {
	if (forecast_days == 0) {
		return;
	}
	forecast_days--;
}

bool WeatherPage::check_time_for_update() {
	long long update_interval_ms = update_frequency * 60 * 1000;
	auto current_time = std::chrono::steady_clock::now();

	// Calculate the elapsed time since the last call
	auto elapsed_time_ms =
		std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_update_time).count();

	return elapsed_time_ms >= update_interval_ms && page_is_valid;
}

std::wstring WeatherPage::Day::get_ascii_art_from_weathercode(int weathercode, int line_number) {
	switch (weathercode) {
		case 0:
		case 1:
			return std::vector<std::wstring>{
				L"    \\ . /    ",
				L"   - .-. -   ",
				L"  ‒ (   ) ‒  ",
				L"   . `-᾿ .    ",
				L"    / ' \\    "
			}[line_number];
		case 2:
			return std::vector<std::wstring>{
				L"    \\  /      ",
				L"  _ /\"\".-.     ",
				L"    \\_(   ).   ",
				L"    /(___(__)  ",
				L"               "
			}[line_number];
		case 3:
			return std::vector<std::wstring>{
				L"             ",
				L"     .--.    ",
				L"  .-(    ).  ",
				L" (___.__)__) ",
				L"             "
			}[line_number];
		case 45:
		case 48:
			return std::vector<std::wstring>{
				L"             ",
				L" _ - _ - _ - ",
				L"  _ - _ - _  ",
				L" _ - _ - _ - ",
				L"             "
			}[line_number];
		case 51:
		case 53:
		case 55:
		case 56:
		case 57:
		case 61:
		case 63:
			return std::vector<std::wstring>{
				L"     .-.     ",
				L"    (   ).   ",
				L"   (___(__)  ",
				L"    ʻ ʻ ʻ ʻ  ",
				L"   ʻ ʻ ʻ ʻ   "
			}[line_number];
		case 65:
			return std::vector<std::wstring>{
				L"     .-.     ",
				L"    (   ).   ",
				L"   (___(__)  ",
				L"  ‚ʻ‚ʻ‚ʻ‚ʻ   ",
				L"  ‚ʻ‚ʻ‚ʻ‚ʻ   "
			}[line_number];
		case 66:
		case 67:
		case 80:
		case 81:
			return std::vector<std::wstring>{
				L" _`/\"\".-.    ",
				L"  ,\\_(   ).  ",
				L"   /(___(__) ",
				L"     ʻ ʻ ʻ ʻ ",
				L"    ʻ ʻ ʻ ʻ  "
			}[line_number];
		case 82:
			return std::vector<std::wstring>{
				L" _`/\"\".-.    ",
				L"  ,\\_(   ). ",
				L"   /(___(__) ",
				L"   ‚ʻ‚ʻ‚ʻ‚ʻ  ",
				L"   ‚ʻ‚ʻ‚ʻ‚ʻ  "
			}[line_number];
		case 71:
		case 73:
			return std::vector<std::wstring>{
				L"     .-.     ",
				L"    (   ).   ",
				L"   (___(__)  ",
				L"    *  *  *  ",
				L"   *  *  *   "
			}[line_number];
		case 75:
			return std::vector<std::wstring>{
				L"     .-.     ",
				L"    (   ).   ",
				L"   (___(__)  ",
				L"   * * * *   ",
				L"  * * * *    "
			}[line_number];
		case 77:
		case 85:
		case 86:
			return std::vector<std::wstring>{
				L" _`/\"\".-.    ",
				L"  ,\\_(   ).  ",
				L"   /(___(__) ",
				L"    * * * *  ",
				L"   * * * *   "
			}[line_number];
		case 95:
		case 96:
		case 99:
			return std::vector<std::wstring>{
				L"     .-.     ",
				L"    (    ).  ",
				L"   (___(__)  ",
				L"    /__      ",
				L"     /       "
			}[line_number];
		default:
			return std::vector<std::wstring>{
				L"  ___ ___ ___  ",
				L" | | |   | | | ",
				L" |_  | | |_  | ",
				L"   |_|___| |_| ",
			}[line_number];
	}
}

std::wstring WeatherPage::Day::weathercode_to_text(int weathercode, int max_width) {
	std::wstring res;

	switch (weathercode) {
		case 0: res = L"Clear sky";
			break;
		case 1: res = L"Mainly clear";
			break;
		case 2: res = L"Partly cloudy";
			break;
		case 3: res = L"Overcast";
			break;
		case 45: res = L"Fog";
			break;
		case 48: res = L"Depositing rime fog";
			break;
		case 51: res = L"Drizzle: Light intensity";
			break;
		case 53: res = L"Drizzle: Moderate intensity";
			break;
		case 55: res = L"Drizzle: Dense intensity";
			break;
		case 56: res = L"Freezing Drizzle: Light intensity";
			break;
		case 57: res = L"Freezing Drizzle: Dense intensity";
			break;
		case 61: res = L"Rain: Slight intensity";
			break;
		case 63: res = L"Rain: Moderate intensity";
			break;
		case 65: res = L"Rain: Heavy intensity";
			break;
		case 66: res = L"Freezing Rain: Light intensity";
			break;
		case 67: res = L"Freezing Rain: Heavy intensity";
			break;
		case 71: res = L"Snow fall: Slight intensity";
			break;
		case 73: res = L"Snow fall: Moderate intensity";
			break;
		case 75: res = L"Snow fall: Heavy intensity";
			break;
		case 77: res = L"Snow grains";
			break;
		case 80: res = L"Rain showers: Slight intensity";
			break;
		case 81: res = L"Rain showers: Moderate intensity";
			break;
		case 82: res = L"Rain showers: Violent intensity";
			break;
		case 85: res = L"Snow showers: Slight intensity";
			break;
		case 86: res = L"Snow showers: Heavy intensity";
			break;
		case 95: res = L"Thunderstorm: Slight or moderate";
			break;
		case 96: res = L"Thunderstorm with slight hail";
			break;
		case 99: res = L"Thunderstorm with heavy hail";
			break;
		default: res = L"Unknown code";
			break;
	}

	if (res.length() > max_width) {
		res = res.substr(0, max_width - 1) + L"…";
	}
	return res;
}

std::wstring WeatherPage::Day::convert_day_format(const std::string& date) {
	std::tm tm_date;
	int year, month, day, hour, minutes;
	std::sscanf(date.c_str(), "%d-%d-%dT%d:%d", &year, &month, &day, &hour, &minutes);
	tm_date.tm_year = year - 1900; // Years since 1900
	tm_date.tm_mon = month - 1; // Months since January (0-11)
	tm_date.tm_mday = day;
	tm_date.tm_hour = hour;
	tm_date.tm_min = minutes;
	tm_date.tm_sec = 0;

	// Convert to time_t and back to fill all the missing fields, specifically day of week
	std::time_t time_temp = std::mktime(&tm_date);
	const std::tm* full_date = std::localtime(&time_temp);

	// Format tm struct to "Fri 21 Jul"
	char output_date[16]; // Buffer to store the formatted date
	strftime(output_date, sizeof(output_date), "%a %d %b", full_date);
	return std::wstring(output_date, output_date + strlen(output_date));
}

std::wstring WeatherPage::Day::GetTemperatureString(int temperature, int apparent_temperature) {
	std::wstring result;
	if (temperature > 0) {
		result += L'+';
	}
	result += std::to_wstring(temperature) + L'(' + std::to_wstring(apparent_temperature) + L") °C";

	return result;
}

std::wstring WeatherPage::Day::GetWindString(int wind_direction, int wind_speed) {
	std::wstring result;
	if (wind_direction < 22.5 || wind_direction > 337.5) {
		result += L'↑';
	} else if (wind_direction > 22.5 && wind_direction < 67.5) {
		result += L'↗';
	} else if (wind_direction > 67.5 && wind_direction < 112.5) {
		result += L'→';
	} else if (wind_direction > 112.5 && wind_direction < 157.5) {
		result += L'↘';
	} else if (wind_direction > 157.5 && wind_direction < 202.5) {
		result += L'↓';
	} else if (wind_direction > 202.5 && wind_direction < 247.5) {
		result += L'↙';
	} else if (wind_direction > 247.5 && wind_direction < 292.5) {
		result += L'←';
	} else {
		result += L'↖';
	}
	result += L' ' + std::to_wstring(wind_speed) + L" km/h";
	return result;
}

std::wstring WeatherPage::Day::GetPrecipitationString(float precipitation, int probability) {
	std::wstringstream ss;
	// Convert the float value to a wstring with two decimal places
	ss << std::fixed << std::setprecision(2) << precipitation;
	ss << L" mm | " + std::to_wstring(probability) + L"%";
	return ss.str();
}

void WeatherPage::Day::draw_day() {
	std::wcout.setf(std::ios::left);
	std::wstring formated_date = convert_day_format(date);
	std::wcout << L"                                                       ┌─────────────┐" << std::endl;
	std::wcout << L"┌──────────────────────────────┬───────────────────────┤ " << formated_date;
	std::wcout << L"  ├───────────────────────┬──────────────────────────────┐" << std::endl;
	std::wcout << L"│            Morning           │             Noon      └──────┬──────┘"
	              L"     Evening           │             Night            │" << std::endl;
	std::wcout
		<< L"├──────────────────────────────┼──────────────────────────────┼"
		   L"──────────────────────────────┼──────────────────────────────┤" << std::endl;

	int line_counter = 0;
	int card_half_width = 15;
	// Weathercode
	for (int i = 0; i < 4; i++) {
		std::wstring ascii = get_ascii_art_from_weathercode(forecast[i]["weathercode"],
		                                                    line_counter);
		std::wcout << L"│" << std::setw(card_half_width) << ascii;
		std::wcout << std::setw(card_half_width) << weathercode_to_text(forecast[i]["weathercode"],
		                                                                card_half_width);
	}
	std::wcout << L"│" << std::endl;
	line_counter++;

	// Temperature
	for (int i = 0; i < 4; i++) {
		std::wstring ascii = get_ascii_art_from_weathercode(forecast[i]["weathercode"],
		                                                    line_counter);
		std::wcout << L"│" << std::setw(card_half_width) << ascii;
		int temperature = static_cast<int>(forecast[i]["temperature_2m"]);
		int apparent_temperature = static_cast<int>(forecast[i]["apparent_temperature"]);

		std::wcout << std::setw(card_half_width) << GetTemperatureString(temperature, apparent_temperature);
	}
	std::wcout << L"│" << std::endl;
	line_counter++;

	// Wind
	for (int i = 0; i < 4; i++) {
		std::wstring ascii = get_ascii_art_from_weathercode(forecast[i]["weathercode"],
		                                                    line_counter);
		std::wcout << L"│" << std::setw(card_half_width) << ascii;
		int windspeed = static_cast<int>(forecast[i]["windspeed_10m"]);
		int wind_direction = forecast[i]["winddirection_10m"];

		std::wcout << std::setw(card_half_width) << GetWindString(wind_direction, windspeed);
	}
	std::wcout << L"│" << std::endl;
	line_counter++;

	// Visibility
	for (int i = 0; i < 4; i++) {
		std::wstring ascii = get_ascii_art_from_weathercode(forecast[i]["weathercode"],
		                                                    line_counter);
		std::wcout << L"│" << std::setw(card_half_width) << ascii;
		int visibility = static_cast<int>(forecast[i]["visibility"]) / 1000;

		std::wcout << std::setw(card_half_width) << std::to_wstring(visibility) + L" km";
	}
	std::wcout << L"│" << std::endl;
	line_counter++;

	// Precipitation
	for (int i = 0; i < 4; i++) {
		std::wstring ascii = get_ascii_art_from_weathercode(forecast[i]["weathercode"],
		                                                    line_counter);
		std::wcout << L"│" << std::setw(card_half_width) << ascii;
		float precipitation = forecast[i]["precipitation"];
		int probability = 0;
		if (!forecast[i]["precipitation_probability"].is_null()) {
			probability = forecast[i]["precipitation_probability"];
		}

		std::wcout << std::setw(card_half_width) << GetPrecipitationString(precipitation, probability);
	}
	std::wcout << L"│" << std::endl;

	std::wcout
		<< L"└──────────────────────────────┴──────────────────────────────┴"
		   L"──────────────────────────────┴──────────────────────────────┘";
	std::wcout << std::endl;
}
} // namespace weatherapp