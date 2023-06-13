#include "ini_parser.hpp"

INIP::Parser::Parser(const std::filesystem::path& filePath) {
	std::ifstream fileIn(filePath, std::ios::binary | std::ios::in); // Open file in binary mode | Open for reading
	std::string forEx = "Can't open file " + filePath.string();
	try {
		if (!fileIn.is_open())
			throw Exception_runtime(forEx); // Хотелось что бы можно было бы передавать так "can't open file \"{}\"", filePath. Как это можно сделать?
	} catch (const std::runtime_error& ex) {
		std::cout << ex.what() << std::endl;
		exit(1);
	}


	fileIn.seekg(0, std::ios::end); // Перемещаем указатель что бы узнать размер данных
	size_t fileSize = fileIn.tellg(); // сохраняем размер данных
	fileIn.seekg(0, std::ios::beg); // переносим в первоначальное положение
	
	std::string fileData(fileSize, ' ');
	fileIn.read(&fileData[0], fileSize); // Переносим данные из файла в строку
	
	ProcessRead(fileData);
}

void INIP::Parser::Print() {
	for (const auto& data : m_sKeyValue) {
		std::cout << data.first << " --> " << data.second << std::endl;
	}
}

void INIP::Parser::ProcessRead(const std::string_view& str) {
	try {
		Reset();
		for (char c : str) {
			// Cath control keys
			if (std::iscntrl(c))
			{
				switch (c) {
					case '\t':
						[[fallthrough]];
					case '\n':
						break;
					default:
						continue;
				}
			} // end if (std::iscntrl(c))

			switch (m_state) {
				case State::ReadyForData:
					if (c == ';')
						m_state = State::Comment;
					else if (c == '[') {
						m_line += 1;
						m_currentSection.clear();
						m_state = State::Section;
					} else if (c == ' ')
						continue;
					else if (c == '\t')
						continue;
					else if (c == '\n') {
						continue;
					} else {
						m_currentKey.clear();
						m_currentKey += c;
						m_state = State::Key;
					}
					break;
				case State::Comment:
					if (c == '\n') {
						m_state = State::ReadyForData;
						m_line += 1;
					}
					break;
				case State::Section:
					if (c == ']') {
						m_state = State::ReadyForData;
						m_line += 1;
					} else if (c == '\t')
						throw Exception_runtime("Tabs are not allowed in section name! At line: " + std::to_string(m_line));
					else if (c == '\n')
						throw Exception_runtime("Newlines are not allowed in section name! At line: " + std::to_string(m_line));
					else
						m_currentSection += c;
					break;
				case State::Key:
					if (c == ' ')
						m_state = State::KeyDone;
					else if (c == '=')
						m_state = State::Equal;
					else if (c == '\t') {
						//throw Exception("Tabs are not allowed in the key! At line: " + std::to_string(line)); // если хотим что бы нельзя было делать табуляцию после ключа
						continue;
					} else if (c == '\n') {
						throw Exception_runtime("Newlines are not allowed in the key! At line: " + std::to_string(m_line));
					} else
						m_currentKey += c;
					break;
				case State::KeyDone:
					if (c == ' ')
						continue;
					else if (c == '\t')
						continue;
					else if (c == '\n')
						throw Exception_runtime("not found '='! At line: " + std::to_string(m_line));
					else if (c == '=')
						m_state = State::Equal;
					else
						throw Exception_runtime("Keys are not allowed to have spaces in them! At line: " + std::to_string(m_line));
					break;
				case State::Equal:
					if (c == ' ')
						continue;
					else if (c == '\t')
						continue;
					else if (c == '\n')
						throw Exception_runtime("Value can't be empty! At line: " + std::to_string(m_line));
					else {
						m_currentValue.clear();
						m_currentValue += c;
						m_state = State::Variable;
					}
					break;
				case State::Variable:
					if (c == '\n') {
						Pair(m_currentSection, m_currentKey, m_currentValue);
						m_line += 1;
						m_state = State::ReadyForData;
					} else if (c == ';') {
						Pair(m_currentSection, m_currentKey, m_currentValue);
						m_state = State::Comment;
					} else {
						m_currentValue += c;
					}
					break;
			} // end switch (m_state)
		} // end for
	} catch (const std::runtime_error& ex) {
		std::cout << ex.what() << std::endl;
		exit(1);
	}


} // end function

void INIP::Parser::Reset() {
	m_currentSection = "";
	m_currentKey = "";
	m_currentValue = "";
	m_state = State::ReadyForData;
}

void INIP::Parser::Pair(const std::string& section, const std::string& key, const std::string& value) {
	std::string temp = section + "." + key;
	if (m_sKeyValue.count(temp))
		m_sKeyValue[temp] = value;
	else if (!m_sKeyValue.count(temp))
		m_sKeyValue.emplace(temp, value);
}
