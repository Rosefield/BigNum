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

typedef unsigned long long limb_t;

class BigInt {

    
    public:
	const static BigInt ZERO;
	const static BigInt ONE;
	const static BigInt TEN;
	
	BigInt();   
	BigInt(const std::string& s);
	BigInt(const int i);
//	BigInt(const long l);
	BigInt(const long long ll);
	BigInt(limb_t ull);
//	BigInt(const char& c);
	BigInt(const BigInt& b);
	BigInt(BigInt&& rhs);

	void swap(BigInt& rhs);
	int size() const;
    
	/*
	void realloc();
    	void realloc(unsigned);
	*/

   	BigInt gcd(const BigInt& rhs) const;
	limb_t Log2(limb_t) const;
	BigInt Log2(BigInt&) const;
	BigInt& lshift(int);
	BigInt& lLimbShift(int);
	BigInt& rLimbShift(int);
	
	BigInt pow(BigInt exp) const;
	BigInt pow(BigInt& exp, BigInt& mod) const;
	BigInt abs(const BigInt&) const;

	BigInt& operator=(BigInt rhs);
	BigInt operator+(const BigInt& rhs) const;
	BigInt operator-(const BigInt& rhs) const;
	BigInt operator/(const BigInt& rhs) const;
	BigInt operator*(const BigInt& rhs) const;
	BigInt operator%(const BigInt& rhs) const;
	BigInt& operator+=(const BigInt& rhs);
	BigInt& operator*=(const BigInt& rhs);
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

	std::string ToBinary() const;
	std::string ToHex() const;
	std::string ToDecimal() const;


	std::vector<limb_t> limbs;
	int bits;
	bool negative;
    
    private:
	limb_t threshold;    
	void CtorHelper(limb_t ull);
	void reallign();
	limb_t getBits(limb_t lt, int bits);

	//Multiplication
    	BigInt karatsuba(const BigInt& n1, const BigInt& n2);
	BigInt naiveMul(const BigInt& n1, const BigInt& n2);

	//Division
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

const BigInt BigInt::ZERO(0);
const BigInt BigInt::ONE(1);
const BigInt BigInt::TEN(10);


std::ostream& operator<<(std::ostream &strm, const BigInt& bn){
    strm << "Bigint= ";
    strm << (bn.negative ? "negative " : "positive ");
    for(int i = bn.size() -1; i >= 0; i--) {
	strm << bn.limbs[i] << " ";
    }	
    return strm;
};

std::string BigInt::ToBinary() const {
    std::string ret;
    for(int i = this->size() - 1; i >= 0; i--) {
	//the value of "bits", make this so that the value is not hardcoded if possible
	std::bitset<31> b(this->limbs[i]);
	ret += b.to_string();
	//ret.push_back(' ');
    }      	
    return ret;
}

std::string BigInt::ToDecimal() const {
    std::string ret;
    BigInt tmp(*this);
    BigInt oneBillion("1000000000");
    while(tmp != BigInt::ZERO) {
	std::stringstream ss;
	ss << std::setfill('0') << std::setw(9);
	BigInt digits;
	if(tmp.size() < 5) {
	    std::cout << "Almost done!" << std::endl;
	}
	BigInt::div(&tmp, tmp, oneBillion, &digits);
	if(digits.size() == 0) {
	    digits.limbs.push_back(0);
	}
	ss << digits.limbs[0];
	std::string s = ss.str();
	std::reverse(s.begin(), s.end());
	ret += s;
	//std::cout << "tmp, tmp%: " << tmp << std::endl << (tmp % 1000000000) << std::endl;	
	//tmp /= oneBillion;
	//std::cout << "tmp: " << tmp << std::endl;
    }

    std::reverse(ret.begin(), ret.end());
    
    if(ret.size() >1 && ret[0] == '0') {
	size_t pos = ret.find_first_not_of('0');
	ret.erase(0,pos);	    
    }


    return ret;
}

//The value that bits is set to should be used for both bits and for the shift in threshold
BigInt::BigInt(): limbs(), bits(31), negative(false), threshold(static_cast<unsigned int>(1 << 31)) {
    limbs.reserve(5);
}

//Assume string is in the form of [sign], digit , {digit}
BigInt::BigInt(const std::string& s): BigInt(){
    if(s.size() < 19) {
	*this = BigInt(stoll(s));
	return;
    }       

    for(auto c : s) {
	if(c == '+') negative = false;
	else if(c == '-') negative = true;
	else if(c >= '0' && c <= '9'){
	    *this *= BigInt::TEN;
	    *this += static_cast<int>(c - '0');
	}
    }       
}

BigInt::BigInt(const int i): BigInt() { 
    //std::cout << "Int constructor! " << i << std::endl;
    limb_t ull;
    if( i < 0 ) {
	negative = true;
	ull = static_cast<limb_t>(~i +1);
    } else {
	ull = static_cast<limb_t>(i);
    }
    
    CtorHelper(ull);

}

/*BigInt(const long l){

}
*/

BigInt::BigInt(const long long ll): BigInt() { 
 //   std::cout << "Long long constructor! " << ll << std::endl;
    limb_t ull;

    if( ll < 0) {
	negative = true;
	//Take the twos compliment of ll, add 1 to get absolute value, then cast as unsigned.
	ull = static_cast<limb_t>(~ll +1);
    } else {
	ull = static_cast<limb_t>(ll);
    }

    CtorHelper(ull);

}
BigInt::BigInt(limb_t ull): BigInt() {
    //std::cout << "Unsigned long long constructor! " << ull << std::endl;
    CtorHelper(ull);
}


void BigInt::CtorHelper(limb_t ull) {
    //Push each set of bits onto the vector
    do {
	//assign the $bits least significant bits to tmp
	limb_t tmp = getBits(ull, bits);	
	limbs.push_back(tmp);
	ull >>= bits;
    } while(ull);
}

/*BigInt(const char& c){

}*/

BigInt::BigInt(const BigInt& b): limbs(b.limbs), bits(b.bits), negative(b.negative), threshold(b.threshold) {
 //   std::cout << "Copy constructor!" << std::endl;
}

BigInt::BigInt(BigInt&& rhs): limbs(std::move(rhs.limbs)), bits(std::move(rhs.bits)), negative(std::move(rhs.negative)), threshold(std::move(rhs.threshold)) {
}

int BigInt::size() const{
    return limbs.size();
}

//Probably broken. Currently its primary use is removing excess leading 0 limbs
void BigInt::reallign() {
    //Carry can be at most (64 - $bits + 1) bits long and hold a value of 2^(64-bits + 1) -1
    //the additional +1 is due to the possibility of overflow in the individual limbs 
    limb_t carry = 0;

    for(int i = 0; i < this->size(); i++) {
	/* do something here to ensure that each limb only contains at most $bits bits
	    Need to consider that each limb can possibly have leading 0 bits, and that
	    those need to be propogated 
	*/
	if(i + 1 == this->size()) {
	    if(limbs[i] == 0 && carry == 0 && limbs.size() > 1) {
		limbs.pop_back();
		while(limbs.size() > 1 && limbs[limbs.size() -1] == 0) {
		    limbs.pop_back();
		}
		break;
	    }

	    limb_t tmp = limbs[i] + carry;
	    if(tmp < carry) {
		std::cout << "Overflow end!" << std::endl;		
		//deal with overflow
		limbs[i] = getBits(tmp, bits);
		tmp >>= bits;
		//set the 64 - bits + 1 bit 
		tmp |= 1 << (64 - bits);
		while(tmp) {
		    limbs.push_back(getBits(tmp, bits));
		    tmp >>= bits;
		}

	    } else if (tmp >= threshold) {
		limbs[i] = getBits(tmp, bits);
		tmp >>= bits;
		while(tmp) {
		    limbs.push_back(getBits(tmp, bits));
		    tmp >>= bits;
		    i += 1;
		}
	    } else {
		limbs[i] = tmp;
	    }
	    
	} 
	    else {
	    limb_t tmp = limbs[i] + carry;
	    if(tmp < limbs[i]) {
		std::cout << "Overflow mid!" << std::endl;
		//deal with overflow
		limbs[i] = getBits(tmp, bits);
		tmp >>= bits;
		//set the 64 - bits + 1 bit 
		tmp |= 1 << (64 - bits);
		carry = tmp;
	    } else if(tmp >= threshold) { 
		limbs[i] = getBits(tmp, bits);
		tmp >>= bits;
		carry = tmp;
	    } else {
		limbs[i] = tmp;
	    }	    
	}

    }   
}

limb_t BigInt::getBits(limb_t lt, int bits) {
    return lt & ((1 << bits) - 1);
}

BigInt BigInt::gcd(const BigInt& rhs) const{
    if(rhs == BigInt::ZERO || rhs.size() == 0) {
	return *this;
    } else if(*this == rhs) {
	return *this; 
    } else {
	return rhs.gcd(*this % rhs);
    }
}


limb_t BigInt::Log2(limb_t lt) const {
    if(lt == 0) return 0;
    limb_t ret = 0;
    asm("bsrq %1, %0" : "=r" (ret) : "r" (lt));
    return ret;
}

BigInt BigInt::Log2(BigInt& num) const {
    BigInt ret = num.size() -1 * bits;
    ret += Log2(num.limbs[num.size() -1]);
    return ret;
}

BigInt& BigInt::lshift(int i){
    if(i == 0) {
	return *this;
    } else if((i % bits) == 0) {
	return lLimbShift(i/bits);
    } else {
	//insert i/bits empty limbs at the beginning then shift every limb left by i%bits


	return *this;
    }
}
BigInt& BigInt::lLimbShift(int i){
    if(i == 0) return *this;

    std::vector<limb_t> tmp(i, 0);
    this->limbs.insert(this->limbs.begin(), tmp.begin(), tmp.end());
    return *this;
}

BigInt& BigInt:: rLimbShift(int i) {
    if(i == 0) return *this;
    if(i >= this->size()) {
	*this = BigInt::ZERO;
	return *this;
    }
    std::vector<limb_t> vt;
    vt.insert(vt.begin(), this->limbs.begin() + i, this->limbs.end());
    this->limbs.swap(vt);
    return *this;
}

//Montogery's Ladder algorithm

//Limited to long long, which if you take 2^(2^63) would give a number that takes 1 Exabyte
// of ram to store
BigInt BigInt::pow(BigInt exp) const {
    if(exp == BigInt::ZERO) {
	return BigInt::ONE;
    }
    else if(exp == BigInt::ONE) return *this;
    else if(exp < BigInt::ZERO) return BigInt::ZERO;
    else {
	BigInt t1(*this);
	BigInt t2(t1*t1);

	std::string bit_string = exp.ToBinary();
	size_t hi = bit_string.find_first_of('1');

	for(auto it = bit_string.begin() + hi +1; it < bit_string.end(); ++it){
	    if(*it == '1') {
		t1 *= t2;
		t2 *= t2;
	    } else {
		t2 *= t1;
		t1 *= t1;
	    }
	}	
	return t1;
    }
}

BigInt BigInt::pow(BigInt& exp, BigInt& mod) const {
    if(exp == BigInt::ZERO) {
	return BigInt::ONE;
    } else if(exp == BigInt::ONE) {
	return *this % mod;
    } else if(exp < BigInt::ZERO) {
	//find inverse, if it exists, and I feel motivated to implement it
    } else {
	BigInt base(*this);
	if(Log2(exp) >= 520) {
	    return modexp_sliding_window(base, exp, mod);
	} else {
	    return modexp_montgomery(base, exp, mod);
	}
    }
    return BigInt::ZERO;   
}

BigInt BigInt::modexp_sliding_window(BigInt& base, BigInt& exp, BigInt& mod, int k) const {

    return BigInt::ZERO;
}

BigInt BigInt::modexp_montgomery(BigInt& base, BigInt& exp, BigInt& mod) const {
    
    BigInt t1(base % mod);
    BigInt t2((t1 * t1) % mod);

    std::string bits = exp.ToBinary();
    size_t hi = bits.find_first_of('1');
    
    for(auto it = bits.begin() + hi +1; it < bits.end(); it++) {
	if(*it == '1') {
	    t1 = ((t1 * t2) % mod);
	    t2 = ((t2 * t2) % mod);
	} else {
	    t2 = ((t2 * t1) % mod);
	    t1 = ((t1 * t1) % mod);
	}
    }
    return t1;
}


BigInt BigInt::abs(const BigInt& lhs) const {
    BigInt tmp(lhs);
    tmp.negative = false;
    return tmp;
}


void BigInt::swap(BigInt& rhs){
    using std::swap;

    swap(this->limbs,   rhs.limbs);
    swap(this->bits,    rhs.bits);
    swap(this->negative, rhs.negative);
}

BigInt& BigInt::operator=(BigInt rhs){
    swap(rhs);
    return *this;
}
BigInt BigInt::operator+(const BigInt& rhs) const{
    BigInt tmp(*this);
    tmp += rhs;
    return tmp;
}
BigInt BigInt::operator-(const BigInt& rhs) const{
    BigInt tmp(*this);
    tmp -= rhs;
    return tmp;
}
BigInt BigInt::operator/(const BigInt& rhs) const{
    BigInt tmp(*this);
    tmp /= rhs;
    return tmp;
}
BigInt BigInt::operator*(const BigInt& rhs) const{
    BigInt tmp(*this);
    tmp *= rhs;
    return tmp;
}
BigInt BigInt::operator%(const BigInt& rhs) const{
    BigInt tmp(*this);
    tmp %= rhs;
    return tmp;
}
BigInt& BigInt::operator+=(const BigInt& rhs){

    //Figure out implementation details of negatives later, either as creating an
    //extended twos compliment number from the limbs, or through manipulation of a
    //sign bit otherwhere (such as a "negative" boolean")
    if(this->negative != rhs.negative) {
	*this -= -rhs;	
	return *this;
    }

    if(rhs.size() > this->size()) this->limbs.resize(rhs.size(), 0);
    limb_t carry = 0;
    for(int i = 0; i < rhs.size(); i++){
	this->limbs[i] += rhs.limbs[i] + carry;
	
	if(this->limbs[i] >= threshold) {
	    carry = (this->limbs[i] >> bits);
	    this->limbs[i] = getBits(this->limbs[i], bits);
//	    std::cout << "carry, this: " << carry << " " << this->limbs[i] << std::endl;
	} else {
	    carry = 0;
	}

	
	//is there a better way to do this than checking for overflow which is undefined behavior?
	if(this->limbs[i] < rhs.limbs[i] + carry || (rhs.limbs[i] == ULLONG_MAX && carry)){
	 //   std::cout << "Overflow, somehow. Debug: " << std::endl;
	 //   std::cout << "this, rhs, carry:" << this->limbs[i] << " " << rhs.limbs[i] << " " 
         //             << carry << std::endl;
	    //carry = 1;
	} else {
	    //carry = 0;	
	}	
    }
      
    if (carry){
	if(this->size() > rhs.size()) {
	    for(int i = rhs.size(); i < this->size(); ++i) {
		if(carry == 0) {break;}	
		limbs[i] += carry;
		if(limbs[i] >= threshold) {
		    carry = this->limbs[i] >> bits;
		    this->limbs[i] = getBits(this->limbs[i], bits);
		 } else {
		    carry = 0;
		}
	    }
	    if(carry) {
		this->limbs.push_back(carry);
	    }
	} else {
	    limbs.push_back(carry);
	}
    }
  


    //this->reallign();

    return *this;
}

BigInt& BigInt::operator-=(const BigInt& rhs){
    if(this->negative != rhs.negative) {
	*this += -rhs;
	return *this;
    }
    BigInt tmp(rhs); 
    if(tmp.size() > this->size()) this->limbs.resize(tmp.size(), 0);
    for(int i = 0; i < tmp.size(); i++) {
	if(tmp.limbs[i] > this->limbs[i]) {
	   // std::cout << "Tmp limb larger than *this" << std::endl;    
	    for(auto it = this->limbs.begin() +i + 1; it < this->limbs.end(); it++) {
		if(*it > 0){
		    *it -= 1;
		    for(auto it2 = it -1; it2 > this->limbs.begin() + i + 1; it2--) {
			*it2 = threshold -1;
		    }
		    this->limbs[i] += threshold;
		    break;
		}
	    }
	    //If this->limbs[i] is still < rhs.limbs[i] then all limbs {this->limbs[x] | x > i} are
	    //zero, and a carry bit could not be retrieved. *this flips sign and adds the remainder
	    //of rhs 
	    if(this->limbs[i] < tmp.limbs[i]) {
		tmp.limbs[i] -= this->limbs[i];
		this->limbs[i] = 0;
		tmp -= *this;
		*this = tmp;
		this->negative = !this->negative;
		return *this;
	    
	    //if the carry was successful, actually subtract the number
	    } else {
		//Sucessful carry!
		this->limbs[i] -= tmp.limbs[i];
		tmp.limbs[i] = 0;
	    }

	} else {
	    //std::cout << "Tmp limb smaller than *this" << std::endl;    
	    this->limbs[i] -= tmp.limbs[i];
	    tmp.limbs[i] = 0;
	}
    } 
    if(this->limbs[this->size()-1] == 0) {
	this->limbs.pop_back();
    }      
   //reallign();
   return *this;
}
BigInt& BigInt::operator*=(const BigInt& rhs){
    bool neg = this->negative != rhs.negative;
    if(this->size() < 2 || rhs.size() < 2) {
	*this = naiveMul(*this, rhs);
    } else {
	//Until subtraction is fixed, always do the naivemul
    //	*this = naiveMul(*this,rhs);
    	*this = karatsuba(*this, rhs);
    }
    this->negative = neg;
    //std::cout << "n: " << rhs << std::endl; 
    //std::cout << "pre-allign:  " << *this << std::endl;
    this->reallign();
    //std::cout << "post-allign: " << *this << std::endl;
    return *this;
}

/**
* Karatsuba multiplication is taking two integers x = x_1*B^m + x_0, y = y_1*B^m +y_0
* where B is the radix of the system, in this case it is 2^(bits) -1. Therefore xy
* is of the form z_2*B^(2m) + z_1*B^m + z_0 
*/
BigInt BigInt::karatsuba(const BigInt& n1, const BigInt& n2) {
    if(n1.size() == 0 || n2.size() == 0) {
	return BigInt::ZERO;
    }

    if(n1.size() < 2 || n2.size() < 2 || n1.size() + n2.size() < 10) {
	return naiveMul(n1, n2);
    }

    //m is chosen to be Ciel(n/2) as that minimizes the number of recursion steps
    //necessary. A number of size n would have limbs of powers 0,1,2,...,n-1
    int m = n1.size() > n2.size() ? (n2.size() +1) /2 : (n1.size() +1) /2;

    BigInt n1l, n1h, n2l, n2h;
    BigInt z0, z1, z2;

    //n1l, n2l are the portions of n1, n2 that are less than B^m
    //n1h, n2h are the portions of n1, n2 that are equal or greater than B^m
    n1l = n1.lowerNLimbs(m);
    n1h = n1.highNLimbs(n1.size() - m);
    n2l = n2.lowerNLimbs(m);
    n2h = n2.highNLimbs(n2.size() - m);  

    z0 = karatsuba(n1l, n2l); 
    z2 = karatsuba(n1h, n2h);
    z1 = karatsuba((n1l + n1h), (n2l + n2h)) - z2 - z0;

    //Ensure that after the subtraction there isn't empty limbs at the head of z1
    for(int i = z1.size() -1; i > 0; --i) {
	if(z1.limbs[i] != 0) { break; }
	z1.limbs.pop_back();
    }    
    BigInt r = (z2.lLimbShift(2*m)) + (z1.lLimbShift(m)) + (z0);
    return r;
}

//Knuth Algorithm M
BigInt BigInt::naiveMul(const BigInt& n1, const BigInt& n2) {
    if(n1 == BigInt::ZERO || n2 == BigInt::ZERO) {
	return BigInt::ZERO;
    }

    BigInt tmp;
    int m = n1.size() + n2.size();
    tmp.limbs.resize(m, 0);
    limb_t partial = 0;
    for(int i = 0; i < n1.size(); i++) {
	limb_t carry = 0;
	for(int j = 0; j < n2.size(); j++) {
	    //tmp.limbs[i+j] += n1.limbs[i] * n2.limbs[j];
	    partial = tmp.limbs[i+j] + n1.limbs[i] * n2.limbs[j] + carry;
	    tmp.limbs[i+j] = partial & (threshold -1);
	    carry = partial >> bits;
	}
	tmp.limbs[i + n2.size()] = carry;
    }
 
    for(int i = tmp.size() -1; i > 0; --i) {
	if(tmp.limbs[i] != 0) { break; }
	tmp.limbs.pop_back();
    }
    return tmp;
}
//Returns only the high limb (limbs[size() -1) in the BigInt
BigInt BigInt::highLimb() const{
    BigInt tmp(*this);
    std::vector<limb_t> vt;
    vt.push_back(tmp.limbs[tmp.size()-1]);
    tmp.limbs.swap(vt);
    return tmp;
}

BigInt BigInt::highNLimbs(int n) const {
    BigInt tmp(*this);
    if(n > tmp.size()) return tmp;

    std::vector<limb_t> vt;
    vt.insert(vt.begin(), tmp.limbs.end() - n, tmp.limbs.end());
    tmp.limbs.swap(vt);
    return tmp;
}

//Return all but the high limb (limbs[size() -1]) in the BigInt
BigInt BigInt::lowerLimbs() const{
    BigInt tmp(*this);
    tmp.limbs.pop_back();
    if(tmp.size() == 0) { tmp = BigInt::ZERO; }   
    return tmp;
}

BigInt BigInt::lowerNLimbs(int n) const {
    BigInt tmp(*this);
    if(n > tmp.size()) return tmp;

    std::vector<limb_t> vt;
    vt.insert(vt.begin(), tmp.limbs.begin(), tmp.limbs.begin() + n);
    tmp.limbs.swap(vt);
    return tmp;
}


BigInt BigInt::getLimbsRange(int start, int end) const {
    BigInt tmp(*this);
    std::vector<limb_t> vt;
    vt.insert(vt.begin(),tmp.limbs.begin() + start, tmp.limbs.begin() + end);
    tmp.limbs.swap(vt);
    return tmp;
}

BigInt& BigInt::operator/=(const BigInt& rhs){
    if(rhs > BigInt::abs(*this)) {
	*this = BigInt::ZERO;
	return *this;
    }

    BigInt::div(this, *this, rhs, nullptr);

    this->reallign();
    return *this;
}

//Seems to work for small inputs
BigInt& BigInt::operator%=(const BigInt& rhs){
    if(*this < BigInt::ZERO) {
	*this = abs(*this);
	if(*this > rhs) {
	   BigInt::div(nullptr, *this, rhs, this);
	}
	*this = rhs - *this;
	this->reallign();
	return *this;
    }

    if(rhs > *this) {
	return *this;
    }
    if(rhs == *this) {
	*this = BigInt::ZERO;
	return *this;
    }
    
    BigInt::div(nullptr, *this, rhs, this);

    this->reallign();
    return *this;
}

//There is no guarantee that this will work if dv, rem, or num share pointers.
void BigInt::div(BigInt * dv, const BigInt& num, const BigInt& denom, BigInt * rem) const {
    if(num == denom) {
	if(rem != nullptr) {
	    *rem = BigInt::ZERO;
	}
	if(dv != nullptr) {
	    *dv = BigInt::ONE;
	}
	return;
    }
    if(num < denom) {
	if(rem != nullptr) {
	    *rem = num;
	}
	if(dv != nullptr) {
	    *dv = BigInt::ZERO;
	}
	return;
    }

    BigInt acc = BigInt::ZERO;
    BigInt tmp = denom;
    BigInt a = num;
    limb_t qhat, v_1, d;
    v_1 = tmp.limbs[denom.size() -1];
    d = (threshold / (v_1 + 1));
    //Normalize
    if(d == 1) {
	a.limbs.push_back(0);
    } else {
	a *= d;
	tmp *= d;
    }
    v_1 = tmp.limbs[denom.size()-1];	

    int j = 0;
    int m = a.size() - tmp.size() -1;
    acc.limbs.resize(m+1, 0);
    //For the algorithm our limbs are indexed so that the highest limb is 0, second highest 1, etc
    //However, internal storage is the opposite where 0 is the minimal limb, so to
    //compensate there are a lot of size() - j calls
    do {
	//Calculate qhat
	if(a.limbs[a.size() -1 -j] == v_1) {
	    qhat = threshold -1;
	} else {
	    qhat = ((a.limbs[a.size() -1 - j] << bits) | a.limbs[a.size() -2 - j]) / v_1; 
	}
	while(tmp.limbs[tmp.size() -2] * qhat > ((a.limbs[a.size() -1 - j] + a.limbs[a.size() -2 -j] - qhat * v_1) * threshold + 
						  a.limbs[a.size() -3 - j])) {
	    --qhat;
	}
	//mult and sub
	BigInt uprime = a.getLimbsRange(a.size()-1-j-tmp.size(), a.size()-j);
	uprime -= tmp * qhat;

	//The estimator was 1 too low, fix
	if(uprime > tmp) {
	    ++qhat;
	    uprime -= tmp;
	}
	//Super special case, should happen about 2/threshold times
	if(uprime < BigInt::ZERO) {
	    //qhat was 1 too big causing uprime to be negative, decrement
	    --qhat;
	    uprime += tmp;	
	}	
	//maintain number of digits
	while(uprime.size() < tmp.size()+1) {
	    uprime.limbs.push_back(0);
	}
	//Replace the values in *this with the "divided" values held in uprime
	std::swap_ranges(uprime.limbs.begin(), uprime.limbs.end(), a.limbs.end()-1-j-tmp.size());
	//uprime should be at most 1 limb long after the subtraction (and without the readd stage)
	acc.limbs[m-j] = qhat;

	++j;
    } while (j <= m);

    acc.negative = false;

    if(dv != nullptr) {
	*dv = acc;
    }

    if(rem != nullptr) {
	a.reallign();
	if(a.size() > tmp.size() +1) {
	    std::cout << "failure in division" << std::endl;
	    a = a.getLimbsRange(0, tmp.size());
	}	

	//Manually resize the leftover value, as sometimes its higher limbs are polluted with 1s; Investigate the cause of that
    	//as it seems to relate to a failure of the division
	if(d == 1) {
	    *rem = a;
	} else {
	    *rem = a / d;
	}		
    }
}

BigInt BigInt::operator-() const{
    BigInt tmp(*this);
    tmp.negative = !tmp.negative;
    return tmp;
}

//Make comparison operator a constant time operation? Useful for crypto focus, otherwise the speed
//loss isn't really justified
bool BigInt::operator==(const BigInt& rhs) const{
    if(this->size() != rhs.size()) return false;
    bool result = true;
    for(int i = this->size() -1; i >= 0; i--) {
	if (this->limbs[i] != rhs.limbs[i]) result = false;	
    }

    return result;
}

bool BigInt::operator!=(const BigInt& rhs) const{
    return !(*this == rhs);
}

bool BigInt::operator>(const BigInt& rhs) const{
    return rhs < *this;
}

bool BigInt::operator<(const BigInt& rhs) const{
    if(this->negative != rhs.negative) return this->negative;
    if(this->negative) {
	if(this->size() != rhs.size()) return this->size() > rhs.size();
	
	/*
	* Starting at "false" and checking if each limb is either over or under ensures 
	* that two equivalent limbs do not return "true"
	*/
	bool result = false;
	for(int i = 0; i < this->size(); i++) {
	    if(this->limbs[i] < rhs.limbs[i]) result = false;
	    if(this->limbs[i] > rhs.limbs[i]) result = true;
	}

	return result;

    } else {
	if(this->size() != rhs.size()) return this->size() < rhs.size();
	
	/*
	* Starting at "false" and checking if each limb is either over or under ensures 
	* that two equivalent limbs do not return "true"
	*/
	bool result = false;
	for(int i = 0; i < this->size(); i++) {
	    if(this->limbs[i] < rhs.limbs[i]) result = true;
	    if(this->limbs[i] > rhs.limbs[i]) result = false;
	}

	return result;
    }
}

bool BigInt::operator>=(const BigInt& rhs) const {
    return !(*this < rhs);   
}

bool BigInt::operator<=(const BigInt& rhs) const{
    return !(rhs < *this);
}

#endif
