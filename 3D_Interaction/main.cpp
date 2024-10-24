# pragma once

#include "Application.h"

int main() {

	Application application;
	
	bool success = application.Initialize();

	if (success) {
		application.RunLoop();
	}

	application.ShutDown();

	return 0;
}