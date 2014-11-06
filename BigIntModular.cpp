#include "BigInt.h"

//TODO: Implement mod_add, mod_sub, mod_inv, mod_mul, mod_sqr
//Should they be self assigning: i.e. mod_add : *this += add; *this %= mod instead of tmp = this; tmp += add; ...
//Currently everything is very naive and is equivalent to just doing the operation followed by a
//Modular reduction. This can all be improved in the future.

BigInt BigInt::mod_add(BigInt& add, BigInt& mod) const {
    BigInt tmp(*this);
    tmp += add;
    tmp %= mod;
    return tmp;
}

BigInt BigInt::mod_sub(BigInt& sub, BigInt& mod) const { 
    BigInt tmp(*this);
    tmp -= sub;
    tmp %= mod;
    return tmp;
}
BigInt BigInt::mod_mul(BigInt& mul, BigInt& mod) const {
    BigInt tmp(*this);
    tmp *= mul;
    tmp %= mod;
    return tmp;
}

/*
* Will return the modular inverse a^-1 of a mod m if gcd(a, m) == 1, otherwise will return BigInt::ZERO
* Slightly modified version of the extended euclidean algorithm since we don't care what the multiplier
* on the modulus is.
*/
BigInt BigInt::mod_inv(BigInt& mod) const { 
    BigInt t = BigInt::ZERO, newt = BigInt::ONE;
    BigInt r = mod, newr = *this;

    while(newr != BigInt::ZERO) {
	BigInt quotient = r / newr;
	BigInt tmp = t;
	t = newt;
	newt = tmp - (quotient * newt);

	tmp = r;
	r = newr;
	newr = (tmp - quotient * newr);	
    }

    if(r > BigInt::ONE) {
	return BigInt::ZERO;
    }
    if(t < BigInt::ZERO) {
	t += mod;
    }
    return t;
}

BigInt BigInt::mod_sqr(BigInt& mod) const { 
    BigInt tmp(*this);
    tmp *= tmp;
    tmp %= mod;
    return tmp;
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
	BigInt log = log2(exp);
	if(log >= 2048) {
	    return modexp_sliding_window(base, exp, mod, 7);	
	} else if(log >= 1024) {
	    return modexp_sliding_window(base, exp, mod, 6);
	} else if(log >= 384) {
	    //Defaults to k=5, but be explicit here to show similarities to the above call
	    return modexp_sliding_window(base, exp, mod, 5);
	} else {
	    return modexp_montgomery(base, exp, mod);
	}
    }
    return BigInt::ZERO;   
}


/**
* Partitions the exponent into variable-length zero words, and constant length non-zero words to minimize 
* the number of multiplications required compared to an m-ary multiplication.
*/
BigInt BigInt::modexp_sliding_window(BigInt& base, BigInt& exp, BigInt& mod, int k) const {

    limb_t m = 1 << k;

    //Temporarily use as static k
    std::vector<BigInt> xs(m/2, BigInt::ZERO);

    BigInt squared = (base * base) % mod;
    xs[0] = base;
    xs[0] %= mod;
    for(int i = 1; i < m/2; ++i) {
	xs[i] = xs[i-1] * squared;
	xs[i] %= mod;
    }

    std::string bits = exp.ToBinary();

    std::vector<std::string> windows;

    auto window_it = bits.rbegin();
    for(; window_it + k < bits.rend(); ) {
	if(*window_it == '1') {
	    std::string window(window_it, window_it + k);
	    std::reverse(window.begin(), window.end());
	    windows.push_back(window);
	    window_it += k;
	} else {
	    int i = 0;
	    while(*(window_it + i) == '0') {
		++i;
	    }
	    windows.push_back(std::string(window_it, window_it + i));	    
	    window_it += i;
	}
    }    
    if(window_it < bits.rend()) {
	std::string window(window_it, bits.rend());
	window.resize(k, 0);
	std::reverse(window.begin(), window.end());
	windows.push_back(window);
    }

    BigInt result = BigInt::ONE;
    
    limb_t index = stoll(*(windows.rbegin()), nullptr,2);
    if(index) {
	result = xs[index/2];
    }

    //The windows are stored starting from the lsb of the exponential, start from
    //the top and work down
    for(auto it = windows.rbegin() + 1; it < windows.rend(); ++it) {
	for(int i = 0; i < it->length(); ++i) {
	    result = result.mod_sqr(mod);
	}

	limb_t index = stoll(*it, nullptr, 2);
	if(index) {
	    result = result.mod_mul(xs[index/2], mod);
	}
    }

    return result;
}

BigInt BigInt::modexp_montgomery(BigInt& base, BigInt& exp, BigInt& mod) const {
    
    BigInt t1(base % mod);
    BigInt t2((t1 * t1) % mod);

    std::string bits = exp.ToBinary();
    size_t hi = bits.find_first_of('1');
    
    for(auto it = bits.begin() + hi +1; it < bits.end(); it++) {
	if(*it == '1') {
	    t1 = t1.mod_mul(t2, mod);
	    t2 = t2.mod_sqr(mod);
	} else {
	    t2 = t2.mod_mul(t1, mod);
	    t1 = t1.mod_sqr(mod);
	}
    }
    return t1;
}
