/* Linear time Closed itemset Miner for Frequent Itemset Mining problems */
/* 2004/4/10 Takeaki Uno */
/* This program is available for only academic use.
   Neither commercial use, modification, nor re-distribution is allowed */

#ifndef _lcm_freq_c_
#define _lcm_freq_c_

#include<time.h>
#include"trsact.c"


/*************************************************************************/
/* LCMfreq iteration (lcm_closed-base type ) */
/* input: T:transactions(database), item:tail(current solution) */
/*************************************************************************/
void LCMfreq_iter( ARY *T, int item ){
  ARY TT;
  int i, ii, im, e, ee, n;
  int js = LCM_jump.s, it = LCM_itemset.t, qt = LCM_add.t;
  QUEUE_INT *q;

  LCM_jump.s = LCM_jump.t;
  LCM_frq = LCM_occ[item].end;
  LCM_iters++;
  QUEUE_ins_ ( &LCM_itemset, LCM_perm[item] );
  n = LCM_freq_calc( T, &LCM_occ[item], &LCM_Occ, &LCM_jump, item-1 );
  LCM_jump_rm_infreq ( &LCM_Occ, &LCM_jump, LCM_th, LCM_frq, &LCM_add );
  for ( i=qt ; i<LCM_add.t ; i++ )
      LCM_add.q[i] = LCM_perm[LCM_add.q[i]];
  LCMFREQ_solution ();

  if ( QUEUE_LENGTH(LCM_jump) == 0 ) goto END2;
  QUEUE_sortr_ ( &LCM_jump );


  i = QUEUE_ext_tail_ ( &LCM_jump );
  LCM_print_last ( i, LCM_occ[i].end );

/******************************************************/

  LCM_mk_freq_trsact(&TT, T, &LCM_Occ, &LCM_occ[item], n+LCM_occ[item].t, LCM_jump.q[LCM_jump.s], LCM_jump.q[LCM_jump.s], LCM_th, LCM_frq);
  q = ((QUEUE *)(TT.h))->q;
  if ( QUEUE_LENGTH(LCM_jump) >= 2 && TT.num>7 )
    TRSACT_shrink ( &TT, &LCM_shrink_jump, LCM_shrink_p );
  TRSACT_occ_deliver_all( &TT, &LCM_Occ, item-1 );
  LCM_occ[i].t = LCM_occ[i].end = 0;

  while ( QUEUE_LENGTH(LCM_jump) > 0 ){
    i = QUEUE_ext_tail_ ( &LCM_jump );
    LCMfreq_iter ( &TT, i);
    LCM_occ[i].t = LCM_occ[i].end = 0;
  }

  free2 ( q );
  ARY_end ( &TT );
  END2:;
  LCM_jump.t = LCM_jump.s;
  LCM_jump.s = js;
  LCM_itemset.t = it;
  LCM_add.t = qt;
}

/*************************************************************************/
/* LCM_freq main (lcm_closed-base type ) */
/*************************************************************************/
void LCMfreq (){
  int i;
  for ( i=0 ; i<LCM_Eend ; i++ ){
    LCMfreq_iter ( &LCM_Trsact, i );
    LCM_occ[i].t = LCM_occ[i].end = 0;
  }
  LCM_iters++;
}

/******************************************************************/
/* main */
/******************************************************************/
main ( int argc, char *argv[] ){
  int i, n;

  LCM_problem = 1;
  LCM_init ( argc, argv );
  LCMfreq ();
  LCM_output ( 1 );
  LCM_end ();
  exit (0);
}

/******************************************************************/
/******************************************************************/

#endif


