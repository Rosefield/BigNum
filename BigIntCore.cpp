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

BigInt BigInt::operator++(int){
    BigInt tmp(*this);
    ++(*this);
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
    limb_t carry = 0;
    std::transform(rhs.limbs.begin(), rhs.limbs.end(), this->limbs.begin(), this->limbs.begin(), 
	//Inline carry and addition
	[&](limb_t a, limb_t b) {
	    a += b;
	    a += carry;
	    carry = a >> bits;;
	    a -= this->base * carry;
	    return a;
	});
    //have to have an extra case for if we still have a carry leftover after the initial add
    for(int i = rhs.size(); i < this->size(); ++i) {
	    if(!carry) { break; }
	    this->limbs[i] += carry;
	    carry = this->limbs[i] >> bits;
	    this->limbs[i] -= this->base * carry;
    }
    //If we overflowed the final limb, correct it and add a new limb
    if(carry) {
	this->limbs.push_back(carry);
    }

    return *this;
}

BigInt& BigInt::operator++() {
    *this += BigInt::ONE;
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
	//Trading (roughly) 1 memory write for 1 branch compared to 
	//if(*it >= this->base) { ... }
	//Given caching and the penalty for branch mispredict this seems to be more efficient
	int i = *it >= this->base;
	*it += this->base * i;
	*(it+1) -= i;
	
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
    if(this->size() < 20 || rhs.size() < 20 || this->size() + rhs.size() < 80) {
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

    limb_t carry = 0;
    for(auto it = this->limbs.begin(); it < this->limbs.end(); ++it) {
	*it *= rhs;
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

//Who needs constant-time operators anyways?
bool BigInt::operator==(const BigInt& rhs) const{
    if(this->size() != rhs.size()) return false;
    for(int i = this->size() -1; i >= 0; i--) {
	if (this->limbs[i] != rhs.limbs[i]) return false;	
    }

    return true;
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

limb_t BigInt::log2(const limb_t lt) const {
    if(lt == 0) return 0;
    limb_t ret = 0;
    asm("bsrq %1, %0" : "=r" (ret) : "r" (lt));
    return ret;
}

BigInt BigInt::log2(const BigInt& num) const {
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
//Currently horribly inefficient and it is more optimal to just use naiveMul even at 140 limbs
BigInt BigInt::karatsuba(const BigInt& n1, const BigInt& n2) {
    if(n1.size() < 20 || n2.size() < 20 || n1.size() + n2.size() < 80) {
	return naiveMul(n1, n2);
    }

    
    std::vector<limb_t> scratch(n1.size() + n2.size(), 0);

    karatsuba(n1.limbs, n2.limbs, scratch, 0, 0, n1.size(), 0, n2.size());     
    
    while(scratch.size() > 1){
	if(scratch.back() != 0) { break; }
	scratch.pop_back();
    }

    BigInt result;
    result.limbs = std::move(scratch);


    return result;
}

void BigInt::karatsuba(const std::vector<limb_t>& n1, const std::vector<limb_t>& n2, std::vector<limb_t>& scratch, 
			unsigned scratch_offset, unsigned n1l_offset, unsigned n1_size, 
			unsigned n2l_offset, unsigned n2_size ) {
    if(n1_size < 20 || n2_size < 20 || n1_size + n2_size < 80) {
	naiveMul(n1.begin() + n1l_offset, n2.begin() + n2l_offset, scratch.begin() + scratch_offset, n1_size, n2_size);
	return;
    }
     
    int m = n1_size > n2_size ? (n2_size + 1)/2 : (n1_size + 1)/2;

    //z0
    karatsuba(n1, n2, scratch, scratch_offset, n1l_offset, m, n2l_offset, m);

    //z2
    karatsuba(n1, n2, scratch, scratch_offset + 2*m, n1l_offset + m, n1_size -m, n2l_offset + m, n2_size -m);
    
    std::vector<limb_t> n1l(n1.begin() + n1l_offset, n1.begin() + n1l_offset + m);
    std::vector<limb_t> n2l(n2.begin() + n2l_offset, n2.begin() + n2l_offset + m);

    if(n1_size - m > m) {
	n1l.resize(n1_size - m, 0);
    }
    if(n2_size - m > m) {
	n2l.resize(n2_size - m, 0);
    }

    limb_t carry = 0;
    auto add_with_carry = [&](limb_t a, limb_t b) {
	    a += b;
	    a += carry;
	    carry = a >> bits;;
	    a -= this->base * carry;
	    return a; 
	};
    
    std::transform(n1.begin() + n1l_offset + m, n1.begin() + n1l_offset + n1_size, 
		    n1l.begin(), n1l.begin(), add_with_carry);
    if(carry) {
	if(n1_size - m == n1l.size()) {
	    n1l.push_back(carry);
	} else {
	    n1l[n1_size -m] += carry;
	    if(n1l[n1_size -m] >= this->base) {
		carry = n1l[n1_size -m] >> bits;
		n1l[n1_size -m] &= this->base -1;
		n1l.push_back(carry);
	    }
	}
    }

    carry = 0;
    
    std::transform(n2.begin() + n2l_offset + m, n2.begin() + n2l_offset + n2_size, 
		    n2l.begin(), n2l.begin(), add_with_carry);
    if(carry) {
	if(n2_size - m == n2l.size()) {
	    n2l.push_back(carry);
	} else {
	    n2l[n2_size -m] += carry;
	    if(n2l[n2_size -m] >= this->base) {
		carry = n2l[n2_size -m] >> bits;
		n2l[n2_size -m] &= this->base -1;
		n2l.push_back(carry);
	    }
	}
    }

    std::vector<limb_t> z1(n1l.size() + n2l.size(), 0);
    //z1
    karatsuba(n1l, n2l, z1, 0, 0, n1l.size(), 0, n2l.size());

    
    carry = 0;
    auto sub_with_carry = [&](limb_t a, limb_t b) {
	    limb_t c = b - a - carry;
	    carry = c >= this->base;
	    c += carry * this->base;
	    return c;
	};

    //z1 -= z2
    std::transform(scratch.begin() + scratch_offset, scratch.begin() + scratch_offset + 2*m, 
		    z1.begin(), z1.begin(), sub_with_carry);
    
    z1[2*m - 1 + carry] -= carry;
    
    carry = 0;
    //z1 -= z0
    std::transform(scratch.begin() + scratch_offset + 2*m, scratch.begin() + scratch_offset + n1_size + n2_size, 
		    z1.begin(), z1.begin(), sub_with_carry);
    
    z1[n1_size + n2_size - 2*m - 1 + carry] -= carry;
    
    carry = 0;

    //z0 * base^{2m} + z1 * base^m + z2
    std::transform(z1.begin(), z1.end(), scratch.begin() + scratch_offset + m,
		     scratch.begin() + scratch_offset + m, add_with_carry);
    if(carry) {
	scratch[scratch_offset + m + z1.size()] += carry;
	if(scratch[scratch_offset + m + z1.size()] >= this->base) {
	    std::cout << "with carry" << std::endl;
	}
    }

}

void BigInt::naiveMul(std::vector<limb_t>::const_iterator n1, std::vector<limb_t>::const_iterator n2, 
			std::vector<limb_t>::iterator scratch, unsigned n1_size, unsigned n2_size ){
    
    if(n1_size == n2_size && std::equal(n1, n1 + n1_size, n2)) {
	//HAC algorithm 14.16 for squaring
	//http://cacr.uwaterloo.ca/hac/about/chap14.pdf
	limb_t uv, c; //,u=0, v=0;
	for(int i = 0; i < n1_size; ++i) {
	    uv = *(scratch + i + i) + *(n1 + i) * *(n1 +i);
	    //u = uv >> bits;
	    //v = uv & (base -1);
	    c = uv >> bits;
	    *(scratch + i + i) = uv & (base-1);
	    for(int j = i+1; j< n1_size; ++j) {
	    	uv = *(scratch + i + j) + 2* *(n1 + i)* *(n1 + j) + c;
		//u = uv >> bits;
		//v = uv & (base -1);
		*(scratch + i + j) = uv & (base -1);
		c = uv >> bits;
	    }

	    *(scratch + i + n1_size) = c;
	}
	
	if(*(scratch + n1_size + n1_size -1) >= base) {
	    c = *(scratch + n1_size + n1_size -1) >> bits;
	    *(scratch + n1_size + n1_size -1) &= (base -1);
	    *(scratch + n1_size + n2_size) = c;
	}

	return;
    }




    for(int j = 0; j < n2_size; ++j) {
	*(scratch + j) = (*n1) * *(n2 + j);
    }

    *(scratch + 1) += *scratch >> bits;
    *scratch &= (base -1);

    /**
    * As each limb is only 31 bits long we can add up to 4 multiplications and a carry without a 64bit int overflowing
    * Unrolling loops, because I might as well find out if it is productive
    * Taken from running valgrind --tool=callgrind --cache-sim=yes --branch-sim=yes ./Test using test4kModExp()
    *		ir	 dr	    dw	     bc		bcm
    *normal	578m	110m	    60m	    34m		.9m
    *unrolled	404m	128.5m	    75.8m   36.9m	2.2m
    * Clearly the compiler is magic, but it seems that the unrolled version is more efficient and running the multiplication
    * in testMulRandom1024Bit() 100,000 times gives 
    * normal   Avg time: 4.86*10^-6
    * unrolled Avg time: 3.95*10^-6
    */
    
    int i = 1;
    for(; i + 4 < n1_size; ++i) {
	for(int j = 0; j < n2_size; j++) {
	    *(scratch + i+j) += *(n1 +i) * *(n2 + j);
	}
	*(scratch + i + 1) += *(scratch + i) >> bits;
	*(scratch + i) &= (base -1);
	++i;

	for(int j = 0; j < n2_size; j++) {
	    *(scratch + i + j) += *(n1 + i) * *(n2 + j);
	}
	*(scratch +i + 1) += *(scratch +i) >> bits;
	*(scratch +i) &= (base -1);
	++i;

	for(int j = 0; j < n2_size; j++) {
	    *(scratch + i + j) += *(n1 + i) * *(n2 +j);
	}
	*(scratch + i + 1) += *(scratch + i) >> bits;
	*(scratch +i) &= (base -1);
	
	//carry after every 4 mult-adds
	for(auto it = scratch + i + 1; it < scratch + i + n2_size; ++it) {
	    *(it + 1) += *it >> bits;
	    *it &= (base - 1);		    	    
	}	
	++i;

	for(int j = 0; j < n2_size; j++) {
	    *(scratch+i+j) += *(n1 +i) * *(n2 + j);
	}
	*(scratch + i + 1) += *(scratch + i) >> bits;
	*(scratch +i) &= (base -1);
    }

    for(; i < n1_size; ++i) {
	for(int j = 0; j < n2_size; j++) {
	    *(scratch + i + j) += *(n1 + i) * *(n2 + j);
	}
	*(scratch + i + 1) += *(scratch + i) >> bits;
	*(scratch +i) &= (base -1);
	if(i % 4 == 3) {
	    for(auto it = scratch + i + 1; it < scratch + i + n2_size; ++it) {
		*(it + 1) += *it >> bits;
		*it &= (base - 1);		    	    
	    }
	}
    }
        
    
    
    for(auto it = scratch + n1_size; it < scratch + n1_size + n2_size -1; ++it) {
	*(it +1) += *it >> bits;
	*it &= (base -1);
    } 
         
}

//Performs a carry when a limb is no longer going to get written to, or after every 4 rows are used
BigInt BigInt::naiveMul(const BigInt& n1, const BigInt& n2) {
    if(n1 == BigInt::ZERO || n2 == BigInt::ZERO) {
	return BigInt::ZERO;
    }

    BigInt tmp;
    int m = n1.size() + n2.size();
    tmp.limbs.resize(m, 0);


    if(n1 == n2) {
	//HAC algorithm 14.16 for squaring
	//http://cacr.uwaterloo.ca/hac/about/chap14.pdf
	limb_t uv, c; //,u=0, v=0;
	for(int i = 0; i < n1.size(); ++i) {
	    uv = tmp.limbs[i+i] + n1.limbs[i] * n1.limbs[i];
	    //u = uv >> bits;
	    //v = uv & (base -1);
	    c = uv >> bits;
	    tmp.limbs[i+i] = uv & (base-1);
	    for(int j = i+1; j< n1.size(); ++j) {
	    	uv = tmp.limbs[i+j] + 2*n1.limbs[i]*n1.limbs[j] + c;
		//u = uv >> bits;
		//v = uv & (base -1);
		tmp.limbs[i+j] = uv & (base -1);
		c = uv >> bits;
	    }

	    tmp.limbs[i + n1.size()] = c;
	}
	while(tmp.size() > 1){
	    if(tmp.limbs.back() != 0) { break; }
	    tmp.limbs.pop_back();
	}
	
	if(tmp.limbs.back() >= base) {
	    std::cout << "squared push_back needed" << std::endl;
	    c = tmp.limbs.back() >> bits;
	    tmp.limbs.back() &= (base -1);
	    tmp.limbs.push_back(c);
	}
	
	return tmp;
    } else {
    for(int j = 0; j < n2.size(); ++j) {
	tmp.limbs[j] = n1.limbs[0] * n2.limbs[j];
    }

    tmp.limbs[1] += tmp.limbs[0] >> bits;
    tmp.limbs[0] &= (base -1);

    /**
    * As each limb is only 31 bits long we can add up to 4 multiplications and a carry without a 64bit int overflowing
    * Unrolling loops, because I might as well find out if it is productive
    * Taken from running valgrind --tool=callgrind --cache-sim=yes --branch-sim=yes ./Test using test4kModExp()
    *		ir	 dr	    dw	     bc		bcm
    *normal	578m	110m	    60m	    34m		.9m
    *unrolled	404m	128.5m	    75.8m   36.9m	2.2m
    * Clearly the compiler is magic, but it seems that the unrolled version is more efficient and running the multiplication
    * in testMulRandom1024Bit() 100,000 times gives 
    * normal   Avg time: 4.86*10^-6
    * unrolled Avg time: 3.95*10^-6
    */
    /*
    for(int i = 1; i < n1.size(); i++) {
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
    */
    int i = 1;
    for(; i + 4 < n1.size(); ++i) {
	for(int j = 0; j < n2.size(); j++) {
	    tmp.limbs[i+j] += n1.limbs[i] * n2.limbs[j];
	}
	tmp.limbs[i + 1] += tmp.limbs[i] >> bits;
	tmp.limbs[i] &= (base -1);
	++i;

	for(int j = 0; j < n2.size(); j++) {
	    tmp.limbs[i+j] += n1.limbs[i] * n2.limbs[j];
	}
	tmp.limbs[i + 1] += tmp.limbs[i] >> bits;
	tmp.limbs[i] &= (base -1);
	++i;

	for(int j = 0; j < n2.size(); j++) {
	    tmp.limbs[i+j] += n1.limbs[i] * n2.limbs[j];
	}
	tmp.limbs[i + 1] += tmp.limbs[i] >> bits;
	tmp.limbs[i] &= (base -1);
	
	//carry after every 4 mult-adds
	for(auto it = tmp.limbs.begin() + i + 1; it < tmp.limbs.begin() + i + n2.size(); ++it) {
	    *(it + 1) += *it >> bits;
	    *it &= (base - 1);		    	    
	}	
	++i;

	for(int j = 0; j < n2.size(); j++) {
	    tmp.limbs[i+j] += n1.limbs[i] * n2.limbs[j];
	}
	tmp.limbs[i + 1] += tmp.limbs[i] >> bits;
	tmp.limbs[i] &= (base -1);
    }

    for(; i < n1.size(); ++i) {
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

    //If we require the result of the divison and not just the remainder, do it in place
    //If sufficient memory is already allocated at dv this saves about 50% of memory accesses, 
    //Otherwise, it is roughly equivalent (saves 0-0.5%) compared to creating additional memory
    
    if(dv != nullptr) {
	dv->limbs.resize(num.size(),0);
	limb_t k = 0;	
	for(int i = num.size() -1; i >= 0; --i) {
	    //Temporary limb in case dv is a pointer to num
	    limb_t t = num.limbs[i];
	    dv->limbs[i] = (k * base + t) / denom;
	    k = (k * base + t) - dv->limbs[i] * denom;
	}

	while(dv->size() > 1) {
	    if(dv->limbs.back() != 0) { break; }
	    dv->limbs.pop_back();
	}

	if(rem != nullptr) {
	    rem->limbs.clear();
	    rem->limbs.push_back(k);	
	    //*rem = k;
	}
	return;
    }
 
   if(rem != nullptr) {
	limb_t k = 0;
	for(int i = num.size() -1; i >= 0; --i) {
	    k = (k * base + num.limbs[i]) % denom;
	}
	rem->limbs.clear();
	rem->limbs.push_back(k);
    }
    


    /*
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
    }
    */   
}

//There is no guarantee that this will work if dv, rem, or num share pointers.
//This function assumes that basic checks have already occurred: if num / denom, num = denom, etc
void BigInt::div(BigInt * dv, const BigInt& num, const BigInt& denom, BigInt * rem) const {
    
    if(denom.size() == 1) {
	BigInt::div(dv, num, denom.limbs[0], rem);
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
	//This check calculates if the second digit of the division will be too small for the multiplication and thus
	//will underflow. With this check qhat is either correct, or 1 too large, and is accounted for in the add-back
	//stage below
	//if(a.size() >= 3) {
	    while(tmp.limbs[tmp.size() -2] * qhat > 
		((((a.limbs[a.size() -1 - j] << bits) | a.limbs[a.size() -2 -j]) - qhat * v_1) << bits) +  a.limbs[a.size() -3 - j]) {
		--qhat;
	    }
	//}
	
   
	size_t first = -1 -j -tmp.size();
	
	//mult and sub. inlined to avoid making a copy of the limbs
	//Equivalent to uprime -= tmp * qhat;
	limb_t carry = 0;
	std::transform(tmp.limbs.begin(), tmp.limbs.end(), a.limbs.end() + first, a.limbs.end() + first, 
	    //This lambda calculates with carries for subtraction and multiplication b -= a * qhat
	    [&](limb_t a, limb_t b) {
		limb_t k  = a * qhat;
		k += carry;
		carry = k >> bits;
		k &= (base -1);
		int i = k > b;
		k -= this->base * i;
		carry += i;
		return b - k;
	    });
	a.limbs[a.size() - 1 -j] -= carry;
	
/*
	for(auto it = a.limbs.end() + first; it < a.limbs.end() -1 - j; ++it) {
	    //If the number underflowed
	    //The same as if(*it >= this->base) { ... }, but removes the branching
	    int i = *it >= this->base;
	     *it += this->base * i;
	     *(it+1) -= i;
	 }
*/

	//If we need to carry from the top limb, "borrow" a carry and then subtract back.  
	//Super special add-back case, should happen about 2/base times
	if(a.limbs[a.size() -1 -j] >= this->base) {
	    a.limbs[a.size() -1 -j] += this->base;
	    auto subtract = [&](limb_t a) {
		return base -1 - a;
	    };
	    //Make a the negative (2^(bits * n) compliment) of itself
	    std::transform(a.limbs.end() + first, a.limbs.end() -j, a.limbs.end() + first, subtract);
	    a.limbs[a.size() + first]++;
	    --qhat;
	    
	    //As a is now negative, a + b -> b - a
	    std::transform(tmp.limbs.begin(), tmp.limbs.end(), a.limbs.end() + first, a.limbs.end() + first
		, std::minus<limb_t>());
	    for(auto it = a.limbs.end() + first; it < a.limbs.end() -1 - j; ++it) {
		//If the number underflowed
		int i = *it >= this->base;
		//if(*it > this->base) {
		     *it += this->base * i;
		     *(it+1) -= i;
		//}
	     }
	}
	
	acc.limbs[m-j] = qhat;

	++j;
    } while (j <= m);

    if(dv != nullptr) {
	*dv = acc;
    }

    if(rem != nullptr) {
	while(a.limbs.size() > 1) {
	    if(a.limbs.back() != 0) { break; }
	    a.limbs.pop_back();
	}

	//The remainder is the de normalized value of a; rem = a / d
	if(d == 1) {
	    *rem = std::move(a);
	} else {
	    BigInt::div(rem, a, d, nullptr);
	}		
    }
}


BigInt BigInt::genRandomBits(const BigInt& bits) const {
    //Static to seed and initialize only once
    static std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<limb_t> distribution(0,base-1);

    BigInt result;    

    //For the moment limit to 2^31 bits generated
    limb_t i = 0;
    for(; i < bits.limbs[0] - this->bits; i += this->bits) {
	result.limbs.push_back(distribution(generator));	
    }
    i = bits.limbs[0] - i;
    if(i != 0){
        std::uniform_int_distribution<limb_t> extra_distribution(0, (1 << i) -1);
	result.limbs.push_back(extra_distribution(generator));
    }

    return result;
}

BigInt BigInt::genRandomNum(const BigInt& high) const {
    BigInt bits = log2(high);
    BigInt result = genRandomBits(bits);
    //Result has at least a 50% chance of being lesser than high, and so the probability of generating
    //a number less than high converges to 1 exponentially quickly
    while(result >= high) {
	result = genRandomBits(bits);
    }
    return result;
}


BigInt BigInt::genRandomNum(const BigInt& low, const BigInt& high) const {
    BigInt diff = high - low;
    BigInt result = low;
    result += genRandomNum(diff);
    return result;
}


BigInt BigInt::genProbabalisticPrime(const BigInt& low, const BigInt& high) const {
    //Gen random number between low and high
    //Check if number is divisible by small primes
    //Run a few (~10) tests of Miller-Rabin
    //If miller-rabin return prime, return number, else gen new random number and repeat
    

    return BigInt::ZERO;
}


BigInt BigInt::genPrime(const BigInt& low,  const BigInt& high) const {

    return BigInt::ZERO;
}

