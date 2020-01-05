#include <mingl/mingl.h>

#include <iostream>

using namespace std;

int main() {
    Window w;
    w.setTitle("Mouse interaction app");

    /* prints coordinates
    w.addHandler(WindowMessage::onMouseMove(), [](Window *, WindowMessage wm) {
        cout << "(" << GET_X_LPARAM(wm.getLparam()) << ", " << GET_Y_LPARAM(wm.getLparam()) << ")" << endl;
    });
    */

    w.addOnMouseMoveHandler([](Window *w, POINT p) {
        cout << "(" << p.x << ", " << p.y << ")" << endl;
    });

    w.show();
    return 0;
}
