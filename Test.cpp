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

    for(int i = 2; i <= n; ++i) {
	//std::cout << "result, i: " << result << " " << i << std::endl;
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
    std::cout<< "fib500 took: " << elapsed_time.count() << " computing " << fib500 << std::endl;
    std::vector<limb_t> actual{31, 268010877,1041980198,1505931748,939335682,475289477,
                   384828411,411953712,1000216971,1090861368,1011151204,315178285};
    std::cout << "Correct? " << testEquals(fib500, actual) << std::endl;   
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
    std::cout<< "500FibSub499Fib took: " << elapsed_time.count() << " computing " << subFibs << std::endl;
    /*
    std::vector<limb_t> actual{31, 268010877,1041980198,1505931748,939335682,475289477,
                   384828411,411953712,1000216971,1090861368,1011151204,315178285};
    */
    BigInt actual = Fibonacci(498);
    std::cout << "Correct? " << (subFibs == actual) << std::endl;   

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
    std::cout<< "fact20quad took: " << elapsed_time.count() << " computing " << factSquared << std::endl;
    std::vector<limb_t> actual{166337208,764515097,443847783,29956932,1554513582,703611904,0,0};
    std::cout << "Correct? " << testEquals(factSquared, actual) << std::endl;   
}

void test60Fact() {
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> elapsed_time;
    start = std::chrono::system_clock::now();
    BigInt fact60 = Factorial(60);
    end = std::chrono::system_clock::now();
    elapsed_time = end - start;
    std::cout<< "fact60 took: " << elapsed_time.count() << " computing " << fact60 << std::endl;
    std::vector<limb_t> actual{18396530,1065373609,393476149,835766904,1949166040,1155626993,1901353954,234881024,0};
    std::cout << "Correct? " << testEquals(fact60, actual) << std::endl;   
}

void testLongStringConstructor() {
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> elapsed_time;
    start = std::chrono::system_clock::now();
    BigInt longString("12345678998765432101234567899876543210");
    end = std::chrono::system_clock::now();
    elapsed_time = end - start;
    std::cout<< "longString took: " << elapsed_time.count() << " computing " << longString << std::endl;
    std::vector<limb_t> actual{1246595009,748116907,1035237483,1632488170};
    std::cout << "Correct? " << testEquals(longString, actual) << std::endl;   
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
    std::cout<< "MulRandom1024Bit took: " << elapsed_time.count() << " computing " << a << std::endl;
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
    std::cout << "Correct? " << testEquals(a, actual) << std::endl;   

}

void test2Pow100() {
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> elapsed_time;
    start = std::chrono::system_clock::now();
    BigInt pow(2);
    pow = pow.pow(100);
    end = std::chrono::system_clock::now();
    elapsed_time = end - start;
    std::cout<< "2Pow100 took: " << elapsed_time.count() << " computing " << pow << std::endl;
    std::vector<limb_t> actual{128,0,0,0};
    std::cout << "Correct? " << testEquals(pow, actual) << std::endl;   
}

void test2Pow1024(){
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> elapsed_time;
    start = std::chrono::system_clock::now();
    BigInt pow(2);
    pow = pow.pow(1024);
    end = std::chrono::system_clock::now();
    elapsed_time = end - start;
    std::cout<< "2Pow1024 took: " << elapsed_time.count() << " computing " << pow << std::endl;
    std::vector<limb_t> actual{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    std::cout << "Correct? " << testEquals(pow, actual) << std::endl;   
}

void test3Pow512() {
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> elapsed_time;
    start = std::chrono::system_clock::now();
    BigInt pow(3);
    pow = pow.pow(512);
    end = std::chrono::system_clock::now();
    elapsed_time = end - start;
    std::cout<< "3Pow512 took: " << elapsed_time.count() << " computing " << pow << std::endl;
    std::vector<limb_t> actual{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    std::cout << "Correct? " << testEquals(pow, actual) << std::endl;   

}

void test10Pow50() {
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> elapsed_time;
    start = std::chrono::system_clock::now();
    BigInt pow(10);
    pow = pow.pow(50);
    end = std::chrono::system_clock::now();
    elapsed_time = end - start;
    std::cout<< "10Pow50 took: " << elapsed_time.count() << " computing " << pow << std::endl;
    std::vector<limb_t> actual{2189,1135697817,322190998,269800438,1791492096,0};
    std::cout << "Correct? " << testEquals(pow, actual) << std::endl;   


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
    std::cout<< "DivRand512Bit took: " << elapsed_time.count() << " computing " << result << std::endl;
    std::vector<limb_t> actual{6395};
    std::cout << "Correct? " << testEquals(result, actual) << std::endl;   
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
    std::cout<< "3Pow512Div2Pow512 took: " << elapsed_time.count() << " computing " << pow << std::endl;
    std::vector<limb_t> actual{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    std::cout << "Correct? " << testEquals(pow, actual) << std::endl;   

}


int main() {

    //Addition Tests
    test500Fib();

    //Subtraction Tests
    test500FibSub499Fib();
    


    //Multiplication Tests
    test20FactQuad();
    test60Fact();
    testLongStringConstructor();   
    testMulRandom1024Bit();

    //Power Tests
    test2Pow100();
    test2Pow1024();
    test10Pow50();
    
    //Division Tests
    testDivRand512Bit();    
    test3Pow512Div2Pow512();
}

