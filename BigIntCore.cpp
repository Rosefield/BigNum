#include "BigInt.h"

const BigInt BigInt::ZERO(0);
const BigInt BigInt::ONE(1);
const BigInt BigInt::TEN(10);

size_t BigInt::size() const{
    return limbs.size();
}

//operator<< needs to be defined outside of the main function
std::ostream& operator<<(std::ostream &strm, const BigInt& bn){
    strm << "Bigint= ";
    strm << (bn.negative ? "negative " : "positive ");
    for(int i = bn.size() -1; i >= 0; i--) {
	strm << bn.limbs[i] << " ";
    }	
    return strm;
};

/**
* CONSTRUCTORS
**/

//The value that bits is set to should be used for both bits and for the shift in base
BigInt::BigInt(): limbs(), bits(31), negative(false), base(static_cast<unsigned int>(1 << 31)) {
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
	    *this *= 10;
	    *this += static_cast<limb_t>(c - '0');
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

BigInt::BigInt(const long long ll): BigInt() { 
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

BigInt::BigInt(const BigInt& b): limbs(b.limbs), bits(b.bits), negative(b.negative), base(b.base) {
}

BigInt::BigInt(BigInt&& rhs): limbs(std::move(rhs.limbs)), bits(std::move(rhs.bits)), negative(std::move(rhs.negative)), base(std::move(rhs.base)) {
}

void BigInt::swap(BigInt& rhs){
    using std::swap;

    swap(this->limbs,   rhs.limbs);
    swap(this->bits,    rhs.bits);
    swap(this->negative, rhs.negative);
}

void BigInt::reallign() {
    //Carry can be at most (64 - $bits + 1) bits long and hold a value of 2^(64-bits + 1) -1
    //the additional +1 is due to the possibility of overflow in the individual limbs 
    limb_t carry = 0;

    for(int i = 0; i < this->size() -1; i++) {
	/* do something here to ensure that each limb only contains at most $bits bits
	    Need to consider that each limb can possibly have leading 0 bits, and that
	    those need to be propogated 
	*/
	    
	//if(carry == 0) { continue; }
	limb_t tmp = limbs[i] + carry;
	if(tmp < limbs[i]) {
	    std::cout << "Overflow mid!" << std::endl;
	    //deal with overflow
	    limbs[i] = getBits(tmp, bits);
	    tmp >>= bits;
	    //set the 64 - bits + 1 bit 
	    tmp |= 1 << (64 - bits);
	    carry = tmp;
	} else if(tmp >= base) { 
	    limbs[i] = getBits(tmp, bits);
	    tmp >>= bits;
	    carry = tmp;
	} else {
	    limbs[i] = tmp;
	}	    

    } 

    if(carry) {
	    limb_t tmp = limbs.back() + carry;
	    if(tmp < carry) {
		std::cout << "Overflow end!" << std::endl;		
		//deal with overflow
		limbs.back() = getBits(tmp, bits);
		tmp >>= bits;
		//set the 64 - bits + 1 bit 
		tmp |= 1 << (64 - bits);
		while(tmp) {
		    limbs.push_back(getBits(tmp, bits));
		    tmp >>= bits;
		}

	    } else if (tmp >= base) {
		limbs.back() = getBits(tmp, bits);
		tmp >>= bits;
		while(tmp) {
		    limbs.push_back(getBits(tmp, bits));
		    tmp >>= bits;
		}
	    } else {
		limbs.back() = tmp;
	    }
    } else {
	while(limbs.size() > 1) {
	    if(limbs.back() != 0) { break; }
	    limbs.pop_back();
	}    
    }
}

limb_t BigInt::getBits(limb_t lt, int bits) {
    return lt & ((1 << bits) - 1);
}

/**
* CONVERSIONS
*/

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
    limb_t oneBillion = 1000000000;
    //BigInt oneBillion("1000000000");
    //limb_t digits;
    BigInt digits;
    while(tmp != BigInt::ZERO) {
	std::stringstream ss;
	ss << std::setfill('0') << std::setw(9);
	BigInt::div(&tmp, tmp, oneBillion, &digits);
	if(digits.size() == 0) {
	    digits.limbs.push_back(0);
	}
	ss << digits.limbs[0];
	//ss << digits;
	std::string s = ss.str();
	std::reverse(s.begin(), s.end());
	ret += s;
    }

    std::reverse(ret.begin(), ret.end());
    
    if(ret.size() >1 && ret[0] == '0') {
	size_t pos = ret.find_first_not_of('0');
	ret.erase(0,pos);	    
    }


    return ret;
}

/*
* BASE OPERATORS
*/

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

BigInt BigInt::operator*(const limb_t& rhs) const{
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
    
    //Is it faster to have the carry and add separate, or would it be better to keep them together?

    //add all of the limbs together
    std::transform(rhs.limbs.begin(), rhs.limbs.end(), this->limbs.begin(), this->limbs.begin(), std::plus<limb_t>());

    //If the two limbs added together "overflow" by exceeding the base, then we have a carry value of 1
    //This cannot be greater than 1 as (2^31 -1)*2 = 2^32 -2 < 2^32 -1; and we are taking the 32nd bit.
    for(auto it = this->limbs.begin(); it < this->limbs.end() -1; ++it) {
	if(*it >= base) {
	    *it &= (base -1);	
	    *(it +1) += 1;
	}    
    }

    //If we overflowed the final limb, correct it and add a new limb
    if(this->limbs.back() >= base) {
	this->limbs.back() &= (base -1);
	this->limbs.push_back(1);	
    }

    return *this;
}

BigInt& BigInt::operator-=(const BigInt& rhs){
    if(this->negative != rhs.negative) {
	*this += -rhs;
	return *this;
    }

    if(rhs.size() > this->size()) {
	this->limbs.resize(rhs.size(), 0);
    }

    //Subtract, done in this order so that we don't need to create a temporary variable that is a copy
    //of rhs just so that we can increase its size
    std::transform(rhs.limbs.begin(), rhs.limbs.end(), this->limbs.begin(), this->limbs.begin(), [](limb_t a, limb_t b) {
	return b - a;
    });

    //Carry
    for(auto it = this->limbs.begin(); it < this->limbs.end() -1; ++it) {
	//If the number underflowed
	if(*it > this->base) {
	    *it += this->base;
	    *(it+1) -= 1;
	}
    }

    //If we need to carry from the top limb, "borrow" a carry and then subtract back.  
    if(this->limbs.back() > this->base) {
	this->limbs.back() += this->base;
	this->negative = !this->negative;
	auto subtract = [this](limb_t a) {
	    return this->base -1 - a;
	};
	std::transform(this->limbs.begin(), this->limbs.end(), this->limbs.begin(), subtract);
	this->limbs[0]++;
    }
    
    //Needed in case this->limb[0] = 0 and we had to borrow the carry above as this would result in a limb outside of acceptable ranges
    //this->reallign();
    return *this;    
}

BigInt& BigInt::operator*=(const BigInt& rhs){
  
  bool neg = this->negative != rhs.negative;
    if(this->size() < 20 || rhs.size() < 20 || this->size() + rhs.size() < 70) {
	*this = std::move(naiveMul(*this, rhs));
    } else {
    	*this = std::move(karatsuba(*this, rhs));
    }
    this->negative = neg;
    return *this;
}

BigInt& BigInt::operator*=(const limb_t& rhs) {
    if(*this == BigInt::ZERO || rhs == 0) {
	*this = BigInt::ZERO;
	return *this;
    }    
    if(rhs == 1) {
	return *this;
    }

    

    //Separate from the carry step to better vectorize the code, or would it be faster together?
    for(auto it = this->limbs.begin(); it < this->limbs.end(); ++it) {
	*it *= rhs;
    }

    limb_t carry = 0;
    for(auto it = this->limbs.begin(); it < this->limbs.end(); ++it) {
	*it += carry;
	carry = *it >> bits;
	*it &= (base -1);
    }
    if(carry) {
	this->limbs.push_back(carry);
    }
 
   return *this;
}

BigInt& BigInt::operator/=(const BigInt& rhs){
    if(rhs > BigInt::abs(*this)) {
	*this = BigInt::ZERO;
	return *this;
    }

    BigInt::div(this, *this, rhs, nullptr);

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
	//this->reallign();
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

bool BigInt::operator<=(const BigInt& rhs) const {
    return !(rhs < *this);
}

/**
* MISCELLANY
*/

BigInt BigInt::abs(const BigInt& lhs) const {
    BigInt tmp(lhs);
    tmp.negative = false;
    return tmp;
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

limb_t BigInt::log2(limb_t lt) const {
    if(lt == 0) return 0;
    limb_t ret = 0;
    asm("bsrq %1, %0" : "=r" (ret) : "r" (lt));
    return ret;
}

BigInt BigInt::log2(BigInt& num) const {
    BigInt ret = static_cast<limb_t>((num.size() -1) * bits);
    ret += log2(num.limbs[num.size() -1]);
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

    this->limbs.insert(this->limbs.begin(), i, 0);
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
    this->limbs = std::move(vt);
    return *this;
}

//Montogery's Ladder algorithm
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

/**
* MULTIPLICATION HELPER METHODS
*/

/**
* Karatsuba multiplication is taking two integers x = x_1*B^m + x_0, y = y_1*B^m +y_0
* where B is the radix of the system, in this case it is 2^(bits) -1. Therefore xy
* is of the form z_2*B^(2m) + z_1*B^m + z_0 
*/
BigInt BigInt::karatsuba(const BigInt& n1, const BigInt& n2) {
    if(n1.size() < 20 || n2.size() < 20 || n1.size() + n2.size() < 70) {
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
    
    //Same as 
    //z1 = karatsuba((n1l + n1h), (n2l + n2h));
    n1l += n1h;
    n2l += n2h;
    z1 = karatsuba(n1l, n2l);
    
    z1 -= z2;
    z1 -= z0;

    z2.lLimbShift(2*m);
    z2 += z1.lLimbShift(m);
    z2 += z0;

    //This is the same as, 
    //return (z2.lLimbShift(2*m)) + (z1.lLimbShift(m)) + (z0);
    //but removes the need for calling operator+()

    return z2;
}

//Performs a carry when a limb is no longer going to get written to, or after every 4 rows are used
BigInt BigInt::naiveMul(const BigInt& n1, const BigInt& n2) {
    if(n1 == BigInt::ZERO || n2 == BigInt::ZERO) {
	return BigInt::ZERO;
    }

    BigInt tmp;
    int m = n1.size() + n2.size();
    tmp.limbs.resize(m, 0);

    for(int j = 0; j < n2.size(); ++j) {
	tmp.limbs[j] = n1.limbs[0] * n2.limbs[j];
    }

    tmp.limbs[1] += tmp.limbs[0] >> bits;
    tmp.limbs[0] &= (base -1);


    //As each limb is only 31 bits long we can add up to 4 multiplications and a carry without a 64bit int overflowing
    for(int i = 1; i < n1.size(); i++) {
	limb_t carry = 0;
	for(int j = 0; j < n2.size(); j++) {
	    tmp.limbs[i+j] += n1.limbs[i] * n2.limbs[j];
	}
	tmp.limbs[i + 1] += tmp.limbs[i] >> bits;
	tmp.limbs[i] &= (base -1);
	if(i % 4 == 3) {
	    for(auto it = tmp.limbs.begin() + i + 1; it < tmp.limbs.begin() + i + n2.size(); ++it) {
		*(it + 1) += *it >> bits;
		*it &= (base - 1);		    	    
	    }	

	}    
    }

    for(auto it = tmp.limbs.end() - n2.size(); it < tmp.limbs.end() -1; ++it) {
	*(it +1) += *it >> bits;
	*it &= (base -1);
    }      

    while(tmp.size() > 1){
	if(tmp.limbs.back() != 0) { break; }
	tmp.limbs.pop_back();
    }
    return tmp;
}

/**
* LIMB MANIPULATION
*/

//Returns only the high limb (limbs[size() -1) in the BigInt
BigInt BigInt::highLimb() const{
    BigInt tmp;
    tmp.limbs.push_back(this->limbs.back());
    return tmp;
}

BigInt BigInt::highNLimbs(int n) const {
    if(n > this->size()) return *this;
    
    BigInt tmp;
    tmp.limbs.insert(tmp.limbs.begin(), this->limbs.end() - n, this->limbs.end());
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
    if(n > this->size()) return *this;

    BigInt tmp;
    tmp.limbs.insert(tmp.limbs.begin(), this->limbs.begin(), this->limbs.begin() + n);
    return tmp;
}

//Trust that the user supplied good data, gets the [start, end) limbs from *this
BigInt BigInt::getLimbsRange(int start, int end) const {
    BigInt tmp;
    tmp.limbs.insert(tmp.limbs.begin(), this->limbs.begin() + start, this->limbs.begin() + end);
    return tmp;
}

/**
* DIVISION HELPERS
*/

void BigInt::div(BigInt * dv, const BigInt& num, const limb_t& denom, BigInt * rem) const {
    if(num.size() == 1 && num.limbs[0] == denom) {
	if(rem != nullptr) {
	    //*rem = 0;
	    *rem = BigInt::ZERO;
	}
	if(dv != nullptr) {
	    *dv = BigInt::ONE;
	}
	return;
    }
    if(num.size() == 1 && num.limbs[0] < denom) {
	if(rem != nullptr) {
	   // *rem = num.limbs[0];
	    *rem = num;
	}
	if(dv != nullptr) {
	    *dv = BigInt::ZERO;
	}
	return;
    }

    BigInt acc = BigInt::ZERO;
    acc.limbs.resize(num.size(), 0);

    limb_t k = 0;
    for(int i = num.size() -1; i >= 0; --i) {
	acc.limbs[i] = (k * base + num.limbs[i]) / denom;
	k = (k * base + num.limbs[i]) - acc.limbs[i] * denom;
    }
    
    if(dv != nullptr) {
	while(acc.size() > 1) {
	    if(acc.limbs.back() != 0) { break; }	
	    acc.limbs.pop_back();
	}
    	*dv = acc;
    }   
    if(rem != nullptr) {
	rem->limbs.clear();
	rem->limbs.push_back(k);	
	//*rem = k;
    }
}

//There is no guarantee that this will work if dv, rem, or num share pointers.
void BigInt::div(BigInt * dv, const BigInt& num, const BigInt& denom, BigInt * rem) const {
    
    if(denom.size() == 1) {
	BigInt::div(dv, num, denom.limbs[0], rem);
	return;
    }

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
    d = (base / (v_1 + 1));
    //Normalize
    if(d == 1) {
	a.limbs.push_back(0);
    } else {
	a *= d;
	tmp *= d;
    }
    if(a.size() == tmp.size()) {
	a.limbs.push_back(0);
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
	    qhat = base -1;
	} else {
	    qhat = ((a.limbs[a.size() -1 - j] << bits) | a.limbs[a.size() -2 - j]) / v_1; 
	}
	if(tmp.size() >= 2 && a.size() >= 3) {
	    while(tmp.limbs[tmp.size() -2] * qhat > 
		((((a.limbs[a.size() -1 - j] << bits) | a.limbs[a.size() -2 -j]) - qhat * v_1) << bits) +  a.limbs[a.size() -3 - j]) {
		--qhat;
	    }
	}
	
   
	size_t first = -1 -j -tmp.size();
	
	//mult and sub. inlined to avoid making a copy of the limbs
	//Equivalent to uprime -= tmp * qhat;
	limb_t carry = 0;
	std::transform(tmp.limbs.begin(), tmp.limbs.end(), a.limbs.end() + first, a.limbs.end() + first, [&](limb_t a, limb_t b) {
	    limb_t k  = a * qhat;
	    k += carry;
	    carry = k >> bits;
	    k &= (base -1);
	    return b - k;
	});
	if(carry) {
	    a.limbs[a.size() - 1 -j] -= carry;
	}
	

	for(auto it = a.limbs.end() + first; it < a.limbs.end() -1 - j; ++it) {
	    //If the number underflowed
	    if(*it >= this->base) {
		 *it += this->base;
	         *(it+1) -= 1;
	    }
	 }

	//If we need to carry from the top limb, "borrow" a carry and then subtract back.  
	//Super special case, should happen about 2/base times
	if(a.limbs[a.size() -1 -j] > this->base) {
	    a.limbs[a.size() -1 -j] += this->base;
	    auto subtract = [&](limb_t a) {
		return base -1 - a;
	    };
	    //Make a the negative (2^(bits * n) compliment) of itself
	    std::transform(a.limbs.end() + first, a.limbs.end() -j, a.limbs.end() + first, subtract);
	    a.limbs[a.size() + first]++;
	    --qhat;
	    
	    //As a is now negative, a + b -> b - a
	    std::transform(tmp.limbs.begin(), tmp.limbs.end(), a.limbs.end() + first, a.limbs.end() + first, std::minus<limb_t>());
	    for(auto it = a.limbs.end() + first; it < a.limbs.end() -1 - j; ++it) {
		//If the number underflowed
		if(*it > this->base) {
		     *it += this->base;
		     *(it+1) -= 1;
		}
	     }
	}
	
	acc.limbs[m-j] = qhat;

	++j;
    } while (j <= m);

    acc.negative = false;

    if(dv != nullptr) {
	*dv = acc;
    }

    if(rem != nullptr) {
	while(a.limbs.size() > 1) {
	    if(a.limbs.back() != 0) { break; }
	    a.limbs.pop_back();
	}
	//a.reallign();
	if(a.size() > tmp.size() +1) {
	    std::cout << "failure in division" << std::endl;
	    a = a.getLimbsRange(0, tmp.size());
	}	

	if(d == 1) {
	    *rem = std::move(a);
	} else {
	    BigInt::div(&a, a, d, nullptr);
	    *rem = std::move(a);
	}		
    }
}
