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

	void swap(BigInt& rhs);
	int size() const;
    
	/*
	void realloc();
    	void realloc(unsigned);
	*/

   	BigInt gcd(BigInt lhs, BigInt rhs) const;
	limb_t Log2(limb_t);
	BigInt& lshift(int);
	BigInt& lLimbShift(int);
	BigInt& rLimbShift(int);
	
	BigInt pow(int) const;
	BigInt pow(int, const BigInt&) const;
	BigInt abs(const BigInt&) const;

	BigInt& operator=(BigInt rhs);
	BigInt operator+(const BigInt& rhs);
	BigInt operator-(const BigInt& rhs);
	BigInt operator/(const BigInt& rhs);
	BigInt operator*(const BigInt& rhs);
	BigInt operator%(const BigInt& rhs);
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
	BigInt karatsuba(const BigInt& n1, const BigInt& n2);
	BigInt naiveMul(const BigInt& n1, const BigInt& n2);
	BigInt highLimb() const;
	BigInt highNLimbs(int n) const;
	BigInt lowerLimbs() const;
};

const BigInt BigInt::ZERO(0);
const BigInt BigInt::ONE(1);
const BigInt BigInt::TEN(10);


std::ostream& operator<<(std::ostream &strm, const BigInt& bn){
    strm << "Bigint= ";
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
	ret.push_back(' ');
    }      	

    return ret;
}

std::string BigInt::ToDecimal() const {
    std::string ret;
    BigInt tmp(*this);
    while(tmp != BigInt::ZERO) {
	std::stringstream ss;
	ss << std::setfill('0') << std::setw(9);
	ss << (tmp % 1000000000).limbs[0];
	std::string s = ss.str();
	std::reverse(s.begin(), s.end());
	ret += s;
	//std::cout << "tmp, tmp%: " << tmp << (tmp % 1000000000) << std::endl;	
	tmp /= 1000000000;
	//std::cout << "tmp: " << tmp << std::endl;
    }

    std::reverse(ret.begin(), ret.end());
    
    return ret;
}

//The value that bits is set to should be used for both bits and for the shift in threshold
BigInt::BigInt(): limbs(), bits(31), negative(false), threshold(static_cast<unsigned int>(1 << 31)) {
    limbs.reserve(5);
}

//Assume string is in the form of [sign], digit , {digit}
BigInt::BigInt(const std::string& s): BigInt(){
 //   std::cout << "String constructor! " << s << std::endl;
    if(s.size() < 19) {
	BigInt(stoll(s));
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
    //std::cout << "Long long constructor! " << ll << std::endl;
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
 //   std::cout << "CtorHelper: " << ull;
    do {
	//assign the $bits least significant bits to tmp
	auto tmp = getBits(ull, bits);	
//	std::cout << " " << tmp;
	limbs.push_back(tmp);
	ull >>= bits;
    } while(ull);
//    std::cout << std::endl;
}

/*BigInt(const char& c){

}*/

BigInt::BigInt(const BigInt& b): limbs(b.limbs), bits(b.bits), negative(b.negative), threshold(b.threshold) {
 //   std::cout << "Copy constructor!" << std::endl;
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
		    std::cout << "stuck in a loop!" << std::endl;
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

BigInt gcd(BigInt lhs, BigInt rhs) {
    if(rhs == BigInt::ZERO) {
	return lhs;
    } else {
	return gcd(rhs, rhs % lhs);
    }
}


limb_t BigInt::Log2(limb_t lt) {
    if(lt == 0) return 0;
    limb_t ret = 0;
    asm("bsrq %1, %0" : "=r" (ret) : "r" (lt));
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
BigInt BigInt::pow(int exp) const {
    if(exp == 0) {
	return BigInt::ONE;
    }
    else if(exp == 1) return *this;
    else {
	BigInt t1(*this);
	BigInt t2(t1*t1);
	std::bitset<32> bs(exp);
	//stupid way to find the highest set bit
	int hi = 0;
	for(int j = 0; j < 32; j++) {
	    if(bs.test(j)) hi =j;
	}
	for(int x = hi -1; x >= 0; x--){
	    if(bs.test(x)) {
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

BigInt BigInt::pow(int i, const BigInt& mod) const {
    
    return BigInt::ZERO;   
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
BigInt BigInt::operator+(const BigInt& rhs){
    BigInt tmp(*this);
    tmp += rhs;
    return tmp;
}
BigInt BigInt::operator-(const BigInt& rhs){
    BigInt tmp(*this);
    tmp -= rhs;
    return tmp;
}
BigInt BigInt::operator/(const BigInt& rhs){
    BigInt tmp(*this);
    tmp /= rhs;
    return tmp;
}
BigInt BigInt::operator*(const BigInt& rhs){
    BigInt tmp(*this);
    tmp *= rhs;
    return tmp;
}
BigInt BigInt::operator%(const BigInt& rhs){
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
	    limbs[rhs.size()] += carry;
	    if(limbs[rhs.size()] >= threshold) {
		std::cout << "end carry overflow" << std::endl;
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
		    for(auto it2 = it; it2 > this->limbs.begin() + i + 1; it2--) {
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
    	*this = naiveMul(*this,rhs);
    //	*this = karatsuba(*this, rhs);
    }
    this->negative = neg;
    //std::cout << "n: " << rhs << std::endl; 
    //std::cout << "pre-allign:  " << *this << std::endl;
    this->reallign();
    //std::cout << "post-allign: " << *this << std::endl;
    return *this;
}

BigInt BigInt::karatsuba(const BigInt& n1, const BigInt& n2) {
    std::cout << "n1: " << n1 << "n2: " << n2 << std::endl;   


    if(n1.size() < 2 || n2.size() < 2) {
	std::cout << "naive mul" << std::endl;
	return naiveMul(n1, n2);
    }
    int m = n1.size() > n2.size() ? (n1.size() + 1)/2 : (n2.size()+1)/2;

    BigInt n1l, n1h, n2l, n2h;
    BigInt z0, z1, z2;

    n1l = n1.lowerLimbs();
    n1h = n1.highLimb();
    n2l = n2.lowerLimbs();
    n2h = n2.highLimb();  

    z0 = karatsuba(n1l, n2l); 
    z1 = karatsuba((n1l + n1h), (n2l + n2h));
    z2 = karatsuba(n1h, n2h);
    
    return (z2.lLimbShift(m)) + ((z1-z2-z0).lLimbShift(m/2)) + (z0);

}

//Knuth Algorithm M
BigInt BigInt::naiveMul(const BigInt& n1, const BigInt& n2) {
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
//Does not work.
BigInt& BigInt::operator/=(const BigInt& rhs){
    if(rhs > *this) {
	*this = BigInt::ZERO;
	return *this;
    }
    BigInt acc = BigInt::ZERO;
    BigInt tmp = rhs;
    tmp.negative = this->negative;
    if(tmp.size() == 1) {
	
	for(int i = this->size() -1; i >= 0; i--) {
	    acc = acc.lLimbShift(1);
	    if(i +1 != this->size()) {
		this->limbs[i] |= (this->limbs[i+1] << bits);
		this->limbs[i+1] = 0;
	    }
	    acc += (this->limbs[i] / tmp.limbs[i]);
	    this->limbs[i] %= tmp.limbs[i];
	}

    //Currently uses the "Grade school" method for division. 
    } else {
	std::cout << "Dividing! num, denom: " << *this << " " << rhs << std::endl;
	tmp = tmp.lLimbShift(this->size() - rhs.size());
	while(abs(tmp) >= abs(rhs)) { 
	    acc = acc.lLimbShift(1);
	    while(*this > tmp) {
		*this -= tmp;
		acc += BigInt::ONE;
	    }
	    std::cout << "acc: " << acc << std::endl;
	    tmp = tmp.rLimbShift(1);
	    std::cout << "tmp, rhs" << tmp << " " << rhs << std::endl;   
	}
    }
    acc.negative = this->negative ^ rhs.negative;
    *this = acc;
    this->reallign();
    return *this;
}

//Seems to work for small inputs
BigInt& BigInt::operator%=(const BigInt& rhs){
    if(rhs > *this) {
	return *this;
    }

    BigInt tmp = rhs;
    tmp.negative = this->negative;
    tmp = tmp.lLimbShift(this->size() - rhs.size());
    while(*this > rhs) { 
	while(*this > tmp) {
	    *this -= tmp;
	}
	tmp = tmp.rLimbShift(1);    
    }  

    this->reallign();
    return *this;
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
