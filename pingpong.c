#include <stdio.h>
#include <unistd.h>
#include <time.h>

int main() {
    int p2c[2], c2p[2];  
    pipe(p2c);           
    pipe(c2p);           

    pid_t pid = fork();
    const int N = 1000000;
    int x;

    if (pid == 0) {
        close(p2c[1]);  
        close(c2p[0]);  

        for (int i = 0; i < N; i++) {
            read(p2c[0], &x, sizeof(x));    
            write(c2p[1], &x, sizeof(x));  
        }

        close(p2c[0]);
        close(c2p[1]);
    } else {
        close(p2c[0]); 
        close(c2p[1]); 

        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);

        for (int i = 0; i < N; i++) {
            write(p2c[1], &i, sizeof(i));  
            read(c2p[0], &x, sizeof(x));   
        }

        clock_gettime(CLOCK_MONOTONIC, &end);

        close(p2c[1]);
        close(c2p[0]);

        double elapsed = (end.tv_sec - start.tv_sec)
                       + (end.tv_nsec - start.tv_nsec) / 1e9;

        printf("Completed %d round trips in %.3f seconds\n", N, elapsed);
        printf("Requests per second: %.2f\n", N / elapsed);
        printf("Average round trip: %.2f ns\n", (elapsed * 1e9) / N);
        printf("Average one-way:    %.2f ns\n", (elapsed * 1e9) / (2.0 * N));
    }
    return 0;
}
