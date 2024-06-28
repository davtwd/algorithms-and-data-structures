#pragma once

#include <string>
#include <functional>
#include <Cli/lib/core/structs.h>

class Library {
	private:
		static std::string formatTwoDigits(int& value);
	public:
		static std::function<bool(const Route&, const Route&)> getAddressComparator();
		static std::function<bool(const Route&, const Route&)> getLifetimeComparator();
		static std::function<bool(const Route*, const Route*)> getAddressComparatorPtr();
		static std::function<bool(const Route*, const Route*)> getLifetimeComparatorPtr();
		// networking
		static unsigned int getDecFromAddress(std::string address);
		static std::string getAddressFromDec(const unsigned int& address);
		static Address getNetworkAddress(const unsigned int& network);
		static bool isValidAddress(const std::string& address);
		static bool isValidLifetime(const std::string& lifetime);
		static bool matchWithAddress(const Route& route, std::string& address);
		static bool matchWithLifetime(const Route& route, const unsigned int& lifetimeFrom, const unsigned int& lifetimeTo);
		// parsing and formatting
		static unsigned int parseTimeToSeconds(std::string& time);
		static std::string formatSecondsToTime(const unsigned int& seconds);
		// instance deletion
		Library() = delete;
};