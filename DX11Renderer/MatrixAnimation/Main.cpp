#include <iostream>
#include <memory>
#include <windows.h>

#include "../shared/Renderer.h"

using namespace std;

int main() {
    Renderer renderer(1600, 1200);

    if (!renderer.Initialize()) {
        cout << "Initialization failed." << endl;
        return -1;
    }

    return renderer.Run();
}
 