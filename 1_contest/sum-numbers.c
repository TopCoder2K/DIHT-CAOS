/*На стандартном потоке ввода подается
последовательно вещественное число x и целое число в 16-ричной записи y.
Единственным аргументов программы является целое число z в 27-ричной системе исчисления.
Необходимо вывести вычислить значение x+y+z
и вывести его на стандартный поток вывода с точностью до 3-го знака после точки. */


#include <stdio.h>
#include <stdlib.h>



int main (int argc, char **argv) {
	float x = 0;
	char *y_str = (char*)malloc(100 * sizeof(char));

	scanf("%f", &x);
	scanf("%s", y_str);

	const int z = strtol(argv[1], 0, 27);
	const int y = strtol(y_str, 0, 16);

	printf("%.3f\n", x + y + z);

	free(y_str);

	return 0;
}
