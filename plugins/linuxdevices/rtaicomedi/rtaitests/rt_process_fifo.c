//#define USEFIFO 1

#include <linux/module.h>
#include <asm/io.h>
#include <rtai.h>
#include <rtai_sched.h>
#ifdef USEFIFO
#include <rtai_fifos.h>
#endif

MODULE_LICENSE( "GPL" );
MODULE_DESCRIPTION( "RTAI test periodic task" );

#define TICK_PERIOD 1000000
#define TASK_PRIORITY 1
#define STACK_SIZE 10000
#ifdef USEFIFO
#define FIFO 0
#define FIFOSIZE 50*sizeof(int)
#endif
 
static RT_TASK rt_task;


static void fun(long int t)
{
  int counter = 0;
  int r = 0;
  int fifo_ok = 1;
  while (1) {
#ifdef USEFIFO
    if ( fifo_ok ) {
      r = rtf_put(FIFO, &counter, sizeof(counter));
      if ( r < sizeof(counter) ) {
	rt_printk( "FIFO OVERFLOW at counter=%d\n", counter );
	fifo_ok = 0;
      }
    }
#else
    if ( counter % 100 == 0 )
      rt_printk( "counter=%d\n", counter );
#endif
    counter++;
    rt_task_wait_period();
  }
}

int init_module(void)
{
  RTIME tick_period;
  rt_printk( "LOADED RT_PROCESS\n" );
  /* rt_set_periodic_mode();*/
  rt_task_init(&rt_task, fun, 1, STACK_SIZE, TASK_PRIORITY, 1, 0);
#ifdef USEFIFO
  rtf_create(FIFO, FIFOSIZE);
#endif
  tick_period = start_rt_timer(nano2count(TICK_PERIOD));
  rt_task_make_periodic(&rt_task, rt_get_time() + tick_period,       tick_period);
  return 0;
}

void cleanup_module(void)
{
  stop_rt_timer();
#ifdef USEFIFO
  rtf_destroy(FIFO);
#endif
  rt_task_delete(&rt_task);
  rt_printk( "CLEANED UP RT_PROCESS\n" );
  return;
}

