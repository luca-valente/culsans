// TestName: read_non-cacheable_locations
// Feature: Each core reads a different non-cacheable memory location
// TestObjective:
// TestPrerequisite: none
// TestSequence:
// PassCriteria: the memory operations succeed
// ID: PROJ-??

#include <stdint.h>
typedef struct {
uint64_t variable_1;
uint32_t variable_2;
uint8_t  variable_3;
uint64_t variable_4;
} mystruct_t;

uint64_t variable_A __attribute__((section(".non_cached_region")));
uint64_t variable_B __attribute__((section(".non_cached_region")));
mystruct_t  variable_C __attribute__((section(".cached_region")));
uint64_t variable_D;

void read_non_cacheable_locations()
{
  uint64_t mhartid;
  asm volatile ("csrr %[reg], mhartid" : [reg] "=r" (mhartid));

  uint64_t core_local_variable = 0;

  if(mhartid == 0){
      core_local_variable = 1;
      variable_A = core_local_variable;
  }
  if(mhartid == 1){
      core_local_variable = 2;
      variable_B = core_local_variable;
  }
}

void thread_entry(int cid, int nc){ // Core ID, Number of Cores

  static volatile int count;

   __sync_synchronize();

   while(__sync_fetch_and_add(&count, 0) != cid)
     ;

   // critical section
   read_non_cacheable_locations();

   if (__sync_fetch_and_add(&count, 1) == nc-1)
       count = 0;

   __sync_synchronize();
}


int main()
{
  uint64_t mhartid;
  asm volatile ("csrr %[reg], mhartid" : [reg] "=r" (mhartid));

  if(mhartid == 1){
    if((variable_A == 1)&&(variable_B == 2)){
      return 0;
    }
  }else{
    { asm volatile ("wfi"); }
  }
    return 1;
}
