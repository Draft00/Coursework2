#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include<ctype.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <Windows.h>
#include <malloc.h>
#include "Neighbor.h"

#define kMaxKeySize 1024

enum
{
	XOR = 1,
	GET_LENGTH_AND_KEY,
	EXIT,
};

unsigned int keystat[kMaxKeySize][256];
unsigned char password[50];
unsigned char* shifr_text, *lolo;
unsigned char* tmp_shifr_text;
int kSize;
int count_for_Brut = 6;

void xor_cypher()
{
	setlocale(0, "");
	FILE* fin = fopen("input.txt", "rb"), * fout = fopen("output.txt", "wb");
	long long i = 0;

	if (fin == NULL)
	{
		printf("Cant read file!\n");
		return;
	}

	printf("Введите пароль: ");
	getchar();
	gets(password);

	long long length = strlen(password);

	int c = fgetc(fin);
	while (c != EOF)
	{
		c ^= password[i % length];
		i++;
		fputc(c, fout);
		c = fgetc(fin);
	}

	fseek(fin, 0, SEEK_SET);
	fclose(fin);
	fclose(fout);

}

int menu()
{
	int num = 0;
	printf("\nКакие действия Вы желаете выполнить?\n");
	printf("Для совершения действия выберите соответствующую цифру:\n");
	printf("1. Заксорить.\n");
	printf("2. Найти длину ключа и сам ключ. Печать получившегося текста в файл.\n");
	printf("3. Выход.\n");
	if (!scanf("%d", &num))
	{
		fflush(stdin);
		printf("input error.\ntry again: ");
	}
	return (num);
}

float sum_matches(unsigned char split_str[], int size)
{
	int i = 0, count_repit = 0;
	float sum = 0.0;
	for (i = 0; i < size - 1; i++)
	{
		if (split_str[i] == split_str[i + 1])
		{
			count_repit++;
		}
		else
		{
			sum += count_repit * (count_repit - 1) / (float)(size * (size - 1));
			count_repit = 0;
		}
	}
	return sum;
}

void qsortt(int l, int r, unsigned char a[])
{
	if (l < r)
	{
		int i = l, j = r, t;
		int d = (l + r) / 2;
		int x = a[d];
		do
		{
			while (a[i] < x) i++;
			while (a[j] > x) j--;
			if (i <= j)
			{
				t = a[i];
				a[i] = a[j];
				a[j] = t;
				i++;
				j--;
			}
		} while (i <= j);
		qsortt(l, j, a);
		qsortt(i, r, a);
	}
}

float split_text(int size_key, unsigned char shifr_text[], int size)
{
	int a = size / size_key + 1;
	unsigned char* split_str = (unsigned char*)malloc(a * sizeof(unsigned char));
	int count = 0;
	float sum = 0.0;
	for (int start = 0; start < size_key; start++)
	{
		for (int i = start; i < size; i += size_key, count++)
		{
			split_str[count] = shifr_text[i];
		}

		qsortt(0, count - 1, split_str);
		sum += sum_matches(split_str, count - 1);
		count = 0;
	}
	free(split_str);
	return sum / (float)size_key;
}

int key_size(unsigned char shifr_text[], int size, float key_long[])
{
	int count_key = 0;
	float sum_index = 0.0;
	for (int i = 2; i < size / 2 + 1; i++)
	{
		key_long[i] = (split_text(i, shifr_text, size));
		sum_index += key_long[i];
	}
	float max_delta = 0.0;
	int kSize = 0;
	float avverage = sum_index / (float)(size / 2 - 1);
	for (int i = 2; i < size / 2 + 1; i++)
	{
		if ((key_long[i] - avverage) > max_delta)
		{
			kSize = i;
			max_delta = key_long[i] - avverage;
		}
	}
	return kSize;
}

unsigned char frequency[] = " etaoinshrdlcumwfgypbvkxjqz";
unsigned char alphabet[] = " etaoinshrdlcumwfgypbvkxjqzETAOINSHRDLCUMWFGYPBVKXJQZ1234567890!_№;%:?*()-=+&^$#@";

int is_it_char(char c)
{
	if ((c >= '0' && c <= '?') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= 32 && c <= 47))
	{
		return 1;
	}
	else return 0;
}


void if_need_correct()
{
	int choise = 5;
	while (choise)
	{
		printf("\n\nЕсли ключ уже был найден верно, введите 0. Иначе введите 1 и результат будет подкорректирован.\n");
		{
			scanf("%d", &choise);
		}
		if (choise == 0)
		{
			printf("Завершение работы.");
		}
		else
		{
			printf("Выберите длину ключа, которая, по Вашему мнению, наиболее подходящая.\n");
			int your_kSize = 0;
			unsigned char your_key[50] = { 0 }, kkey[50] = { 0 };
			scanf("%d", &your_kSize);
			printf("Введите несколько ПЕРВЫХ символов текста (достаточно количества, равного длине ключа), который, по вашему мнению, должен был получиться.\n");
			getchar();
			gets(your_key);
			unsigned char c;
			for (int i = 0; i < your_kSize; i++)
			{
				for (int j = 0; j < strlen(alphabet); j++)
				{
					c = shifr_text[i] ^ alphabet[j];
					if (c == your_key[i])
					{
						kkey[i] = alphabet[j];
						break;
					}
				}
			}
			for (int i = 0; i < your_kSize; i++)
			{
				printf("%c", kkey[i]);
			}
		}
	}
}

int check(char c, const char* neig)
{
	if (c != ' ')
	{
		for (int i = 0; i < strlen(neig); i++)
		{
			if (c == neig[i])
			{
				if (i < strlen(neig) / 2)
				{
					return 2;
					break;
				}
				else
				{
					return 1;
					break;
				}
			}
		}
		return -1;
	}
	else return 0;
}

int analysis_bin(unsigned char* tempkey, int sizetext)
{
	//memset(tmp_shifr_text, 0, sizetext);
	int needsize = need_size(100, sizetext) , tmp_count_bin = 0;
	for (int j = 0; j < needsize; j++)
	{
		tmp_shifr_text[j] = shifr_text[j] ^ tempkey[j % kSize];
	}
	for (int j = 1; j < needsize - 1; j++)
	{
		unsigned char tmp = tolower(tmp_shifr_text[j]);
		switch (tmp)
		{
		case 'a':
			tmp_count_bin += check(tolower(tmp_shifr_text[j - 1]), neigA_left);
			tmp_count_bin += check(tolower(tmp_shifr_text[j + 1]), neigA_right);
			break;
		case 'b':
			tmp_count_bin += check(tolower(tmp_shifr_text[j - 1]), neigB_left);
			tmp_count_bin += check(tolower(tmp_shifr_text[j + 1]), neigB_right);
			break;
		case 'c':
			tmp_count_bin += check(tolower(tmp_shifr_text[j - 1]), neigC_left);
			tmp_count_bin += check(tolower(tmp_shifr_text[j + 1]), neigC_right);
			break;
		case 'd':
			tmp_count_bin += check(tolower(tmp_shifr_text[j - 1]), neigD_left);
			tmp_count_bin += check(tolower(tmp_shifr_text[j + 1]), neigD_right);
			break;
		case 'e':
			tmp_count_bin += check(tolower(tmp_shifr_text[j - 1]), neigE_left);
			tmp_count_bin += check(tolower(tmp_shifr_text[j + 1]), neigE_right);
			break;
		case 'f':
			tmp_count_bin += check(tolower(tmp_shifr_text[j - 1]), neigF_left);
			tmp_count_bin += check(tolower(tmp_shifr_text[j + 1]), neigF_right);
			break;
		case 'g':
			tmp_count_bin += check(tolower(tmp_shifr_text[j - 1]), neigG_left);
			tmp_count_bin += check(tolower(tmp_shifr_text[j + 1]), neigG_right);
			break;
		case 'h':
			tmp_count_bin += check(tolower(tmp_shifr_text[j - 1]), neigH_left);
			tmp_count_bin += check(tolower(tmp_shifr_text[j + 1]), neigH_right);
			break;
		case 'i':
			tmp_count_bin += check(tolower(tmp_shifr_text[j - 1]), neigI_left);
			tmp_count_bin += check(tolower(tmp_shifr_text[j + 1]), neigI_right);
			break;
		case 'j':
			tmp_count_bin += check(tolower(tmp_shifr_text[j - 1]), neigJ_left);
			tmp_count_bin += check(tolower(tmp_shifr_text[j + 1]), neigJ_right);
			break;
		case 'k':
			tmp_count_bin += check(tolower(tmp_shifr_text[j - 1]), neigK_left);
			tmp_count_bin += check(tolower(tmp_shifr_text[j + 1]), neigK_right);
			break;
		case 'l':
			tmp_count_bin += check(tolower(tmp_shifr_text[j - 1]), neigL_left);
			tmp_count_bin += check(tolower(tmp_shifr_text[j + 1]), neigL_right);
			break;
		case 'm':
			tmp_count_bin += check(tolower(tmp_shifr_text[j - 1]), neigM_left);
			tmp_count_bin += check(tolower(tmp_shifr_text[j + 1]), neigM_right);
			break;
		case 'n':
			tmp_count_bin += check(tolower(tmp_shifr_text[j - 1]), neigN_left);
			tmp_count_bin += check(tolower(tmp_shifr_text[j + 1]), neigN_right);
			break;
		case 'o':
			tmp_count_bin += check(tolower(tmp_shifr_text[j - 1]), neigO_left);
			tmp_count_bin += check(tolower(tmp_shifr_text[j + 1]), neigO_right);
			break;
		case 'p':
			tmp_count_bin += check(tolower(tmp_shifr_text[j - 1]), neigP_left);
			tmp_count_bin += check(tolower(tmp_shifr_text[j + 1]), neigP_right);
			break;
		case 'q':
			tmp_count_bin += check(tolower(tmp_shifr_text[j - 1]), neigQ_left);
			tmp_count_bin += check(tolower(tmp_shifr_text[j + 1]), neigQ_right);
			break;
		case 'r':
			tmp_count_bin += check(tolower(tmp_shifr_text[j - 1]), neigR_left);
			tmp_count_bin += check(tolower(tmp_shifr_text[j + 1]), neigR_right);
			break;
		case 's':
			tmp_count_bin += check(tolower(tmp_shifr_text[j - 1]), neigS_left);
			tmp_count_bin += check(tolower(tmp_shifr_text[j + 1]), neigS_right);
			break;
		case 't':
			tmp_count_bin += check(tolower(tmp_shifr_text[j - 1]), neigT_left);
			tmp_count_bin += check(tolower(tmp_shifr_text[j + 1]), neigT_right);
			break;
		case 'u':
			tmp_count_bin += check(tolower(tmp_shifr_text[j - 1]), neigU_left);
			tmp_count_bin += check(tolower(tmp_shifr_text[j + 1]), neigU_right);
			break;
		case 'v':
			tmp_count_bin += check(tolower(tmp_shifr_text[j - 1]), neigV_left);
			tmp_count_bin += check(tolower(tmp_shifr_text[j + 1]), neigV_right);
			break;
		case 'w':
			tmp_count_bin += check(tolower(tmp_shifr_text[j - 1]), neigW_left);
			tmp_count_bin += check(tolower(tmp_shifr_text[j + 1]), neigW_right);
			break;
		case 'x':
			tmp_count_bin += check(tolower(tmp_shifr_text[j - 1]), neigX_left);
			tmp_count_bin += check(tolower(tmp_shifr_text[j + 1]), neigX_right);
			break;
		case 'y':
			tmp_count_bin += check(tolower(tmp_shifr_text[j - 1]), neigY_left);
			tmp_count_bin += check(tolower(tmp_shifr_text[j + 1]), neigY_right);
			break;
		case 'z':
			tmp_count_bin += check(tolower(tmp_shifr_text[j - 1]), neigZ_left);
			tmp_count_bin += check(tolower(tmp_shifr_text[j + 1]), neigZ_right);
			break;
		default:
			break;
		}
		
		
	}
	return tmp_count_bin;
}


int Max = 0;

int need_size(int want, int sizetext)
{
	if (sizetext < want)
	{
		return sizetext;
	}
	else return want;
}

void show_result(unsigned char *tmp_key, int want, int sizetext, int currentMax)
{
	int needsize = need_size(want, sizetext);

	printf("Коэффициент \"хороших соседей\" = %d \n", currentMax);
	printf("Предполагаемый ключ: ");
	for (int j = 0; j < kSize; j++)
	{
		printf("%c", tmp_key[j]);
	}
	printf("\n");
	printf("Текст: \n");
	for (int j = 0; j < needsize; j++)
	{
		printf("%c", tmp_shifr_text[j]);
	}
	printf("\n");

}


void BruteForce(unsigned char* key, unsigned char* hu, int size, int number, int sizetext)
{
	if (size == 0)
	{

		if (strcmp(hu, kk) == 0)
		{
			printf("YES");
		}

		int currentMax = analysis_bin(hu, sizetext);
		if (currentMax > Max)
		{
			show_result(hu, 100, sizetext, currentMax);

			Max = currentMax;
		}
		else if (Max - currentMax <= 10)
		{
			show_result(hu, 100, sizetext, currentMax);
		}
		
		return;
	}
	else
	{
		//count_for_Brut
		for (int i = 0; i < count_for_Brut; i++)
		{
			hu[number] = key[number] ^ frequency[i];

			if (is_it_char(hu[number]))
			{
				BruteForce(key, hu, size - 1, number + 1, sizetext);
			}
		}
	}
}

void get_key(unsigned char* text, unsigned int size, unsigned char* key, unsigned int ksize)
{
	memset(keystat, 0, sizeof(keystat));
	unsigned int read = ksize;

	for (unsigned int start = 0; start < size;)
	{
		if (read > (size - start))
			read = (size - start);

		for (unsigned int i = 0; i < read; i++)
		{
			keystat[i][text[start + i]]++;
		}
		start += ksize;
	}

	for (int i = 0; i < ksize; i++)
	{
		unsigned int prev_max = 0;
		unsigned int max = 0;

		for (unsigned int i = 0; i < ksize; i++)
		{
			unsigned int v = 0;
			unsigned int max = 0;

			for (int j = 0; j < 256; j++)
			{
				if (keystat[i][j] > max)
				{
					max = keystat[i][j];
					v = j;
				}
			}

			key[i] = v;
			//key[i] ^= ' ';
		}
		char temp_key[50] = { 0 };
		kSize = ksize;
		BruteForce(key, temp_key, ksize, 0, size);

		if (ksize)
			break;
	}

}

void print(unsigned char* key, int length)
{
	FILE* fin = fopen("output.txt", "rb"), * fout = fopen("finish.txt", "wb");
	long long i = 0;
	if (fin == NULL)
	{
		printf("Cant read file!\n");
		return;
	}

	int c = fgetc(fin);
	while (c != EOF)
	{
		c ^= key[i % length];
		i++;
		fputc(c, fout);
		c = fgetc(fin);
	}
	fseek(fin, 0, SEEK_SET);
	fclose(fin);
	fclose(fout);

}

int main()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	FILE* fin = fopen("output.txt", "rb");

	if (fin == NULL)
	{
		printf("Can't read file!\n");
		return 0;
	}

	fseek(fin, 0, SEEK_END);
	int size = ftell(fin);
	fseek(fin, 0, SEEK_SET);

	//unsigned char* shifr_text = (unsigned char*)malloc(size * sizeof(unsigned char));

	shifr_text = (unsigned char*)malloc(size * sizeof(unsigned char));
	tmp_shifr_text = (unsigned char*)malloc(size * sizeof(unsigned char));

	unsigned int c = fgetc(fin), i = 0;

	while (c != EOF)
	{
		shifr_text[i] = c;
		i++;
		c = fgetc(fin);
	}
	fclose(fin);

	float* keys_long = (float*)calloc(size / 2 + 1, sizeof(float));
	kSize = key_size(shifr_text, size, keys_long);
	int maxKsize = 0;

	if (keys_long[kSize] < 0.04)
	{
		maxKsize = 12;
	}

	if (size > 300)
	{
		count_for_Brut = 5;
	}
	unsigned char key[50] = { 0 };
	
	int n = menu();
	while (n != 4)
	{
		switch (n)
		{
		case XOR:
			xor_cypher();
			return 0;
		case GET_LENGTH_AND_KEY:
			if (maxKsize)
			{
				for (kSize = 2; kSize < maxKsize; kSize++)
				{
					if (kSize < 10)
						count_for_Brut = 7;
					else
					{
						if (kSize == 10)
							count_for_Brut = 6;
						else
						{
							if (kSize < 12)
								count_for_Brut = 3;
							else
								count_for_Brut = 1;
						}
					}
					get_key(shifr_text, size, key, kSize);
				}
			}
			else
			{
				get_key(shifr_text, size, key, kSize);
			}
			if_need_correct();

		case EXIT:
			system("pause");
			exit(0);
		}
		n = menu();
	}
	
	free(shifr_text);
	free(tmp_shifr_text);
	
	return 0;
}
