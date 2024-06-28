#include <regex>
#include <sstream>
#include <tuple>

#include <Cli/lib/core/lib.h>

// private method(s)

std::string Library::formatTwoDigits(int& value) {
	return value < 10 ? "0" + std::to_string(value) : std::to_string(value);
}

//

std::function<bool(const Route&, const Route&)> Library::getAddressComparator() {
	return [](const Route& r1, const Route& r2) -> bool {
		return std::tie(r1.network, r1.prefix, r1.lifetime) < std::tie(r2.network, r2.prefix, r2.lifetime);
	};
}

#include <tuple>

std::function<bool(const Route&, const Route&)> Library::getLifetimeComparator() {
	return [](const Route& r1, const Route& r2) {
		return std::tie(r1.lifetime, r1.network, r1.prefix) < std::tie(r2.lifetime, r2.network, r2.prefix);
	};
}

std::function<bool(const Route*, const Route*)> Library::getAddressComparatorPtr() {
	return [](const Route* r1, const Route* r2) {
		return std::tie(r1->network, r1->prefix, r1->lifetime) < std::tie(r2->network, r2->prefix, r2->lifetime);
	};
}

std::function<bool(const Route*, const Route*)> Library::getLifetimeComparatorPtr() {
	return [](const Route* r1, const Route* r2) {
		return std::tie(r1->lifetime, r1->network, r1->prefix) < std::tie(r2->lifetime, r2->network, r2->prefix);
	};
}

// networking

unsigned int Library::getDecFromAddress(std::string address) { // creating a copy of the string to avoid problems with erase method

	std::string octetA, octetB, octetC, octetD;

	size_t pos = 0;
	pos = address.find('.'); octetA = address.substr(0, pos); address.erase(0, pos + 1);
	pos = address.find('.'); octetB = address.substr(0, pos); address.erase(0, pos + 1);
	pos = address.find('.'); octetC = address.substr(0, pos); address.erase(0, pos + 1);
	octetD = address;

	return (std::stoul(octetA) << 24) + (std::stoul(octetB) << 16) + (std::stoul(octetC) << 8) + std::stoul(octetD);
}

std::string Library::getAddressFromDec(const unsigned int& address) {

	unsigned int octetA = (address & 0xFF000000) >> 24;
	unsigned int octetB = (address & 0x00FF0000) >> 16;
	unsigned int octetC = (address & 0x0000FF00) >> 8;
	unsigned int octetD = address & 0x000000FF;

	return std::to_string(octetA) + "." + std::to_string(octetB) + "." + std::to_string(octetC) + "." + std::to_string(octetD);
}

Address Library::getNetworkAddress(const unsigned int& network) {

	unsigned int address = network;

	unsigned int octetA = (address & 0xFF000000) >> 24;
	unsigned int octetB = (address & 0x00FF0000) >> 16;
	unsigned int octetC = (address & 0x0000FF00) >> 8;
	unsigned int octetD = address & 0x000000FF;

	Address temp = {
		octetA,
		octetB,
		octetC,
		octetD
	};

	return temp;
}

bool Library::isValidAddress(const std::string& address) {

	const std::regex pattern(
		"(^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$)" // 127.0.0.1
	);

	return std::regex_match(address, pattern);
}

bool Library::isValidLifetime(const std::string& lifetime) {

	const std::regex whd(R"(^(\d+w)?(\d+d)?(\d+h)?$)"); // 6w03d2h; 6w3d; 6w
	const std::regex hms(R"(^(\d+w)?(\d+d)?(\d+h)?$|^(\d{1,2}:\d{2}(:\d{2})?)$)"); // 12:30:30; 12:30

	return std::regex_match(lifetime, whd) || std::regex_match(lifetime, hms);
}

bool Library::matchWithAddress(const Route& route, std::string& address) {
	const unsigned int prefix = route.prefix;
	const unsigned int network = (prefix == 0) ? (0) : (getDecFromAddress(address) >> (32 - prefix) << (32 - prefix));
	return network == route.network;
}

bool Library::matchWithLifetime(const Route& route, const unsigned int& lifetimeFrom, const unsigned int& lifetimeTo) {
	return route.lifetime >= lifetimeFrom && route.lifetime <= lifetimeTo;
}

// parsing and formatting

unsigned int Library::parseTimeToSeconds(std::string& lifetime) {

	if (lifetime.empty()) return 0; // empty string

	if (lifetime.find(':') != std::string::npos) {
		char colon;
		std::istringstream iss(lifetime);
		int hours, minutes, seconds;
		iss >> hours >> colon >> minutes >> colon >> seconds;
		return hours * 3600 + minutes * 60 + seconds;
	}

	size_t pos_w = lifetime.find('w');
	size_t pos_d = lifetime.find('d');
	size_t pos_h = lifetime.find('h');

	int weeks = pos_w != std::string::npos ? std::stoi(lifetime.substr(0, pos_w)) : 0;
	int days = pos_d != std::string::npos ? std::stoi(lifetime.substr(pos_w + 1, pos_d - pos_w - 1)) : 0;
	int hours = pos_h != std::string::npos ? std::stoi(lifetime.substr(pos_d + 1, pos_h - pos_d - 1)) : 0;

	return weeks * 604800 + days * 86400 + hours * 3600;
}

std::string Library::formatSecondsToTime(const unsigned int& seconds) {

	int weeks = seconds / 604800;
	int remainingSeconds = seconds % 604800;
	int days = remainingSeconds / 86400;
	remainingSeconds %= 86400;
	int hours = remainingSeconds / 3600;

	std::string formattedTime;
	if (weeks == 0 && days == 0) {
		remainingSeconds %= 3600;
		int minutes = remainingSeconds / 60;
		remainingSeconds %= 60;
		int secs = remainingSeconds;
		return formatTwoDigits(hours) + ":" + formatTwoDigits(minutes) + ":" + formatTwoDigits(secs);
	}

	if (weeks > 0)
		formattedTime += formatTwoDigits(weeks) + "w";
	if (days > 0)
		formattedTime += formatTwoDigits(days) + "d";
	if (hours > 0)
		formattedTime += formatTwoDigits(hours) + "h";
	return formattedTime;
}