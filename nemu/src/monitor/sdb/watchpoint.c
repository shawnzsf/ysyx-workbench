/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include "sdb.h"

#define NR_WP 32

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  int value;
  char *expr;
  /* TODO: Add more members if necessary */

} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

WP *new_wp(){
  if(!free_){
    printf("No free watchpoints.\n");
    assert(0);
  }
  WP* new = free_;
  free_ = free_ -> next;
  head = new;
  return new;
}

void free_wp(WP *wp){
  WP *flag = &wp_pool[0];
  if(wp == head) head = NULL;
  else{
    while(flag && flag->next != head) flag = flag->next;
    flag->next = wp->next;
  }
  free_ = wp;
}

void set_wp(char *expr, int result){
  WP* wp = new_wp();
  wp->expr = strndup(expr, strlen(expr));
  //wp->expr = expr
  wp->value = result;
  printf("Set watchpoint %d: %s\n", wp->NO, expr);
}

void delete_wp(int NO){
  assert(NO < NR_WP);
  WP* wp = &wp_pool[NO];
  WP* bwp = &wp_pool[NO-1];
  WP* pwp = &wp_pool[NO+1];
  printf("Delete watchpoint %d: %s\n", wp->NO, wp->expr);
  free_wp(wp);
  wp->expr = NULL;
  bwp->next = wp;
  wp->next = pwp;
}

void display_wp(){
  WP* first = &wp_pool[0];
  printf("%-8s%-8s\n", "Number", "Expression");
  while(first){
    printf("%-8d%-8s\n", first->NO, first->expr);
    first = first->next;
  }
}

void check_wp(){
  WP* first = &wp_pool[0];
  while(first != head + 1){
    bool success;
    int new = expr(first->expr, &success);
    if(first->value != new){
      printf("Watchpoint %d: %s\n", first->NO, first->expr);
      printf("Old value = %d\n", first->value);
      printf("New value = %d\n", new);
      nemu_state.state = NEMU_STOP;
    }
    first = first->next;
  }
}
/* TODO: Implement the functionality of watchpoint */

