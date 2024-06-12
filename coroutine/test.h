#pragma once

#ifndef TEST_H
#define TEST_H

#include <filesystem>
#include <array>


class Test final {
public:
	enum class Sex {
		Female = 0,
		Male,
		Unknown,
	};
	enum class Choice {
		A,
		B,
		C,
		D,
		Unknown,
	};

	explicit Test(const std::filesystem::path& path);

	std::wstring getScoreRecord() const;

	void print() const;

private:
	bool read(const std::filesystem::path& path);

	void countTrueOrFalseQuestionsCorrectAnswer() {
		const bool correctAnswer[] = { false, true, false, true, true, false, false, true, true, false };
		for (int i = 0; i < m_trueOrFalseQuestionsAnswer.size(); ++i) {
			m_trueOrFalseQuestionsCorrectAnswerCount +=
					(m_trueOrFalseQuestionsAnswer[i] == correctAnswer[i]);
		}
	}

	void countSingleChoiceQuestionsCorrectAnswer() {
		using
		enum Choice;
		const Choice correctAnswer[] = { A, B, C, C, D, D, A, A, C, D };
		for (int i = 0; i < m_singleChoiceQuestionsAnswer.size(); ++i) {
			m_singleChoiceQuestionsCorrectAnswerCount +=
					(m_singleChoiceQuestionsAnswer[i] == correctAnswer[i]);
		}

	}

	std::wstring m_id;
	Sex m_sex{ Sex::Unknown };
	std::wstring m_name;
	std::array<bool, 10> m_trueOrFalseQuestionsAnswer{};
	std::array<Choice, 10> m_singleChoiceQuestionsAnswer{};
	int m_trueOrFalseQuestionsCorrectAnswerCount{ 0 };
	int m_singleChoiceQuestionsCorrectAnswerCount{ 0 };
	mutable std::wstring m_scoreRecord;
};


#endif // TEST_H