// stb_c_lexer.h - v0.12 - public domain Sean Barrett 2013
// lexer for making little C-like languages with recursive-descent parsers
//
// This file provides both the interface and the implementation.
// To instantiate the implementation,
//      #define STB_C_LEXER_IMPLEMENTATION
// in *ONE* source file, before #including this file.
//
// The default configuration is fairly close to a C lexer, although
// suffixes on integer constants are not handled (you can override this).
//
// History:
//     0.12 fix compilation bug for NUL support; better support separate inclusion
//     0.11 fix clang static analysis warning
//     0.10 fix warnings
//     0.09 hex floats, no-stdlib fixes
//     0.08 fix bad pointer comparison
//     0.07 fix mishandling of hexadecimal constants parsed by strtol
//     0.06 fix missing next character after ending quote mark (Andreas Fredriksson)
//     0.05 refixed get_location because github version had lost the fix
//     0.04 fix octal parsing bug
//     0.03 added STB_C_LEX_DISCARD_PREPROCESSOR option
//          refactor API to simplify (only one struct instead of two)
//          change literal enum names to have 'lit' at the end
//     0.02 first public release
//
// Status:
//     - haven't tested compiling as C++
//     - haven't tested the float parsing path
//     - haven't tested the non-default-config paths (e.g. non-stdlib)
//     - only tested default-config paths by eyeballing output of self-parse
//
//     - haven't implemented multiline strings
//     - haven't implemented octal/hex character constants
//     - haven't implemented support for unicode CLEX_char
//     - need to expand error reporting so you don't just get "CLEX_parse_error"
//
// Contributors:
//   Arpad Goretity (bugfix)
//   Alan Hickman (hex floats)
//   github:mundusnine (bugfix)
//
// LICENSE
//
//   See end of file for license information.
#define STB_C_LEXER_IMPLEMENTATION
#ifdef STB_C_LEXER_IMPLEMENTATION
#ifndef STB_C_LEXER_DEFINITIONS
// to change the default parsing rules, copy the following lines
// into your C/C++ file *before* including this, and then replace
// the Y's with N's for the ones you don't want. This needs to be
// set to the same values for every place in your program where
// stb_c_lexer.h is included.
// --BEGIN--

#if defined(Y) || defined(N)
#error "Can only use stb_c_lexer in contexts where the preprocessor symbols 'Y' and 'N' are not defined"
#endif

#define STB_C_LEX_C_DECIMAL_INTS    Y   //  "0|[1-9][0-9]*"                        CLEX_intlit
#define STB_C_LEX_C_HEX_INTS        Y   //  "0x[0-9a-fA-F]+"                       CLEX_intlit
#define STB_C_LEX_C_OCTAL_INTS      Y   //  "[0-7]+"                               CLEX_intlit
#define STB_C_LEX_C_DECIMAL_FLOATS  Y   //  "[0-9]*(.[0-9]*([eE][-+]?[0-9]+)?)     CLEX_floatlit
#define STB_C_LEX_C99_HEX_FLOATS    N   //  "0x{hex}+(.{hex}*)?[pP][-+]?{hex}+     CLEX_floatlit
#define STB_C_LEX_C_IDENTIFIERS     Y   //  "[_a-zA-Z][_a-zA-Z0-9]*"               CLEX_id
#define STB_C_LEX_C_DQ_STRINGS      Y   //  double-quote-delimited strings with escapes  CLEX_dqstring
#define STB_C_LEX_C_SQ_STRINGS      N   //  single-quote-delimited strings with escapes  CLEX_ssstring
#define STB_C_LEX_C_CHARS           Y   //  single-quote-delimited character with escape CLEX_charlits
#define STB_C_LEX_C_COMMENTS        Y   //  "/* comment */"
#define STB_C_LEX_CPP_COMMENTS      Y   //  "// comment to end of line\n"
#define STB_C_LEX_C_COMPARISONS     Y   //  "==" CLEX_eq  "!=" CLEX_noteq   "<=" CLEX_lesseq  ">=" CLEX_greatereq
#define STB_C_LEX_C_LOGICAL         Y   //  "&&"  CLEX_andand   "||"  CLEX_oror
#define STB_C_LEX_C_SHIFTS          Y   //  "<<"  CLEX_shl      ">>"  CLEX_shr
#define STB_C_LEX_C_INCREMENTS      Y   //  "++"  CLEX_plusplus "--"  CLEX_minusminus
#define STB_C_LEX_C_ARROW           Y   //  "->"  CLEX_arrow
#define STB_C_LEX_EQUAL_ARROW       N   //  "=>"  CLEX_eqarrow
#define STB_C_LEX_C_BITWISEEQ       Y   //  "&="  CLEX_andeq    "|="  CLEX_oreq     "^="  CLEX_xoreq
#define STB_C_LEX_C_ARITHEQ         Y   //  "+="  CLEX_pluseq   "-="  CLEX_minuseq
                                        //  "*="  CLEX_muleq    "/="  CLEX_diveq    "%=" CLEX_modeq
                                        //  if both STB_C_LEX_SHIFTS & STB_C_LEX_ARITHEQ:
                                        //                      "<<=" CLEX_shleq    ">>=" CLEX_shreq

#define STB_C_LEX_PARSE_SUFFIXES    N   // letters after numbers are parsed as part of those numbers, and must be in suffix list below
#define STB_C_LEX_DECIMAL_SUFFIXES  ""  // decimal integer suffixes e.g. "uUlL" -- these are returned as-is in string storage
#define STB_C_LEX_HEX_SUFFIXES      ""  // e.g. "uUlL"
#define STB_C_LEX_OCTAL_SUFFIXES    ""  // e.g. "uUlL"
#define STB_C_LEX_FLOAT_SUFFIXES    ""  //

#define STB_C_LEX_0_IS_EOF             N  // if Y, ends parsing at '\0'; if N, returns '\0' as token
#define STB_C_LEX_INTEGERS_AS_DOUBLES  N  // parses integers as doubles so they can be larger than 'int', but only if STB_C_LEX_STDLIB==N
#define STB_C_LEX_MULTILINE_DSTRINGS   N  // allow newlines in double-quoted strings
#define STB_C_LEX_MULTILINE_SSTRINGS   N  // allow newlines in single-quoted strings
#define STB_C_LEX_USE_STDLIB           Y  // use strtod,strtol for parsing #s; otherwise inaccurate hack
#define STB_C_LEX_DOLLAR_IDENTIFIER    Y  // allow $ as an identifier character
#define STB_C_LEX_FLOAT_NO_DECIMAL     Y  // allow floats that have no decimal point if they have an exponent

#define STB_C_LEX_DEFINE_ALL_TOKEN_NAMES  N   // if Y, all CLEX_ token names are defined, even if never returned
                                              // leaving it as N should help you catch config bugs

#define STB_C_LEX_DISCARD_PREPROCESSOR    Y   // discard C-preprocessor directives (e.g. after prepocess
                                              // still have #line, #pragma, etc)

//#define STB_C_LEX_ISWHITE(str)    ... // return length in bytes of whitespace characters if first char is whitespace

#define STB_C_LEXER_DEFINITIONS         // This line prevents the header file from replacing your definitions
// --END--
#endif
#endif

#ifndef INCLUDE_STB_C_LEXER_H
#define INCLUDE_STB_C_LEXER_H

typedef struct
{
   // lexer variables
   char *InputStream;
   char *Eof;
   char *ParsePoint;
   char *StringStorage;
   int   StringStorageLen;

   // lexer parse location for error messages
   char *WhereFirstchar;
   char *WhereLastchar;

   // lexer token variables
   long Token;
   double RealNumber;
   long   IntNumber;
   char *String;
   int StringLen;
} stb_lexer;

typedef struct
{
   int LineNumber;
   int LineOffset;
} stb_lex_location;

#ifdef __cplusplus
extern "C" {
#endif

extern void stbCLexerInit(stb_lexer *Lexer, const char *InputStream, const char *InputStreamEnd, char *StringStore, int StoreLength);
// this function initialize the 'lexer' structure
//   Input:
//   - input_stream points to the file to parse, loaded into memory
//   - input_stream_end points to the end of the file, or NULL if you use 0-for-EOF
//   - string_store is storage the lexer can use for storing parsed strings and identifiers
//   - store_length is the length of that storage

extern int stbCLexerGetToken(stb_lexer *Lexer);
// this function returns non-zero if a token is parsed, or 0 if at EOF
//   Output:
//   - lexer->token is the token ID, which is unicode code point for a single-char token, < 0 for a multichar or eof or error
//   - lexer->real_number is a double constant value for CLEX_floatlit, or CLEX_intlit if STB_C_LEX_INTEGERS_AS_DOUBLES
//   - lexer->int_number is an integer constant for CLEX_intlit if !STB_C_LEX_INTEGERS_AS_DOUBLES, or character for CLEX_charlit
//   - lexer->string is a 0-terminated string for CLEX_dqstring or CLEX_sqstring or CLEX_identifier
//   - lexer->string_len is the byte length of lexer->string

extern void stbCLexerGetLocation(const stb_lexer *Lexer, const char *Where, stb_lex_location *Loc);
// this inefficient function returns the line number and character offset of a
// given location in the file as returned by stb_lex_token. Because it's inefficient,
// you should only call it for errors, not for every token.
// For error messages of invalid tokens, you typically want the location of the start
// of the token (which caused the token to be invalid). For bugs involving legit
// tokens, you can report the first or the range.
//    Output:
//    - loc->line_number is the line number in the file, counting from 1, of the location
//    - loc->line_offset is the char-offset in the line, counting from 0, of the location


#ifdef __cplusplus
}
#endif

enum
{
   CLEX_eof = 256,
   CLEX_parse_error,
   CLEX_intlit        ,
   CLEX_floatlit      ,
   CLEX_id            ,
   CLEX_dqstring      ,
   CLEX_sqstring      ,
   CLEX_charlit       ,
   CLEX_eq            ,
   CLEX_noteq         ,
   CLEX_lesseq        ,
   CLEX_greatereq     ,
   CLEX_andand        ,
   CLEX_oror          ,
   CLEX_shl           ,
   CLEX_shr           ,
   CLEX_plusplus      ,
   CLEX_minusminus    ,
   CLEX_pluseq        ,
   CLEX_minuseq       ,
   CLEX_muleq         ,
   CLEX_diveq         ,
   CLEX_modeq         ,
   CLEX_andeq         ,
   CLEX_oreq          ,
   CLEX_xoreq         ,
   CLEX_arrow         ,
   CLEX_eqarrow       ,
   CLEX_shleq, CLEX_shreq,

   CLEX_first_unused_token

};
#endif // INCLUDE_STB_C_LEXER_H

#ifdef STB_C_LEXER_IMPLEMENTATION

// Hacky definitions so we can easily #if on them
#define Y(x) 1
#define N(x) 0

#if STB_C_LEX_INTEGERS_AS_DOUBLES(x)
typedef double     stb__clex_int;
#define intfield   real_number
#define STB__clex_int_as_double
#else
typedef long       stb__clex_int;
#define intfield   int_number
#endif

// Convert these config options to simple conditional #defines so we can more
// easily test them once we've change the meaning of Y/N

#if STB_C_LEX_PARSE_SUFFIXES(x)
#define STB__clex_parse_suffixes
#endif

#if STB_C_LEX_C99_HEX_FLOATS(x)
#define STB__clex_hex_floats
#endif

#if STB_C_LEX_C_HEX_INTS(x)
#define STB__clex_hex_ints
#endif

#if STB_C_LEX_C_DECIMAL_INTS(x)
#define STB__clex_decimal_ints
#endif

#if STB_C_LEX_C_OCTAL_INTS(x)
#define STB__clex_octal_ints
#endif

#if STB_C_LEX_C_DECIMAL_FLOATS(x)
#define STB__clex_decimal_floats
#endif

#if STB_C_LEX_DISCARD_PREPROCESSOR(x)
#define STB__clex_discard_preprocessor
#endif

#if STB_C_LEX_USE_STDLIB(x) && (!defined(STB__clex_hex_floats) || __STDC_VERSION__ >= 199901L)
#define STB__CLEX_use_stdlib
#include <stdlib.h>
#endif

// Now for the rest of the file we'll use the basic definition where
// where Y expands to its contents and N expands to nothing
#undef  Y
#define Y(a) a
#undef N
#define N(a)

// API function
void stbCLexerInit(stb_lexer *Lexer, const char *InputStream, const char *InputStreamEnd, char *StringStore, int StoreLength)
{
   Lexer->InputStream = (char *) InputStream;
   Lexer->Eof = (char *) InputStreamEnd;
   Lexer->ParsePoint = (char *) InputStream;
   Lexer->StringStorage = StringStore;
   Lexer->StringStorageLen = StoreLength;
}

// API function
void stbCLexerGetLocation(const stb_lexer *Lexer, const char *Where, stb_lex_location *Loc)
{
   char *P = Lexer->InputStream;
   int LineNumber = 1;
   int CharOffset = 0;
   while (*P && P < Where) {
      if (*P == '\n' || *P == '\r') {
         P += (P[0]+P[1] == '\r'+'\n' ? 2 : 1); // skip newline
         LineNumber += 1;
         CharOffset = 0;
      } else {
         ++P;
         ++CharOffset;
      }
   }
   Loc->LineNumber = LineNumber;
   Loc->LineOffset = CharOffset;
}

// main helper function for returning a parsed token
static int stbClexToken(stb_lexer *Lexer, int Token, char *Start, char *End)
{
   Lexer->Token = Token;
   Lexer->WhereFirstchar = Start;
   Lexer->WhereLastchar = End;
   Lexer->ParsePoint = End+1;
   return 1;
}

// helper function for returning eof
static int stbClexEof(stb_lexer *Lexer)
{
   Lexer->Token = CLEX_eof;
   return 0;
}

static int stbClexIswhite(int X)
{
   return X == ' ' || X == '\t' || X == '\r' || X == '\n' || X == '\f';
}

static const char *stbStrchr(const char *Str, int Ch)
{
   for (; *Str; ++Str)
      if (*Str == Ch)
         return Str;
   return 0;
}

// parse suffixes at the end of a number
static int stbClexParseSuffixes(stb_lexer *Lexer, long Tokenid, char *Start, char *Cur, const char *Suffixes)
{
   #ifdef STB__clex_parse_suffixes
   lexer->string = lexer->string_storage;
   lexer->string_len = 0;

   while ((*cur >= 'a' && *cur <= 'z') || (*cur >= 'A' && *cur <= 'Z')) {
      if (stb__strchr(suffixes, *cur) == 0)
         return stb__clex_token(lexer, CLEX_parse_error, start, cur);
      if (lexer->string_len+1 >= lexer->string_storage_len)
         return stb__clex_token(lexer, CLEX_parse_error, start, cur);
      lexer->string[lexer->string_len++] = *cur++;
   }
   #else
   Suffixes = Suffixes; // attempt to suppress warnings
   #endif
   return stbClexToken(Lexer, Tokenid, Start, Cur-1);
}

#ifndef STB__CLEX_use_stdlib
static double stb__clex_pow(double base, unsigned int exponent)
{
   double value=1;
   for ( ; exponent; exponent >>= 1) {
      if (exponent & 1)
         value *= base;
      base *= base;
   }
   return value;
}

static double stb__clex_parse_float(char *p, char **q)
{
   char *s = p;
   double value=0;
   int base=10;
   int exponent=0;

#ifdef STB__clex_hex_floats
   if (*p == '0') {
      if (p[1] == 'x' || p[1] == 'X') {
         base=16;
         p += 2;
      }
   }
#endif

   for (;;) {
      if (*p >= '0' && *p <= '9')
         value = value*base + (*p++ - '0');
#ifdef STB__clex_hex_floats
      else if (base == 16 && *p >= 'a' && *p <= 'f')
         value = value*base + 10 + (*p++ - 'a');
      else if (base == 16 && *p >= 'A' && *p <= 'F')
         value = value*base + 10 + (*p++ - 'A');
#endif
      else
         break;
   }

   if (*p == '.') {
      double pow, addend = 0;
      ++p;
      for (pow=1; ; pow*=base) {
         if (*p >= '0' && *p <= '9')
            addend = addend*base + (*p++ - '0');
#ifdef STB__clex_hex_floats
         else if (base == 16 && *p >= 'a' && *p <= 'f')
            addend = addend*base + 10 + (*p++ - 'a');
         else if (base == 16 && *p >= 'A' && *p <= 'F')
            addend = addend*base + 10 + (*p++ - 'A');
#endif
         else
            break;
      }
      value += addend / pow;
   }
#ifdef STB__clex_hex_floats
   if (base == 16) {
      // exponent required for hex float literal
      if (*p != 'p' && *p != 'P') {
         *q = s;
         return 0;
      }
      exponent = 1;
   } else
#endif
      exponent = (*p == 'e' || *p == 'E');

   if (exponent) {
      int sign = p[1] == '-';
      unsigned int exponent=0;
      double power=1;
      ++p;
      if (*p == '-' || *p == '+')
         ++p;
      while (*p >= '0' && *p <= '9')
         exponent = exponent*10 + (*p++ - '0');

#ifdef STB__clex_hex_floats
      if (base == 16)
         power = stb__clex_pow(2, exponent);
      else
#endif
         power = stb__clex_pow(10, exponent);
      if (sign)
         value /= power;
      else
         value *= power;
   }
   *q = p;
   return value;
}
#endif

static int stbClexParseChar(char *P, char **Q)
{
   if (*P == '\\') {
      *Q = P+2; // tentatively guess we'll parse two characters
      switch(P[1]) {
         case '\\': return '\\';
         case '\'': return '\'';
         case '"': return '"';
         case 't': return '\t';
         case 'f': return '\f';
         case 'n': return '\n';
         case 'r': return '\r';
         case '0': return '\0'; // @TODO ocatal constants
         case 'x': case 'X': return -1; // @TODO hex constants
         case 'u': return -1; // @TODO unicode constants
      }
   }
   *Q = P+1;
   return (unsigned char) *P;
}

static int stbClexParseString(stb_lexer *Lexer, char *P, int Type)
{
   char *Start = P;
   char Delim = *P++; // grab the " or ' for later matching
   char *Out = Lexer->StringStorage;
   char *Outend = Lexer->StringStorage + Lexer->StringStorageLen;
   while (*P != Delim) {
      int K;
      if (*P == '\\') {
         char *Q;
         K = stbClexParseChar(P, &Q);
         if (K < 0)
            return stbClexToken(Lexer, CLEX_parse_error, Start, Q);
         P = Q;
      } else {
         // @OPTIMIZE: could speed this up by looping-while-not-backslash
         K = (unsigned char) *P++;
      }
      if (Out+1 > Outend)
         return stbClexToken(Lexer, CLEX_parse_error, Start, P);
      // @TODO expand unicode escapes to UTF8
      *Out++ = (char) K;
   }
   *Out = 0;
   Lexer->String = Lexer->StringStorage;
   Lexer->StringLen = (int) (Out - Lexer->StringStorage);
   return stbClexToken(Lexer, Type, Start, P);
}

int stbCLexerGetToken(stb_lexer *Lexer)
{
   char *P = Lexer->ParsePoint;

   // skip whitespace and comments
   for (;;) {
      #ifdef STB_C_LEX_ISWHITE
      while (p != lexer->stream_end) {
         int n;
         n = STB_C_LEX_ISWHITE(p);
         if (n == 0) break;
         if (lexer->eof && lexer->eof - lexer->parse_point < n)
            return stb__clex_token(tok, CLEX_parse_error, p,lexer->eof-1);
         p += n;
      }
      #else
      while (P != Lexer->Eof && stbClexIswhite(*P))
         ++P;
      #endif

      STB_C_LEX_CPP_COMMENTS(
         if (P != Lexer->Eof && P[0] == '/' && P[1] == '/') {
            while (P != Lexer->Eof && *P != '\r' && *P != '\n')
               ++P;
            continue;
         }
      )

      STB_C_LEX_C_COMMENTS(
         if (P != Lexer->Eof && P[0] == '/' && P[1] == '*') {
            char *Start = P;
            P += 2;
            while (P != Lexer->Eof && (P[0] != '*' || P[1] != '/'))
               ++P;
            if (P == Lexer->Eof)
               return stbClexToken(Lexer, CLEX_parse_error, Start, P-1);
            P += 2;
            continue;
         }
      )

      #ifdef STB__clex_discard_preprocessor
         // @TODO this discards everything after a '#', regardless
         // of where in the line the # is, rather than requiring it
         // be at the start. (because this parser doesn't otherwise
         // check for line breaks!)
         if (P != Lexer->Eof && P[0] == '#') {
            while (P != Lexer->Eof && *P != '\r' && *P != '\n')
               ++P;
            continue;
         }
      #endif

      break;
   }

   if (P == Lexer->Eof)
      return stbClexEof(Lexer);

   switch (*P) {
      default:
         if (   (*P >= 'a' && *P <= 'z')
             || (*P >= 'A' && *P <= 'Z')
             || *P == '_' || (unsigned char) *P >= 128    // >= 128 is UTF8 char
             STB_C_LEX_DOLLAR_IDENTIFIER( || *P == '$' ) )
         {
            int K = 0;
            Lexer->String = Lexer->StringStorage;
            do {
               if (K+1 >= Lexer->StringStorageLen)
                  return stbClexToken(Lexer, CLEX_parse_error, P, P+K);
               Lexer->String[K] = P[K];
               ++K;
            } while (
                  (P[K] >= 'a' && P[K] <= 'z')
               || (P[K] >= 'A' && P[K] <= 'Z')
               || (P[K] >= '0' && P[K] <= '9') // allow digits in middle of identifier
               || P[K] == '_' || (unsigned char) P[K] >= 128
                STB_C_LEX_DOLLAR_IDENTIFIER( || P[K] == '$' )
            );
            Lexer->String[K] = 0;
            Lexer->StringLen = K;
            return stbClexToken(Lexer, CLEX_id, P, P+K-1);
         }

         // check for EOF
         STB_C_LEX_0_IS_EOF(
            if (*p == 0)
               return stb__clex_eof(lexer);
         )

      single_char:
         // not an identifier, return the character as itself
         return stbClexToken(Lexer, *P, P, P);

      case '+':
         if (P+1 != Lexer->Eof) {
            STB_C_LEX_C_INCREMENTS(if (P[1] == '+') return stbClexToken(Lexer, CLEX_plusplus, P,P+1);)
            STB_C_LEX_C_ARITHEQ(   if (P[1] == '=') return stbClexToken(Lexer, CLEX_pluseq  , P,P+1);)
         }
         goto single_char;
      case '-':
         if (P+1 != Lexer->Eof) {
            STB_C_LEX_C_INCREMENTS(if (P[1] == '-') return stbClexToken(Lexer, CLEX_minusminus, P,P+1);)
            STB_C_LEX_C_ARITHEQ(   if (P[1] == '=') return stbClexToken(Lexer, CLEX_minuseq   , P,P+1);)
            STB_C_LEX_C_ARROW(     if (P[1] == '>') return stbClexToken(Lexer, CLEX_arrow     , P,P+1);)
         }
         goto single_char;
      case '&':
         if (P+1 != Lexer->Eof) {
            STB_C_LEX_C_LOGICAL(  if (P[1] == '&') return stbClexToken(Lexer, CLEX_andand, P,P+1);)
            STB_C_LEX_C_BITWISEEQ(if (P[1] == '=') return stbClexToken(Lexer, CLEX_andeq , P,P+1);)
         }
         goto single_char;
      case '|':
         if (P+1 != Lexer->Eof) {
            STB_C_LEX_C_LOGICAL(  if (P[1] == '|') return stbClexToken(Lexer, CLEX_oror, P,P+1);)
            STB_C_LEX_C_BITWISEEQ(if (P[1] == '=') return stbClexToken(Lexer, CLEX_oreq, P,P+1);)
         }
         goto single_char;
      case '=':
         if (P+1 != Lexer->Eof) {
            STB_C_LEX_C_COMPARISONS(if (P[1] == '=') return stbClexToken(Lexer, CLEX_eq, P,P+1);)
            STB_C_LEX_EQUAL_ARROW(  if (p[1] == '>') return stb__clex_token(lexer, CLEX_eqarrow, p,p+1);)
         }
         goto single_char;
      case '!':
         STB_C_LEX_C_COMPARISONS(if (P+1 != Lexer->Eof && P[1] == '=') return stbClexToken(Lexer, CLEX_noteq, P,P+1);)
         goto single_char;
      case '^':
         STB_C_LEX_C_BITWISEEQ(if (P+1 != Lexer->Eof && P[1] == '=') return stbClexToken(Lexer, CLEX_xoreq, P,P+1));
         goto single_char;
      case '%':
         STB_C_LEX_C_ARITHEQ(if (P+1 != Lexer->Eof && P[1] == '=') return stbClexToken(Lexer, CLEX_modeq, P,P+1));
         goto single_char;
      case '*':
         STB_C_LEX_C_ARITHEQ(if (P+1 != Lexer->Eof && P[1] == '=') return stbClexToken(Lexer, CLEX_muleq, P,P+1));
         goto single_char;
      case '/':
         STB_C_LEX_C_ARITHEQ(if (P+1 != Lexer->Eof && P[1] == '=') return stbClexToken(Lexer, CLEX_diveq, P,P+1));
         goto single_char;
      case '<':
         if (P+1 != Lexer->Eof) {
            STB_C_LEX_C_COMPARISONS(if (P[1] == '=') return stbClexToken(Lexer, CLEX_lesseq, P,P+1);)
            STB_C_LEX_C_SHIFTS(     if (P[1] == '<') {
                                       STB_C_LEX_C_ARITHEQ(if (P+2 != Lexer->Eof && P[2] == '=')
                                                              return stbClexToken(Lexer, CLEX_shleq, P,P+2);)
                                       return stbClexToken(Lexer, CLEX_shl, P,P+1);
                                    }
                              )
         }
         goto single_char;
      case '>':
         if (P+1 != Lexer->Eof) {
            STB_C_LEX_C_COMPARISONS(if (P[1] == '=') return stbClexToken(Lexer, CLEX_greatereq, P,P+1);)
            STB_C_LEX_C_SHIFTS(     if (P[1] == '>') {
                                       STB_C_LEX_C_ARITHEQ(if (P+2 != Lexer->Eof && P[2] == '=')
                                                              return stbClexToken(Lexer, CLEX_shreq, P,P+2);)
                                       return stbClexToken(Lexer, CLEX_shr, P,P+1);
                                    }
                              )
         }
         goto single_char;

      case '"':
         STB_C_LEX_C_DQ_STRINGS(return stbClexParseString(Lexer, P, CLEX_dqstring);)
         goto single_char;
      case '\'':
         STB_C_LEX_C_SQ_STRINGS(return stb__clex_parse_string(lexer, p, CLEX_sqstring);)
         STB_C_LEX_C_CHARS(
         {
            char *Start = P;
            Lexer->IntNumber = stbClexParseChar(P+1, &P);
            if (Lexer->IntNumber < 0)
               return stbClexToken(Lexer, CLEX_parse_error, Start,Start);
            if (P == Lexer->Eof || *P != '\'')
               return stbClexToken(Lexer, CLEX_parse_error, Start,P);
            return stbClexToken(Lexer, CLEX_charlit, Start, P+1);
         })
         goto single_char;

      case '0':
         #if defined(STB__clex_hex_ints) || defined(STB__clex_hex_floats)
            if (P+1 != Lexer->Eof) {
               if (P[1] == 'x' || P[1] == 'X') {
                  char *Q;

                  #ifdef STB__clex_hex_floats
                  for (q=p+2;
                       q != lexer->eof && ((*q >= '0' && *q <= '9') || (*q >= 'a' && *q <= 'f') || (*q >= 'A' && *q <= 'F'));
                       ++q);
                  if (q != lexer->eof) {
                     if (*q == '.' STB_C_LEX_FLOAT_NO_DECIMAL(|| *q == 'p' || *q == 'P')) {
                        #ifdef STB__CLEX_use_stdlib
                        lexer->real_number = strtod((char *) p, (char**) &q);
                        #else
                        lexer->real_number = stb__clex_parse_float(p, &q);
                        #endif

                        if (p == q)
                           return stb__clex_token(lexer, CLEX_parse_error, p,q);
                        return stb__clex_parse_suffixes(lexer, CLEX_floatlit, p,q, STB_C_LEX_FLOAT_SUFFIXES);

                     }
                  }
                  #endif   // STB__CLEX_hex_floats

                  #ifdef STB__clex_hex_ints
                  #ifdef STB__CLEX_use_stdlib
                  Lexer->IntNumber = strtol((char *) P, (char **) &Q, 16);
                  #else
                  {
                     stb__clex_int n=0;
                     for (q=p+2; q != lexer->eof; ++q) {
                        if (*q >= '0' && *q <= '9')
                           n = n*16 + (*q - '0');
                        else if (*q >= 'a' && *q <= 'f')
                           n = n*16 + (*q - 'a') + 10;
                        else if (*q >= 'A' && *q <= 'F')
                           n = n*16 + (*q - 'A') + 10;
                        else
                           break;
                     }
                     lexer->int_number = n;
                  }
                  #endif
                  if (Q == P+2)
                     return stbClexToken(Lexer, CLEX_parse_error, P-2,P-1);
                  return stbClexParseSuffixes(Lexer, CLEX_intlit, P,Q, STB_C_LEX_HEX_SUFFIXES);
                  #endif
               }
            }
         #endif // defined(STB__clex_hex_ints) || defined(STB__clex_hex_floats)
         // can't test for octal because we might parse '0.0' as float or as '0' '.' '0',
         // so have to do float first

         /* FALL THROUGH */
      case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':

         #ifdef STB__clex_decimal_floats
         {
            char *Q = P;
            while (Q != Lexer->Eof && (*Q >= '0' && *Q <= '9'))
               ++Q;
            if (Q != Lexer->Eof) {
               if (*Q == '.' STB_C_LEX_FLOAT_NO_DECIMAL(|| *Q == 'e' || *Q == 'E')) {
                  #ifdef STB__CLEX_use_stdlib
                  Lexer->RealNumber = strtod((char *) P, (char**) &Q);
                  #else
                  lexer->real_number = stb__clex_parse_float(p, &q);
                  #endif

                  return stbClexParseSuffixes(Lexer, CLEX_floatlit, P,Q, STB_C_LEX_FLOAT_SUFFIXES);

               }
            }
         }
         #endif // STB__clex_decimal_floats

         #ifdef STB__clex_octal_ints
         if (P[0] == '0') {
            char *Q = P;
            #ifdef STB__CLEX_use_stdlib
            Lexer->IntNumber = strtol((char *) P, (char **) &Q, 8);
            #else
            stb__clex_int n=0;
            while (q != lexer->eof) {
               if (*q >= '0' && *q <= '7')
                  n = n*8 + (*q - '0');
               else
                  break;
               ++q;
            }
            if (q != lexer->eof && (*q == '8' || *q=='9'))
               return stb__clex_token(lexer, CLEX_parse_error, p, q);
            lexer->int_number = n;
            #endif
            return stbClexParseSuffixes(Lexer, CLEX_intlit, P,Q, STB_C_LEX_OCTAL_SUFFIXES);
         }
         #endif // STB__clex_octal_ints

         #ifdef STB__clex_decimal_ints
         {
            char *Q = P;
            #ifdef STB__CLEX_use_stdlib
            Lexer->IntNumber = strtol((char *) P, (char **) &Q, 10);
            #else
            stb__clex_int n=0;
            while (q != lexer->eof) {
               if (*q >= '0' && *q <= '9')
                  n = n*10 + (*q - '0');
               else
                  break;
               ++q;
            }
            lexer->int_number = n;
            #endif
            return stbClexParseSuffixes(Lexer, CLEX_intlit, P,Q, STB_C_LEX_OCTAL_SUFFIXES);
         }
         #endif // STB__clex_decimal_ints
         goto single_char;
   }
}
#endif // STB_C_LEXER_IMPLEMENTATION

#ifdef STB_C_LEXER_SELF_TEST
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

static void print_token(stb_lexer *lexer)
{
   switch (lexer->token) {
      case CLEX_id        : printf("_%s", lexer->string); break;
      case CLEX_eq        : printf("=="); break;
      case CLEX_noteq     : printf("!="); break;
      case CLEX_lesseq    : printf("<="); break;
      case CLEX_greatereq : printf(">="); break;
      case CLEX_andand    : printf("&&"); break;
      case CLEX_oror      : printf("||"); break;
      case CLEX_shl       : printf("<<"); break;
      case CLEX_shr       : printf(">>"); break;
      case CLEX_plusplus  : printf("++"); break;
      case CLEX_minusminus: printf("--"); break;
      case CLEX_arrow     : printf("->"); break;
      case CLEX_andeq     : printf("&="); break;
      case CLEX_oreq      : printf("|="); break;
      case CLEX_xoreq     : printf("^="); break;
      case CLEX_pluseq    : printf("+="); break;
      case CLEX_minuseq   : printf("-="); break;
      case CLEX_muleq     : printf("*="); break;
      case CLEX_diveq     : printf("/="); break;
      case CLEX_modeq     : printf("%%="); break;
      case CLEX_shleq     : printf("<<="); break;
      case CLEX_shreq     : printf(">>="); break;
      case CLEX_eqarrow   : printf("=>"); break;
      case CLEX_dqstring  : printf("\"%s\"", lexer->string); break;
      case CLEX_sqstring  : printf("'\"%s\"'", lexer->string); break;
      case CLEX_charlit   : printf("'%s'", lexer->string); break;
      #if defined(STB__clex_int_as_double) && !defined(STB__CLEX_use_stdlib)
      case CLEX_intlit    : printf("#%g", lexer->real_number); break;
      #else
      case CLEX_intlit    : printf("#%ld", lexer->int_number); break;
      #endif
      case CLEX_floatlit  : printf("%g", lexer->real_number); break;
      default:
         if (lexer->token >= 0 && lexer->token < 256)
            printf("%c", (int) lexer->token);
         else {
            printf("<<<UNKNOWN TOKEN %ld >>>\n", lexer->token);
         }
         break;
   }
}

/* Force a test
of parsing
multiline comments */

/*/ comment /*/
/**/ extern /**/

void dummy(void)
{
   double some_floats[] = {
      1.0501, -10.4e12, 5E+10,
#if 0   // not supported in C++ or C-pre-99, so don't try to compile it, but let our parser test it
      0x1.0p+24, 0xff.FP-8, 0x1p-23,
#endif
      4.
   };
   (void) sizeof(some_floats);
   (void) some_floats[1];

   printf("test %d",1); // https://github.com/nothings/stb/issues/13
}

int main(int argc, char **argv)
{
   FILE *f = fopen("stb_c_lexer.h","rb");
   char *text = (char *) malloc(1 << 20);
   int len = f ? (int) fread(text, 1, 1<<20, f) : -1;
   stb_lexer lex;
   if (len < 0) {
      fprintf(stderr, "Error opening file\n");
      free(text);
      fclose(f);
      return 1;
   }
   fclose(f);

   stb_c_lexer_init(&lex, text, text+len, (char *) malloc(0x10000), 0x10000);
   while (stb_c_lexer_get_token(&lex)) {
      if (lex.token == CLEX_parse_error) {
         printf("\n<<<PARSE ERROR>>>\n");
         break;
      }
      print_token(&lex);
      printf("  ");
   }
   return 0;
}
#endif
/*
------------------------------------------------------------------------------
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT License
Copyright (c) 2017 Sean Barrett
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
ALTERNATIVE B - Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------
*/