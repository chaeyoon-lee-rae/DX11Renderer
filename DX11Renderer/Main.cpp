#include <iostream>
#include <memory>
#include <windows.h>

#include "Renderer.h"

using namespace std;

int main() {
    Renderer exampleApp(1600, 1200);

    if (!exampleApp.Initialize()) {
        cout << "Initialization failed." << endl;
        return -1;
    }

    return exampleApp.Run();
}
 