/*
 * @FilePath: /simtochat/test/src/test.cpp
 * @Author: CGL
 * @Date: 2021-05-13 20:39:02
 * @LastEditors: CGL
 * @LastEditTime: 2021-05-19 22:09:24
 * @Description: 
 */
#include "ThreadPool.h"

#include <iostream>
using namespace std;

int main()
{
    ThreadPool pool(4);
    std::vector< std::future<int> > results;

    for(int i = 0; i < 8; ++i) {
        results.emplace_back(
          pool.commit([i] {
            std::cout << "hello " << i << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(5));
            std::cout << "world " << i << std::endl;
            return i*i;
        })
      );
    }

    for(auto && result: results)
        std::cout << result.get() << ' ';
    std::cout << std::endl;

    return 0;
}