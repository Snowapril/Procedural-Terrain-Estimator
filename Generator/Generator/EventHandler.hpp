#ifndef EVENT_HANDLER_HPP
#define EVENT_HANDLER_HPP

class EventHandler
{
public:
	virtual void processCursorPos(double xpos, double ypos) = 0;
	virtual void processWheelOffset(double yoffset) = 0;
	virtual void processMouseBtn(int button, int action) = 0;
	virtual void processToggleKey(int key, int scancode, int action) = 0;
public:
	virtual ~EventHandler() {};
};


#endif