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

#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ,
  TK_NUM, TK_NEG, TK_HEX, TK_REG, 
  TK_NEQ, TK_DEREF, TK_AND, 
  TK_BT, TK_LT, TK_BE, TK_LE,
  TK_OR, TK_NOT,
  /* TODO: Add more token types */

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +",          TK_NOTYPE   },               // spaces
  {"\\+",         '+'         },               // plus
  {"\\-",         '-'         },               // minus
  {"\\*",         '*'         },               // times
  {"\\/",         '/'         },               // divide
  {"\\(",         '('         },               // left-parentheses
  {"\\)",         ')'         },               // right-parentheses
  {"[0][x]",      TK_HEX      },               // hexadecimal-number
  {"[0-9A-F]+",   TK_NUM      },               // decimal-number
  {"\\$",         TK_REG      },               // register
  {"\\&&",        TK_AND      },               // AND
  {"\\|\\|",      TK_OR       },               // OR
  {"\\!",         TK_NOT      },               // NOT
  {"==",          TK_EQ       },               // equal 
  {"!=",          TK_NEQ      },               // not equal 
  {"\\>=",        TK_BE       },               // bigger or equal
  {"\\<=",        TK_LE       },               // less or equal
  {"\\<",         TK_LT       },               // less than
  {"\\>",         TK_BT       },               // bigger than
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case '+':         tokens[nr_token].type = rules[i].token_type;
          case '-':         tokens[nr_token].type = rules[i].token_type;
          case '*':         tokens[nr_token].type = rules[i].token_type;
          case '/':         tokens[nr_token].type = rules[i].token_type;
          case '(':         tokens[nr_token].type = rules[i].token_type;
          case ')':         tokens[nr_token].type = rules[i].token_type;
          case TK_REG:      tokens[nr_token].type = rules[i].token_type;
          case TK_EQ:       tokens[nr_token].type = rules[i].token_type;
          case TK_NEQ:      tokens[nr_token].type = rules[i].token_type;
          case TK_BE:       tokens[nr_token].type = rules[i].token_type;
          case TK_BT:       tokens[nr_token].type = rules[i].token_type;
          case TK_LE:       tokens[nr_token].type = rules[i].token_type;
          case TK_LT:       tokens[nr_token].type = rules[i].token_type;
          case TK_AND:      tokens[nr_token].type = rules[i].token_type;
          case TK_OR:       tokens[nr_token].type = rules[i].token_type;
          case TK_NOT:      tokens[nr_token].type = rules[i].token_type;
          case TK_HEX: 
                            tokens[nr_token].type = rules[i].token_type;
                            strncpy(tokens[nr_token++].str, substr_start, substr_len);
                            tokens[nr_token].str[substr_len] = '\0';
                            break;
          case TK_NUM:
                            tokens[nr_token].type = rules[i].token_type;
                            strncpy(tokens[nr_token++].str, substr_start, substr_len);
                            tokens[nr_token].str[substr_len] = '\0';
                            break;
          case TK_NOTYPE:
                            break;
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

void check_unary(int p, int q){
  for(int i = p; i <= q; i++){
    if(tokens[i].type == '-'){ 
      if(tokens[i].type == '-' && i == 0) tokens[i].type = TK_NEG;
      else if(tokens[i].type == '-' && i != 0 && tokens[i-1].type != TK_NUM && tokens[i-1].type != ')') tokens[i].type = TK_NEG;
    }
    if(tokens[i].type == '*'){ 
        if(tokens[i].type == '*' && i == 0) tokens[i].type = TK_DEREF;
        else if(tokens[i].type == '*' && i != 0 && tokens[i-1].type != TK_NUM && tokens[i-1].type != ')') tokens[i].type = TK_DEREF;
    }
  }
}

bool check_parentheses(int p, int q){
  if(tokens[p].type == '(' && tokens[q].type == ')'){
    int cnt = 0;
    for(int i = p; i <= q; i++){
      if(tokens[i].type == '(') cnt++;
      else if(tokens[i].type == ')') cnt--;
      if(cnt == 0) return i == q;
    }
  }
  return false;
}

int find_major(int p, int q){
  int cnt = 0, op_type = 0, position = -1;
  for(int i = p; i <= q; i++){
    // printf("In find major, start = %d, end = %d\n", p, q);
    if(tokens[i].type == TK_NUM) continue;
    if(tokens[i].type == '(') cnt++;
    else if(tokens[i].type == ')'){
      if (cnt == 0) return -1;
      cnt--;
    }
    else if(cnt > 0) continue;
    else{
      int tmp_type = 0;
      switch (tokens[i].type){
        case TK_NEG:   tmp_type = 0; break;
        case TK_DEREF: tmp_type = 0; break;
        case TK_REG:   tmp_type = 0; break;
        case TK_HEX:   tmp_type = 0; break;
        case TK_NOT:   tmp_type = 0; break;
        case TK_EQ:    tmp_type = 3; break;   
        case TK_NEQ:   tmp_type = 3; break;
        case TK_BE:    tmp_type = 3; break;
        case TK_BT:    tmp_type = 3; break;
        case TK_LE:    tmp_type = 3; break;
        case TK_LT:    tmp_type = 3; break;
        case TK_AND:   tmp_type = 3; break;
        case TK_OR:    tmp_type = 3; break;
        case '*':      tmp_type = 1; break;
        case '/':      tmp_type = 1; break;
        case '+':      tmp_type = 2; break;
        case '-':      tmp_type = 2; break;
        default: assert(0);
      }
      if(tmp_type >= op_type){
        op_type = tmp_type;
        position = i;
      }
    }
  }
  if(cnt != 0) return -1;
  return position;
}

int Hex_Conversion_Dec(int hex){    
     int Dec = 0;                // 保存转换为10进制的结果
     int k = 16;                 // 16进制
     int n = 1;                  // 位权
     while(hex != 0){
         Dec += (hex % 10) * n;  // 取出各位位码值，并乘以对应的位权值
         hex /= 10;              // 去掉16进制数的最低位，次低位变为最低位
         n *= k;                 // 位权乘以16
     }
     return Dec;                 // 输出转换后的结果
}

extern word_t vaddr_read(vaddr_t addr, int len);

int eval(int p, int q){
  //printf("start = %d, end = %d\n", p, q);
  if (p > q){
    printf("Bad Expression!.\n");
    assert(0);
  }
  else if(p == q){
    if(tokens[p].type != TK_NUM){
      printf("type = %d\n", tokens[p].type);
      assert(0);
      }
    int num = 0;
    if(tokens[p].type == TK_NUM){
      num = strtol(tokens[p].str, NULL, 10);
    }
    return num;
  }
  else if(check_parentheses(p, q) == true){
    //printf("parentheses start = %d, parentheses end = %d\n", p, q);
    return eval(p+1, q-1);
  }
  else{
    int major = find_major(p, q);
    //printf("check parentheses = %d\n", check_parentheses(p, q));
    //printf("major = %d\n", major);
    if(major < 0) assert(0);
    int val1 = 0;
    if(tokens[major].type != TK_NEG 
    && tokens[major].type != TK_DEREF 
    && tokens[major].type != TK_REG 
    && tokens[major].type != TK_HEX
    && tokens[major].type != TK_NOT) val1 = eval(p, major-1);
    int val2 = eval(major+1, q);
    //printf("val1 = %d, val2 = %d\n", val1, val2);

    switch(tokens[major].type){
      case '+':             return val1 + val2;
      case '-':             return val1 - val2;
      case '*':             return val1 * val2;
      case '/':
                            if(val2 == 0) assert(0);
                            return val1 / val2;
      case TK_NEG:          return val2 * -1;
      case TK_REG:          return cpu.gpr[val2];
      case TK_DEREF:        return vaddr_read(val2, 4);
      case TK_HEX:          return Hex_Conversion_Dec(val2);
      case TK_EQ:           return val1 == val2;
      case TK_NEQ:          return val1 != val2;
      case TK_BE:           return val1 >= val2;
      case TK_BT:           return val1 > val2;
      case TK_LE:           return val1 <= val2;
      case TK_LT:           return val1 < val2;
      case TK_NOT:          return !val2;
      case TK_OR:           return val1 || val2;
      case TK_AND:          return val1 && val2;
      default: assert(0);
    }
  }

}

word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  //printf("token type = %d\n", tokens[0].type);
  check_unary(0, nr_token-1);
  //printf("token type = %d\n", tokens[0].type);
  //printf("nr_token = %d\n", nr_token);
  int num = eval(0, nr_token-1);
  /* TODO: Insert codes to evaluate the expression. */
  return num;
}
