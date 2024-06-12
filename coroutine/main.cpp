#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <set>
#include <thread>
#include <chrono>

#include "mycoroutine.h"
#include "test.h"


const char* g_targetDir = "./assets";


coroutine checkNewFile(std::set<std::wstring> &doneTest) {
	for (const auto &dirEntry: std::filesystem::directory_iterator{ g_targetDir }) {
		const auto& path = dirEntry.path();
		const auto id = path.stem().wstring();
		if (auto it = doneTest.find(id); it != doneTest.end()) {
			std::wcout << L"Duplicate file: " << path.filename() << std::endl;
			continue;
		}
		std::wcout << L"New file: " << path.filename() << std::endl;

		doneTest.insert(id);
		co_yield Test(path).getScoreRecord();
	}
}

int main() {
	using namespace std::chrono_literals;
	std::set<std::wstring> doneTest;

//	system("chcp 65001");
	std::wofstream ofs("score.txt");
	if (!ofs.is_open()) {
		exit(EXIT_FAILURE);
	}

	auto checkOnce = [&ofs, &doneTest]() {
		const auto c = checkNewFile(doneTest);
		c.resume();

		while (!c.done()) {
			const auto &scoreRecord = c.promise().getScoreRecord();
			ofs << scoreRecord << '\n';
			c.resume();
		}
		ofs.flush();

	};

	std::jthread t([&checkOnce]() {
		std::wcout << L"begin..." << std::endl;
		const auto startTime = std::chrono::system_clock::now();
		do {
			checkOnce();
			std::this_thread::sleep_for(1s);
		} while ((std::chrono::system_clock::now() - startTime) < 3s);
		checkOnce();
		std::wcout << L"end..." << std::endl;
	});

	return 0;
}