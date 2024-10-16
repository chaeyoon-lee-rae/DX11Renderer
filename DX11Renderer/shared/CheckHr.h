#pragma once

#include <windows.h>
#include <iostream>
#include <string>

inline bool CheckFailed(HRESULT hr, std::string msg) { 
	if (FAILED(hr)) {
        std::cerr << msg << std::endl;
        return true;
	}
	return false;
}
