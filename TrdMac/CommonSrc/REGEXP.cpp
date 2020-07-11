 ///////////////////////////////////////////////////////////////////////////
/*
 * regcomp and regexec -- regsub and regerror are elsewhere
 *
 *  Copyright (c) 1986 by University of Toronto.
 *  Written by Henry Spencer.  Not derived from licensed software.
 *
 *  Permission is granted to anyone to use this software for any
 *  purpose on any computer system, and to redistribute it freely,
 *  subject to the following restrictions:
 *
 *  1. The author is not responsible for the consequences of use of
 *      this software, no matter how awful, even if they arise
 *      from defects in it.
 *
 *  2. The origin of this software must not be misrepresented, either
 *      by explicit claim or by omission.
 *
 *  3. Altered versions must be plainly marked as such, and must not
 *      be misrepresented as being the original software.
 *
 * Beware that some of this code is subtly aware of the way operator
 * precedence is structured in regular expressions.  Serious changes in
 * regular-expression syntax might require a total rethink.
 */
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "RegExp.h"
//#pragma warning ( disable : 4996 )

IMPLEMENT_SERIAL(CSGrep,CObject,1);   

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

////////////////////////////////////////////////////////////////////////////
/*
 * The "internal use only" fields in regexp.h are present to pass info from
 * compile to execute that permits the execute phase to run lots faster on
 * simple cases.  They are:
 *
 * regstart char that must begin a match; '\0' if none obvious
 * reganch  is the match anchored (at beginning-of-line only)?
 * regmust  string (pointer into program) that match must include, or NULL
 * regmlen  length of regmust string
 *
 * Regstart and reganch permit very fast decisions on suitable starting points
 * for a match, cutting down the work a lot.  Regmust permits fast rejection
 * of lines that cannot possibly match.  The regmust tests are costly enough
 * that regcomp() supplies a regmust only if the r.e. contains something
 * potentially expensive (at present, the only such thing detected is * or +
 * at the start of the r.e., which can involve a lot of backup).  Regmlen is
 * supplied because the test in regexec() needs it and regcomp() is computing
 * it anyway.
 */

////////////////////////////////////////////////////////////////////////////
/*
 * Structure for regexp "program".  This is essentially a linear encoding
 * of a nondeterministic finite-state machine (aka syntax charts or
 * "railroad normal form" in parsing technology).  Each node is an opcode
 * plus a "next" pointer, possibly plus an operand.  "Next" pointers of
 * all nodes except BRANCH implement concatenation; a "next" pointer with
 * a BRANCH on both ends of it is connecting two alternatives.  (Here we
 * have one of the subtle syntax dependencies:  an individual BRANCH (as
 * opposed to a collection of them) is never concatenated with anything
 * because of operator precedence.)  The operand of some types of node is
 * a literal string; for others, it is a node leading into a sub-FSM.  In
 * particular, the operand of a BRANCH node is the first node of the branch.
 * (NB this is *not* a tree structure:  the tail of the branch connects
 * to the thing following the set of BRANCHes.)  The opcodes are:
 */
////////////////////////////////////////////////////////////////////////////

/* definition   number  opnd?   meaning */
enum NumberOpnd 
    {
    END=0,      /* no   End of program. */
    BOL,        /* no   Match "" at beginning of line. */
    EOL,        /* no   Match "" at end of line. */
    ANY,        /* no   Match any one character. */
    ANYOF,      /* str  Match any character in this string. */
    ANYBUT,     /* str  Match any character not in this string. */
    BRANCH,     /* node Match this alternative, or the next... */
    BACK,       /* no   Match "", "next" ptr points backward. */
    EXACTLY,    /* str  Match this string. */
    NOTHING,    /* no   Match empty string. */
    STAR,       /* node Match this (simple) thing 0 or more times. */
    PLUS,       /* node Match this (simple) thing 1 or more times. */
    
    BLANK,      /* :B Blanks */
    BLANKM,     /* :L Blanks */
    ALPHA,      /* :A Alphabetic characters */
    NALPHA,     /* :N Alphabatics characters */
    ALPHANUM,   /* :P Alphanumeric characters */
    NALPHANUM,  /* :G Alphanumerics characters */
    ALPHAWE,    /* :H Alphabetic characters without extenders*/
    NALPHAWE,   /* :K Alphabetics characters without extenders*/
    DIGIT,      /* :D Digits */
    DIGITM,     /* :M Digits */
    
    OPEN,       /* no   Mark this point in input as start of #n. */
                /*  OPEN+1 is number 1, etc. */
    OPEN1,
    OPEN2,
    OPEN3,
    OPEN4,
    OPEN5,
    OPEN6,
    OPEN7,
    OPEN8,
    OPEN9,
    CLOSE,      /* no   Analogous to OPEN. */
    CLOSE1,
    CLOSE2,
    CLOSE3,
    CLOSE4,
    CLOSE5,
    CLOSE6,
    CLOSE7,
    CLOSE8,
    CLOSE9,
    };

//#define   END 0   /* no   End of program. */
//#define   BOL 1   /* no   Match "" at beginning of line. */
//#define   EOL 2   /* no   Match "" at end of line. */
//#define   ANY 3   /* no   Match any one character. */
//#define   ANYOF   4   /* str  Match any character in this string. */
//#define   ANYBUT  5   /* str  Match any character not in this string. */
//#define   BRANCH  6   /* node Match this alternative, or the next... */
//#define   BACK    7   /* no   Match "", "next" ptr points backward. */
//#define   EXACTLY 8   /* str  Match this string. */
//#define   NOTHING 9   /* no   Match empty string. */
//#define   STAR    10  /* node Match this (simple) thing 0 or more times. */
//#define   PLUS    11  /* node Match this (simple) thing 1 or more times. */
//#define   OPEN    20  /* no   Mark this point in input as start of #n. */
            /*  OPEN+1 is number 1, etc. */
//#define   CLOSE   30  /* no   Analogous to OPEN. */

////////////////////////////////////////////////////////////////////////////
/*
 * Opcode notes:
 *
 * BRANCH   The set of branches constituting a single choice are hooked
 *      together with their "next" pointers, since precedence prevents
 *      anything being concatenated to any individual branch.  The
 *      "next" pointer of the last BRANCH in a choice points to the
 *      thing following the whole choice.  This is also where the
 *      final "next" pointer of each individual branch points; each
 *      branch starts with the operand node of a BRANCH node.
 *
 * BACK     Normal "next" pointers all implicitly point forward; BACK
 *      exists to make loop structures possible.
 *
 * STAR,PLUS    '?', and complex '*' and '+', are implemented as circular
 *      BRANCH structures using BACK.  Simple cases (one character
 *      per match) are implemented with STAR and PLUS for speed
 *      and to minimize recursive plunges.
 *
 * OPEN,CLOSE   ...are numbered at compile time.
 */
////////////////////////////////////////////////////////////////////////////
/*
 * A node is one char of opcode followed by two chars of "next" pointer.
 * "Next" pointers are stored as two 8-bit pieces, high order first.  The
 * value is a positive offset from the opcode of the node containing it.
 * An operand, if any, simply follows the node.  (Note that much of the
 * code generation knows about this implicit relationship.)
 *
 * Using two bytes for the "next" pointer is vast overkill for most things,
 * but allows patterns to get big without disasters.
 */
////////////////////////////////////////////////////////////////////////////
#define OP(p)   (*(p))
#define NEXT(p) (((*((p)+1)&0377)<<8) + (*((p)+2)&0377))
#define OPERAND(p)  ((p) + 3)

/*
 * See regmagic.h for one further detail of program structure.
 */


/*
 * Utility definitions.
 */
#ifndef CHARBITS
#define UCHARAT(p)  ((int)*(unsigned char *)(p))
#else
#define UCHARAT(p)  ((int)*(p)&CHARBITS)
#endif

#define FAIL(m)  { regerror(m); return(NULL);}
#define FAILB(m) { regerror(m); return(TRUE);}
#define ISMULT(c)   ((c) == '*' || (c) == '+' || (c) == '?')
#define META    "^$.[()|?+*\\"

/*
 * Flags to be passed up and down.
 */
#define HASWIDTH    01  /* Known never to match null string. */
#define SIMPLE      02  /* Simple enough to be STAR/PLUS operand. */
#define SPSTART     04  /* Starts with * or +. */
#define WORST       0   /* Worst case. */


////////////////////////////////////////////////////////////////////////////
/*
 - regcomp - compile a regular expression into internal code
 *
 * We can't allocate space until we know how big the compiled form will be,
 * but we can't compile it (and thus know how big it is) until we've got a
 * place to put the code.  So we cheat:  we compile it twice, once with code
 * generation turned off and size counting turned on, and once "for real".
 * This also means that we don't allocate space until we are sure that the
 * thing really will compile successfully, and we never have to move the
 * code and thus invalidate pointers into it.  (Note that it has to be in
 * one piece because free() must be able to free it all.)
 *
 * Beware that the optimization-preparation code in here knows about some
 * of the structure of the compiled regexp.
 */
////////////////////////////////////////////////////////////////////////////

regexp *CSGrep::regcomp(const char *exp)
    {
    register regexp *r;
    register char *scan;
    register char *longest;
    register int len;
    int flags;

    if (exp == NULL)
        FAIL("NULL argument");

    /* First pass: determine size, legality. */
    m_RegParse = exp;
    m_RegNPar = 1;
    m_RegSize = 0L;
    m_RegCode = &m_RegDummy;
    regc((char)MAGIC);
    if (reg(0, &flags) == NULL)
        return(NULL);

    /* Small enough for pointer-storage convention? */
    if (m_RegSize >= 32767L)        /* Probably could be 65535L. */
        FAIL("regexp too big");

    /* Allocate space. */
    r = (regexp *)malloc(sizeof(regexp) + (unsigned)m_RegSize);
    if (r == NULL)
        FAIL("out of space");

    /* Second pass: emit code. */
    m_RegParse = exp;
    m_RegNPar = 1;
    m_RegCode = r->program;
    regc((char)MAGIC);
    if (reg(0, &flags) == NULL)
        return(NULL);

    /* Dig out information for optimizations. */
    r->regstart = '\0'; /* Worst-case defaults. */
    r->reganch = 0;
    r->regmust = NULL;
    r->regmlen = 0;
    scan = r->program+1;            /* First BRANCH. */
    if (OP(regnext(scan)) == END) 
        {       /* Only one top-level choice. */
        scan = OPERAND(scan);

        /* Starting-point info. */
        if (OP(scan) == EXACTLY)
            r->regstart = *OPERAND(scan);
        else if (OP(scan) == BOL)
            r->reganch++;

        /*
         * If there's something expensive in the r.e., find the
         * longest literal string that must appear and make it the
         * regmust.  Resolve ties in favor of later strings, since
         * the regstart check works with the beginning of the r.e.
         * and avoiding duplication strengthens checking.  Not a
         * strong reason, but sufficient in the absence of others.
         */
        if (flags&SPSTART) 
            {
            longest = NULL;
            len = 0;
            for (; scan != NULL; scan = regnext(scan))
                {
                if (OP(scan) == EXACTLY && (int)strlen(OPERAND(scan)) >= len) 
                    {
                    longest = OPERAND(scan);
                    len = (int)strlen(OPERAND(scan));
                    }
                }   
            r->regmust = longest;
            r->regmlen = len;
            }
        }
    return(r);
    }

////////////////////////////////////////////////////////////////////////////
/*
 - reg - regular expression, i.e. main body or parenthesized thing
 *
 * Caller must absorb opening parenthesis.
 *
 * Combining parenthesis handling with the base level of regular expression
 * is a trifle forced, but the need to tie the tails of the branches to what
 * follows makes it hard to avoid.
 */
////////////////////////////////////////////////////////////////////////////

char * CSGrep::reg(int paren, int *flagp) /* Parenthesized? */
    {
    register char *ret;
    register char *br;
    register char *ender;
    register int parno = 0;
    int flags;

    *flagp = HASWIDTH;  /* Tentatively. */

    /* Make an OPEN node, if parenthesized. */
    if (paren) 
        {
        if (m_RegNPar >= NSUBEXP)
            FAIL("too many ()");
        parno = m_RegNPar;
        m_RegNPar++;
        ret = regnode(OPEN+parno);
        } 
    else
        ret = NULL;

    /* Pick up the branches, linking them together. */
    br = regbranch(&flags);
    if (br == NULL)
        return(NULL);
    if (ret != NULL)
        regtail(ret, br);   /* OPEN -> first. */
    else
        ret = br;
    if (!(flags&HASWIDTH))
        *flagp &= ~HASWIDTH;
    *flagp |= flags&SPSTART;
    while (*m_RegParse == '|') 
        {
        m_RegParse++;
        br = regbranch(&flags);
        if (br == NULL)
            return(NULL);
        regtail(ret, br);   /* BRANCH -> BRANCH. */
        if (!(flags&HASWIDTH))
            *flagp &= ~HASWIDTH;
        *flagp |= flags&SPSTART;
        }

    /* Make a closing node, and hook it on the end. */
    ender = regnode((paren) ? CLOSE+parno : END);
    regtail(ret, ender);

    /* Hook the tails of the branches to the closing node. */
    for (br = ret; br != NULL; br = regnext(br))
        regoptail(br, ender);

    /* Check for proper termination. */
    if (paren && *m_RegParse++ != ')') 
        {
        FAIL("unmatched ()");
        } 
    else if (!paren && *m_RegParse != '\0') 
        {
        if (*m_RegParse == ')') 
            {
            FAIL("unmatched ()");
            } 
        else
            FAIL("junk on end");    /* "Can't happen". */
        /* NOTREACHED */
        }
    return(ret);
    }

////////////////////////////////////////////////////////////////////////////
/*
 - regbranch - one alternative of an | operator
 *
 * Implements the concatenation operator.
 */
////////////////////////////////////////////////////////////////////////////

char *CSGrep::regbranch(int *flagp)
    {
    register char *ret;
    register char *chain;
    register char *latest;
    int flags;

    *flagp = WORST;     /* Tentatively. */

    ret = regnode(BRANCH);
    chain = NULL;
    while (*m_RegParse != '\0' && *m_RegParse != '|' && *m_RegParse != ')') 
        {
        latest = regpiece(&flags);
        if (latest == NULL)
            return(NULL);
        *flagp |= flags&HASWIDTH;
        if (chain == NULL)  /* First piece. */
            *flagp |= flags&SPSTART;
        else
            regtail(chain, latest);
        chain = latest;
        }
    if (chain == NULL)  /* Loop ran zero times. */
        (void) regnode(NOTHING);

    return(ret);
    }

////////////////////////////////////////////////////////////////////////////
/*
 - regpiece - something followed by possible [*+?]
 *
 * Note that the branching code sequences used for ? and the general cases
 * of * and + are somewhat optimized:  they use the same NOTHING node as
 * both the endmarker for their branch list and the body of the last branch.
 * It might seem that this node could be dispensed with entirely, but the
 * endmarker role is not redundant.
 */
////////////////////////////////////////////////////////////////////////////

char *CSGrep::regpiece(int *flagp)
    {
    register char *ret;
    register char op;
    register char *next;
    int flags;

    ret = regatom(&flags);
    if (ret == NULL)
        return(NULL);

    op = *m_RegParse;
    if (!ISMULT(op)) 
        {
        *flagp = flags;
        return(ret);
        }

    if (!(flags&HASWIDTH) && op != '?')
        FAIL("*+ operand could be empty");
    *flagp = (op != '+') ? (WORST|SPSTART) : (WORST|HASWIDTH);

    if (op == '*' && (flags&SIMPLE))
        reginsert(STAR, ret);
    else if (op == '*') 
        {
        /* Emit x* as (x&|), where & means "self". */
        reginsert(BRANCH, ret);         /* Either x */
        regoptail(ret, regnode(BACK));      /* and loop */
        regoptail(ret, ret);            /* back */
        regtail(ret, regnode(BRANCH));      /* or */
        regtail(ret, regnode(NOTHING));     /* null. */
        } 
    else if (op == '+' && (flags&SIMPLE))
        reginsert(PLUS, ret);
    else if (op == '+') 
        {
        /* Emit x+ as x(&|), where & means "self". */
        next = regnode(BRANCH);         /* Either */
        regtail(ret, next);
        regtail(regnode(BACK), ret);        /* loop back */
        regtail(next, regnode(BRANCH));     /* or */
        regtail(ret, regnode(NOTHING));     /* null. */
        } 
    else if (op == '?') 
        {
        /* Emit x? as (x|) */
        reginsert(BRANCH, ret);         /* Either x */
        regtail(ret, regnode(BRANCH));      /* or */
        next = regnode(NOTHING);        /* null. */
        regtail(ret, next);
        regoptail(ret, next);
        }
    m_RegParse++;
    if (ISMULT(*m_RegParse))
        FAIL("nested *?+");

    return(ret);
    }

////////////////////////////////////////////////////////////////////////////
/*
 - regatom - the lowest level
 *
 * Optimization:  gobbles an entire sequence of ordinary characters so that
 * it can turn them into a single node, which is smaller to store and
 * faster to run.  Backslashed characters are exceptions, each becoming a
 * separate node; the code is simpler that way and it's not worth fixing.
 */
////////////////////////////////////////////////////////////////////////////

char *CSGrep::regatom(int *flagp)
    {
    register char *ret;
    int flags;

    *flagp = WORST;     /* Tentatively. */

    switch (*m_RegParse++) 
        {
    case ':': 
        switch(*m_RegParse++)
            {
        case 'B':   
        case 'b':
            ret = regnode(BLANK);
            break;
        case 'L':   
        case 'l':
            ret = regnode(BLANKM);
            break;
        case 'a':
        case 'A':
            ret = regnode(ALPHA);
            break;
        case 'n':
        case 'N':
            ret = regnode(NALPHA);
            break;
        case 'p':
        case 'P':
            ret = regnode(ALPHANUM);
            break;
        case 'g':
        case 'G':
            ret = regnode(NALPHANUM);
            break;
        case 'h':
        case 'H':
            ret = regnode(ALPHAWE);
            break;
        case 'k':
        case 'K':
            ret = regnode(NALPHAWE);
            break;
        case 'd':
        case 'D':
            ret = regnode(DIGIT);
            break;
        case 'M':
        case 'm':
            ret = regnode(DIGITM);
            break;
        default:
            FAIL("Unknown [:type]");
            }
        *flagp |= HASWIDTH|SIMPLE;
        break;
        
    case '^':
        ret = regnode(BOL);
        break;
    case '$':
        ret = regnode(EOL);
        break;
    case '.':
        ret = regnode(ANY);
        *flagp |= HASWIDTH|SIMPLE;
        break;
    case '[': 
        {
        register int Clazz;
        register int classend;

        if (*m_RegParse == '^') 
            {   /* Complement of range. */
            ret = regnode(ANYBUT);
            m_RegParse++;
            } 
        else
            ret = regnode(ANYOF);
        if (*m_RegParse == ']' || *m_RegParse == '-')
            regc(*m_RegParse++);
        while (*m_RegParse != '\0' && *m_RegParse != ']') 
            {
            if (*m_RegParse == '-') 
                {
                m_RegParse++;
                if (*m_RegParse == ']' || *m_RegParse == '\0')
                    regc('-');
                else 
                    {
                    Clazz = UCHARAT(m_RegParse-2)+1;
                    classend = UCHARAT(m_RegParse);
                    if (Clazz > classend+1)
                        FAIL("invalid [] range");
                    for (; Clazz <= classend; Clazz++)
                        regc(Clazz);
                    m_RegParse++;
                    }
                } 
            else
                regc(*m_RegParse++);
            }
        regc('\0');
        if (*m_RegParse != ']')
            FAIL("unmatched []");
        m_RegParse++;
        *flagp |= HASWIDTH|SIMPLE;
        }
        break;
    case '(':
        ret = reg(1, &flags);
        if (ret == NULL)
            return(NULL);
        *flagp |= flags&(HASWIDTH|SPSTART);
        break;
    case '\0':
    case '|':
    case ')':
        FAIL("internal urp");   /* Supposed to be caught earlier. */
        break;
    case '?':
    case '+':
    case '*':
        FAIL("?+* follows nothing");
        break;
    case '\\':
        if (*m_RegParse == '\0')
            FAIL("trailing \\");
        ret = regnode(EXACTLY);
        regc(*m_RegParse++);
        regc('\0');
        *flagp |= HASWIDTH|SIMPLE;
        break;
    default: 
        {
        register int len;
        register char ender;

        m_RegParse--;
        len = (int)strcspn(m_RegParse, META);
        if (len <= 0)
            FAIL("internal disaster");
        ender = *(m_RegParse+len);
        if (len > 1 && ISMULT(ender))
            len--;      /* Back off clear of ?+* operand. */
        *flagp |= HASWIDTH;
        if (len == 1)
            *flagp |= SIMPLE;
        ret = regnode(EXACTLY);
        while (len > 0) 
            {
            regc(*m_RegParse++);
            len--;
            }
        regc('\0');
        }
        break;
    }
    return(ret);
    }

////////////////////////////////////////////////////////////////////////////
/*
 - regnode - emit a node
 */
////////////////////////////////////////////////////////////////////////////

inline char *CSGrep::regnode(char op)   /* Location */
    {
    register char *ret;
    register char *ptr;

    ret = m_RegCode;
    if (ret == &m_RegDummy) 
        {
        m_RegSize += 3;
        return(ret);
        }

    ptr = ret;
    *ptr++ = op;
    *ptr++ = '\0';      /* Null "next" pointer. */
    *ptr++ = '\0';
    m_RegCode = ptr;

    return(ret);
    }

////////////////////////////////////////////////////////////////////////////
/*
 - regc - emit (if appropriate) a byte of code
 */
////////////////////////////////////////////////////////////////////////////

void CSGrep::regc(char b)
    {
    if (m_RegCode != &m_RegDummy)
        *m_RegCode++ = b;
    else
        m_RegSize++;
    }

////////////////////////////////////////////////////////////////////////////
/*
 - reginsert - insert an operator in front of already-emitted operand
 *
 * Means relocating the operand.
 */
////////////////////////////////////////////////////////////////////////////

void CSGrep::reginsert(char op, char *opnd)
    {
    register char *src;
    register char *dst;
    register char *place;

    if (m_RegCode == &m_RegDummy) 
        {
        m_RegSize += 3;
        return;
        }

    src = m_RegCode;
    m_RegCode += 3;
    dst = m_RegCode;
    while (src > opnd)
        *--dst = *--src;

    place = opnd;       /* Op node, where operand used to be. */
    *place++ = op;
    *place++ = '\0';
    *place++ = '\0';
    }

////////////////////////////////////////////////////////////////////////////
/*
 - regtail - set the next-pointer at the end of a node chain
 */
////////////////////////////////////////////////////////////////////////////

void CSGrep::regtail(char *p, char *val)
    {
    register char *scan;
    register char *temp;
    register int offset;

    if (p == &m_RegDummy)
        return;

    /* Find last node. */
    scan = p;
    for (;;) 
        {
        temp = regnext(scan);
        if (temp == NULL)
            break;
        scan = temp;
        }

    if (OP(scan) == BACK)
        offset = (int)(scan - val);
    else
        offset = (int)(val - scan);
    *(scan+1) = (offset>>8)&0377;
    *(scan+2) = offset&0377;
    }

////////////////////////////////////////////////////////////////////////////
/*
 - regoptail - regtail on operand of first argument; nop if operandless
 */
////////////////////////////////////////////////////////////////////////////

void CSGrep::regoptail(char *p, char *val)
    {
    /* "Operandless" and "op != BRANCH" are synonymous in practice. */
    if (p == NULL || p == &m_RegDummy || OP(p) != BRANCH)
        return;
    regtail(OPERAND(p), val);
    }

////////////////////////////////////////////////////////////////////////////
/*
 * regexec and friends
 */

/*
 * Global work variables for regexec().
 */

/*
 * Forwards.
 */

#ifdef DEBUG
int regnarrate = 0;
void regdump();
//static char *regprop();
#endif

////////////////////////////////////////////////////////////////////////////
/*
 - regexec - match a regexp against a string
 */                             
////////////////////////////////////////////////////////////////////////////

BOOL CSGrep::regexec(const char* string)
    {
		//COLLECT_INTERVAL("regexec");
    register const char *s;

    /* Be paranoid... */
    if (m_ptrData == NULL || string == NULL) 
        {
        regerror("NULL parameter");
        return(FALSE);
        }

    /* Check validity of program. */
    if (UCHARAT(m_ptrData->program) != MAGIC) 
        {
        regerror("corrupted program");
        return(FALSE);
        }

    /* If there is a "must appear" string, look for it. */
    if (m_ptrData->regmust != NULL) 
        {
        s = string;
        while ((s = strchr(s, m_ptrData->regmust[0])) != NULL) 
            {
            if (strncmp(s, m_ptrData->regmust, m_ptrData->regmlen) == 0)
                break;  /* Found it. */
            s++;
            }
        if (s == NULL)  /* Not present. */
            return(FALSE);
        }

    /* Mark beginning of line for ^ . */
    m_RegBol = string;

    /* Simplest case:  anchored match need be tried only once. */
    if (m_ptrData->reganch)
        return(regtry(string) != 0);

    /* Messy cases:  unanchored match. */
    s = string;
    if (m_ptrData->regstart != '\0')
        /* We know what char it must start with. */
        while ((s = strchr(s, m_ptrData->regstart)) != NULL) 
            {
            if (regtry(s))
                return(TRUE);
            s++;
            }
    else
        /* We don't -- general case. */
        do  
            {
            if (regtry(s))
                return(TRUE);
            } while (*s++ != '\0');

    /* Failure. */
    return(FALSE);
    }

////////////////////////////////////////////////////////////////////////////
/*
 - regtry - try match at specific point
 */
/* 0 failure, 1 success */    
////////////////////////////////////////////////////////////////////////////
int CSGrep::regtry(const char *string)
    {
    register int i;
    register char **sp;
    register char **ep;

    m_RegInput = string;
    m_RegStartP = m_ptrData->startp;
    m_RegEndP = m_ptrData->endp;

    sp = m_ptrData->startp;
    ep = m_ptrData->endp;
    for (i = NSUBEXP; i > 0; i--) 
        {
        *sp++ = NULL;
        *ep++ = NULL;
        }
    if (regmatch(m_ptrData->program + 1)) 
        {
        m_ptrData->startp[0] = (char *)string;
        m_ptrData->endp[0] = (char *)m_RegInput;
        return(1);
        } 
    else
        return(0);
    }

////////////////////////////////////////////////////////////////////////////
/*
 - regmatch - main matching routine
 *
 * Conceptually the strategy is simple:  check to see whether the current
 * node matches, call self recursively to see whether the rest matches,
 * and then act accordingly.  In practice we make some effort to avoid
 * recursion, in particular by going through "ordinary" nodes (that don't
 * need to know whether the rest of the match failed) by a loop instead of
 * by recursion.
 */
/* 0 failure, 1 success */
////////////////////////////////////////////////////////////////////////////
                         
int CSGrep::regmatch(char *prog)
    {
    register char *scan;    /* Current node. */
    char *next;     /* Next node. */
    

    scan = prog;
    while (scan != NULL) 
        {
        next = regnext(scan);

        switch (OP(scan)) 
            {   
        case BOL:
            if (m_RegInput != m_RegBol)
                return(0);
            break;
        case EOL:
            if (*m_RegInput != '\0')
                return(0);
            break;
        case ANY:
            if (*m_RegInput == '\0')
                return(0);
            m_RegInput++;
            break;
        case EXACTLY: 
            {
            register int len;
            register char *opnd;

            opnd = OPERAND(scan);
            /* Inline the first character, for speed. */
            if (*opnd != *m_RegInput)
                return(0);
            len = (int)strlen(opnd);
            if (len > 1 && strncmp(opnd, m_RegInput, len) != 0)
                return(0);
            m_RegInput += len;
            }
            break;
        case ANYOF:
            if (*m_RegInput == '\0' || strchr(OPERAND(scan), *m_RegInput) == NULL)
                return(0);
            m_RegInput++;
            break;
        case ANYBUT:
            if (*m_RegInput == '\0' || strchr(OPERAND(scan), *m_RegInput) != NULL)
                return(0);
            m_RegInput++;
            break;
        case NOTHING:
            break;
        case BACK:
            break;
        case OPEN1:
        case OPEN2:
        case OPEN3:
        case OPEN4:
        case OPEN5:
        case OPEN6:
        case OPEN7:
        case OPEN8:
        case OPEN9: 
            {
            register int no;
            register const char *save;

            no = OP(scan) - OPEN;
            save = m_RegInput;

            if (regmatch(next)) 
                {
                    /*
                    * Don't set startp if some later
                     * invocation of the same parentheses
                     * already has.
                     */
                if (m_RegStartP[no] == NULL)
                    m_RegStartP[no] = (char *)save;
                return(1);
                } 
            else
                return(0);
            }
            break;
        case CLOSE1:
        case CLOSE2:
        case CLOSE3:
        case CLOSE4:
        case CLOSE5:
        case CLOSE6:
        case CLOSE7:
        case CLOSE8:
        case CLOSE9: 
            {
            register int no;
            register const char *save;
            
            no = OP(scan) - CLOSE;
            save = m_RegInput;

            if (regmatch(next)) 
                {
                    /*
                     * Don't set endp if some later
                     * invocation of the same parentheses
                     * already has.
                     */
                if (m_RegEndP[no] == NULL)
                    m_RegEndP[no] = (char *)save;
                return(1);
                } 
            else
                return(0);
            }
            break;
        case BRANCH: 
            {
            register const char *save;

            if (OP(next) != BRANCH)     /* No choice. */
                next = OPERAND(scan);   /* Avoid recursion. */
            else 
                {
                do  {
                    save = m_RegInput;
                    if (regmatch(OPERAND(scan)))
                        return(1);
                    m_RegInput = save;
                    scan = regnext(scan);
                    } while (scan != NULL && OP(scan) == BRANCH);
                return(0);
                /* NOTREACHED */
                }
            }
            break;
        case STAR:
        case PLUS: 
            {
            register char nextch;
            register int no;
            register const char *save;
            register int min;

                /*
                 * Lookahead to avoid useless match attempts
                 * when we know what character comes next.
                 */
            nextch = '\0';
            if (OP(next) == EXACTLY)
                nextch = *OPERAND(next);
            min = (OP(scan) == STAR) ? 0 : 1;
            save = m_RegInput;
            no = regrepeat(OPERAND(scan));
            while (no >= min) 
                {
                /* If it could work, try it. */
                if (nextch == '\0' || *m_RegInput == nextch)
                    if (regmatch(next))
                        return(1);
                /* Couldn't or didn't -- back up. */
                no--;
                m_RegInput = save + no;
                }
            return(0);
            }
            break;
        case END:
            return(1);  /* Success! */
            break;
        case BLANK:
            if( isspace((BYTE)*m_RegInput) )
//            if(!IsSpace(*m_RegInput))
                return 0;
            break;  
        case BLANKM:
            {
            BOOL entre = FALSE;
            
            while(isspace((BYTE)*m_RegInput))   
//            while(IsSpace(*m_RegInput))   
                {
                entre = TRUE;
                m_RegInput++;
                }
                
            if(entre == FALSE)    
                return 0;
            }
            break;  
        case ALPHA:
            if(!IsAlpha(*m_RegInput))   
                return 0;
            break;  
        case ALPHANUM:
            if(!IsAlphaNum(*m_RegInput))   
                return 0;
            break;  
        case ALPHAWE:
            if(!isalpha(*m_RegInput))   
                return 0;
            break;  
        case DIGIT:
            if(!isdigit(*m_RegInput))   
                return 0;
            break;  
        case DIGITM:         
            {
            BOOL entre = FALSE;
            
            while(isdigit(*m_RegInput))   
                {
                entre = TRUE;
                m_RegInput++;
                }
                
            if(entre == FALSE)    
                return 0;
            }
            break;  
        case NALPHA:         
            {
            BOOL entre = FALSE;
            
            while(IsAlpha(*m_RegInput))   
                {
                entre = TRUE;
                m_RegInput++;
                }
                
            if(entre == FALSE)    
                return 0;
            }
            break;  
        case NALPHANUM:         
            {
            BOOL entre = FALSE;
            
            while(IsAlphaNum(*m_RegInput))   
                {
                entre = TRUE;
                m_RegInput++;
                }
                
            if(entre == FALSE)    
                return 0;
            }
            break;  
        case NALPHAWE:         
            {
            BOOL entre = FALSE;
            
            while(isalpha(*m_RegInput))   
                {
                entre = TRUE;
                m_RegInput++;
                }
                
            if(entre == FALSE)    
                return 0;
            }
            break;  
        default:
            regerror("memory corruption");
            return(0);
            break;
            }

        scan = next;
        }

    /*
     * We get here only if there's trouble -- normally "case END" is
     * the terminating point.
     */
    regerror("corrupted pointers");
    return(0);
    }

////////////////////////////////////////////////////////////////////////////
// - regrepeat - repeatedly match something simple, report how many
////////////////////////////////////////////////////////////////////////////

int CSGrep::regrepeat(char *p)
    {
    register int count = 0;
    register const char *scan;
    register char *opnd;

    scan = m_RegInput;
    opnd = OPERAND(p);
    switch (OP(p)) 
        {
    case ANY:
        count = (int)strlen(scan);
        scan += count;
        break;
    case EXACTLY:
        while (*opnd == *scan) 
            {
            count++;
            scan++;
            }
        break;
    case ANYOF:
        while (*scan != '\0' && strchr(opnd, *scan) != NULL) 
            {
            count++;
            scan++;
            }
        break;
    case ANYBUT:
        while (*scan != '\0' && strchr(opnd, *scan) == NULL) 
            {
            count++;
            scan++;
            }
        break;
    case BLANK:
        //if(IsSpace(*scan))
        //   {
        //   count++;
        //   scan++;
        //   }
        break;  
    case BLANKM:
        //while(IsSpace(*scan))
        //   {
        //   count++;
        //   scan++;
        //   }
        break;  
    case DIGIT:
        if(isdigit(*scan))
            {
            count++;
            scan++;
            }
        break;  
    case DIGITM:
        while(isdigit(*scan))
            {
            count++;
            scan++;
            }
        break;  
    case ALPHA:
            if(isalpha(*scan))
                {
                count++;
                scan++;
                }
            break;  
    case NALPHA:
            while(isalpha(*scan))
                {
                count++;
                scan++;
                }
            break;  
    case ALPHANUM:
            if(IsAlphaNum(*scan))
                {
                count++;
                scan++;
                }
            break;  
    case NALPHANUM:
            while(IsAlphaNum(*scan))
                {
                count++;
                scan++;
                }
            break;  
    case ALPHAWE:
            if(IsAlpha(*scan))
                {
                count++;
                scan++;
                }
            break;  
    case NALPHAWE:
            while(IsAlpha(*scan))
                {
                count++;
                scan++;
                }
            break;  
    default:        /* Oh dear.  Called inappropriately. */
        regerror("internal foulup");
        count = 0;  /* Best compromise. */
        break;
        }
    m_RegInput = scan;

    return(count);
    }

////////////////////////////////////////////////////////////////////////////
// - regnext - dig the "next" pointer out of a node
////////////////////////////////////////////////////////////////////////////

inline char *CSGrep::regnext(register char *p)
    {
    register int offset;

    if (p == &m_RegDummy)
        return(NULL);

    offset = NEXT(p);
    if (offset == 0)
        return(NULL);

    if (OP(p) == BACK)
        return(p-offset);
    else
        return(p+offset);
    }

#ifdef DEBUG
////////////////////////////////////////////////////////////////////////////
// - regdump - dump a regexp onto stdout in vaguely comprehensible form
////////////////////////////////////////////////////////////////////////////

void CSGrep::regdump(regexp *r)
    {
    register char *s;
    register char op = EXACTLY; /* Arbitrary non-END op. */
    register char *next;

    s = r->program + 1;
    while (op != END) 
        {   /* While that wasn't END last time... */
        op = OP(s);
        next = regnext(s);
        s += 3;
        if (op == ANYOF || op == ANYBUT || op == EXACTLY) 
            {
            /* Literal string, where present. */
            while (*s != '\0') 
                {
                putchar(*s);
                s++;
                }
            s++;
            }
        putchar('\n');
        }
    }

////////////////////////////////////////////////////////////////////////////
// - regprop - printable representation of opcode
////////////////////////////////////////////////////////////////////////////

char *CSGrep::regprop(char *op)
    {
    register const char *p = NULL;
    static char buf[50];

    (void) strcpy_s(buf, 50, ":" );

    switch (OP(op)) 
        {
    case BOL:
        p = "BOL";
        break;
    case EOL:
        p = "EOL";
        break;
    case ANY:
        p = "ANY";
        break;
    case ANYOF:
        p = "ANYOF";
        break;
    case ANYBUT:
        p = "ANYBUT";
        break;
    case BRANCH:
        p = "BRANCH";
        break;
    case EXACTLY:
        p = "EXACTLY";
        break;
    case NOTHING:
        p = "NOTHING";
        break;
    case BACK:
        p = "BACK";
        break;
    case END:
        p = "END";
        break;
    case OPEN1:
    case OPEN2:
    case OPEN3:
    case OPEN4:
    case OPEN5:
    case OPEN6:
    case OPEN7:
    case OPEN8:
    case OPEN9:
        {
        int n = (int)strlen(buf);
  //      sprintf_s(buf+n, 50-n, "OPEN%d", OP(op)-OPEN);
        sprintf(buf+n, "OPEN%d", OP(op)-OPEN);
        p = NULL;
        }
        break;
    case CLOSE1:
    case CLOSE2:
    case CLOSE3:
    case CLOSE4:
    case CLOSE5:
    case CLOSE6:
    case CLOSE7:
    case CLOSE8:
    case CLOSE9:
        { 
        int n = (int)strlen(buf);
//        sprintf_s(buf+n, 50-n, "CLOSE%d", OP(op)-CLOSE);
        sprintf(buf+n, "CLOSE%d", OP(op)-CLOSE);
        p = NULL;
        }
        break;
    case STAR:
        p = "STAR";
        break;
    case PLUS:
        p = "PLUS";
        break;
    default:
        regerror("corrupted opcode");
        break;
        }
#if defined(_MSC_VER)         
    if (p != NULL) strcat_s(buf, 50, p);
#else
    if( p != NULL ) strlcat(buf, p, 50);
#endif
    return(buf);
    }
#endif

////////////////////////////////////////////////////////////////////////////
/*
 * The following is provided for those people who do not have strcspn() in
 * their C libraries.  They should get off their butts and do something
 * about it; at least one public-domain implementation of those (highly
 * useful) string routines has been published on Usenet.
 */
#ifdef STRCSPN
/*
 * strcspn - find length of initial segment of s1 consisting entirely
 * of characters not from s2
 */

int strcspn(char *s1, char *s2)
    {
    register char *scan1;
    register char *scan2;
    register int count;

    count = 0;
    for (scan1 = s1; *scan1 != '\0'; scan1++) 
        {
        for (scan2 = s2; *scan2 != '\0';)   /* ++ moved down. */
            if (*scan1 == *scan2++)
                return(count);
        count++;
        }
    return(count);
    }
#endif

////////////////////////////////////////////////////////////////////////////

void CSGrep::regerror(const char* /*s*/)
    {
//#ifdef ERRAVAIL
//  error("regexp: %s", s);
//#else
//  sprintf(stderr, "regexp(3): %s\n", s);
//  exit(1);
/*
    note:   if the program using regexp(3) wants to handle the regexp
        errors, comment out the above and use this
*/
 /* return; */  /* let using program handle errors */

//#endif        
#ifdef _DEBUG
    //TCHAR error[255];
    
    //wsprintf(error,_T("%s"),CA2CT(s) );
    //::MessageBox(NULL,error,_T("* Error *"),MB_OK|MB_ICONSTOP);
		ASSERT(FALSE);
    //MessageBox(CString(s));
    /* NOTREACHED */
#endif
    }

////////////////////////////////////////////////////////////////////////////
// - regsub - perform substitutions after a regexp match
////////////////////////////////////////////////////////////////////////////

void CSGrep::regsub(const char* source,const char* sOrig,CStringA &dest)
    {
    register const char *src;
    register char *dst;
    register char c;
    register int no;
    register int len;
    
    if (m_ptrData == NULL || source == NULL) 
        {
        regerror("NULL parm to regsub");
        return;
        }
    if (UCHARAT(m_ptrData->program) != MAGIC) 
        {
        regerror("damaged regexp fed to regsub");
        return;
        }

    src = source; 
    //dst = dest;       
    int n = (int)(strlen(sOrig) + strlen(source) + dest.GetLength() + 255);
    dst = dest.GetBuffer(n);
    
    len = (int)(m_ptrData->startp[0] - sOrig);
    if(len > 0)
        {
        (void) strncpy_s(dst, n, sOrig, len);
        dst += len;
				n -= len;
        }
    while ((c = *src++) != '\0') 
        {
        //if (c == '&')
        //  no = 0;
        if (c == '?' && '0' <= *src && *src <= '9')
            no = *src++ - '0';
        else
            no = -1;
        if (no < 0) 
            {   /* Ordinary character. */             // era & en vez de ?
            if (c == '\\' && (*src == '\\' || *src == '?'))
                c = *src++;
            *dst++ = c;
						n--;
            } 
        else if (m_ptrData->startp[no] != NULL && m_ptrData->endp[no] != NULL) 
            {
            len = (int)(m_ptrData->endp[no] - m_ptrData->startp[no]);
            (void) strncpy_s(dst, n, m_ptrData->startp[no], len);
            dst += len;
						n -= len;
            if (len != 0 && *(dst-1) == '\0') 
                {   /* strncpy hit NUL. */
                regerror("damaged match string");
                return;
                }
            }
        }
    len = (int)(&sOrig[strlen(sOrig)] - m_ptrData->endp[0]);
    
    if(len > 0)
        {   
        (void) strncpy_s( dst, n, m_ptrData->endp[0], len );
        dst += len;
        }
    *dst++ = '\0';
    dest.ReleaseBuffer();
    }                                                                       
    
////////////////////////////////////////////////////////////////////////////
