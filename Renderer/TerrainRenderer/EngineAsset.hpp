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
#include <filesystem>

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
public:
	/**
	* @ brief		listen to file change
	* @ details		based on file last modified time, check whether if file is changed or not.
	* @ return		if file is changed, then return true. otherwise, return false.
	*/
	bool listenToAssetChange(void)
	{
		namespace fs = std::experimental::filesystem;

		for (auto& file : assetPaths)
		{
			if (!file.second.empty())
			{
				auto& lastTime = file.first;
				const auto newLastTime = fs::last_write_time(file.second).time_since_epoch().count();

				if (newLastTime != lastTime)
				{
					lastTime = newLastTime;
					return true;
				}
			}
		}

		return false;
	}
};

#endif