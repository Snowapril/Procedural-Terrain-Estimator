#ifndef NON_COPYABLE_HPP
#define NON_COPYABLE_HPP

class NonCopyable
{
public:
	NonCopyable() = default;
	NonCopyable(const NonCopyable& other) = delete;
	NonCopyable& operator=(const NonCopyable& other) = delete;
};

#endif