/*
 *   This file was automatically generated by version 1.7 of cextract.
 *   Manual editing not recommended.
 *
 *   Created: Wed Aug 30 10:10:08 2006
 */

#ifdef __cplusplus
extern "C" {
#endif

PBoolean pu_encode_genom_double ( const char *name, Expression *tc, double *val_addr, int size );
PBoolean pu_encode_genom_float ( const char *name, Expression *tc, float *val_addr, int size );
PBoolean pu_encode_genom_long_long_int ( const char *name, Expression *tc, long long int *val_addr, int size );
PBoolean pu_encode_genom_unsigned_long_long_int ( const char *name, Expression *tc, unsigned long long int *val_addr, int size );
PBoolean pu_encode_genom_int ( const char *name, Expression *tc, int *val_addr, int size );
PBoolean pu_encode_genom_unsigned_int ( const char *name, Expression *tc, unsigned int *val_addr, int size );
PBoolean pu_encode_genom_short ( const char *name, Expression *tc, short *val_addr, int size );
PBoolean pu_encode_genom_short_int ( const char *name, Expression *tc, short int *val_addr, int size );
PBoolean pu_encode_genom_unsigned_short_int ( const char *name, Expression *tc, unsigned short int *val_addr, int size );
PBoolean pu_encode_genom_long_int ( const char *name, Expression *tc, long int *val_addr, int size );
PBoolean pu_encode_genom_unsigned_long_int ( const char *name, Expression *tc, unsigned long int *val_addr, int size );
PBoolean pu_encode_genom_char ( const char *name, Expression *tc, char *val_addr, int size );
PBoolean pu_encode_genom_unsigned_char ( const char *name, Expression *tc, unsigned char *val_addr, int size );
PBoolean pu_encode_genom_addr ( const char *name, Expression *tc, void * *val_addr, int size );
PBoolean pu_encode_genom_string ( const char *name, Expression *tc, char *val_addr, int size, int max_size );
Term *pu_decode_genom_int ( const char *name, int *addr, int size );
Term *pu_decode_genom_long_long_int ( const char *name, long long int *addr, int size );
Term *pu_decode_genom_unsigned_long_long_int ( const char *name, unsigned long long int *addr, int size );
Term *pu_decode_genom_unsigned_int ( const char *name, unsigned int *addr, int size );
Term *pu_decode_genom_unsigned_char ( const char *name, unsigned char *addr, int size );
Term *pu_decode_genom_short ( const char *name, short *addr, int size );
Term *pu_decode_genom_short_int ( const char *name, short int *addr, int size );
Term *pu_decode_genom_unsigned_short_int ( const char *name, unsigned short int *addr, int size );
Term *pu_decode_genom_long_int ( const char *name, long int *addr, int size );
Term *pu_decode_genom_unsigned_long_int ( const char *name, unsigned long int *addr, int size );
Term *pu_decode_genom_addr ( const char *name, void * *addr, int size );
Term *pu_decode_genom_char ( const char *name, char *addr, int size );
Term *pu_decode_genom_float ( const char *name, float *addr, int size );
Term *pu_decode_genom_double ( const char *name, double *addr, int size );
Term *pu_decode_genom_string ( const char *name, const char *addr, int size, int max_size );

PBoolean pu_check_ttc_name(Expression *tc, const char *name, const char *type_name);
PBoolean pu_check_ttc_name_alter(Expression *tc, const char *name);
PBoolean pu_check_ttc_name_strict(Expression *tc, const char *name);
PBoolean pu_check_ttc_symbol_alter(Expression *tc, Symbol name);
PBoolean pu_check_ttc_symbol_strict(Expression *tc, Symbol name);

PBoolean pu_encode_genom3_double ( const char *name, Expression *tc, double *val_addr);
PBoolean pu_encode_genom3_float ( const char *name, Expression *tc, float *val_addr);
PBoolean pu_encode_genom3_long_long_int ( const char *name, Expression *tc, long long int *val_addr);
PBoolean pu_encode_genom3_unsigned_long_long_int ( const char *name, Expression *tc, unsigned long long int *val_addr);
PBoolean pu_encode_genom3_int ( const char *name, Expression *tc, int *val_addr);
PBoolean pu_encode_genom3_unsigned_int ( const char *name, Expression *tc, unsigned int *val_addr);
PBoolean pu_encode_genom3_short ( const char *name, Expression *tc, short *val_addr);
PBoolean pu_encode_genom3_short_int ( const char *name, Expression *tc, short int *val_addr);
PBoolean pu_encode_genom3_unsigned_short_int ( const char *name, Expression *tc, unsigned short int *val_addr);
PBoolean pu_encode_genom3_long_int ( const char *name, Expression *tc, long int *val_addr);
PBoolean pu_encode_genom3_unsigned_long_int ( const char *name, Expression *tc, unsigned long int *val_addr);
PBoolean pu_encode_genom3_char ( const char *name, Expression *tc, char *val_addr);
PBoolean pu_encode_genom3_unsigned_char ( const char *name, Expression *tc, unsigned char *val_addr);
PBoolean pu_encode_genom3_addr ( const char *name, Expression *tc, void * *val_addr);

Term *pu_decode_genom3_int ( const char *name, int *addr);
Term *pu_decode_genom3_long_long_int ( const char *name, long long int *addr);
Term *pu_decode_genom3_unsigned_long_long_int ( const char *name, unsigned long long int *addr);
Term *pu_decode_genom3_unsigned_int ( const char *name, unsigned int *addr);
Term *pu_decode_genom3_unsigned_char ( const char *name, unsigned char *addr);
Term *pu_decode_genom3_short ( const char *name, short *addr);
Term *pu_decode_genom3_short_int ( const char *name, short int *addr);
Term *pu_decode_genom3_unsigned_short_int ( const char *name, unsigned short int *addr);
Term *pu_decode_genom3_long_int ( const char *name, long int *addr);
Term *pu_decode_genom3_unsigned_long_int ( const char *name, unsigned long int *addr);
Term *pu_decode_genom3_addr ( const char *name, void * *addr);
Term *pu_decode_genom3_char ( const char *name, char *addr);
Term *pu_decode_genom3_float ( const char *name, float *addr);
Term *pu_decode_genom3_double ( const char *name, double *addr);


Term *pu_simple_decode_int ( const char *name, int addr );
Term *pu_simple_decode_long_long_int ( const char *name, long long int addr );
Term *pu_simple_decode_unsigned_long_long_int ( const char *name, unsigned long long int addr );
Term *pu_simple_decode_unsigned_int ( const char *name, unsigned int addr );
Term *pu_simple_decode_unsigned_char ( const char *name, unsigned char addr );
Term *pu_simple_decode_short ( const char *name, short addr );
Term *pu_simple_decode_short_int ( const char *name, short int addr );
Term *pu_simple_decode_unsigned_short_int ( const char *name, unsigned short int addr );
Term *pu_simple_decode_long_int ( const char *name, long int addr );
Term *pu_simple_decode_unsigned_long_int ( const char *name, unsigned long int addr );
Term *pu_simple_decode_addr ( const char *name, void *addr );
Term *pu_simple_decode_char ( const char *name, char addr );
Term *pu_simple_decode_float ( const char *name, float addr );
Term *pu_simple_decode_double ( const char *name, double addr );
Term *pu_simple_decode_string ( const char *name, const char *addr );
Term *pu_simple_decode_atom(const char *key, const char *value);

#ifdef __cplusplus
}
#endif
