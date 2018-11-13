#ifndef SINGLETON_HPP
#define SINGLETON_HPP

#include "NonCopyable.hpp"
#include <memory>

template <typename T>
class Singleton : public NonCopyable {
private:
	static std::shared_ptr<T> instance;
public:
	static const T& getConstInstance(void) {
		return *instance;
	}
	static T& getMutableInstance(void) {
		return *instance;
	}
	static bool isDestroyed(void) {
		return instance == nullptr;
	}
	static void destroyInstance(void) {
		instance.reset();
	}
};

#endif //SINGLETON_HPP