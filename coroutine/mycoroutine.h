#pragma once

#ifndef COROUTINE_H
#define COROUTINE_H

#include <coroutine>
#include <string>


struct promise;

struct coroutine : std::coroutine_handle<promise> {
	using promise_type = ::promise;

	~coroutine() {
		if (address()) {
			destroy();
		}
	}
};

struct promise {
	coroutine get_return_object() {
		coroutine handle = { coroutine::from_promise(*this) };
		return handle;
	}

	std::suspend_always initial_suspend() const noexcept { return {}; }

	std::suspend_always final_suspend() const noexcept { return {}; }

	void return_void() const noexcept {}

	void unhandled_exception() const noexcept {}

	std::suspend_always yield_value(std::wstring scoreRecord) {
		m_scoreRecord = std::move(scoreRecord);
		return {};
	}

	const std::wstring &getScoreRecord() const noexcept {
		return m_scoreRecord;
	}

private:
	std::wstring m_scoreRecord;
};



#endif // COROUTINE_H