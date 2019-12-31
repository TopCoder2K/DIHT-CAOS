/*Реализуйте калькулятор выражений над множествами в обратной польской записи.

На стандартном потоке ввода задается последовательность, состоящая из символов-значений,
и символов-операций.

Значениями являются цифры от 0 до 9, и буквы английского алфавита (заглавные и строчные).
Последовательности значений кодируют множества из 62 элементов.

Операциями являются:

    & - пересечение множеств;
    | - объединение множеств;
    ^ - симметрическая разность;
    ~ - дополнение текущего результата до полного множества.

В начальный момент времени, множество-результат является пустым.
При чтении символов со стандартного потока ввода, когда встречается символ операции,
то эта операция применяется к множеству-результату и последнему прочитанному множеству
(как в обратной польской записи).

Вывести на экран полученное итоговое значение множества в нормализованной форме:
сначала цифры, затем заглавные буквы, потом - строчные (упорядоченные по алфавиту).

Для хранения множеств использовать, суммарно, не более 16 байт памяти.*/

#include <stdio.h>
#include <stdint.h>

int64_t AddElement(int64_t set, const char element) {
	// Numbers are added from 52 to 61.
	if (element >= 48 && element <= 57) {
		set |= (int64_t)1 << (element - 48 + 52);
	}

	// Lower case letters are added from 26 to 51.
	if (element >= 97 && element <= 122) {
		set |= (int64_t)1 << (element - 97 + 26);
	}

	// Upper case letters are added from 0 to 25.
	if (element >= 65 && element <= 90) {
		set |= (int64_t)1 << (element - 65);
	}

	return set;
}

void PrintSet(int64_t set) {
	for (int32_t i = 52; i <= 61; ++i) {
		int8_t cur_bit = 1 & (set >> i);
		// Numbers are from 52 to 61.
			if (cur_bit)
				printf("%c", i - 52 + 48);
	}

	for (int32_t i = 0; i <= 25; ++i) {
		int8_t cur_bit = 1 & (set >> i);
		// Upper case letters are from 0 to 25.
			if (cur_bit)
				printf("%c", i + 65);
	}

	for (int32_t i = 26; i <= 51; ++i) {
		int8_t cur_bit = 1 & (set >> i);
		// Lower case letters are from 26 to 51.
			if (cur_bit)
				printf("%c", i - 26 + 97);
	}

	printf("\n");
}

int64_t Intersection(int64_t result_set, int64_t intermediate_set) {
	return (result_set & intermediate_set);
}

int64_t Union(int64_t result_set, int64_t intermediate_set) {
	return (result_set | intermediate_set);
}

int64_t Complement(int64_t result_set) {
	return ~result_set;
}

int64_t SymmetricDif(int64_t result_set, int64_t intermediate_set) {
	return (result_set ^ intermediate_set);
}

int64_t Calculator() {
	char sym = 0;
	int64_t result_set = 0, intermediate_set = 0;

	while(1) {
		sym = getchar();
		if (sym == -1 || sym == '_')
			break;

		if (sym == '&') {
			result_set = Intersection(result_set, intermediate_set);
			intermediate_set = 0;
		} else if (sym == '|') {
			result_set = Union(result_set, intermediate_set);
			intermediate_set = 0;
		} else if (sym == '~') {
			result_set = Complement(result_set);
			intermediate_set = 0;
		} else if (sym == '^') {
			result_set = SymmetricDif(result_set, intermediate_set);
			intermediate_set = 0;
		} else {
			intermediate_set = AddElement(intermediate_set, sym);
		}
	}

	return result_set;
}

int main(int argc, char **argv) {
	int64_t result_set = 0;

	result_set = Calculator();
	PrintSet(result_set);

	return 0;
}