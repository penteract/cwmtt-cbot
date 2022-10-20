
char* skipTo(char* from, char test){
  while(*from!=test){
    if(*from == 0)return 0;
    from++;
  }
  return from+1;
}

int readnum(char** ptrref){
  int sign=1;
  if(**ptrref=='-'){
    (*ptrref)++;
    sign=-1;
  }
  int result=0;
  char c = (**ptrref) - '0';
  while(0<=c && c<=9){
    result=result*10 + c;
    (*ptrref)++;
    c = (**ptrref) - '0';
  }
  return result*sign;
}

int parse(char* pgn){
  char* ptr = pgn;
  int srcl,srct,srcx,srcy,
      dstl,dstt,dstx,dsty;
  srcl=MAXL;
  srct=0; // includes color
  struct Move mv;
  while(1){
    switch(*ptr){
      case '[':
        ptr = skipTo(ptr,']'); // todo: ignore internal comments and quoted strings
        if(ptr==0) return -1;
        break;
      case '{':
        ptr = skipTo(ptr,'}');
        if(ptr==0) return -1;
        break;
      case ' ':
      case '\r':
      case '\n':
      case '\t':
        ptr++;
        break;
      case '\0':
        return -1;
      default:
        goto parse_turn_start;
    }
  }
parse_turn_start:
  //printf("starting turn %.20s\n", ptr);
  switch(*ptr){
    case '/':
    case '.':
      ptr++;
      goto parse_moves;
    case '{':
      ptr = skipTo(ptr,'}');
      if(ptr==0) return -1;
      goto parse_turn_start;
    case ' ':
    case '\r':
    case '\n':
    case '\t':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case 'w':
    case 'b': // there's unfortunate potential for ambiguity here
      ptr++;
      goto parse_turn_start;
    default:
      return -1;
  }
parse_moves:
 // printf("parse_moves %.20s\n",ptr);
  switch(*ptr){
    case '(': //parse move
      if(*ptr == '('){
        ptr++;
        srcl = MAXL + readnum(&ptr);
        if(*(ptr++)!='T') return -1;
        srct = (srct&1) + 2*readnum(&ptr);
        if(*(ptr++)!=')') return -1;
      }
      if('A'<=*ptr && *ptr<='Z')ptr++;
      srcx = (*(ptr++))- 'a';
      if(srcx<0 || srcx>7) return -1;
      srcy = (*(ptr++))- '1';
      if(srcy<0 || srcy>7) return -1;
      while(*ptr=='>'){ptr++;}
      
      dstl=srcl;
      dstt=srct;
      if(*ptr == '('){
        ptr++;
        dstl = MAXL + readnum(&ptr);
        if(*(ptr++)!='T') return -1;
        dstt = (srct&1) + 2*readnum(&ptr);
        if(*(ptr++)!=')') return -1;
      }
      dstx = (*(ptr++))- 'a';
      if(dstx<0 || dstx>7) return -1;
      dsty = (*(ptr++))- '1';
      if(dsty<0 || dsty>7) return -1;
      printf("src: %d %d %d %d\n",srcl,srct,srcx,srcy);
      printf("dst: %d %d %d %d\n",dstl,dstt,dstx,dsty);
      mv.src = toPos(srct&1,srcl,srct>>1,srcx,srcy);
      mv.dst = toPos(dstt&1,dstl,dstt>>1,dstx,dsty);
      apply(&mv);
      goto parse_moves;
    case '\n':
    case '\r':
    case '/':
      srct+=1;
      goto parse_turn_start;
    case '{':
      ptr = skipTo(ptr,'}');
      goto parse_moves;
    case ' ':
    case '\t':
      ptr++;
      goto parse_moves;
    case '\0':
      return 0;
    default:
      return -1;
  }
}
