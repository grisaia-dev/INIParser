﻿#pragma once

#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <filesystem>

namespace INIP {
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
		T get_value(const std::string& section_key, const T& defaultValue = T()) {
			try {
				auto it = m_sKeyValue.find(section_key);
				if (it != m_sKeyValue.end()) {
					T temp = defaultValue; 
					std::stringstream ss;
					ss << m_sKeyValue.find(section_key)->second;
					ss >> temp;
					return temp;
				} else {
					throw Exception_notValid("Section and key not found!! Please use one of this:");
				}

			} catch (const std::invalid_argument& ex) {
				std::cout << ex.what() << std::endl;
				for (const auto& skv : m_sKeyValue)
					std::cout << " ---> " << skv.first << std::endl;
				m_sKeyValue.clear();
				exit(2);
			}

		}
		template<>
		std::string get_value(const std::string& section_key, const std::string& defaultValue) { return m_sKeyValue.find(section_key)->second; }

	private:
		void ProcessRead(const std::string_view& str);
		void Reset();
		void Pair(const std::string& section, const std::string& key, const std::string& value);

	private:
		class Exception_runtime : public std::runtime_error {
		public:
			using std::runtime_error::runtime_error;
		};

		class Exception_notValid : public std::invalid_argument {
		public:
			using std::invalid_argument::invalid_argument;
		};

	private:
		State m_state = State::ReadyForData;
		size_t m_line = 1;
		std::string m_currentSection, m_currentKey, m_currentValue;
		std::map<std::string, std::string> m_sKeyValue;
	}; // end class Parser

} // end INIP