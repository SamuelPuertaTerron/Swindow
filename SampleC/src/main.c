#include <stdio.h>

__declspec(dllimport) void Show(void);

int main(void)
{
	printf("Hello, World!");
	Show();
	return 0;
}