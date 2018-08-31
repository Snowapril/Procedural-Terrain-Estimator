/**
* @file		EngineAsset.hpp
* @author	Shinjihong
* @date		27 August 2018
* @version	1.0.0
* @brief	EngineAsset is base class of all resources (shader, textures, sound, etc ..).
			provide pure method for hot-reload.
* @see
*/

#ifndef ENGINE_ASSET_HPP
#define ENGINE_ASSET_HPP

#include <vector>
#include <memory>

template <class T, class L> std::shared_ptr<T> make_shared_from_list(std::initializer_list<L>&& list) noexcept
{
	return std::make_shared<T>(std::move(list));
}

template <class T, class L> std::unique_ptr<T> make_unique_from_list(std::initializer_list<L>&& list) noexcept
{
	return std::make_unique<T>(std::move(list));
}

class GLShader;

template <class Type>
class EngineAsset
{
private:
protected:
	std::vector<std::pair<int64_t, std::string>> assetPaths;
public:
	EngineAsset()
	{
	}
	virtual ~EngineAsset()
	{
		assetPaths.clear();
	}
	EngineAsset(const EngineAsset & other)
		: assetPaths(other.assetPaths)
	{
	}
	EngineAsset & operator=(const EngineAsset & other)
	{
		if (&other == this)
			return *this;

		assetPaths = other.assetPaths;

		return *this;
	}
public:
	Type & operator~() noexcept
	{
		return *static_cast<Type*>(this);
	}
	Type const& operator~() const noexcept
	{
		return *static_cast<Type const*>(this);
	}
};

#endif