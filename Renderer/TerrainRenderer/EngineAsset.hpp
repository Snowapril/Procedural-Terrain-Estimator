#ifndef ENGINE_ASSET_HPP
#define ENGINE_ASSET_HPP

#include <vector>
#include <memory>

template <class T, class L> std::shared_ptr<T> make_shared_from_list(std::initializer_list<L>&& list)
{
	return std::make_shared<T>(std::move(list));
}

template <class T, class L> std::unique_ptr<T> make_unique_from_list(std::initializer_list<L>&& list)
{
	return std::make_unique<T>(std::move(list));
}

class EngineAsset
{
private:
protected:
	std::vector<std::pair<int64_t, std::string>> assetPaths;

	virtual ~EngineAsset();
public:
	virtual void reloadAsset(void) = 0;
	virtual void loadAsset(const std::vector<std::string>& assetPath) = 0;
	virtual bool listenToAssetChange(void) = 0;
};

#endif