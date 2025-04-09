 #include <stdio.h>
 #include <inttypes.h>
 #include "xbrtime.h"
 #include "dataset1.h"
 #include "idx1.h"
 
static double RTSEC(void)
{
  double retval;
  struct __kernel_timespec t;
  rev_clock_gettime( 0, &t );
  retval = ((double)t.tv_nsec/(double)1.0e9);
  return retval;
}
 
 int main()
 {
   uint64_t *array;
   uint64_t *idx;
   int *target;
   double real_time = 0.0;
 
   if( ELEMS < ITERS ) {
     printf( "Number of elements is too small for iteration count; increase the number of elements or reduce the iteratoin count." );
     return 1;
   }
 
   /* Initialize the runtime */ 
   xbrtime_init();
 
   if ( xbrtime_mype() == 0 ){
     printf( "XBGAS RAND_ADD TEST. PEs = %d", xbrtime_num_pes() );
   }
 
   xbrtime_barrier_all();
 
   // array resides in symmetric heap space
   array = (uint64_t *)xbrtime_malloc( ELEMS * sizeof(uint64_t) );
 
   if ( array == NULL ){
     printf( "Array could not be allocated" );
     xbrtime_close();
     return 1;
   }
 
   // idx resides in symmetric heap space
   idx = (uint64_t *)xbrtime_malloc( ITERS * sizeof(uint64_t) );
 
   if ( idx == NULL ){
     printf( " Idx could not be allocated" );
     xbrtime_free( array );
     xbrtime_close();
     return 1;
   }
 
   // target resides in local PE memory
   target = (int *)(malloc( sizeof( int ) * ITERS ));
 
   if ( target == NULL ){
     printf( " Target could not be allocated" );
     xbrtime_free( array );
     xbrtime_free( idx );
     xbrtime_close();
     return 1;
   }
 
   if ( xbrtime_mype() == 0 ) {
     printf( "Initializing xBGAS data members" );
   }
 
   xbrtime_barrier_all();
 
   // Init the target array
   if ( xbrtime_num_pes() == 1 ){
     for( unsigned i=0; i< ITERS; i++ ){
       target[i] = 0;
     }
   } else {
     for( unsigned i=0; i< ITERS; i++ ){
       if( xbrtime_mype() == xbrtime_num_pes()-1 ){
         // last pe
         target[i] = 0;
       } else {
         target[i] = xbrtime_mype() + 1;
       }
     }
   }
 
   // Setup the Idx and array values
   for( unsigned i=0; i<ITERS; i++ ){
     idx[i] = input_idx[i];
   }
   for( unsigned i=0; i<ELEMS; i++ ){
     array[i] = input_data[i];
   }
 
   if( xbrtime_mype() == 0 ){
     printf( "Done initializing XBGAS data members" );
   }
 
   xbrtime_barrier_all();
 
   double kams = 0.0;
   uint64_t start = 0;
 
   real_time = -RTSEC();

   // Start the RAND_ADD test
   for( unsigned i=0; i<ITERS; i++ ){
     xbrtime_long_get((long *)(&start), (long *)(&idx[i]), 1, target[i]);
     start = (uint64_t) xbrtime_long_atomic_add((long *)(&array[start]), (long)(0x01), target[i]);
   }

   real_time = +RTSEC();
 
   kams = (double)((xbrtime_num_pes() * ITERS * 1.0e-3) / real_time);
 
   if ( xbrtime_mype() == 0 ){
     printf( " XBGAS Atomic RAND ADD Test: Complete " );
     printf( " Total iterations: %d ", ITERS * xbrtime_num_pes() );
     printf( " Time: %.6f seconds ", real_time);
     printf( " KAMS: %.9f Kilo AMOs/second", kams );
   }
 
   xbrtime_close();
 
 }
 
 /* EOF */