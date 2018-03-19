#include <iostream>
#include "WindowManager.h"
int main()
{
	while (WindowManager::getInstance()->run());
	return 0;
}