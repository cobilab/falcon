#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#include "models.h"
#include "defs.h"
#include "param.h"

typedef struct{
  uint64_t size;
  uint64_t bytes;
  }
INF;

typedef struct{
  uint8_t sym;
  uint8_t value;
  }
SymValue;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void        Fclose           (FILE *);
void        Fseeko           (FILE *, off_t, int32_t);
uint64_t    Ftello           (FILE *);
double      BoundDouble      (double, double, double);
double      BPBB             (double, uint64_t);
uint8_t     QuadQuantization (double);
uint8_t     PackByte         (double, uint8_t);
void        UnPackByte       (SymValue *, uint8_t);
FILE        *Fopen           (const char *, const char *);
uint64_t    SumWriteBits     (uint8_t **, int, uint64_t, FILE *, FILE *);
void        ShiftBuffer      (uint8_t *, int, uint8_t);
void        FillLogTable     (uint32_t, uint32_t, uint32_t);
double      SearchLog        (uint32_t );
double      Power            (double, double);
uint32_t    FLog2            (uint64_t );
char        *ReplaceSubStr   (char *, char *, char *);
int32_t     Strcasecmp       (char const *, char const *);
int         FileExists       (const char *);
uint8_t     DNASymToNum      (uint8_t  );
uint8_t     NumToDNASym      (uint8_t  );
uint8_t     GetCompNum       (uint8_t  );
uint8_t     GetCompSym       (uint8_t  );
uint64_t    NDNASyminFile    (FILE *);
uint64_t    NDNASymInFastq   (FILE *);
uint64_t    NDNASymInFasta   (FILE *);
uint64_t    NBytesInFile     (FILE *);
uint64_t    FopenBytesInFile (const char *);
uint8_t     *ReverseStr      (uint8_t *, uint32_t);
char        *CloneString     (char *   );
char        *RepString       (const char *, const char *, const char *);
uint32_t    ArgsNum          (uint32_t , char *[], uint32_t, char *, uint32_t,
                              uint32_t);
uint64_t    ArgsNum64        (uint64_t , char *[], uint32_t, char *, uint64_t,
                              uint64_t);
ModelPar    ArgsUniqModel    (char *, uint8_t);
ModelPar    ArgsModel        (uint32_t , char *[], uint32_t, char *);
double      ArgsDouble       (double, char *[], uint32_t, char *);
uint8_t     ArgsState        (uint8_t  , char *[], uint32_t, char *);
char        *ArgsString      (char    *, char *[], uint32_t, char *);
char        *ArgsFiles       (char *[], uint32_t, char *);
void        TestReadFile     (char *);
uint8_t     CmpCheckSum      (uint32_t, uint32_t);
void        FAccessWPerm     (char    *);
void        CalcProgress     (uint64_t , uint64_t);
uint32_t    ReadFNames       (Parameters *, char *);
void        PrintArgs        (Parameters *, Threads, char *, char *, uint32_t);
void        PrintArgsFilter  (EYEPARAM *);
void        PrintArgsInter   (Parameters *, Threads);
void        PrintArgsEye     (EYEPARAM *);
char        *concatenate     (char *, char *);
int32_t     StrToArgv        (char *, char ***);
char        *ArgsFilesImg    (char *[], uint32_t, char *);
char        *ArgsFile        (char *[], uint32_t, char *);
char        *ArgsFileGen     (char *[], uint32_t, char *, char *, char *);

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif
