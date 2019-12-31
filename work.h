#define THREAD_COUNT 16

void *work();

struct WorkItem {
  void (*work)(void *work_args);
  void *work_args;
  struct WorkItem *next;
  int *batch_counter;
  pthread_mutex_t *batch_counter_lock;
  pthread_cond_t *batch_complete_cond;
};

struct WorkLibSpec {
  void (*init)();
  void (*do_batch)(int num_work_items, void (*work)(void *work_args), void *work_args_arr_p);
};
extern const struct WorkLibSpec WorkLib;
