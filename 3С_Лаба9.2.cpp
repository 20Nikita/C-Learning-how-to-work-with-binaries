#include <locale.h>	//нужна для подключения русского языка
#include <stdio.h>	//основная библиотека 
#include <errno.h>
#define конст 256
bool dec_print();	//чтение из двоичного файла
int assert(int errn) {
	if (errn)
	{
		printf("Ошибка: %d\n", errn);
		return errn;
	}
	return 0;
}

int main()
{
	setlocale(LC_ALL, "Russian");	//подключаю русский язык
	char s[конст] = { 0 };			//массив символов для заполнения
	FILE* in = NULL;				//файловая переменная
	FILE* ptr = NULL;				//файловая переменная
	int	j = 0;						//счетчик букв
	int	i = 0;						//счетчик букв
	size_t	Max_Len = 0;			//максимальная длинна строки
	double	k = 0;					//счетчик строк
	int	m = 0;						//счетчик реальных строк
	int endline = 0;				//флаг конца строки
	int lastline = 0;				//посчитали последнюю строчку

	in = fopen("Text.txt", "r");	//открываем файл для чтения
	if (!in) {						//файл не найден
		printf(" Не удалось открыть файл. \n");
		return 1;
	}
	//поиск количества строк и максимальной длинны строки
	while (fgets(s, конст, in) != NULL) {
		while (s[i] && s[i] != '\n'){				// пока не терминальный 0 считаем буквы
			lastline = 0; i++; j++;
		}
		if (s[i] == '\n' && !lastline)
		{
			k++; lastline = 1;						//+ 1 строка
			Max_Len = (j > Max_Len) ? j : Max_Len;	//если нашли строку больше предыдущей запоминаем
			j = 0;
		}
		i = 0; m++;
	}
	if (!lastline)	//если в последней строке небыло \n
	{
		Max_Len = (j > Max_Len) ? j : Max_Len;		//если нашли строку больше предыдущей запоминаем
		k++;
	}

	//Фай пуст
	if (Max_Len == 0) {
		printf(" Фай пуст. \n");
		return 2;
	}

	ptr = fopen("Text1.bin", "wb");				//открываем файл для записи
	fwrite(&k, sizeof(k), 1, ptr);				//запись количества строк в файл
	if (assert(errno)) return errno;
	rewind(in);									//переход в начало файла
	if (assert(errno)) return errno;
	endline = 0;
	for (i = 0; i < m; i++)						//читаем до конца файла 
	{
		fgets(s, Max_Len +2, in); 				//читаем из файла строку
		if (assert(errno)) return errno;
		for (j = 0; j < Max_Len; j++) {			//очистка строки
			endline = (s[j] == '\n' || !s[j]) ? 1 : endline;//был конец строки
			s[j] = (endline) ? '\0' : s[j];		//если строка кончилась отчищаем ее
		}
		if(s[0] && s[0]!= '\n')					//если в текущей строке есть символы
			fwrite(s, 1, Max_Len, ptr);			//записываем ее в двоичный файл
		if (assert(errno)) return errno;
		endline = 0;
	}

	fclose(in);	fclose(ptr);					//закрываем файлы
	dec_print();								//чтение результата
}

bool dec_print()
{
	setlocale(LC_ALL, "Russian");	//подключаю русский язык
	char buf[256];					//массив для считываемых байт
	FILE* ptr;						//откуда читаем
	size_t n_obj;					//длинна прочитанного
	double n = 0;					//количество срок
	double max_length = 0;
	ptr = fopen("Text1.bin", "rb");				//открываем файл для чтения
	if (ptr != NULL) {							//если открылся
		fread(&n, sizeof(double), 1, ptr);		//читаем количество строк

		if (n < 0) 
			{ printf("Ошибка чтения n.\n"); return 1; }
		else if (n != (int)n) 
			{ printf("Ошибка чтения n.\n"); return 2; }
		else if (assert(errno)) return errno;
		else if (n == 0) { printf("Файл пуст.\n"); return 3; }

		size_t beg = ftell(ptr);		//запомнели текущую позицию
		fseek(ptr, 0L, SEEK_END);		//перешли в конец файла
		size_t end = ftell(ptr);		//запомняли текущую позицию
		if (assert(errno)) return errno;

		max_length = (end - beg) / n;	//пощитали длинну строк
		if (max_length != (int)(max_length))
			{ printf("Ошибка чтения файла.\n"); return 4; }

		fseek(ptr, sizeof(n), SEEK_SET);//переход в начало файла
		if (assert(errno)) return errno;

		printf("Количество строк = %g.\tФайл размером %g байт.\n", n, sizeof(double) + max_length * n);

		while ((n_obj = fread(buf, 1, max_length, ptr)) != 0)	//читаем до конца файла
		{
			if (assert(errno)) return errno;
			for (int i = 0; i < n_obj; i++)		//печать результата
				printf("%c\t", buf[i]);			//в формате букв
			printf("\n");
			if(n-- == 0)
			{
				printf(" Неверный формат файла. \n");
				return 6;
			}
		}
		if (n > 0)
		{
			printf(" Неверный формат файла. \n");
			return 7;
		}
		if (assert(errno)) return errno;
		fclose(ptr);	//закрываем файл
	}
	else {
		printf(" Не удалось открыть файл. \n");
		return 5;
	}
}