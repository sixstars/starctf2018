#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#define AC 0
#define WA 1
#define ERROR -1
#define LIM 10000
#define MAXN 20
#define LENGTH 1024
#define Cases 10
#define MAXTIMES 99

#if 0
#define DEBUG
#endif

const int TestCases[Cases] = {1, 2, 4, 6, 8, 10, 11, 12, 13, 14};
int A[Cases][MAXN];
int Case, n, pos;
char buf[LENGTH];

void swap(int *u, int *v)
{
	(*u) += (*v);
	(*v) = (*u) - (*v);
	(*u) -= (*v);
}

void sort(int *begin, int *end)
{
	for (int *i = begin; i != end; i ++)
		for (int *j = end - 1; j != i; j --)
			if ((*j) < (*(j - 1)))
				swap(j, j - 1);
}

void PrintStatement()
{
	puts("Given n(1<=n<=14) integers a1,a2,...,an in interval [0,1024), you should determine them by sending several queries.");

	puts("For each query, you can ask \"how many integers are in interval [l,r)?\" through stdout in format \"? l r\" where 0<=l<r<=1024, and you will recieve an integer through stdin as the answer.");

	puts("Finally, if all the integers are determined, you should output them in arbitrary order and in format \"! a1 a2 ... an\".");

	puts("Please notice that some of the integers can be the same and that you can send no more than 99 queries in each level.");
}

void GenerateInput()
{
	if (Case < 6)
	{
		for (int i = 0; i < TestCases[Case]; i ++)
			A[Case][i] = rand() & 0x3ff;
		sort(A[Case], A[Case] + TestCases[Case]);
	}
	else if (Case < 8)
	{
		for (int i = 0; i < TestCases[Case]; i ++)
			A[Case][i] = rand() & 0x3ff;
		for (int i = 0; i < 3; i ++)
		{
			int a = rand() % TestCases[Case];
			int b = rand() % TestCases[Case];
			A[Case][a] = A[Case][b];
		}
		sort(A[Case], A[Case] + TestCases[Case]);
	}
	else
	{
		static int T[MAXN];
		for (int i = 0; i < TestCases[Case]; i ++)	
		{
			T[i] = (i == 0 ? 0 : T[i - 1] + 1) + (rand() & 1);
			if (32 - T[i] < TestCases[Case] - i) T[i] --;
		}
		for (int i = 0; i < TestCases[Case]; i ++)
			A[Case][i] = T[i] * 32 + (rand() & 31);
	}
}

void PrintInput()
{
	puts("------------------------");
	printf("Level %d : n = %d\n", Case, TestCases[Case]);
	fflush(stdout);
}

#ifdef DEBUG
void ShowTestData()
{
	for (int i = 0; i < TestCases[Case]; i ++)
		printf("%d ", A[Case][i]);
	puts("");
}
#endif

int IsBlank(char c)
{
	int res = (c == ' ' || c == '\t' || c == '\n') ? 1 : 0;
	return res;
}

int IsDigit(char c)
{
	int res = (c >= '0' && c <= '9') ? 1 : 0;
	return res;
}

int ReadInt(char *p, int *st, int *x)
{
	for (; p[*st] != '\n' && IsBlank(p[*st]); (*st) ++) ;
	if (p[*st] == '\n') return 0;
	if (p[*st] != '-' && !IsDigit(p[*st])) return 0;
	int f = p[*st] == '-' ? -1 : 1;
	*x = p[*st] == '-' ? 0 : p[*st] - '0';
	for ((*st) ++; IsDigit(p[*st]); (*st) ++)
	{
		*x = (*x) * 10 + p[*st] - '0';
		if (*x > LIM) return 0;
	}
	*x = *x * f;
	return 1;
}

int ReadAny(char *p, int *st)
{
	for (; p[*st] != '\n' && IsBlank(p[*st]); (*st) ++) ;
	if (p[*st] == '\n') return 0;
	return 1;
}

int Interact()
{
	n = TestCases[Case];
	int Times = 0;
	while (1)
	{
		if (fgets(buf, LENGTH, stdin) == NULL) exit(ERROR);
		if (buf[0] == '?')
		{
			pos = 1;
			int l, r;
			if (ReadInt(buf, &pos, &l) != 1) exit(ERROR);
			if (ReadInt(buf, &pos, &r) != 1) exit(ERROR);
			if (ReadAny(buf, &pos)) exit(ERROR);
			if (!(0 <= l && l < r && r <= 1024)) return 0;
			Times ++;
			if (Times > MAXTIMES) return 0;
			int cnt = 0;
			for (int i = 0; i < n; i ++)
				cnt += (l <= A[Case][i] && A[Case][i] < r);
			printf("%d\n", cnt);
			fflush(stdout);
		}
		else if (buf[0] == '!')
		{
			pos = 1;
			static int Result[MAXN];
			for (int i = 0; i < n; i ++)
				if (ReadInt(buf, &pos, Result + i) != 1) exit(ERROR);
			if (ReadAny(buf, &pos)) exit(ERROR);
			sort(Result, Result + n);
			for (int i = 0; i < n; i ++)
				if (Result[i] != A[Case][i]) return 0;
			return 1;
		}
		else return 0;
	}
	return 0;
}

void EndOfAccepted()
{
	printf("Congratulations! You have solved this problem!\n");
	printf("*ctf{magic algorithm produces magic numbers!}\n");
}

int main()
{
	srand((unsigned) time (NULL));
	PrintStatement();

	for (Case = 0; Case < Cases; Case ++)
	{
		GenerateInput();
		PrintInput();
	#ifdef DEBUG
		ShowTestData();
	#endif
		setbuf(stdin, 0);
		setbuf(stdout, 0);
		if (!Interact())
			return WA;
	}
	EndOfAccepted();
	return AC;
}
