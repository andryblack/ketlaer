#ifndef LIBBITAP_H
#define LIBBITAP_H
#ifdef __cplusplus
extern "C" {
#endif 

typedef struct {
  int l, n;            /* l is the number of states. n is the number of */
                       /* 'or'ing operations stored. */
  int *s, *x;          /* s shows which letter is allowed in which place. */
                           /* x determines in which areas exact matches are */
  struct bitapJumpType *j; /* required. */
  int approxMode;          /* Only used during parsing. To determine x */
} bitapType;

int NewBitap (bitapType *b, const unsigned char *regex);
const unsigned char *FindWithBitap (bitapType *b, const unsigned char *in, int inl, int e, int *ereturn,
		     const unsigned char **bReturn);
void DeleteBitap (bitapType *b);

#ifdef __cplusplus
}
#endif 
#endif
