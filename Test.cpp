#include "BigInt.h"
#include <chrono>

BigInt Fibonacci(int n) {
    BigInt fibs[3];

    fibs[0] = 1;
    fibs[1] = 1;
    int i;
    for(i = 2; i < n; ++i) {
	fibs[i % 3] = fibs[(i -1) % 3] + fibs[(i - 2) % 3];
	//std::cout << "fibs i: " << i << "  " << fibs[i%3] << std::endl;
    }
    return fibs[(i-1) % 3];
}

BigInt Factorial(int n) {
    BigInt result(1);

    result.limbs.reserve(200000);

    for(limb_t i = 2; i <= n; ++i) {
	result *= i;
    }
    
    return result;
}

bool testEquals(const BigInt& result, const std::vector<limb_t> actual) {
    if(result.size() != actual.size()) return false;
    
    for(int i = result.size() -1; i >= 0; i--) {
	if(result.limbs[i] != actual[result.size() - i -1]) return false;
    }
    return true;
}
void test500Fib() {
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> elapsed_time;
    start = std::chrono::system_clock::now();
    BigInt fib500 = Fibonacci(500);
    end = std::chrono::system_clock::now();
    elapsed_time = end - start;
#ifdef _PRINT_VALS
    std::cout<< "fib500 took: " << elapsed_time.count() << " computing " << fib500 << std::endl;
#endif
    std::vector<limb_t> actual{31, 268010877,1041980198,1505931748,939335682,475289477,
                   384828411,411953712,1000216971,1090861368,1011151204,315178285};
    std::cout << "Fib(500) Correct? " << testEquals(fib500, actual) << std::endl;   
}



void test500FibSub499Fib() {
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> elapsed_time;
    BigInt fib500 = Fibonacci(500);
    BigInt fib499 = Fibonacci(499);
    start = std::chrono::system_clock::now();
    BigInt subFibs = fib500 - fib499;
    end = std::chrono::system_clock::now();
    elapsed_time = end - start;
#ifdef _PRINT_VALS
    std::cout<< "500FibSub499Fib took: " << elapsed_time.count() << " computing " << subFibs << std::endl;
#endif
    BigInt actual = Fibonacci(498);
    std::cout << "Fib(500) - Fib(499) Correct? " << (subFibs == actual) << std::endl;   
}


void test20FactQuad() {
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> elapsed_time;
    start = std::chrono::system_clock::now();
    BigInt fact = Factorial(20);
    BigInt factSquared = fact * fact;
    factSquared *= factSquared;
    end = std::chrono::system_clock::now();
    elapsed_time = end - start;
#ifdef _PRINT_VALS
    std::cout<< "fact20quad took: " << elapsed_time.count() << " computing " << factSquared << std::endl;
#endif
    std::vector<limb_t> actual{166337208,764515097,443847783,29956932,1554513582,703611904,0,0};
    std::cout << "(20!)^4 Correct? " << testEquals(factSquared, actual) << std::endl;   
}

void test60Fact() {
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> elapsed_time;
    start = std::chrono::system_clock::now();
    BigInt fact60 = Factorial(60);
    end = std::chrono::system_clock::now();
    elapsed_time = end - start;
#ifdef _PRINT_VALS
    std::cout<< "fact60 took: " << elapsed_time.count() << " computing " << fact60 << std::endl;
#endif
    std::vector<limb_t> actual{18396530,1065373609,393476149,835766904,1949166040,1155626993,1901353954,234881024,0};
    std::cout << "60! Correct? " << testEquals(fact60, actual) << std::endl;   
}

void testLongStringConstructor() {
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> elapsed_time;
    start = std::chrono::system_clock::now();
    BigInt longString("12345678998765432101234567899876543210");
    end = std::chrono::system_clock::now();
    elapsed_time = end - start;
#ifdef _PRINT_VALS
    std::cout<< "longString took: " << elapsed_time.count() << " computing " << longString << std::endl;
#endif
    std::vector<limb_t> actual{1246595009,748116907,1035237483,1632488170};
    std::cout << "String Constructor Correct? " << testEquals(longString, actual) << std::endl;   
}

void testVeryLongStringConstructor() {
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> elapsed_time;
    start = std::chrono::system_clock::now();
    BigInt longString(
#include "bignum.txt"
    );
    end = std::chrono::system_clock::now();
    elapsed_time = end - start;
#ifdef _PRINT_VALS
    std::cout<< "VeryLongString took: " << elapsed_time.count() << " computing " << longString << std::endl;
#endif
    std::vector<limb_t> actual{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    std::cout << "Very-long String Constructor Correct? " << testEquals(longString, actual) << std::endl;   

}

void testMulRandom1024Bit() {
    BigInt a("276601373654928975871754674997064595262751078392398403"
	     "140078759572403299733275944955495590985079635677358326"
	     "437768288362804324337725761906816178599761686458110239"
	     "720130379865178259907677503176456269594835847095657438"
	     "537293270309725637889873193915190724261885472342077067"
	     "844166147815436649001795340162949998568");
    BigInt b("908270564241715128928475652095664817489270259211612103"
	     "998361943067766201208031918379136862415371024909615030"
	     "639734030078204866227763421426612690401640345569552459"
	     "305270873069670449962612360682955706072635452841457707"
	     "721202565127178715538792506240083189193164965778468706"
	     "43406500278810140967997440157799795150");
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> elapsed_time;
    start = std::chrono::system_clock::now();
    a *=  b;
    end = std::chrono::system_clock::now();
    elapsed_time = end - start;
#ifdef _PRINT_VALS
    std::cout<< "MulRandom1024Bit took: " << elapsed_time.count() << " computing " << a << std::endl;
#endif
    std::vector<limb_t> actual{3,235270659,568911404,922708634,1696526154,1010925488,110054269,
                            1158468645,1442079807,1577453968,299033823,519200492,1286835655,
                            1419899891,1067667472,1499562046,108550203,1558147072,1198215629,
                            78649644,329564816,225153618,1824298395,790487858,1994024926,1000868029,
                            1812712781,1156872405,1574232354,425287274,962686480,152563639,234031561,
                            964340897,896011126,672551734,473018745,1981102438,768108938,571079643,
                            276511614,1067268780,904046332,1878006326,943061062,675882021,1428995088,
                            41648230,344962290,897227716,95647102,355939536,379735193,346761280,
                            548236789,445511246,409511175,1229211920,1463486602,1827214088,1092666381,
                            1236026364,628671149,562427339,201968067,1990132260,823182512,};
    std::cout << "Mul random 1024bit numbers Correct? " << testEquals(a, actual) << std::endl;   

}

void test2Pow100() {
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> elapsed_time;
    start = std::chrono::system_clock::now();
    BigInt pow(2);
    pow = pow.pow(100);
    end = std::chrono::system_clock::now();
    elapsed_time = end - start;
#ifdef _PRINT_VALS
    std::cout<< "2Pow100 took: " << elapsed_time.count() << " computing " << pow << std::endl;
#endif
    std::vector<limb_t> actual{128,0,0,0};
    std::cout << "2^100 Correct? " << testEquals(pow, actual) << std::endl;   
}

void test2Pow1024(){
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> elapsed_time;
    start = std::chrono::system_clock::now();
    BigInt pow(2);
    pow = pow.pow(1024);
    end = std::chrono::system_clock::now();
    elapsed_time = end - start;
#ifdef _PRINT_VALS
    std::cout<< "2Pow1024 took: " << elapsed_time.count() << " computing " << pow << std::endl;
#endif
    std::vector<limb_t> actual{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    std::cout << "2^1024 Correct? " << testEquals(pow, actual) << std::endl;   
}

void test3Pow512() {
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> elapsed_time;
    start = std::chrono::system_clock::now();
    BigInt pow(3);
    pow = pow.pow(512);
    end = std::chrono::system_clock::now();
    elapsed_time = end - start;
#ifdef _PRINT_VALS
    std::cout<< "3Pow512 took: " << elapsed_time.count() << " computing " << pow << std::endl;
#endif
    std::vector<limb_t> actual{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    std::cout << "3^512 Correct? " << testEquals(pow, actual) << std::endl;   

}

void test10Pow50() {
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> elapsed_time;
    start = std::chrono::system_clock::now();
    BigInt pow(10);
    pow = pow.pow(50);
    end = std::chrono::system_clock::now();
    elapsed_time = end - start;
#ifdef _PRINT_VALS
    std::cout<< "10Pow50 took: " << elapsed_time.count() << " computing " << pow << std::endl;
#endif
    std::vector<limb_t> actual{2189,1135697817,322190998,269800438,1791492096,0};
    std::cout << "10^50 Correct? " << testEquals(pow, actual) << std::endl;   


}

void testDivRand512Bit() {
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> elapsed_time;
    //512 bits
    BigInt num("22479199231365811817124860008034"
               "86229160630675450396401816569207"
               "37104752501976414843519588079979"
               "01710354041585586289392397420347"
               "216513925045972181584884921");
    //500 bits
    BigInt denom("351508594074011603394279375147"
                 "626833956407011572250308540816"
                 "854541429976970595757637034702"
                 "593071101429245293222692961211"
                 "2896690197169368617519259819528");
    start = std::chrono::system_clock::now();
    end = std::chrono::system_clock::now();
    BigInt result = num / denom;
    elapsed_time = end - start;
#ifdef _PRINT_VALS
    std::cout<< "DivRand512Bit took: " << elapsed_time.count() << " computing " << result << std::endl;
#endif
    std::vector<limb_t> actual{6395};
    std::cout << "512b num / 500b num Correct? " << testEquals(result, actual) << std::endl;   
}

void test3Pow512Div2Pow512() {
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> elapsed_time;
    BigInt pow2(2);
    pow2 = pow2.pow(512);
    BigInt pow3(3);
    pow3 = pow3.pow(512);
    start = std::chrono::system_clock::now();
    BigInt pow = pow3 / pow2;
    end = std::chrono::system_clock::now();
    elapsed_time = end - start;
#ifdef _PRINT_VALS
    std::cout<< "3Pow512Div2Pow512 took: " << elapsed_time.count() << " computing " << pow << std::endl;
#endif
    std::vector<limb_t> actual{1483733,662018629,1236000823,1498554168,837056997,892058291,616689941,1251421047,1182766622,442154954};
    std::cout << "3^512 / 2^512 Correct? " << testEquals(pow, actual) << std::endl;   
}

void test2Pow31Pow2Pow10() {
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> elapsed_time;
    BigInt twoPow31(2);
    twoPow31 = twoPow31.pow(31);
    start = std::chrono::system_clock::now();
    twoPow31 = twoPow31.pow(1024);
    end = std::chrono::system_clock::now();
    elapsed_time = end - start;
#ifdef _PRINT_VALS
    std::cout<< "2Pow31Pow2Pow10 took: " << elapsed_time.count() << " computing " << twoPow31 << std::endl;
#endif
    std::vector<limb_t> actual{ 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    std::cout << "(2^31)^(2^10) Correct? " << testEquals(twoPow31, actual) << std::endl;   
}


void testDivAddBack() {
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> elapsed_time;
    BigInt num("12345678998765432112345678901111111111222222222");
    BigInt denom("12345678998765431212340871");
    start = std::chrono::system_clock::now();
    BigInt q = num / denom;
    end = std::chrono::system_clock::now();
    elapsed_time = end - start;
#ifdef _PRINT_VALS
    std::cout<< "testDivAddBack took: " << elapsed_time.count() << " computing " << q << std::endl;
#endif 
   std::vector<limb_t> actual{216,1804819339,1587616964};
    std::cout << "Division\'s 'add back' case Correct? " << testEquals(q, actual) << std::endl;   
}

void testRemainderlessMod() {
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> elapsed_time;
    BigInt num("10000000000000000000000000000000000");
    BigInt modulus("10000000000000000");
    start = std::chrono::system_clock::now();
    BigInt q = num % modulus;
    end = std::chrono::system_clock::now();
    elapsed_time = end - start;
#ifdef _PRINT_VALS
    std::cout<< "testRemainderlessMod took: " << elapsed_time.count() << " computing " << q << std::endl;
#endif
    std::vector<limb_t> actual{0};
    std::cout << "Remainderless mod Correct? " << testEquals(q, actual) << std::endl;   
}

void testLongToDecimal() {
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> elapsed_time;
    
    std::string actual = "22479199231365811817124860008034"
               "86229160630675450396401816569207"
               "37104752501976414843519588079979"
               "01710354041585586289392397420347"
               "216513925045972181584884921";
    BigInt t(actual);
    start = std::chrono::system_clock::now();
    std::string q = t.ToDecimal();
    end = std::chrono::system_clock::now();
    elapsed_time = end - start;
#ifdef _PRINT_VALS
    std::cout<< "testLongToDecimal took: " << elapsed_time.count() << " computing " << q << std::endl;
#endif
    std::cout << "LongToDecimal Correct? " << (actual == q) << std::endl;   
}

void testVeryLongToDecimal() {
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> elapsed_time;
    
    std::string actual = 
#include "bignum.txt"
    ;

    std::vector<limb_t> limbs{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 1};

    BigInt t;
    t.limbs = std::move(limbs);    
    start = std::chrono::system_clock::now();
    std::string q = t.ToDecimal();
    end = std::chrono::system_clock::now();
    elapsed_time = end - start;
#ifdef _PRINT_VALS
    std::cout<< "testVeryLongToDecimal took: " << elapsed_time.count() << " computing " << q << std::endl;
#endif
    std::cout << "VeryLongToDecimal Correct? " << (actual == q) << std::endl;   
}

void testSmallModExp() {
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> elapsed_time;
    BigInt base(3);
    BigInt exp(6);
    BigInt mod(7);
    start = std::chrono::system_clock::now();
    BigInt result = base.pow(exp, mod);
    end = std::chrono::system_clock::now();
    elapsed_time = end - start;
#ifdef _PRINT_VALS
    std::cout<< "testSmallExp took: " << elapsed_time.count() << " computing " << result << std::endl;
#endif
    std::vector<limb_t> actual{1};
    std::cout << "3^6 mod 7 Correct? " << testEquals(result, actual) << std::endl;   
}

void test512ModExp() {
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> elapsed_time;
    BigInt m(2);
    m = m.pow(128);
    BigInt e(65537);
    //Random ~256 bit primes
    BigInt p("90920301086832428064790445863602542431397528935205269974512244031053835934561");
    BigInt q("88093521957739528656999318948821526825072711349854666270556593711408857684143");
    //~512 bit semi-prime
    BigInt n(p * q);

    BigInt c;
    int trials = 1;
    start = std::chrono::system_clock::now();
    for(int i = 0; i < trials; ++i) {
	c = m.pow(e, n);
    }
    end = std::chrono::system_clock::now();
    elapsed_time = end - start;
#ifdef _PRINT_VALS
    std::cout<< "n x test512ModExp1 took: " << elapsed_time.count() << " computing " << c << std::endl;
    std::cout<< "average time: " << elapsed_time.count() / trials << std::endl;
#endif
    BigInt actual("2964838268779463116114800686128261984992461004875135797684366739503797344419"
		 "53324251806585497342796724135747651574033488424137969860812870453290786776434");

    std::cout << "512 RSA Encrypt Correct? " << (c == actual) << std::endl; 

    BigInt d("61209282410124760555153387834751911935998153976979367427081753749948289104979274"
	     "79612549027573377098818014420611341932265518557602531048137183493875578113");

    BigInt m2;
    start = std::chrono::system_clock::now();
    for(int i = 0; i < trials; ++i) {
	m2 = c.pow(d, n);
    }
    end = std::chrono::system_clock::now();
    elapsed_time = end - start;

#ifdef _PRINT_VALS
    std::cout<< "n x test512ModExp2 took: " << elapsed_time.count() << " computing " << m2 << std::endl;
    std::cout << "average time: " << elapsed_time.count() / trials << std::endl;
#endif
    std::cout << "512 RSA Decrypt Correct? " << (m2 == m) << std::endl; 
  
}

void test4kModExp() {
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> elapsed_time;
    BigInt b("871558235017954093423437590158590628576522773126904481253126523179267142125286304352393077866324795157339320786423508289272116085416842855143464712439539883373268917770245418623285499407304566889357091240261558139325550966132979571771152760731778603034571090771087902874492310806083403976257564499677737504843686996319014398421985793001884654550383345767471735087218095963179050837247884989892233067942652027725098958471864879606438936656962334323543076101671954058862860328543317921399082230338232862484564448378546659444704784255183546367969340136595753679260270626994921826376567619214497002400091509874421140539351536173538200523636050188074044357751591081129259500637405567077627224030842912078026058657709346269407387927199730818458602119680933899365805347577547201462016719347148212081753349895805222900775905600750438373314206521426696928758093654512428661667563567479440235549607734132197597567812557485989994268967748511073925331793468006167432154454105958231564674930279517873346761577068146084155316835656269346679395175430812912452315904939514343794398632582828540636618627387267851764594841893758368438625413929089459018144349231837651461800373972402629742544939709446461989984665472679153151410543687666115500808015226");
    BigInt e("562564654634773305724146920274948283092163015811199882994962952975295375648412241218837092106069604007901759357101569204271482173675200780578029240018226244645598370283024837749190912666306617470163431244209056319858025676795401403839796356023654843647682743240862215403163820946679492560196661751468676581889510275447215506184331955247209470171160957893742965200563411441393071483827476317831077940303026585913176209171204836516477870600603536242478446058428487609015999337299869603314357988259297487173872276129413501866707442175960405342579293643356094148801395355742235656708807699242197087678660887156243944384742065359866787377898296148698842649688872010866293768682679008988207193881647527961370863276688142496467731210650359694868770179349499486327715430885934322034555659571161478382595304739223823692049039519651780230181241756590956352622884166963846619137550753894739177333927611496044539561557371871372317210729880255293901915940825359614969548833666406560533385245517921857781041673061000467111263846716388987669240385559100872530627324990117919970652406458565526294685019067500313038307949276939525715506116110524118052275840416335673391672228170167589417289558658585366649286920613204807113398837956432539312413458680");
    BigInt m("861203906135959933341188274748390329818647305919417023682793011359095399307387752191840980020165279380268024866381407785182129063579597512708598009828770409037202682153884916214365830206435728038654314634846835245278789747188801216929507728727035718857494125340697324384806796989207804927049408400121765549348033974793213917633696633477456250889460791732857912926798901524540296705073237401687624175733051514442057160510806997009956572140469328787146375855595614028181082609324666577209615858518020036130431194558053251557501023947933085015180778012434343735086815584737627130869643198535557483264868365560316597825959966422501818549028849459616340481586461366752627903005336899095076956668306403948935069803634896581554084927747145767239413284844457658418893524786042305509530206804760021433658901625683480114374133890789323154291307943046160902472412078557157337707645542632353750069388237499944796039022732088832928432093633046870841057278161989215187170296897620535908336295634478969083348208876258813483495090079559481317862937873503395145073518926726065117610318934607343903699218608367225869725366445865889459999105045782301769686540545135085455952466770226081899511030077438198213476776462946663200357706811392454681984547942");
    

    start = std::chrono::system_clock::now();
    BigInt c = b.pow(e, m);
    end = std::chrono::system_clock::now();
    elapsed_time = end - start;
#ifdef _PRINT_VALS
    std::cout<< "test4kModExp took: " << elapsed_time.count() << " computing " << c << std::endl;
#endif
    BigInt actual("692377383377186198558765832231811893812963278174446701824751087292959555903035678344944798139197340579257816418637092356423998742062917491300274634525188712268704487078495335978301011601369665818608970577698257379553692898686510591888274984576240039636835645954422326743766553569212425175750583018776669808144958630116383384682221957442812389616818580038822863456649702345725646353474906762362658989567955572039728622802721771227410008670097717472728643285811538726062610187045635193176821253240887222297592422428316667017511334867176023035904817455136364044816921331773989562916974046032270099542188726770352569126628238736967063618488770624142151524367149521575893843314467514501700632440086284548201472879175107176880622196005393361112494184741006454702017723616261171634243180867461015832316772653064590105540501448094849348222011148968628832529872602927078503935466905096457208792106076984197457606939397319248667791200193049234251782975868180329607930757391238199962128009664361834243894189657007011312508003867323832499257665552895508691807142481110049871583659222384320271614245096353518238642233226383215746672651843533233441364534606076014623780619323325942623309910423302186322063317299739003032418138457989026063170276910");
  
    std::cout << "4k ModExp Correct? " << (actual == c) << std::endl;
}

int main() {

/*
    //Addition Tests
    test500Fib();
/**/

/*
    //Subtraction Tests
    test500FibSub499Fib();
/**/

/*
    //Multiplication Tests
    test20FactQuad();
    test60Fact();
    testLongStringConstructor();   
    testVeryLongStringConstructor(); 
    testMulRandom1024Bit();
/**/

/*
    //Power Tests
    test2Pow100();
    test2Pow1024();
    test10Pow50();
    test2Pow31Pow2Pow10();
/**/

/*
    //Division Tests
    testDivRand512Bit();       
    test3Pow512Div2Pow512();
    testDivAddBack();
 
    testRemainderlessMod();
    testLongToDecimal();
    testVeryLongToDecimal();
/**/


    //Modexp Tests
//    testSmallModExp();
//    test512ModExp();
    test4kModExp();
/**/


}

