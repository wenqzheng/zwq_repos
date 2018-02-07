#include "utility/thread_pool_block.hpp"
#include <iostream>
#include <thread>
#include <unistd.h>
#include <stdio.h>
#include <atomic>
int main()
{
    thread_pool thrp;
    std::atomic<int> r(0);
    std::thread thd1([&]{
	for (int i = 0; i < 10; ++i) {
	    auto thdId = std::this_thread::get_id();
	    thrp.submit([&]{
		//std::cout << "thread 1 ID: " << thdId << std::endl;
		++r;
		printf("%s\t%d\n","thread 1 ID",thdId);
	    });
	}
    });

    std::thread thd2([&]{
	for (int i = 0; i < 10; ++i) {
	    auto thdId = std::this_thread::get_id();
	    thrp.submit([&]{
		//std::cout << "thread 2 ID: " << thdId << std::endl; 
		++r;
		printf("%s\t%d\n","thread 2 ID",thdId);
	    });
	}		    
    });

    sleep(2);
    std::cout << "r = " << r << std::endl;
    for (int i = 0; i < std::thread::hardware_concurrency(); ++i)
	thrp.submit([]{pthread_exit(0);});
    thd1.join();
    thd2.join();
}

