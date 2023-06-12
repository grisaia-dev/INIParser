#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <filesystem>
#include <unordered_map>

namespace INIP {
	class Exception : public std::runtime_error {
	public:
		using std::runtime_error::runtime_error;
	};

	class Parser {
	private:
		enum class State {
			ReadyForData,
			Comment,
			Section,
			Key,
			KeyDone,
			Equal,
			Variable
		};

	public:
		Parser(const std::filesystem::path& filePath);
		void Print();
		
		template<typename T>
		T get_value(const std::string& section, const std::string& key, const T& defaultValue = T()) {
			T temp = defaultValue;
			std::stringstream ss;
			ss << m_sections.find(section)->second.find(key)->second;
			ss >> temp;
			return temp;
		}
		template<>
		std::string get_value(const std::string& section, const std::string& key, const std::string& defaultValue) { return m_sections.find(section)->second.find(key)->second; }

	private:
		void ProcessRead(const std::string_view& str);
		void Reset();
		void Pair(const std::string& section, const std::string& key, const std::string& value);

	private:

	private:
		State m_state = State::ReadyForData;
		size_t m_line = 1;
		std::string m_currentSection, m_currentKey, m_currentValue;
		std::unordered_map<std::string, std::unordered_map<std::string, std::string>> m_sections;
	}; // end class Parser

} // end INIP