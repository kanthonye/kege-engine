//
//  bench.h
//  KE-GE
//
//  Created by Kenneth Esdaile on 11/25/25.
//

#ifndef bench_h
#define bench_h

#define BENCH(name, code)           \
    do {                            \
        auto begin = Clock::now();  \
        {                           \
            code                    \
        }                           \
        auto end = Clock::now();    \
        double ms = std::chrono::duration<double, std::milli>(end - begin).count(); \
        std::cout << name << ": " << ms << " ms\n"; \
    } while (0)

#endif /* bench_h */
