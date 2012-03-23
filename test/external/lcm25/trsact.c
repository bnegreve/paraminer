/*  routines used FIM algorithms */
/* 2003/9/1 Takeaki Uno */
/* This program is available for only academic use.
   Neither commercial use, modification, nor re-distribution is allowed */

#ifndef _trsact_c_
#define _trsact_c_

#define QUEUE_INT int

#include<stdio.h>
#include"lib_e.c"

int LCM_Trsact_num;
int LCM_iter1=0,LCM_iter2=0,LCM_iter3=0,LCM_iter4=0, LCM_iter5=0;


/***********************************/
/*   print transactions            */
/***********************************/
void TRSACT_print ( ARY *T ){
  int i, j, e;
  QUEUE *Q = T->h;
  for ( i=0 ; i<T->num ; i++ ){
    for ( j=0 ; j<Q[i].t ; j++ ){
      e = Q[i].q[j];
      printf ("%d,", e);
    }
    printf (" (%d)\n", Q[i].end );
  }
}

/***********************************/
/* count items and transactions in the given file */
/* input: filename */
/* output: counters for items, #transactions */
/* set *Tnum to the number of transactions (!= # of newlines) */
/***********************************/
#define TRSACT_BUFSIZ 50000
ARY TRSACT_file_count ( char *fname, int *Tnum ){

  ARY E;
  int i, j, pp, item;
  int fc=TRSACT_BUFSIZ, p= TRSACT_BUFSIZ;
  char ch, *buf;
  FILE *fp;

  fopen2r ( fp, fname, "TRSACT_file_count");
  *Tnum=0; /* # transactions */
  ARY_init ( &E, sizeof(int) );
  malloc2 ( buf, char, TRSACT_BUFSIZ, "TRSACT_file_count", "buf");

  while (1){
    do {
      for ( pp=p,item=0 ; 1 ; item=item*10 +(int)(ch-'0') ){
        if ( p == fc ){
          if ( fc < TRSACT_BUFSIZ ) goto END;
          fc = fread(buf, 1, TRSACT_BUFSIZ, fp);
          if ( fc <= 0 ) goto END;
          p = 0;
        }
        ch = buf[p++];
        if ( (ch < '0') || (ch > '9')) break;
      }
      if ( p != pp+1 ){
        ARY_exp_const ( &E, item, 0 );
        ((int *)E.h)[item]++;
      }
    } while (ch != '\n');
    (*Tnum)++;  /* increase #transaction */
  }
  END:;
  fclose ( fp );
  free ( buf );
  return ( E );
}

/***********************************/
/* sort indices of frequent items by their frequency */
/* input: E:counter of items, th:minimum support */
/* output: permutation, #freq items */
/* change: counters (infreq -> -1 ) */
/* set *num to the size of database composed only of frequent items */
/* set *Enum to # frequent items */
/***********************************/
int *TRSACT_perm_freq_item ( ARY *E, int th, int *num, int *Enum ){
  int *Eq = E->h, *perm, i;
  *num = 0;
  *Enum = 0;
  malloc2 ( perm, int, E->num*2, "TRSACT_perm_freq_item", "perm");
  for ( i=0 ; i<E->num ; i++ ){
    if ( Eq[i] >= th ){
      perm[(*Enum)*2] = Eq[i];
      perm[(*Enum)*2+1] = i;
      (*Enum)++;
      *num += Eq[i];
    } else Eq[i] = -1;
  }
  qsort ( perm, *Enum, sizeof(int)*2, qsort_int_cmp_ );
  for ( i=0 ; i<*Enum ; i++ ){
    Eq[perm[i*2+1]] = i;
    perm[i] = perm[i*2+1];
  }
  return ( perm );
}

/***********************************/
/* load transaction database from file */
/* input: fname:filename, th:minimum support */
/* output: the size of database ( size of T ) */
/* set *T to the transactions loaded from file */
/***********************************/
int *TRSACT_load ( char *fname, ARY *T, int th ){
  QUEUE *Q;
  ARY E;
  int Tnum, Enum, bnum; /* #transactions, max item, #all freq items */
  int i, j, pp, item;
  QUEUE_INT *ibuf;  /* buffer for storing all items */
  int fc=TRSACT_BUFSIZ, p= TRSACT_BUFSIZ;
  int *perm;
  char ch, *buf;
  FILE *fp;
  
  E = TRSACT_file_count ( fname, &Tnum );
  int *Eq = E.h;
  LCM_Trsact_num = Tnum;
  perm = TRSACT_perm_freq_item ( &E, th, &bnum, &Enum );
  malloc2 ( buf, char, TRSACT_BUFSIZ, "TRSACT_load", "buf");
  fopen2r ( fp, fname, "TRSACT_load");

  ARY_init ( T, sizeof(QUEUE) );
  ARY_exp ( T, Tnum+2 );
  Q = T->h;
  malloc2 ( ibuf, QUEUE_INT, bnum+Tnum+4, "TRSACT_load", "ibuf");
    /* read file and store active elements in memory */
  i= Tnum = 0;

  while (1){
    Q[Tnum].q = &(ibuf[i]);
    j = i;
    START:;
    do {
      for ( pp=p,item=0 ; 1 ; item=item*10 +(int)(ch-'0') ){
        if ( p == fc ){
          if ( fc < TRSACT_BUFSIZ ) goto END;
          fc = fread(buf, 1, TRSACT_BUFSIZ, fp);
          if ( fc <= 0 ) goto END;
          p = 0;
        }
        ch = buf[p++];
        if ( (ch < '0') || (ch > '9')) break;
      }
      if ( p != pp+1 ){
        if ( Eq[item] >= 0 ){
          ibuf[i] = Eq[item];
          i++;
        }
      }
    } while (ch != '\n');
    if ( j == i ) goto START; /* no item is inserted */
    Q[Tnum].t = i-j;
    Q[Tnum].s = 0;      /* used for flag */
    if ( Q[Tnum].t>1 ){
      QUEUE_sort_ ( &Q[Tnum] );
      for ( item=0,j=1 ; j<Q[Tnum].t ; j++ ){
        if ( Q[Tnum].q[item] != Q[Tnum].q[j] ){
          item++;
          if ( item < j ) Q[Tnum].q[item]=Q[Tnum].q[j];
        }
      }
      Q[Tnum].q[item+1] = Enum;
    }
    Q[Tnum].end = 1;    /* multiplicity */
    Tnum++;
    ibuf[i] = Enum;   /* loop stopper */
    i++;
  }
  END:;
  T->num = Tnum;
  T->dellist = Enum;   /* store max_item: illigal use */
  free ( buf );
  ARY_end ( &E );
  fclose ( fp );
  return ( perm );
}


/*******************************************************/
/*       Occurrence Deliver                            */
/* input: T:transactions, max_item  */
/* for each item e<max_item, set O->h[e].q to the transactions including e,
 and set O->h[e].t to the # of such transactions */
/*******************************************************/
void TRSACT_occ_deliver_all( ARY *T, ARY *O, QUEUE_INT max_item){
  int t;
  QUEUE_INT *x;
  QUEUE *Q = T->h, *QQ = O->h, *QQQ;
  for ( t=0 ; t<T->num ; t++ ){
    for ( x=Q[t].q ; *x<=max_item ; x++ ){
      QQQ = &QQ[*x];
      ((long *)(QQ[*x].q))[QQ[*x].t++] = t;
    }
  }
}

/**************************************************************/
/* initialization of shrink operation */
/* input: T:transactions, Eend:# of items */
/* output: working space (array of long integers) */
/* alloc the working space to *jump (QUEUE) */
/**************************************************************/
long *TRSACT_shurink_init ( ARY *T, QUEUE_INT Eend, QUEUE *jump ){
  int i;
  long *p;
  malloc2 ( p, long, Eend*2+T->num*4+4, "TRSACT_shurink_init", "p");
  for ( i=0 ; i<Eend*2+2 ; i++ ) p[i] = -1;
  QUEUE_init ( jump, Eend*2+2 );
  jump->end = Eend;
  return (p);
}

/**************************************************************/
/* shrink T */
/* input: T:transactions, jump,q:working space(re-use for omiting initialization) */
/* find the identical transactions from T by radix sort (bucket sort) 
  and merge them into one transaction */
/* set T->h[i].end to the multiplicity of transaction i
     (# of transactions merged to i) */
/*************************************************************************/
void TRSACT_shrink ( ARY *T, QUEUE *jump, long *p ){
  int ii, j, t, tt, v, vv;
  QUEUE_INT *jt, *jtt, *jq=jump->q, *jqq=jump->q+jump->end+1;
  long *pp=&p[jump->end+1],*q=&p[jump->end*2+2],*qq=&p[jump->end*2+2+T->num*2];
  QUEUE *Q = T->h;
  
  for ( t=0,jtt=jqq ; t<T->num ; t++ ){
    ii = Q[t].q[0];
    if ( pp[ii] == -1 ){ *jtt = ii; jtt++; }
    qq[t*2] = pp[ii];
    qq[t*2+1] = 0;
    pp[ii] = t;
  }

  for ( j=1 ; jtt>jqq ; j++ ){
    for ( jt=jq ; jtt>jqq ; ){
      jtt--;
      if ( *jtt == jump->end ) goto END2;
      t = pp[*jtt];
      pp[*jtt] = -1;
      v = -1;
      do{
        tt = qq[t*2];
        if ( v != qq[t*2+1] ){
          v = qq[t*2+1];
          vv = t;
          if ( tt<0 ) goto END2;
          if ( qq[tt*2+1] != v ) goto END1;
        }
        ii = Q[t].q[j];
        if ( p[ii] == -1 ){ *jt = ii; jt++; }
        q[t*2] = p[ii];
        p[ii] = t;
        q[t*2+1] = vv;
        END1:;
        t = tt;
      } while ( tt>=0 );
      END2:;
    }

    if ( (tt=pp[jump->end]) < 0 ) goto END3;
    pp[jump->end] = -1;
    while (1){
      t = tt;
      v = qq[t*2+1];
      while (1){   /* shrink the same transactions into one */
        if ( (tt=qq[tt*2]) < 0 ) goto END3;
        if ( qq[tt*2+1] != v ) break;
        Q[t].end += Q[tt].end; /* add multiplicity */
        Q[tt].end = 0;
      }
    }

    END3:;
    SWAP_PNT ( p, pp );
    SWAP_PNT ( q, qq );
    SWAP_PNT ( jq, jqq );
    SWAP_PNT ( jt, jtt );
  }

  for ( t=j=0 ; t<T->num ; t++ ){
    if ( Q[t].end > 0 ){
      if ( t!= j) Q[j] = Q[t];
      j++;
    }
  }
  T->num = j;
}

/*****************************/
/* sort transactions by their sizes */
/* input: T:transactions, jump:list of frequnet items, p:working space */
/*****************************/
void TRSACT_sort_size ( ARY *T, QUEUE *jump, long *p ){
  int *q, t, tt, ii;
  QUEUE *Q = T->h, *Qtmp;
  QUEUE_INT *jq, *jqq;

  malloc2 ( Qtmp, QUEUE, T->num, "TRSACT_sort_size", "Qtmp"); 
  malloc2 ( q, int, T->num, "TRSACT_sort_size", "q"); 
  
  for ( t=0,jq=jump->q ; t<T->num ; t++ ){
    ii = Q[t].t;
    if ( p[ii] == -1 ){ *jq = ii; jq++; }  /* insert each tr */
    q[t] = p[ii];
    p[ii] = t;
  }

  jump->t = jq-jump->q;
  QUEUE_sort_ ( jump );  /* sort "sizes" */
  jump->t = 0;

//  for ( tt=0,jqq=jump->q ; jqq<jq ; jqq++ ){
  for ( tt=0,jqq=jq-1 ; jqq>=jump->q ; jqq-- ){
    for ( t=p[*jqq] ; t>=0 ; t=q[t] ){
      Qtmp[tt] = Q[t];
      tt++;
    }
    p[*jqq] = -1;
  }
  memcpy ( Q, Qtmp, sizeof(QUEUE)*T->num );

  free ( Qtmp );
  free ( q );
}


/**********************************************************************/
/*  Global Variables                                                  */
/**********************************************************************/

time_t LCM_start_time;
int LCM_maximum_time = 1000;

int *LCM_perm, LCM_Eend, LCM_th, LCM_frq, *LCM_ary;
int LCM_iters=0;
long *LCM_shrink_p;   /* working space for shrinking */
QUEUE LCM_shrink_jump;  /* working space for shrinking */
QUEUE LCM_jump, LCM_add, LCM_itemset;
QUEUE_INT *LCM_buf;
ARY LCM_Trsact, LCM_Occ; /* transactions, and denotations */
QUEUE *LCM_occ; /* LCM_occ := LCM_Occ.h */
int *LCM_sc;    /* # of itemsets of size i, and max.size of itemsets */
unsigned int tc1=0, tc2=0, c1=0, c2=0, cc1, cc2;
  /* print_flag&1:output to file &2:print messages &4 print solutions */
int LCM_problem = 0, LCM_method=0;
long *LCM_VBM_trsact;

#define LCM_FREQSET 1
#define LCM_CLOSED  2
#define LCM_MAXIMAL 3

#ifdef LCM_MESSAGES
int LCM_print_flag=2;
#else 
int LCM_print_flag=0;
#endif


/**********************************************************************/
/*  Fast Output                                                       */
/**********************************************************************/
/* FAST file output */
#define fprint_int_bufsiz 1000000
char fprint_int_buf[fprint_int_bufsiz], *fprint_int_pnt, *fprint_int_th;
FILE *fprint_int_fp;

void fprint_int_init ( char *filename ){
  fprint_int_pnt = fprint_int_buf;
  fprint_int_th = fprint_int_buf + fprint_int_bufsiz/2;
  fopen2w ( fprint_int_fp, filename, "fprint_int_init");
}
void fprint_int_end (){
  fwrite ( fprint_int_buf, 1, ((int)(fprint_int_pnt-fprint_int_buf)), fprint_int_fp );
  fclose ( fprint_int_fp );
}
void fprint_int_flush (){
  if ( fprint_int_pnt > fprint_int_th ){
    fwrite ( fprint_int_buf, (int)(fprint_int_pnt-fprint_int_buf), 1, fprint_int_fp );
    fprint_int_pnt = fprint_int_buf;
  }
}

void fprint_int ( int x ){
  int xx;
  char *p;
  if ( x==0 ){
    *fprint_int_pnt = '0';
    fprint_int_pnt++;
    return;
  }
  for ( xx=x ; xx>0 ; xx/=10 ) fprint_int_pnt++;
  p = fprint_int_pnt;
  while ( x>0 ){
    p--;
    *p = x%10+'0';
    x /= 10;
  }
}
void fprint_char ( char x ){
  *fprint_int_pnt = x;
  fprint_int_pnt++;
}

/*   print a solution(itemset) to file */
void LCM_output_itemset (){
  int i, e;
  QUEUE_FE_LOOP_ ( LCM_itemset, i, e ){
    fprint_int ( LCM_perm[e] );
    fprint_char (' ');
  }
  if ( (LCM_print_flag&4)==0 ){
    fprint_char ('(');
    fprint_int ( LCM_frq );
    fprint_char (')');
  }
  fprint_char ('\n');
  fprint_int_flush ();
}


void LCMFREQ_output_itemset ( int *q ){
  LCM_output_itemset ();
  for ( q-- ; q>=LCM_add.q ; q-- ){
    QUEUE_ins_ ( &LCM_itemset, *q );
    LCMFREQ_output_itemset ( q );
    LCM_itemset.t--;
  }
}

void LCM_solution (){
  if ( LCM_print_flag&1 ) LCM_output_itemset ();
  LCM_sc[LCM_itemset.t] ++;
}

/* process when find a solution (for LCM_freq) */
void LCMFREQ_solution (){
  if ( LCM_print_flag&1 ) LCMFREQ_output_itemset ( LCM_add.q+LCM_add.t );
  int i, cc=1, s = LCM_itemset.t;
  for ( i=0 ; i<=LCM_add.t ; i++ ){
    LCM_sc[s] += cc;
    cc = cc*(LCM_add.t - i) / (i+1);
    s++;
  }
}
//////////////////////////////////////////

/*******************************************************************/
/* add an item to the cullent solution and print it (for LCM_freq) */
/* input: item:item to be added, frq:frequency of obtained itemset */
/*******************************************************************/
void LCM_print_last ( int item, int frq ){
  int xx = LCM_frq;
  LCM_iters++;
  QUEUE_ins_ ( &LCM_itemset, LCM_perm[item] );
  LCM_frq = frq;
  LCMFREQ_solution ();
  LCM_itemset.t--;
  LCM_frq = xx;
}

/*******************************************************************/
/* output at the termination of the algorithm */
/* print #of itemsets of size k, for each k */
/* flag=1:all,closed,   0:maximal */
/*******************************************************************/
void LCM_output ( int flag ){
  int i, n, nn;
  if ( flag ) LCM_sc[0] = 1;
  for ( n=i=0 ; i<=LCM_Eend ; i++ ){
    n += LCM_sc[i];
    if ( LCM_sc[i] != 0 ) nn = i;
  }
  printf ("%d\n", n);
  for ( i=0 ; i<=nn ; i++ ) printf ("%d\n", LCM_sc[i] );
  if ( LCM_print_flag&2 ){
    printf ("iters=%d\n", LCM_iters);
    if ( LCM_problem == LCM_FREQSET )
        printf ("compress ratio = %f\n", ((double)n)/LCM_iters);
  }
  if ( (LCM_print_flag&1) == 0 || (LCM_print_flag&4) ) return;
  if ( flag ){
    fprint_char ('(');
    fprint_int ( LCM_Trsact_num );
    fprint_char (')');
    fprint_char ('\n');
  }
}

/*************************************************************************/
/* Common initialization for LCM, LCMfreq, LCMmax */
/*************************************************************************/
int LCM_init ( int argc, char *argv[] ){
  int i, n=0, m, c=1;
  QUEUE *Q;

  if ( argc<3 ){
    printf ("LCM: [-f] input-filename support [output-filename]\n");
    printf ("f:not output frequency\n");
    printf ("if the 1st letter of input-filename is '-', be considered as 'parameter list'\n");
    exit (1);
  }
  if ( argv[1][0] == '-' ){
    c++;
    if ( strchr( argv[1], 'f' ) ) LCM_print_flag |= 4;
  }
  LCM_start_time = time(NULL);
  LCM_th = atoi(argv[c+1]);
  LCM_print_flag |= (argc>c+2? 1: 0);
  LCM_perm = TRSACT_load ( argv[c], &LCM_Trsact, LCM_th );
  LCM_Eend = LCM_Trsact.dellist;
  LCM_buf = ((QUEUE *)(LCM_Trsact.h))->q;
  if ( LCM_Trsact.num == 0 ){
    if ( LCM_print_flag &2 ) printf ("there is no frequent itemset\n");
    ARY_end ( &LCM_Trsact );
    free ( LCM_perm );
    exit (0);
  }
  
  
  LCM_shrink_p = TRSACT_shurink_init( &LCM_Trsact, LCM_Eend, &LCM_shrink_jump);
  TRSACT_shrink ( &LCM_Trsact, &LCM_shrink_jump, LCM_shrink_p );
  Q = LCM_Trsact.h;
  QUEUE_INT *x = Q->q;

  if ( LCM_print_flag &2 )
     printf ("#transactions=%d, #item=%d #elements=%d\n", LCM_Trsact.num, LCM_Eend, n );

  malloc2 ( LCM_ary, int, LCM_Eend*2, "LCM_init", "LCM_ary" );
  malloc2 ( LCM_sc, int, LCM_Eend+1, "LCM_init", "LCM_sc" );
  QUEUE_init ( &LCM_jump, LCM_Eend*2+2 );
  LCM_jump.end = LCM_Eend;
  QUEUE_init ( &LCM_itemset, LCM_Eend );
  QUEUE_init ( &LCM_add, LCM_Eend*2+2 );
  ARY_init ( &LCM_Occ, sizeof(QUEUE) );
  ARY_exp ( &LCM_Occ, LCM_Eend-1 );
  LCM_occ = LCM_Occ.h;
  for ( i=0 ; i<LCM_Eend ; i++ ){
    LCM_sc[i] = LCM_occ[i].end = LCM_occ[i].s = LCM_occ[i].t = LCM_ary[i] = 0;
  }
  LCM_sc[LCM_Eend] = 0;
  for ( Q=LCM_Trsact.h,i=0 ; i<LCM_Trsact.num ; i++ ){
    Q[i].s = i;     /* shrinked queue */
    m = Q[i].end;
    for ( x=Q[i].q ; *x<LCM_Eend ; x++ ){
      LCM_occ[*x].t++;
      LCM_occ[*x].end += m;
    }
  }
  for ( i=0,n=0 ; i<LCM_Eend ; i++ ) n += LCM_occ[i].t;
  for ( i=0 ; i<LCM_Eend ; i++ ){
    LCM_occ[i].q = (QUEUE_INT *)malloc( sizeof(long) * (LCM_occ[i].t+2) );
    if ( LCM_occ[i].q == NULL ){
      printf ("LCM_init: memory short for LCM_occ[i].q");
      exit(1);
    }
    LCM_occ[i].t = 0;
  }

  TRSACT_occ_deliver_all( &LCM_Trsact, &LCM_Occ, LCM_Eend-1 );
  if ( LCM_print_flag&1 ) fprint_int_init ( argv[c+2] );
  return ( n );
}

/*************************************************************************/
/* Common end processing for LCM, LCMfreq, LCMmax */
/*************************************************************************/
void LCM_end (){
  int j, i, *x, n=0, m;
  QUEUE *Q;

  if ( LCM_print_flag&1 ) fprint_int_end ();
  QUEUE_end ( &LCM_jump );
  QUEUE_end ( &LCM_itemset );
  QUEUE_end ( &LCM_add );
  for ( i=0 ; i<LCM_Eend ; i++ ) QUEUE_end( &LCM_occ[i] );
  ARY_end ( &LCM_Occ );
  ARY_end ( &LCM_Trsact );
  free2 ( LCM_sc );
  free2 ( LCM_buf );
}

/*************************************************************************/
/* INPUT: O:occurrence, jump:items, th:support, frq:frequency, add:itemset
   OUTPUT: maximum item i s.t. frq(i)=frq
   OPERATION: remove infrequent items from jump, and 
    insert items i to "add" s.t. frq(i)=frq                              */
/*************************************************************************/
int LCM_jump_rm_infreq (ARY *O, QUEUE *jump, int th, int frq, QUEUE *add){
  QUEUE_INT *x, *xx, *e=jump->q+jump->t;
  QUEUE *Q = O->h;
  int flag=-1;
  for ( x=xx=(jump->q+jump->s) ; x!=e ; x++ ){
    if ( Q[*x].end >= th ){
      if ( Q[*x].end < frq ){
        if ( x != xx ) *xx = *x;
        xx++;
        continue;
      } else {
        QUEUE_ins_ ( add, *x );
        if ( *x > flag ) flag = *x;
      }
    }
    Q[*x].end = 0;
  }
  jump->t = xx-jump->q;
  return ( flag );
}
int LCM_jump_rm_infreq_ (ARY *O, QUEUE *jump, int th, int frq, QUEUE *add){
  QUEUE_INT *x, *xx, *e=jump->q+jump->t;
  QUEUE *Q = O->h;
  int flag=-1;
  for ( x=xx=(jump->q+jump->s) ; x!=e ; x++ ){
    if ( Q[*x].end >= th ){
      if ( Q[*x].end < frq ){
        if ( x != xx ) *xx = *x;
        xx++;
        continue;
      } else {
        if ( LCM_VBM_trsact[*x]&0x80000000 ){
          QUEUE_ins_ ( add, *x );
          if ( *x > flag ) flag = *x;
        }
      }
    }
    Q[*x].end = Q[*x].t = 0;
  }
  jump->t = xx-jump->q;
  return ( flag );
}

/*************************************************************************/
/* compute the frequency by occurrence deliver , for LCMmax */
/* INPUT: T:transactions, occ:occurences, max_item.
   OPERATION:
   1. for each item i<=max_item, 
        set O->h[i] to the list of transactions in occ including i,
   2. set jump to the list of items i s.t. at least one occurrence of occ 
      includes i.   */
/*************************************************************************/
int LCM_freq_calc(ARY *T,QUEUE *occ,ARY *O,QUEUE *jump,QUEUE_INT max_item){
  QUEUE_INT *x, *jq=&(jump->q[jump->t]);
  int ee, n=0;
  long *t=(long *)(occ->q), *tt=((long *)(occ->q))+occ->t;
  QUEUE *Q = T->h, *QQ = O->h, *QQQ;
  if ( T->num < max_item ){
    for (  ; t<tt ; t++ ){
      ee=Q[*t].end;    /* multiplicity of transaction t */
      for ( x=Q[*t].q ; *x<=max_item ; x++ ){
        QQQ = &QQ[*x];
        if ( QQQ->end == 0 ){ *jq=*x; jq++; }
        QQQ->end += ee; n++;
      }
    }
  } else {
    for ( ; t<tt ; t++ ){
      ee=Q[*t].end;    /* multiplicity of transaction t */
      for ( x=Q[*t].q ; *x<=max_item ; x++ ){ QQ[*x].end += ee; n++; }
    }
    for ( ee=max_item ; ee>=0 ; ee-- )
        if ( QQ[ee].end > 0 ) { *jq = ee; jq++; }
  }
  jump->t = jq-jump->q;
  return ( n );
}

int LCM_freq_calc_(ARY *T,QUEUE *occ,ARY *O,QUEUE *jump,QUEUE_INT max_item){
  QUEUE_INT *x, *jq=&(jump->q[jump->t]);
  int ee, n=0;
  long *t=(long *)(occ->q), *tt=((long *)(occ->q))+occ->t;
  QUEUE *Q = T->h, *QQ = O->h, *QQQ;
  if ( T->num < max_item ){
    for (  ; t<tt ; t++ ){
      ee=Q[*t].end;    /* multiplicity of transaction t */
      for ( x=Q[*t].q ; *x<=max_item ; x++ ){
        QQQ = &QQ[*x];
        if ( QQQ->end == 0 ){ *jq=*x; jq++; }
        QQQ->end += ee; n++;
        ((long *)(QQQ->q))[QQQ->t++] = *t;
      }
    }
  } else {
    for ( ; t<tt ; t++ ){
      ee=Q[*t].end;    /* multiplicity of transaction t */
      for ( x=Q[*t].q ; *x<=max_item ; x++ ){
        QQQ = &QQ[*x];
        QQQ->end += ee; n++;
        ((long *)(QQQ->q))[QQQ->t++] = *t;
      }
    }
    for ( ee=max_item ; ee>=0 ; ee-- )
        if ( QQ[ee].end > 0 ) { *jq = ee; jq++; }
  }
  jump->t = jq-jump->q;
  return ( n );
}

/*************************************************************************/
/* set TT to the transactions of occ with frequency in the given
  upper and lower bounds */
/* return #all freq items */
/*************************************************************************/
int LCM_mk_freq_trsact( ARY *TT, ARY *T, ARY *O, QUEUE *occ, int cnt,
                       QUEUE_INT item, QUEUE_INT max_item, int th, int frq ){
  QUEUE_INT *x, *buf, *bbuf, *b;
  long *t, *tt=((long *)(occ->q))+occ->t;
  int i, n=0;
  QUEUE *Q=T->h, *QQ, *OQ=O->h;

  ARY_init ( TT, sizeof(QUEUE) );
  ARY_exp (TT, occ->t-1 );
  QQ = TT->h;
  malloc2 ( buf, QUEUE_INT, cnt, "TRSACT_mk_freq_trsact", "buf");
  bbuf = buf;
  for ( t=(long *)(occ->q) ; t<tt ; t++ ){
    QQ[n].q = b = buf;
    x=Q[*t].q;
    if ( *x>item ) continue;
    for ( ; *x<=max_item ; x++ )
        if ( OQ[*x].end > 0 ){ *buf = *x; buf++; }
    if ( buf != b ){
      QQ[n].t = buf-b;
      QQ[n].end  = Q[*t].end;   /* multiplicity of transaction t */
      *buf = LCM_Eend;
      buf++;
      n++;
    }
  }
  TT->num = n;
  return ( buf-bbuf );
}

/*************************************************************************/
/* remove large items from jump and sort in increasing order
     INPUT: jump, max_item
     OPERATION: 1. for each item i>max_item in jump, remove i from jump
     2. set occ[e].s = occ[e].end for each i<max_item (preserve frequency)
     3. set occ[i].end = 0 for each i in jump
     4. sort items in jump (from jump.s to jump.t) in the increasing order */
/*************************************************************************/
void LCM_remove_large_items ( QUEUE *jump, int max_item, QUEUE *occ ){
  QUEUE_INT e;
  int i, ii=jump->s;
  QUEUE_FE_LOOP_ ( *jump, i, e ){
    if ( e<max_item ){
      occ[e].s = occ[e].end;
      if ( ii != i ) jump->q[ii] = e;
      ii++;
    }
    occ[e].end = 0;
  }
  LCM_jump.t = ii;
}

/*************************************************************************/
/* delete marks for pruning of non-closed itemsets 
   (items not making recursive call) */
/*************************************************************************/
void LCMclosed_pruning_del ( int tail, int upper_th ){
  while ( tail < LCM_add.t ){
    if ( LCM_add.q[LCM_Eend+LCM_add.q[LCM_add.t-1]] > upper_th ) break;
    LCM_add.t--;
    LCM_add.q[LCM_Eend+LCM_add.q[LCM_add.t]] = -1;
  }
}

/*************************************************************************/
/* set mark for pruning  of non-closed itemsets  
   (items not making recursive call) */
/*************************************************************************/
void LCMclosed_pruning_set ( int item, int mark_item, int qt ){
  int j = LCM_add.t;
  if ( mark_item < 0 ) return;
  LCM_add.t++;
  while ( qt < j ){
    if ( LCM_add.q[LCM_Eend+LCM_add.q[j-1]] >= mark_item ) break;
    LCM_add.q[j] = LCM_add.q[j-1];
    j--;
  }
  LCM_add.q[LCM_Eend+item] = mark_item;
  LCM_add.q[j] = item;
}

#endif
