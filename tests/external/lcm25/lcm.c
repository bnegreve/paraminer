/* Linear time Closed itemset Miner for Frequent Itemset Mining problems */
/* 2004/4/10 Takeaki Uno   e-mail:uno@nii.jp, 
    homepage:   http://research.nii.ac.jp/~uno/index.html  */
/* This program is available for only academic use, basically.
   Anyone can modify this program, but he/she has to write down 
    the change of the modification on the top of the source code.
   Neither contact nor appointment to Takeaki Uno is needed.
   If one wants to re-distribute this code, do not forget to 
    refer the newest code, and show the link to homepage of 
    Takeaki Uno, to notify the news about LCM for the users.
   For the commercial use, please make a contact to Takeaki Uno. */


#ifndef _lcm_c_
#define _lcm_c_

#include<time.h>
#include"trsact.c"

ARY *LCM_Trsact2;


int qsort_QUEUEt_cmp ( const void *x, const void *y ){
  if ( ((QUEUE *)x)->t < ((QUEUE *)y)->t ) return (-1);
  else return ( ((QUEUE *)x)->t > ((QUEUE *)y)->t  );
}


/**************************************************************/
/* shrink T for LCM ver 2 */
/* find the identical transactions from T by radix sort (bucket sort) 
  and merge them into one transaction, and compute the interior intersection */
/* input: T:transactions, jump,q:working space(re-use for omiting initialization) */
/* set T->h[i].end to the multiplicity of transaction i
     (# of transactions merged to i) */
/*************************************************************************/
void LCM_shrink ( ARY *T, QUEUE *jump, long *p, int max_item ){
  int ii, j, t, tt, v, vv;
  QUEUE_INT *jt, *jtt, *jq=jump->q, *jqq=jump->q+jump->end+1;
  long *pp=&p[jump->end+1],*q=&p[jump->end*2+2],*qq=&p[jump->end*2+2+T->num*2];
  QUEUE *Q = T->h;
  QUEUE_INT *x1, *xx, *x2;
  
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
      if ( *jtt == max_item ) goto END2;
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

    if ( (tt=pp[max_item]) < 0 ) goto END3;
    pp[max_item] = -1;
    while (1){
      t = tt;
      v = qq[t*2+1];
      while (1){
        tt = qq[tt*2];
        if ( tt < 0 ) goto END3;
        if ( qq[tt*2+1] != v ) break;
        Q[t].end += Q[tt].end;
        Q[tt].end = 0;

          /* intersection of items>max_item */
        for ( xx=x1=(Q[t].q+j),x2=Q[tt].q+j ; *x1<jump->end ; x1++ ){
          while ( *x2<*x1 ) x2++;
          if ( *x2 == *x1 ){
            if ( x1!=xx ) *xx=*x1;
            xx++;
            x2++;
          }
        }
        *xx = jump->end;
        Q[t].t = xx - Q[t].q;
      }
      END4:;
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

/* make bitmap for closure */
int LCMclosed_VBM_iter ( int item, long vrt ){
  ARY TT;
  QUEUE *oc = &LCM_occ[item];
  int i, ii, js=LCM_jump.s, itemt=LCM_itemset.t, qt=LCM_add.t;
  QUEUE_INT *q, *qq;

  vrt = vrt & LCM_VBM_trsact[item];
  for ( q=((QUEUE *)(LCM_Trsact2->h))[*(oc->q)].q+((QUEUE *)(LCM_Trsact2->h))[*(oc->q)].t-1 ; *q>item ; q-- ){
    if ( ((LCM_VBM_trsact[*q]&vrt)==vrt)) return(*q);
  }

  LCM_jump.s = LCM_jump.t;
  LCM_iters++;
  QUEUE_ins_ ( &LCM_itemset, item );
  LCM_frq = oc->end;

  LCM_freq_calc_ ( LCM_Trsact2, oc, &LCM_Occ, &LCM_jump, item-1 );
  LCM_jump_rm_infreq_ (&LCM_Occ, &LCM_jump, LCM_th, LCM_frq, &LCM_itemset);
  LCM_solution ();
  if ( QUEUE_LENGTH(LCM_jump)==0 ) goto END;   /* no rec.call */
  for ( i=itemt ; i<LCM_itemset.t ; i++ )
      LCM_VBM_trsact[LCM_itemset.q[i]] &= 0x7fffffff;
  QUEUE_sortr_ ( &LCM_jump );

  do {
    i = QUEUE_ext_tail_ ( &LCM_jump );
    LCMclosed_pruning_del ( qt, i );
    if ( LCM_add.q[LCM_Eend+i] == -1 ){
      ii = LCMclosed_VBM_iter ( i, vrt );
      LCMclosed_pruning_set ( i, ii, qt );
    }
    LCM_occ[i].t = LCM_occ[i].end = 0;
  } while ( LCM_jump.t > LCM_jump.s );
  LCMclosed_pruning_del ( qt, LCM_Eend );

  END:;
  for ( ; LCM_itemset.t>itemt ; LCM_itemset.t-- )
      LCM_VBM_trsact[LCM_itemset.q[LCM_itemset.t-1]] |= 0x80000000;
  LCM_jump.s = js;
  return ( -1 );
}




/* make virtical bitmap for closure */
void LCMclosed_VBM ( ARY *T, int item ){
  int i, y, ii, qt=LCM_add.t;
  QUEUE_INT *x, *xx;
  QUEUE *Q = T->h;
  
  for ( i=0 ; i<T->num ; i++ ){
    for ( x=Q[i].q,xx=Q[i].q+Q[i].t ; x<xx ; x++ )
       LCM_VBM_trsact[*x] &= 0x80000000;
  }
  for ( i=LCM_jump.s ; i<LCM_jump.t ; i++ ){
    LCM_occ[LCM_jump.q[i]].end = LCM_occ[LCM_jump.q[i]].s;
  }
  for ( i=0 ; i<T->num ; i++ ){
    y = BITMASK_1[i];
    for ( x=Q[i].q,xx=Q[i].q+Q[i].t ; x<xx ; x++ ) LCM_VBM_trsact[*x] |= y;
  }
  LCM_Trsact2 = T;

  do {
    i = QUEUE_ext_tail_ ( &LCM_jump );
    LCMclosed_pruning_del ( qt, i );
    if ( LCM_add.q[LCM_Eend+i] == -1 ){
      ii = LCMclosed_VBM_iter ( i, 0xffffffff);
      LCMclosed_pruning_set ( i, ii, qt );
    }
    LCM_occ[i].t = LCM_occ[i].end = 0;
  } while ( LCM_jump.t > LCM_jump.s );
  LCMclosed_pruning_del ( qt, LCM_Eend );
}






/***************************************************************/
/* iteration of LCM ver. 2 */
/* INPUT: T:transactions(database), item:tail of the current solution */
/*************************************************************************/
int LCMclosed_iter ( ARY *T, int item, int prv ){
  ARY TT;
  QUEUE *oc = &LCM_occ[item];
  int i, ii, e, ee, n, js=LCM_jump.s, itemt=LCM_itemset.t;
  int flag=-1, tt, qt=LCM_add.t;
  QUEUE_INT *q;

  LCM_jump.s = LCM_jump.t;
  LCM_iters++;
  QUEUE_ins_ ( &LCM_itemset, item );
  LCM_frq = oc->s;

  n = LCM_freq_calc ( T, oc, &LCM_Occ, &LCM_jump, LCM_Eend-1 );
  if ( LCM_itemset.t >= 2 ) LCM_occ[prv].end = 0;
  oc->end = 0;
  ii = LCM_jump_rm_infreq (&LCM_Occ, &LCM_jump, LCM_th, LCM_frq, &LCM_itemset);
  if ( ii > item ){ flag = ii; goto END2; }  /* itemset is not closed */
  LCM_solution ();
  if ( LCM_itemset.q[0] >= LCM_Eend ) printf (" okasii \n");
  for ( i=itemt ; i<LCM_itemset.t ; i++ )
      LCM_VBM_trsact[LCM_itemset.q[i]] &= 0x7fffffff;
  if ( QUEUE_LENGTH(LCM_jump)==0 ) goto END2;   /* no rec.call */
  ii=0,ee=-1; QUEUE_FE_LOOP_ ( LCM_jump, i, e ){
    if ( e<item ){
      ii++;
      if ( e>ee ) ee = e;
    }
  }
  if ( ii==0 ) goto END2; /* no rec.call */

  oc->end = LCM_th;
  LCM_mk_freq_trsact(&TT, T, &LCM_Occ, oc, n+LCM_occ[item].t, ee, LCM_Eend-1, LCM_th, LCM_frq);
  q = ((QUEUE *)(TT.h))->q;
  if ( ii >= 2 && TT.num>5 )
      LCM_shrink ( &TT, &LCM_shrink_jump, LCM_shrink_p, item );
  LCM_remove_large_items ( &LCM_jump, item, LCM_occ );
  QUEUE_sortr_ ( &LCM_jump );
#if 1
  if ( TT.num<32 ){
    qsort ( TT.h, TT.num, sizeof(QUEUE), qsort_QUEUEt_cmp );
    TRSACT_occ_deliver_all( &TT, &LCM_Occ, item-1 );
    LCMclosed_VBM ( &TT, item );
    goto END1;
  }
#endif
  TRSACT_occ_deliver_all( &TT, &LCM_Occ, item-1 );

  do {
    i = QUEUE_ext_tail_ ( &LCM_jump );
#if 1
    LCMclosed_pruning_del ( qt, i );
    if ( LCM_add.q[LCM_Eend+i] == -1 ){
      ii = LCMclosed_iter ( &TT, i, item);
      LCMclosed_pruning_set ( i, ii, qt );
    }
#else
    LCMclosed_iter ( &TT, i, item);
#endif
    LCM_occ[i].t = LCM_occ[i].end = 0;
  } while ( LCM_jump.t > LCM_jump.s );
  LCMclosed_pruning_del ( qt, LCM_Eend );

  END1:;
  free2 ( q );
  ARY_end ( &TT );
  goto END3;
  END2:;
  while ( QUEUE_LENGTH(LCM_jump)>0 ){
    LCM_jump.t--;
    LCM_occ[LCM_jump.q[LCM_jump.t]].end = 0;
  }
  END3:;
  for ( ; LCM_itemset.t>itemt ; LCM_itemset.t-- )
      LCM_VBM_trsact[LCM_itemset.q[LCM_itemset.t-1]] |= 0x80000000;
  LCM_jump.s = js;
  return ( flag );
}

/***************************************************************/
/* main of LCM ver. 2 */
/*************************************************************************/
void LCMclosed (){
  int i;
  for ( i=0 ; i<LCM_Eend ; i++ ){
    LCM_occ[i].s = LCM_occ[i].end;
    LCM_occ[i].end = 0;
  }
  for ( i=0 ; i<LCM_Eend ; i++ ){
    if ( LCM_occ[i].s >= LCM_th )
        LCMclosed_iter ( &LCM_Trsact, i, -1 );
    LCM_occ[i].t = LCM_occ[i].end = 0;
  }
  LCM_iters++;
}

/*************************************************************************/
/*************************************************************************/
main ( int argc, char *argv[] ){
  int i, f=1;
  LCM_init ( argc, argv );

  if ( LCM_occ[0].end == LCM_Trsact_num ) f = 0;
  malloc2 ( LCM_VBM_trsact, long, LCM_Eend, "main", "LCM_VBM_trsact");
  for ( i=0 ; i<LCM_Eend ; i++ ){
    LCM_add.q[i+LCM_Eend] = -1;
    LCM_VBM_trsact[i] = 0x80000000;
  }
  LCMclosed ();
  LCM_output ( f );
  LCM_end ();

  free2 ( LCM_VBM_trsact );

  exit (0);
}


#endif


