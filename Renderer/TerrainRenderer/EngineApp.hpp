#ifndef ENGINE_APP_HPP
#define ENGINE_APP_HPP

#include "GLApp.hpp"
#include <memory>

class GLShader;
class AssetManager;

class EngineApp : public GLApp
{
private:
	std::shared_ptr<GLShader> simpleShader; /// for test
	unsigned int VAO; /// for test
	std::unique_ptr<AssetManager> assetManager; /// for hot reload test.

protected:
	bool initShader(void);
	bool initAssets(void);
	bool initGeometryBuffer(void);

	void updateScene(float dt);
	void drawScene(void) const;
public:
	EngineApp();
	virtual ~EngineApp();
public:
	/// at the start of program, this method must be called first.
	bool initEngine(void);

	/// callback functions below.
	void keyCallback(int key, int scancode, int action, int mode);
	void mousePosCallback(double xpos, double ypos);
	void mouseBtnCallback(int btn, int action, int mods);
	void scrollCallback(double xoffset, double yoffset);
};

#endif