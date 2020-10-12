#include <stdio.h>

int main() {
	int value;

	printf("Type an integer value: ");
	scanf("%d", &value);

	printf("Input value : %d %x\n", value, value);

	return 0;
}
