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

template <typename T>
using iList = std::initializer_list<T>;

template <class T, class L> std::shared_ptr<T> make_shared_from_list(iList<L>&& list)
{
	return std::make_shared<T>(std::move(list));
}

template <class T, class L> std::unique_ptr<T> make_unique_from_list(iList<L>&& list)
{
	return std::make_unique<T>(std::move(list));
}

class GLShader;

class EngineAsset
{
private:
protected:
	std::vector<std::pair<int64_t, std::string>> assetPaths;
public:
	EngineAsset();
	virtual ~EngineAsset();
	EngineAsset(const EngineAsset & other);
	EngineAsset & operator=(const EngineAsset & other);
public:
	virtual void reloadAsset(void) = 0;
	/**
	* @ brief		listen to file change
	* @ details		based on file last modified time, check whether if file is changed or not.
	* @ return		if file is changed, then return true. otherwise, return false.
	*/
	bool listenToAssetChange(void);
};

#endif