#include "guiobject.hpp"
#include "window.hpp"

GUIObject::GUIObject(const char *className, const char *name, int x, int y, int width, int height,
                     HINSTANCE hinstance, LPVOID additionalData)
        : className(className), name(name), x(x), y(y), width(width), height(height), hinstance(hinstance),
          additionalData(additionalData) {}

GUIObject::GUIObject(Window *parent, const char *className, long id, const char *name, int x, int y, int width,
                     int height)
        : parent(parent), className(className), id(id), name(name), x(x), y(y), width(width), height(height) {
    if (parent != nullptr) style |= WS_CHILD;
}

bool GUIObject::create() {
    if (isCreated()) return false;
    hwnd =
#ifdef USE_WNDCLASSEX
            CreateWindowEx(extendedStyle,
#else
            CreateWindow(
#endif
                    className, name, style,
x, y, width, height, parent != nullptr ? parent->getWindowHandle() : nullptr,
id != 0L ? (HMENU) id : nullptr, hinstance, additionalData);
    return isCreated();
}

int GUIObject::getX() const {
    return x;
}

int GUIObject::getY() const {
    return y;
}

int GUIObject::getWidth() const {
    return width;
}

int GUIObject::getHeight() const {
    return height;
}

long GUIObject::getId() const {
    return id;
}

HWND GUIObject::getWindowHandle() const {
    return hwnd;
}

bool GUIObject::isCreated() const {
    return hwnd != nullptr;
}
