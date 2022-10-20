// Rough Memory bound: 100MiB
#define MAXL 128
#define MAXT 128

//char board [2][2][2*MAXL][MAXT][8][8]; // 2^(3+3+7+8+1+1) B = 2^22 B = 4MiB
//const char* boardp = (char*) board;
char board [2*2*(2*MAXL)*MAXT*8*8]; // 2^(3+3+7+8+1+1) B = 2^22 B = 4MiB

unsigned char timelineEnds[2*MAXL]; // tracks doubled times (includes color)
unsigned char timelineStarts[2*MAXL];
unsigned char minL;
unsigned char maxL;


#define PIECECOL ((char)0x20)
#define PRESENT ((char)0x40)
#define UX (1)
#define UY (1<<3)
#define UT (1<<6)
#define UL (1<<13)
#define UC (1<<21)
#define UR (1<<22)
#define BOARDSIZE (1<<23)
#define BRD(c,l,t,x,y) (board[((c)<<21)|((l)<<13)|((t)<<6)|((y)<<3)|(x)])
#define BRDW(l,t,x,y) BRD(0,l,t,x,y)
#define BRDB(l,t,x,y) BRD(1,l,t,x,y)
#define BRDP(p) (((char*) board) + (p))

typedef int piecepos;

#define Tpos (1<<6)
#define Lpos (1<<13)

piecepos toPos(int c, int l, int t, int x, int y){
  return (&(BRD(c,l,t,x,y))) - board;
}

struct Move {
  piecepos src;
  piecepos dst;
};

typedef struct Move* move;

void copyBoard(piecepos old, piecepos nw){
    assert(!(old&63));
    assert(!(nw&63));
    assert(old!=nw);
    memcpy(BRDP(nw), BRDP(old),64*(sizeof(board[0])));
}

piecepos addHalfTurn(piecepos brd){
    return (brd^UC)+((brd&UC)>>15);
}

void apply(move mv){
    assert(UX==1); //If this changes, some of this code needs changing
    char c = *(BRDP(mv->src));
    
    piecepos srcBrd = (mv->src & (UR-UT));
    piecepos newBrd = addHalfTurn(srcBrd);
    copyBoard(srcBrd, newBrd);
    
    int srcL = (mv->src&(UC-UL))>>13;
    printf("%x %x\n", timelineEnds[srcL], mv->src);
    assert( timelineEnds[srcL]==((mv->src&(UL-UT))>>5 | ((mv->src&UC)>>21)) );
    timelineEnds[srcL]+=1;
    
    *(BRDP(newBrd | (mv->src&(UT-1)))) = 0; // remove moving piece
    if(srcBrd == (mv->dst & (UR-UT))){ //same board move
        piecepos newDst = newBrd | (mv->dst & 63);
        //en Passant
        if ( (c&0x5f) == 'P' //pawn (of either color that may have moved)
          && ((mv->src - mv->dst)&1) == 1 // moved an odd number horizontally
          && (* BRDP(mv->dst)) == (char)0 ){ // into an empty square
            *(BRDP(newBrd | (mv->src&(UT-UY)) | (mv->dst&(UY-UX))))=0; //capture a piece horizontally aligned with the src and vertically aligned with the dst
        }
        if((c=='K' || c=='k') && (((mv->dst-mv->src)&3) == 2) ){ //castling (unmoved king moved 2 mod 4 horizontally )
            piecepos rook =  newBrd | ( mv->src &(UT-UY) ) | (mv->dst < mv->src ? 0 : (UY-UX)); // Needs changing to support variants where rooks don't start at a and h
            *BRDP(newBrd | (((mv->src+mv->dst)>>1)&(UT-1)) ) = *BRDP(rook);
            *BRDP(rook) = 0;
        }
        *BRDP(newDst) = c|0x80;
    }
    else{
        piecepos dstBrd = (mv->dst & (UR-UT));
        int t = (dstBrd&(UL-UT))>>5 | ((dstBrd&UC)>>21); // (T*2) + (isblack?1:0)
        int l = (dstBrd&(UC-UL))>>13;
        printf("t %d l %d\n",t,l);
        assert(timelineEnds[l]>=t);
        if(timelineEnds[l]==t){ // hop
            newBrd = addHalfTurn(dstBrd);
            copyBoard(dstBrd, newBrd);
            timelineEnds[l]+=1;
            *BRDP(newBrd | (mv->dst & 63)) = c|0x80;
        }
        else{ // branch
            unsigned char newL = (dstBrd&UC)?minL-1:maxL+1;
//            printf(dstBrd);
            newBrd = (((t&1)*UC)^UC) | (newL<<13) | (((t+1)<<5)&(UL-UT));
            //newBrd = addHalfTurn(newBrd);
            copyBoard(dstBrd,newBrd);
            //board[dstBrd]='D';
            //board[newBrd]='W';
            //printf("%c \n", board[newBrd]);
            timelineStarts[newL]=t+1;
            timelineEnds[newL]=t+1;
            *BRDP(newBrd | (mv->dst & 63)) = c|0x80;
            if(dstBrd&UC) minL = newL;
            else maxL = newL;
        }
    }
}

void setup(){
  
  for(int x=0;x<8;x++){
    BRDW(MAXL,1,x,1)='P';
    BRDW(MAXL,1,x,6)='p';
    BRDW(MAXL,1,x,0)="RNBQKBNR"[x];
    BRDW(MAXL,1,x,7)="rnbqkbnr"[x];
    for(int y=2;y<6;y++){
      BRDW(MAXL,1,x,y)=0;
    }
  }
  minL=MAXL;
  maxL=MAXL;
  timelineStarts[MAXL]=2;
  timelineEnds[MAXL]=2;
}
