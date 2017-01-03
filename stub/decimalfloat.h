#if __GNUC__ && ! __clang__ && ! __STRICT_ANSI__

#if __SIZEOF_FLOAT__ == 4
#define decifloat _Decimal32
#elif __SIZEOF_FLOAT__ == 8
#define decifloat _Decimal64
#elif __SIZEOF_FLOAT__ == 16
#define decifloat _Decimal128
#else
#define decifloat float
#endif

#if __SIZEOF_DOUBLE__ == 4
#define decidouble _Decimal32
#elif __SIZEOF_DOUBLE__ == 8
#define decidouble _Decimal64
#elif __SIZEOF_DOUBLE__ == 16
#define decidouble _Decimal128
#else
#define decidouble double
#endif

#else
#define decifloat float
#define decidouble double
#endif
