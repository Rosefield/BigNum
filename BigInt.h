#ifndef _BigInt
#define _BigInt
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <climits>
#include <bitset>
#include <algorithm>
#include <chrono>

typedef unsigned long long limb_t;

class BigInt {

    
    public:
	std::vector<limb_t> limbs;
	int bits;
	bool negative;
	
	const static BigInt ZERO;
	const static BigInt ONE;
	const static BigInt TEN;
	
	BigInt();   
	BigInt(const std::string& s);
	BigInt(const int i);
	BigInt(const long long ll);
	BigInt(limb_t ull);
	BigInt(const BigInt& b);
	BigInt(BigInt&& rhs);

	void swap(BigInt& rhs);
	size_t size() const;
    
	std::string ToBinary() const;
	std::string ToHex() const;
	std::string ToDecimal() const;


	BigInt& operator=(BigInt rhs);
	BigInt operator+(const BigInt& rhs) const;
	BigInt operator-(const BigInt& rhs) const;
	BigInt operator/(const BigInt& rhs) const;
	BigInt operator*(const BigInt& rhs) const;
	BigInt operator*(const limb_t& rhs) const;
	BigInt operator%(const BigInt& rhs) const;
	BigInt& operator+=(const BigInt& rhs);
	BigInt& operator*=(const BigInt& rhs);
	BigInt& operator*=(const limb_t& rhs);
	BigInt& operator/=(const BigInt& rhs);
	BigInt& operator%=(const BigInt& rhs);
	BigInt& operator-=(const BigInt& rhs);
	BigInt operator-() const;

	bool operator==(const BigInt& rhs) const;
	bool operator!=(const BigInt& rhs) const;
	bool operator>(const BigInt& rhs) const;
	bool operator<(const BigInt& rhs) const;
	bool operator>=(const BigInt& rhs) const;
	bool operator<=(const BigInt& rhs) const;


   	BigInt gcd(const BigInt& rhs) const;
	limb_t log2(const limb_t) const;
	BigInt log2(const BigInt&) const;
	BigInt& lshift(int);
	BigInt& lLimbShift(int);
	BigInt& rLimbShift(int);
	
	BigInt pow(BigInt exp) const;
	BigInt abs(const BigInt&) const;

	BigInt genRandomBits(const BigInt& bits) const;
	BigInt genRandomNum(const BigInt& high) const;
	BigInt genRandomNum(const BigInt& low, const BigInt& high) const;
	BigInt genProbabalisticPrime(const BigInt& low, const BigInt& high) const;
	BigInt genPrime(const BigInt& low,  const BigInt& high) const;

	BigInt mod_add(BigInt& add, BigInt& mod) const;
	BigInt mod_sub(BigInt& sub, BigInt& mod) const;
	BigInt mod_mul(BigInt& mul, BigInt& mod) const;
	BigInt mod_inv(BigInt& mod) const;
	BigInt mod_sqr(BigInt& mod) const;
	BigInt pow(BigInt& exp, BigInt& mod) const;

	BigInt naiveMul(const BigInt& n1, const BigInt& n2);
    
    private:
	limb_t base;    
	void CtorHelper(limb_t ull);
	void reallign();
	limb_t getBits(limb_t lt, int bits);

	//Multiplication
    	BigInt karatsuba(const BigInt& n1, const BigInt& n2);
	void karatsuba(const std::vector<limb_t>& n1, const std::vector<limb_t>& n2, std::vector<limb_t>& scratch, 
			unsigned scratch_offset, unsigned n1l_offset, unsigned n1_size, 
			unsigned n2l_offset, unsigned n2_size );
	
	void naiveMul(std::vector<limb_t>::const_iterator n1, std::vector<limb_t>::const_iterator n2, 
			std::vector<limb_t>::iterator scratch, unsigned n1_size, unsigned n2_size );
	//BigInt naiveMul(const BigInt& n1, const BigInt& n2);
	BigInt naiveMul(const BigInt& n1, const limb_t& n2);

	//Division
	void div(BigInt * dv, const BigInt& num, const limb_t& denom, BigInt * rem) const;
	void div(BigInt * dv, const BigInt& num, const BigInt& denom, BigInt * rem) const;

	//Exponentiation
	BigInt modexp_sliding_window(BigInt& base, BigInt& exp, BigInt& mod, int k = 5) const;
	BigInt modexp_montgomery(BigInt& base, BigInt& exp, BigInt& mod) const;

	//Limb manipulation
	BigInt highLimb() const;
	BigInt highNLimbs(int n) const;
	BigInt lowerLimbs() const;
	BigInt lowerNLimbs(int n) const;
	BigInt getLimbsRange(int start, int end) const;

    
};

std::ostream& operator<<(std::ostream &strm, const BigInt& bn);



#endif
