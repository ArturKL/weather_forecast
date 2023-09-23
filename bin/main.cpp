#include <lib/weather_app.h>

#include <codecvt>
#include <fcntl.h>
#include <iostream>
#include <locale>
#include <string>
#include <Windows.h>

std::wstring StringToWideString(const std::string& str) {
	// Get the required buffer size for the conversion
	int bufferSize = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);

	// Allocate buffer
	wchar_t* buffer = new wchar_t[bufferSize];

	// Do the actual conversion
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, buffer, bufferSize);

	std::wstring result(buffer);
	delete[] buffer;

	return result;
}

int main() {
	using namespace weatherapp;

	// Makes wcout work
	std::ios_base::sync_with_stdio(false);
	std::locale utf8(std::locale(), new std::codecvt_utf8_utf16<wchar_t>);
	std::wcout.imbue(utf8);
	std::wcerr.imbue(utf8);

//	std::string city_name = "Kazan";
//	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
//	std::wstring wstring = converter.from_bytes(city_name);
//	std::wcerr << wstring << '\n';

	WeatherApp wa("config.json");
	wa.Start();
	return 0;
}