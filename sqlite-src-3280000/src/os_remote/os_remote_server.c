
#define RPC_RUN 0
#define LOCAL_TEST 1

#if LOCAL_TEST
#include<iostream>

int main() {
    return 0;
}

#endif

#if  RPC_RUN

int main()
{
    return 0;
}


#endif