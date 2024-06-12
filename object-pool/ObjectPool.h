#pragma once
#ifndef OBJECTPOOL_H
#define OBJECTPOOL_H

#include <vector>
#include <numeric>
#include <cassert>
#include <memory>
#include <cmath>

/**
* �ṩһ������أ��������ؿ������ṩĬ�Ϲ��캯���ĵ��κ��ࡣ
*
* acquireObject() �ӿ��ж����б��з���һ���������û�и���Ŀ��ж���acquireObject() ����
* һ���µĶ���顣
*
* �ö����ֻ��������������Զ����Ӷ�������Ƴ���֪������ر����١�
*
* acquireObject() ����һ�������Զ���ɾ������ std::shared_ptr����std::shared_ptr����������
* ���ü���Ϊ0ʱ����ɾ�������Զ�������Żص�������С�
*/
template <typename T, typename Allocator = std::allocator<T>>
class ObjectPool {
public:
	ObjectPool() = default;
	explicit ObjectPool(const Allocator& allocator) : m_allocator(allocator) {}
	virtual ~ObjectPool();

	// �����ƶ�������ƶ���ֵ
	ObjectPool(ObjectPool&&) = default;
	ObjectPool& operator=(ObjectPool&&) noexcept = default;

	// ��ֹ��������Ϳ�����ֵ
	ObjectPool(const ObjectPool&) = delete;
	ObjectPool& operator=(const ObjectPool&) = delete;

	// �Ӷ�����з���һ�����󣬿��ṩ��������Щ����������ת����T�Ĺ��캯��
	template <typename ...Args>
	std::shared_ptr<T> acquireObject(Args&& ...args);

private:
	// �����������д���Tʵ�����ڴ�飬
	// ����ÿ���ڴ�飬�洢ָ�����һ�������ָ��
	std::vector<T*> m_pool;
	// ����ָ��������п���Tʵ����ָ��
	std::vector<T*> m_freeObjects;
	// ����ÿ���ڴ��ĳ�ʼ��С
	static const size_t ms_initialChunkSize{ 5 };
	// ����ÿ���ڴ���������С
	size_t m_newChunkSize{ ms_initialChunkSize };
	// ����һ��û�г�ʼ���ڴ�Ŀ飬�����������m_newChunkSize��Tʵ��
	void addChunk();
	// ���ڷ�����ͷſ�ķ�����
	Allocator m_allocator;
};

template <typename T, typename Allocator>
ObjectPool<T, Allocator>::~ObjectPool() {
	// Note: ���ʵ�ּٶ������ٶ����֮ǰ�����ж����Ѿ��Żض�����С�
	assert(m_freeObjects.size() ==
		ms_initialChunkSize * (std::pow(2, m_pool.size()) - 1));

	size_t chunkSize{ ms_initialChunkSize };
	for (auto* chunk : m_pool) {
		m_allocator.deallocate(chunk, chunkSize);
		chunkSize *= 2;
	}
	m_pool.clear();
}

template <typename T, typename Allocator>
void ObjectPool<T, Allocator>::addChunk() {
	std::cout << "Allocating new chunk..." << std::endl;

	auto* firstNewObject{ m_allocator.allocate(m_newChunkSize) };
	m_pool.push_back(firstNewObject);

	auto oldFreeObjectSize{ m_freeObjects.size() };
	m_freeObjects.resize(oldFreeObjectSize + m_newChunkSize);
	std::iota(std::begin(m_freeObjects) + oldFreeObjectSize, std::end(m_freeObjects),
		firstNewObject);

	m_newChunkSize *= 2;
}

template <typename T, typename Allocator>
template <typename ...Args>
std::shared_ptr<T> ObjectPool<T, Allocator>::acquireObject(Args&& ...args) {
	if (m_freeObjects.empty()) { addChunk(); }

	T* object{ m_freeObjects.back() };

	new(object) T{ std::forward<Args>(args)... };

	m_freeObjects.pop_back();

	return std::shared_ptr<T>{object, [this](T* object) {
		std::destroy_at(object);
		m_freeObjects.push_back(object);
	}};
}


#endif // OBJECTPOOL_H