/**
 * Simple experiment to test
 * mmap_private's ability to not reflect values
 * with two threads, with the changes being only reflected
 * in the mutator thread
 * 
 * @author Amol Moses Jha, jha8@purdue.edu
 */

#include <stdio.h>
#include <pthread.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

#define MMAP_SIZE 10

void setup();
void run();

void* gc_check(void* fd_ptr){
  int fd = *(int*)fd_ptr;
  char * mem = (char*)mmap(NULL, MMAP_SIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE, fd, 0);
  int i = 0;
  while(1){
    if(mem[i] != (i + '0')){  //Mutations should not be reflected because of MMAP_PRIVATE
      printf("Values mutated! Expected: %c but was %c\n", i + '0', mem[i] + '0');
      exit(1);
    }
    i = (i+1)%(MMAP_SIZE);
  }
}

void* mutate(void* fd_ptr){
  int fd = *(int*)fd_ptr;
  char * mem = (char*)mmap(NULL, MMAP_SIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE, fd, 0);
  int i = 0;
  while(1){
    mem[i] = mem[i] + 7; //Randomly mutate values
    i = (i+1)%(MMAP_SIZE);
  }
}

int main(int argc, char** argv) {
  setup();
  run();
}

void setup(){
  FILE * fp = fopen("thread_test_file", "w");
  for(int i = 0;i < MMAP_SIZE; i++){
    fprintf(fp, "%d", i);
  }
  fclose(fp);
}

void run(){
  pthread_t gc_thread;
  pthread_t mutator_thread;
  int fd = open("thread_test_file", O_RDWR);
  pthread_create(&gc_thread, NULL, gc_check, &fd);
  pthread_create(&mutator_thread, NULL, mutate, &fd);
  pthread_join(gc_thread, NULL);
  pthread_join(mutator_thread, NULL);
}


