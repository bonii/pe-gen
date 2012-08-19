
/* This is AUTOMATICALLY GENERATED code!
 * It would be foolish to try editing it
 * (unless you're debugging C-Mix/II and know what you're doing, that is)
 *
 * Generation date: Wed May 23 20:28:30 2012
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cmix/speclib.h>
#if cmixSPECLIB_VERSION != 2011
#error Wrong speclib version
#endif

struct arg;

struct arg1;

struct name;

struct Func;

struct Prog;

struct arg {
  Code cmix;
  Code val;
  Code next;
};

struct arg1 {
  int val;
  struct arg1 *next;
};

struct name {
  char *name1;
  struct name *next;
};

struct Func {
  char *name1;
  struct name *params;
  struct Func *next;
};

struct Prog {
  struct Func *funcs;
};

static struct {
  struct {
    Code val;
    Code next;
  } arg;
} cmixMember;

static void
cmixPutName1(struct arg *cmixThis,Code cmixIt)
{
  cmixThis->cmix = cmixIt;
  cmixPutName0(&cmixThis->val,cmixMkExp("?.?",cmixIt,cmixMember.arg.val));
  cmixPutName0(&cmixThis->next,cmixMkExp("?.?",cmixIt,cmixMember.arg.next));
}

static int
cmix_follow1(cmixPointerCallback cmixCallback, cmixDataObjectCopy **cmixCopy, struct arg1 *cmixSrc)
{
   {
     if (!cmixCallback(cmixCopy,cmixSrc->next)) return 0;
   }
   return 1;
}

static int
cmix_follow2(cmixPointerCallback cmixCallback, cmixDataObjectCopy **cmixCopy, struct name *cmixSrc)
{
   {
     if (!cmixCallback(cmixCopy,cmixSrc->name1)) return 0;
   }
   {
     if (!cmixCallback(cmixCopy,cmixSrc->next)) return 0;
   }
   return 1;
}

static int
cmix_follow3(cmixPointerCallback cmixCallback, cmixDataObjectCopy **cmixCopy, struct Func *cmixSrc)
{
   {
     if (!cmixCallback(cmixCopy,cmixSrc->name1)) return 0;
   }
   {
     if (!cmixCallback(cmixCopy,cmixSrc->params)) return 0;
   }
   {
     if (!cmixCallback(cmixCopy,cmixSrc->next)) return 0;
   }
   return 1;
}

static int
cmix_follow4(cmixPointerCallback cmixCallback, cmixDataObjectCopy **cmixCopy, struct Prog *cmixSrc)
{
   {
     if (!cmixCallback(cmixCopy,cmixSrc->funcs)) return 0;
   }
   return 1;
}

static int cmix_follow5
    (cmixPointerCallback cmixCallback, cmixDataObjectCopy **cmixCopy, void *cmixVoid, unsigned cmixI) {
  struct name *(*const cmixSrc) = (struct name *(*))cmixVoid ;
  while ( cmixI-- )
   {
     if (!cmixCallback(cmixCopy,cmixSrc[cmixI])) return 0;
   }
  return 1;
}

static int cmix_follow6
    (cmixPointerCallback cmixCallback, cmixDataObjectCopy **cmixCopy, void *cmixVoid, unsigned cmixI) {
  struct name (*const cmixSrc) = (struct name (*))cmixVoid ;
  while ( cmixI-- )
   {
     if (!cmix_follow2(cmixCallback,cmixCopy,&cmixSrc[cmixI])) return 0;
   }
  return 1;
}

static int cmix_follow7
    (cmixPointerCallback cmixCallback, cmixDataObjectCopy **cmixCopy, void *cmixVoid, unsigned cmixI) {
  struct Func *(*const cmixSrc) = (struct Func *(*))cmixVoid ;
  while ( cmixI-- )
   {
     if (!cmixCallback(cmixCopy,cmixSrc[cmixI])) return 0;
   }
  return 1;
}

static int cmix_follow8
    (cmixPointerCallback cmixCallback, cmixDataObjectCopy **cmixCopy, void *cmixVoid, unsigned cmixI) {
  struct Prog *(*const cmixSrc) = (struct Prog *(*))cmixVoid ;
  while ( cmixI-- )
   {
     if (!cmixCallback(cmixCopy,cmixSrc[cmixI])) return 0;
   }
  return 1;
}

static int cmix_follow9
    (cmixPointerCallback cmixCallback, cmixDataObjectCopy **cmixCopy, void *cmixVoid, unsigned cmixI) {
  char *(*const cmixSrc) = (char *(*))cmixVoid ;
  while ( cmixI-- )
   {
     if (!cmixCallback(cmixCopy,cmixSrc[cmixI])) return 0;
   }
  return 1;
}

static char s_[] = "%s = ";
static Code dn;
static char a0[] = "a\0";
static char main1[] = "main";
static char main2[] = "main";
static int dyncond;
static struct name *global;
static struct arg *Uglobal0;
static cmixDataObject cmix_globals[] = {
	{0,&a0,sizeof a0, cmix_follow0},
	{1,&global,sizeof global, cmix_follow5},
	{2,&Uglobal0,sizeof Uglobal0, cmix_follow0}, {0} };

static Code test3(void);
static Code test4(struct Func *,Code,struct Prog *);
static Code test5(char *,Code);
void readProg(char *,struct Prog **);

static Code 
test3(void)
{
  struct name *currentg;
  struct name *newg;
  Code currentv;
  struct arg *newv;
  Code cmixSR;
  cmixDataObject cmix_locals[] = {
	{6,0,sizeof currentg, cmix_follow5},
	{7,0,sizeof newg, cmix_follow5},
	{8,0,sizeof newv, cmix_follow0}, {0} };
  cmix_locals[0].obj = &currentg;
  cmix_locals[1].obj = &newg;
  cmix_locals[2].obj = &newv;
  cmixPushLocals(cmix_locals);
  cmixPutName0(&currentv,cmixRequestName(1,0));
  cmixDeclare(cmixLocal,currentv,"struct arg *?",currentv);
cmixL1:
  currentg = global;
  cmixStmt("? = :&?'(struct arg *)0'",currentv,cmixLiftStructPtr(Uglobal0));
  goto cmixL2;
cmixL2:
  if( currentg != (struct name *)0 ) goto cmixL3;
	else goto cmixL4;
cmixL4:
  if( dyncond ) goto cmixL5;
	else goto cmixL6;
cmixL6:
  cmixSR = cmixMkExp("2 + ?",cmixLiftPtr(&Uglobal0->val));
  goto cmix_return;
cmixL5:
  newg = (struct name (*))cmixAllocStatic(3,sizeof(struct name ),1,cmix_follow6);
  newg->name1 = (char (*))cmixAllocStatic(4,sizeof(char ),(size_t )2,cmix_follow0);
  strcpy(newg->name1,(char const *)a0);
  { typedef struct arg cmixAloctype;
    cmixAloctype *cmixSA = malloc(sizeof(cmixAloctype));
  cmixPutName1(&cmixSA[0],cmixRequestName(2,0));
  cmixDeclare(cmixGlobal,cmixSA[0].cmix,"static struct arg ?",cmixSA[0].cmix);
    newv=cmixSA;
  }
  cmixStmt("? = ? + 20",cmixLiftPtr(&newv->val),cmixLiftPtr(&Uglobal0->val));
  newg->next = global;
  cmixStmt("? = :&?'(struct arg *)0'",cmixLiftPtr(&newv->next),cmixLiftStructPtr(Uglobal0));
  global = newg;
  Uglobal0 = newv;
  goto cmixL6;
cmixL3:
  printf((char const *)s_,currentg->name1);
  cmixStmt("printf((char const *)?,?->?)",dn,currentv,cmixMember.arg.val);
  cmixStmt("?->? = ?->? + 2",currentv,cmixMember.arg.val,currentv,cmixMember.arg.val);
  currentg = currentg->next;
  cmixStmt("? = ?->?",currentv,currentv,cmixMember.arg.next);
  goto cmixL2;
cmix_return:
  cmixPopLocals(3);
  return cmixSR;
}

static Code 
test4(struct Func *f,Code cmixRP2,struct Prog *p)
{
  struct name *param;
  struct name *currentg;
  Code currentv;
  Code iTmp;
  Code argv;
  Code cmixSR;
  cmixDataObject cmix_locals[] = {
	{11,0,sizeof param, cmix_follow5},
	{12,0,sizeof currentg, cmix_follow5},
	{9,0,sizeof f, cmix_follow7},
	{10,0,sizeof p, cmix_follow8}, {0} };
  cmix_locals[0].obj = &param;
  cmix_locals[1].obj = &currentg;
  cmix_locals[2].obj = &f;
  cmix_locals[3].obj = &p;
  cmixPushLocals(cmix_locals);
  cmixPutName0(&argv,cmixRequestName(3,0));
  cmixDeclare(cmixLocal,argv,"struct arg *?",argv);
  cmixStmt("? = ?",argv,cmixRP2);
  cmixPutName0(&currentv,cmixRequestName(1,0));
  cmixDeclare(cmixLocal,currentv,"struct arg *?",currentv);
  cmixPutName0(&iTmp,cmixRequestName(4,0));
  cmixDeclare(cmixLocal,iTmp,"int ?",iTmp);
cmixL7:
  currentg = global;
  cmixStmt("? = :&?'(struct arg *)0'",currentv,cmixLiftStructPtr(Uglobal0));
  param = f->params;
  goto cmixL8;
cmixL8:
  if( param != (struct name *)0 ) goto cmixL9;
	else goto cmixL10;
cmixL10:
  cmixStmt("? = ?",iTmp,test3());
  cmixSR = cmixMkExp("?",iTmp);
  goto cmix_return;
cmixL9:
  currentg->name1 = param->name1;
  currentg->next = (struct name (*))cmixAllocStatic(5,sizeof(struct name ),1,cmix_follow6);
  currentg = currentg->next;
  cmixStmt("?->? = ?->?",currentv,cmixMember.arg.val,argv,cmixMember.arg.val);
  cmixStmt("?->? = malloc(sizeof(struct arg ))",currentv,cmixMember.arg.next);
  cmixStmt("? = ?->?",currentv,currentv,cmixMember.arg.next);
  param = param->next;
  goto cmixL8;
cmix_return:
  cmixPopLocals(4);
  return cmixSR;
}

static Code 
test5(char *file,Code cmixRP2)
{
  struct Prog *p;
  struct Func *f;
  int iTmp;
  int iTmp1;
  Code iTmp2;
  Code argv;
  Code cmixSR;
  cmixDataObject cmix_locals[] = {
	{14,0,sizeof p, cmix_follow8},
	{15,0,sizeof f, cmix_follow7},
	{16,0,sizeof iTmp, cmix_follow0},
	{17,0,sizeof iTmp1, cmix_follow0},
	{13,0,sizeof file, cmix_follow9}, {0} };
  cmix_locals[0].obj = &p;
  cmix_locals[1].obj = &f;
  cmix_locals[2].obj = &iTmp;
  cmix_locals[3].obj = &iTmp1;
  cmix_locals[4].obj = &file;
  cmixPushLocals(cmix_locals);
  cmixPutName0(&argv,cmixRequestName(3,0));
  cmixDeclare(cmixLocal,argv,"struct arg *?",argv);
  cmixStmt("? = ?",argv,cmixRP2);
  cmixPutName0(&iTmp2,cmixRequestName(4,0));
  cmixDeclare(cmixLocal,iTmp2,"int ?",iTmp2);
cmixL11:
  readProg(file,&p);
  f = p->funcs;
  goto cmixL12;
cmixL12:
  if( f != (struct Func *)0 ) goto cmixL13;
	else goto cmixL14;
cmixL14:
  iTmp1 = strcmp((char const *)f->name1,(char const *)main2);
  if( iTmp1 == 0 ) goto cmixL15;
	else goto cmixL16;
cmixL16:
  cmixSR = cmixMkExp(" -1");
  goto cmix_return;
cmixL15:
  cmixStmt("? = ?",iTmp2,test4(f,cmixMkExp("?",argv),p));
  cmixSR = cmixMkExp("?",iTmp2);
  goto cmix_return;
cmixL13:
  iTmp = strcmp((char const *)f->name1,(char const *)main1);
  if( iTmp != 0 ) goto cmixL17;
	else goto cmixL14;
cmixL17:
  f = f->next;
  goto cmixL12;
cmix_return:
  cmixPopLocals(5);
  return cmixSR;
}

void
cmixMain(char *goalparam)
{
  static cmixDataObject cmix_params[1] = {{0}};
  Code cmix_fun;
  Code r;
  Code residual_arg;
  static const char cmixFunname[] = "test";
  cmixPutName0(&residual_arg,cmixRequestName(5,0));
  cmixPushFun();
  cmix_fun = cmixMkExp(cmixFunname);
  cmixFunHeading(cmix_fun,"int \n?(struct arg *?)",cmix_fun,residual_arg);
  cmixPutName0(&r,cmixRequestName(6,0));
  cmixDeclare(cmixLocal,r,"int ?",r);
  cmixStmt("? = ?",r,test5(goalparam,cmixMkExp("?",residual_arg)));
  cmixReturn("?",r);
  cmixPopFun();
}
int
main(int argc,char *argv[])
{
  extern int cmixRestruct;
  for ( ; argc > 2; argc--, argv++ ) {
    if ( argv[1][0] != '-' ) break ;
    if ( argv[1][1] == 'R' ) {
      cmixRestruct = 0 ;
    } else break ;
  }
  if ( argc != 2) {
    fprintf(stderr,"Expected 1 parameters\n");
    return 1 ;
  }
  cmixGenInit();
  cmixMain(argv[1]);
  cmixGenExit(stdout);
  return 0;
}

void
cmixGenInit()
{
  cmixSpeclibInit();
  cmixMember.arg.val=cmixRequestName(7,0);
  cmixMember.arg.next=cmixRequestName(8,0);
  cmixDeclare(cmixStruct,0,"struct arg");
  cmixDeclare(cmixMemberDecl,cmixMember.arg.val,"\n  int ?;",cmixMember.arg.val);
  cmixDeclare(cmixMemberDecl,cmixMember.arg.next,"\n  struct arg *?;",cmixMember.arg.next);
  cmixPutName0(&dn,cmixRequestName(9,0));
  cmixDeclare(cmixGinit,dn,"static char ?[] = \"%d\\n\"",dn);
  cmixDeclare(cmixGlobal,dn,"static char ?[]",dn);
  cmixPushGlobals(cmix_globals);
  return;
}
static struct cmixNameRec cmixNametable[] = {
	{"v",0,0},
	{"currentv",0,0},
	{"cmixHeap",0,0},
	{"argv",0,0},
	{"iTmp",0,0},
	{"residual_arg",0,0},
	{"r",0,0},
	{"val",0,0},
	{"next",0,0},
	{"dn",0,0},	};
static unsigned const cmixNametableS[] = {
	3,2,1,9,4,8,6,5,0,7,};

void
cmixGenExit(FILE *fp)
{
  fprintf(fp,"/* This program was generated by C-Mix/II\n"
	" *\n"
	" * THIS IS AUTOMATICALLY GENERATED CODE\n"
	" */\n"
	"#include <stdio.h>\n"
	"#include <stdlib.h>\n"
	"#include <string.h>\n"
	);
  fprintf(fp,
	"struct arg;\n"
);
  cmixUnparse(cmixNametable,cmixNametableS,10,fp);
}
