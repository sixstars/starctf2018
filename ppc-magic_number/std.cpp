#include <string>
#include <iostream>
using namespace std;
#define N 20 + 5

int n, A[N];

bool Check(std::string line)
{
	if (line.empty()) return 0;
	for (char c : line)
		if (c != '-') return 0;
	return 1;
}

inline bool IsDigit(char ch)
{
	return ch >= '0' && ch <= '9';
}

int Getint(int *dest = NULL)
{
	char ch = '\n';
	for (; ch != '-' && !IsDigit(ch); ch = getchar()) ;
	int f = ch == '-' ? -1 : 1;
	int res = ch == '-' ? 0 : ch - '0';
	for (ch = getchar(); IsDigit(ch); ch = getchar())
		res = res * 10 + ch - '0';
	res *= f;
	if (dest != NULL) *dest = res;
	return res;
}

void Solve(int l, int r, int ret)
{
	if (!ret) return ;
	if (l == r - 1)
	{
		for (int i = 1; i <= ret; i ++)
			A[++ A[0]] = l;
		return ;
	}
	int mid = l + r >> 1, cnt;
	std::cout << "? " << l << " " << mid << std::endl << std::flush;
	std::cin >> cnt;
	Solve(l, mid, cnt);
	Solve(mid, r, ret - cnt);
}

int main()
{
	std::string line;
	while (1)
	{
		while (std::cin >> line)
			if (Check(line)) break;
		if (line.empty()) break ;
		Getint();
		Getint(&n);
		A[0] = 0;
		Solve(0, 1024, n);
		std::cout << "!";
		for (int i = 1; i <= n; i ++)
			std::cout << " " << A[i];
		std::cout << std::endl << std::flush;
	}
	return 0;
}
