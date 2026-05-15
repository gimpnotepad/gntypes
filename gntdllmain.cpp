#include "gnt.h"
#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif
#define _WIN32_WINNT 0x0601
#include <windows.h>
#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>
#include <vector>
#include <map>
#include <string>

void GnWriteR(int bufsize, const char* f, ...) {
	std::vector<char> buf(bufsize);
	va_list a;
	va_start(a, f);
	int s = vsnprintf(buf.data(), bufsize, f, a);
	va_end(a);
	if (s > 0 && (size_t)s < bufsize) {
		HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		if (hOut != INVALID_HANDLE_VALUE) {
			DWORD w;
			WriteConsoleA(hOut, buf.data(), (DWORD)s, &w, NULL);
		}
	} else if (s >= (int)bufsize) {
		return;
	}
}

void GnTestNum(unsigned long long v, bool* r) {
	asm volatile (
		"test %[val], %[val]\n\t"
		"setz %[zf]"
		: [zf] "=r" (*r)
		: [val] "r" (v)
		: "cc"
	);
}

void gnasm(const char* str) {
	char n1[32], n2[32], n3[32], n4[32];
	int val = 0;
	static std::map<std::string, int> r = {
		{"a", 0},
		{"b", 0},
		{"c", 0},
		{"d", 0}
	};
	static bool l = 0;
	if (sscanf(str, "mov %s, %d", n1, &val) == 2) {
		if (r.count(n1)) r[n1] = val;
	} else if (sscanf(str, "add %s, %s", n1,n2) == 2) {
		if (r.count(n1) && r.count(n2)) r[n1] += r[n2];
	} else if (sscanf(str, "sub %s, %s", n1,n2) == 2) {
		if (r.count(n1) && r.count(n2)) r[n1] -= r[n2];
	} else if (sscanf(str, "mul %s, %s", n1,n2) == 2) {
		if (r.count(n1) && r.count(n2)) r[n1] *= r[n2];
	} else if (sscanf(str, "div %s, %s", n1,n2) == 2) {
		if (r.count(n1) && r.count(n2)) { 
			if (r[n2] != 0) r[n1] /= r[n2];
		}
	} else if (sscanf(str, "mod %s, %s", n1,n2) == 2) {
		if (r.count(n1) && r.count(n2)) { 
			if (r[n2] != 0) r[n1] %= r[n2];
		}
	} else if (sscanf(str, "swap %s, %s", n1,n2) == 2) {
		if (r.count(n1) && r.count(n2)) {
			int a = r[n1];
			int b = r[n2];
			asm volatile(
				"xchg %%eax, %%ebx" 
				: "=a"(a),
				"=b"(b)
				: "a"(a),
				"b"(b)
			);
			r[n1] = a;
			r[n2] = b;
		}
	} else if (sscanf(str, "cmp %s, %s", n1,n2) == 2) {
		if (r.count(n1) && r.count(n2)) l = (r[n1] == r[n2]);
	} else if (sscanf(str, "test %s, %s", n1,n2) == 2) {
		if (r.count(n1) && r.count(n2)) l = (r[n1] & r[n2]);
	} else if (std::string(str) == "printl") {
		printf("%d", l);
	}
}

int4::int4(int n) {
	num = n;
	normalize();
}

void int4::normalize(){
	while (num > 7) num -= 16;
	while (num < -8) num += 16;
}

int4 int4::operator+(const int4& other) const {
	return int4(num + other.num);
}
int4 int4::operator-(const int4& other) const {
	return int4(num - other.num);
}
int4 int4::operator*(const int4& other) const {
	return int4(num * other.num);
}
int4 int4::operator/(const int4& other) const {
	return int4(num / other.num);
}
void int4::print() {
	GnWriteR(2, "%d", num);
}
bool int4::operator==(const int4& other) const {
	return num == other.num;
}
bool int4::operator>(const int4& other) const {
	return num > other.num;
}
bool int4::operator<(const int4& other) const {
	return num < other.num;
}

uint4::uint4(int n) {
	num = n;
	normalize();
}

void uint4::normalize(){
	num = num & 0xF;
}

uint4 uint4::operator+(const uint4& other) const {
	return uint4(num + other.num);
}
uint4 uint4::operator-(const uint4& other) const {
	return uint4(num - other.num);
}
uint4 uint4::operator*(const uint4& other) const {
	return uint4(num * other.num);
}
uint4 uint4::operator/(const uint4& other) const {
	return uint4(num / other.num);
}
void uint4::print() {
	printf("%u", num);
}
bool uint4::operator==(const uint4& other) const {
	return num == other.num;
}
bool uint4::operator>(const uint4& other) const {
	return num > other.num;
}
bool uint4::operator<(const uint4& other) const {
	return num < other.num;
}

void GetCPUName(char* cpub) {
	unsigned int cpui[4];
	for (int i = 0; i < 3; ++i) {
		unsigned int f = 0x80000002 + i;
		asm volatile (
			"cpuid"
			: "=a"(cpui[0]),
			"=b"(cpui[1]),
			"=c"(cpui[2]),
			"=d"(cpui[3])
			: "a"(f)
		);
		memcpy(cpub + (i * 16), cpui, sizeof(cpui));
	}
	cpub[48] = '\0';
}

template <typename T>
Bufferd<T>::Bufferd(size_t n) {
	size = n;
	data = new T[n];
}
template <typename T>
Bufferd<T>::~Bufferd() {
	delete[] data;
}

template <typename T>
T& Bufferd<T>::operator[](size_t i) {
	return data[i];
}
template <typename T>
size_t Bufferd<T>::getSize() const {
	return size;
}

void GetPromptElement(char* buf, int code) {
	SYSTEMTIME st;
	GetLocalTime(&st);
	switch (code) {
		case PROMPT_P:
			GetCurrentDirectoryA(BUF_L, buf);
			break;
		case PROMPT_T:
			sprintf(buf, "%02d:%02d:%02d,%02d", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
			break;
		case PROMPT_D:
			sprintf(buf, "%02d.%02d.%04d", st.wDay, st.wMonth, st.wYear);
			break;
		case PROMPT_M:
			ULONGLONG ms = GetTickCount64();
			sprintf(buf, "%llu", ms);
			break;
		#ifdef GNPRO
		case PROMPT_RAM:
			PROCESS_MEMORY_COUNTERS pmc;
			if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc)) {
				size_t usageMB = pmc.WorkingSetSize / (1024*1024);
				snprintf(buf, BUF_L, "%zuMB", usageMB);
			} else {
				return;
			}
			break;
		case PROMPT_CL:
			static FILETIME pi, pk, pu;
			FILETIME i, k, u;
			if (GetSystemTimes(&i,&k,&u)) {
				auto ti64 = [](FILETIME ft) {
					return (uint64_t(ft.dwHighDateTime) << 32) | ft.dwLowDateTime;
				};
				uint64_t im = ti64(i);
				uint64_t km = ti64(k);
				uint64_t um = ti64(u);
				uint64_t t = km + um;
				int p = (t > 0) ? (int)(100-(i*100/t)) : 0;
				pi = i;
				pk = k;
				pu = u;
				snprintf(buf, BUF_L, "%d%%", p);
			}
			break;
		#endif
	}
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL,DWORD fdwReason,LPVOID lpvReserved)
{
	switch(fdwReason)
	{
		case DLL_PROCESS_ATTACH:
		{
			break;
		}
		case DLL_PROCESS_DETACH:
		{
			break;
		}
		case DLL_THREAD_ATTACH:
		{
			break;
		}
		case DLL_THREAD_DETACH:
		{
			break;
		}
	}
	return TRUE;
}
