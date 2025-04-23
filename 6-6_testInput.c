// Vainstein K 2025 apr 22
// Test input program, only meant as input to preprocessor.
#undef STRING_A

static const char klotho  [SIZE_D];
#define SIZE_D 22
static const char lachesis[SIZE_D];
#define EXPR_F + (SIZE_D * 5)
long bar = 33 EXPR_F - 7;
#undef SIZE_D
long qux = 44 EXPR_F - 8;
static const char atropos [SIZE_D];

#define STRING_A "aaa bbb ccc"
const char *flarp = "xxx " STRING_A " yyy";
#define flarp       "xZz " STRING_A " yZy"
main(){
    printf("At first, %s\n", flarp);
#undef flarp
    printf("But then, %s\n", flarp);
}

#define  dyadicPOW(p,q) pow(p,q)
#define niladicPOW      pow(55.5,6)
const double first_pow = dyadicPOW(77.7,9);
const double other_pow = niladicPOW;
