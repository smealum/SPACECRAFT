#ifndef MT19937_H
#define MT19937_H

/* initializes mt[NN] with a seed */
void init_genrand64(unsigned long long seed);
/* initialize by an array with array-length */
/* init_key is the array for initializing keys */
/* key_length is its length */
void init_by_array64(unsigned long long init_key[], unsigned long long key_length);
/* generates a random number on [0, 2^64-1]-interval */
unsigned long long genrand64_int64(void);
/* generates a random number on [0, 2^63-1]-interval */
long long genrand64_int63(void);
/* generates a random number on [0,1]-real-interval */
double genrand64_real1(void);
/* generates a random number on [0,1)-real-interval */
double genrand64_real2(void);
/* generates a random number on (0,1)-real-interval */
double genrand64_real3(void);

#endif
