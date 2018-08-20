#ifndef ENGINE_ASSET_HPP
#define ENGINE_ASSET_HPP


class EngineAsset
{
private:
protected:
public:
	virtual void reloadAsset(const std::vector<std::string>& assetPath) = 0;
};


#endif