#include <stdio.h>

__declspec(dllexport) void show(void);

void show(void) {
	printf("Hello from swindow dll");
}
