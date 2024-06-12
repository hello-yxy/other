// object-pool.cpp: 定义应用程序的入口点。
//

#include "object-pool.h"

class ExpensiveObject {
public:
	ExpensiveObject() = default;
	virtual ~ExpensiveObject() = default;

private:
	std::array<double, 4 * 1024 * 1024> m_data{};
};

using MyPool = ObjectPool<ExpensiveObject>;
std::shared_ptr<ExpensiveObject> getExpensiveObject(MyPool& pool) {
	auto object{ pool.acquireObject() };
	return object;
}

void processExpensiveObject(ExpensiveObject* object) {}

int main() {
	const size_t NumberOfIterations{ 500 };

	std::cout << "Starting loop using pool..." << std::endl;
	MyPool pool;
	auto start1{ std::chrono::steady_clock::now() };
	for (size_t i{ 0 }; i < NumberOfIterations; ++i) {
		auto object{ getExpensiveObject(pool) };
		processExpensiveObject(object.get());
	}
	auto end1{ std::chrono::steady_clock::now() };
	auto diff1{ end1 - start1 };
	std::cout << std::format("{}ms\n",
		std::chrono::duration<double, std::milli>(diff1).count());

	std::cout << "Startign loop using new/delete..." << std::endl;
	auto start2{ std::chrono::steady_clock::now() };
	for (size_t i{ 0 }; i < NumberOfIterations; ++i) {
		auto object{ new ExpensiveObject{} };
		processExpensiveObject(object);
		delete object;
		object = nullptr;
	}
	auto end2{ std::chrono::steady_clock::now() };
	auto diff2{ end2 - start2 };
	std::cout << std::format("{}ms\n",
		std::chrono::duration<double, std::milli>(diff2).count());

	return 0;
}
