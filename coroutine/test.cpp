#include "test.h"

#include <iostream>
#include <format>
#include <fstream>


Test::Test(const std::filesystem::path& path) {
	if (read(path)) {
		countTrueOrFalseQuestionsCorrectAnswer();
		countSingleChoiceQuestionsCorrectAnswer();
	}
}


std::wstring Test::getScoreRecord() const {
	if (m_scoreRecord.empty()) {
		m_scoreRecord += m_id;
		m_scoreRecord += L", ";
		m_scoreRecord += m_name;
		m_scoreRecord += L", ";
		m_scoreRecord += (L'0' + (wchar_t)m_sex); // L'0'女，L'1'男
		m_scoreRecord += L", ";
		m_scoreRecord += std::to_wstring(m_trueOrFalseQuestionsCorrectAnswerCount);
		m_scoreRecord += L", ";
		m_scoreRecord += std::to_wstring(m_singleChoiceQuestionsCorrectAnswerCount);
		m_scoreRecord += L", ";
		m_scoreRecord += std::to_wstring(3 * m_trueOrFalseQuestionsCorrectAnswerCount);
		m_scoreRecord += L", ";
		m_scoreRecord += std::to_wstring(7 * m_singleChoiceQuestionsCorrectAnswerCount);
	}

	return m_scoreRecord;
}

void Test::print() const {
	std::wcout << std::format(
		L"id: {}\n"
		L"name: {}\n"
		L"sex: {}\n", m_id, m_name, (int)m_sex
	) << std::endl;
}

bool Test::read(const std::filesystem::path& path) {
	m_id = path.stem().wstring();

	std::wifstream ifs(path);
	if (!ifs.is_open()) {
		return false;
	}

	int countLine{ 1 };
	for (std::wstring line; std::getline(ifs, line); ++countLine) {
		if (1 == countLine) { // 读取性别
			if (1 == line.length() && (L'0' == line[0] || L'1' == line[0])) {
				// 0 女 1 男
				m_sex = static_cast<Sex>(line[0] - L'0');
			} else {
				m_sex = Sex::Unknown;
			}
		} else if (2 == countLine) { // 读取姓名
			m_name = line;
		} else if (3 <= countLine && 12 >= countLine) { // 读取判断题答案
			m_trueOrFalseQuestionsAnswer[countLine - 3] = (L"O" == line);
		} else if (13 <= countLine && 22 >= countLine) { // 读取选择题答案
			Choice choice = Choice::Unknown;
			if (L"A" == line) {
				choice = Choice::A;
			} else if (L"B" == line) {
				choice = Choice::B;
			} else if (L"C" == line) {
				choice = Choice::C;
			} else if (L"D" == line) {
				choice = Choice::D;
			}
			m_singleChoiceQuestionsAnswer[countLine - 13] = choice;
		}
	}

	return true;
}
