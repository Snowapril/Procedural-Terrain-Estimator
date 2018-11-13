#ifndef NON_COPYABLE_HPP
#define NON_COPYABLE_HPP

class NonCopyable {
public:
	NonCopyable() = default;
	NonCopyable(const NonCopyable& other) = delete;
	NonCopyable& operator=(const NonCopyable& other) = delete;
	NonCopyable(NonCopyable&& other) = delete;
	NonCopyable& operator=(NonCopyable&& other) = delete;
};

#endif //NON_COPYABLE_HPP