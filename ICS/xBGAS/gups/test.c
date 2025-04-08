
#include <xbrtime.h>


int main(int argc, char *argv[])
{
    int myid, numprocs, i;
    char *s_buf, *r_buf;
    double t_start = 0, t_end = 0;
    size_t size = 64;

    xbrtime_init();
    myid = xbrtime_mype();
    numprocs = xbrtime_num_pes();

    s_buf = (char *)xbrtime_malloc(size);
    r_buf = (char *)xbrtime_malloc(size);

    memset(s_buf, 0, size);
    memset(r_buf, 1, size);
    
    printf("Hello from %d of %d\n", myid, numprocs);
    
    xbrtime_barrier_all();

    if (myid == 0) {
      xbrtime_getmem(r_buf, s_buf, size, 1);
    }

    xbrtime_barrier_all();

    if (myid == 0) {
      printf("r_buf[0] = %d\n", r_buf[0]);
    }

    xbrtime_barrier_all();

    xbrtime_free(s_buf);
    xbrtime_free(r_buf);

    xbrtime_close();
}

