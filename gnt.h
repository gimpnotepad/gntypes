#ifndef _GNTYPES_H_
#define _GNTYPES_H_

#include <stddef.h>

#if BUILDING_DLL
#define GNT __declspec(dllexport)
#else
#define GNT __declspec(dllimport)
#endif

#define PROMPT_P 0x01
#define PROMPT_T 0x02
#define PROMPT_D 0x03
#define PROMPT_M 0x04
#define BUF_L 260

void GNT GnWriteR(int bufsize, const char* f, ...);
void GNT GnTestNum(unsigned long long v, bool* r);
void GNT gnasm(const char* str);

class GNT int4 {
public:
	int num;
	void normalize();
	int4(int n);
	int4 operator+(const int4& other) const;
	int4 operator-(const int4& other) const;
	int4 operator*(const int4& other) const;
	int4 operator/(const int4& other) const;
	void print();
	bool operator==(const int4& other) const;
	bool operator>(const int4& other) const;
	bool operator<(const int4& other) const;
};

class GNT uint4 {
public:
	unsigned int num;
	void normalize();
	uint4(int n);
	uint4 operator+(const uint4& other) const;
	uint4 operator-(const uint4& other) const;
	uint4 operator*(const uint4& other) const;
	uint4 operator/(const uint4& other) const;
	void print();
	bool operator==(const uint4& other) const;
	bool operator>(const uint4& other) const;
	bool operator<(const uint4& other) const;
};

void GNT GetCPUName(char* cpub);

template <typename T>
class Bufferd {
private:
	T* data;
	size_t size;
public:
	Bufferd(size_t n);
	~Bufferd();
	T& operator[](size_t i);
	size_t getSize() const;
};

void GNT GetPromptElement(char* buf, int code);

#endif
