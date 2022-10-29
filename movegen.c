
struct Move availableMoves [MAXL*2][64*(MAXL*2+MAXT*3+23)]; // this is a bound on the number of available moves if every square is filled with queens
int numMoves[MAXL*2];

int knightDeltas[40] = {UX+2*UY, UX-2*UY, -UX+2*UY, -UX-2*UY, UX-2*UT, -UX-2*UT, UX+2*UL, UX-2*UL, -UX+2*UL, -UX-2*UL, UY+2*UX, UY-2*UX, -UY+2*UX, -UY-2*UX, UY-2*UT, -UY-2*UT, UY+2*UL, UY-2*UL, -UY+2*UL, -UY-2*UL, -UT+2*UX, -UT-2*UX, -UT+2*UY, -UT-2*UY, UT+2*UL, UT-2*UL, -UT+2*UL, -UT-2*UL, UL+2*UX, UL-2*UX, -UL+2*UX, -UL-2*UX, UL+2*UY, UL-2*UY, -UL+2*UY, -UL-2*UY, UL+2*UT, UL-2*UT, -UL+2*UT, -UL-2*UT};
int rookDeltas[7] = {+UX, -UX, +UY, -UY, -UT, +UL, -UL};
int bishopDeltas[20] = {+UX+UY, -UX+UY, +UX-UY, -UX-UY, +UX-UT, -UX-UT, +UY-UT, -UY-UT, +UX+UL, -UX+UL, +UY+UL, -UY+UL, +UT+UL, -UT+UL, +UX-UL, -UX-UL, +UY-UL, -UY-UL, +UT-UL, -UT-UL};
int unicornDeltas[28] = {+UX+UY-UT, -UX+UY-UT, +UX-UY-UT, -UX-UY-UT, +UX+UY+UL, -UX+UY+UL, +UX-UY+UL, -UX-UY+UL, +UX+UT+UL, -UX+UT+UL, +UY+UT+UL, -UY+UT+UL, +UX-UT+UL, -UX-UT+UL, +UY-UT+UL, -UY-UT+UL, +UX+UY-UL, -UX+UY-UL, +UX-UY-UL, -UX-UY-UL, +UX+UT-UL, -UX+UT-UL, +UY+UT-UL, -UY+UT-UL, +UX-UT-UL, -UX-UT-UL, +UY-UT-UL, -UY-UT-UL};
int dragonDeltas[16] = {+UX+UY+UT+UL, -UX+UY+UT+UL, +UX-UY+UT+UL, -UX-UY+UT+UL, +UX+UY-UT+UL, -UX+UY-UT+UL, +UX-UY-UT+UL, -UX-UY-UT+UL, +UX+UY+UT-UL, -UX+UY+UT-UL, +UX-UY+UT-UL, -UX-UY+UT-UL, +UX+UY-UT-UL, -UX+UY-UT-UL, +UX-UY-UT-UL, -UX-UY-UT-UL};

int pawnDeltas[2][2] = {{UY,-UL}, {-UY,UL}};
int pawnCaptureDeltas[2][4] = {{UY+UX, UY-UX, -UL+UT, -UL-UT}, {-UY+UX, -UY-UX, UL+UT, UL-UT}};

/* Test whether a delta can be applied to a move without taking it out of bounds */
bool inBounds(int src, int delta, int dst){
    // dst=src+delta
    //using the idea from https://stackoverflow.com/a/44017503/1779797
    int carry = src^delta^dst;
    int shouldcarry = (delta<<1)&(UY|UT|UL|UC);
    if (((UY|UT|UL|UC)&carry) == shouldcarry){ //there has been no overflow when adding signed components of delta to unsigned conmponents of src (dst is unsigned)
        int dstl = (dst&(UC-UL))>>13;
        if(minL<=dstl && dstl<=maxL){
            int dstt = (dst&(UL-UT))>>6;
            if (timelineEnds[dstl]>=dstt && timelineStarts[dstl]<=dstt){
                return true;
            }
        }
    }
    return false;
}

/*Generate moves from a given timeline*/
void genMoves(int l){
    int t = timelineEnds[l];
    int c = t&1;
    int c5=c<<5;
    t = t>>1;
    int start = c*UC|t*UT|l*UL;
    for(int srcpos=start;srcpos<start+64;srcpos++){ //int x=0;x<8;x++)for(int y=0;y<8;y++){
        //int srcpos = c*UC|t*UT|l*UL|y*UY|x*UX;
        int piece = board[srcpos];
        if(piece==0 || ((piece&0x20) != c5)) continue;
        if((piece&0x5f)=='N'){
            for(int i=0;i<40;i++){
                int d = knightDeltas[i];
                int dstpos = d+srcpos;
                if (inBounds(srcpos,d,dstpos) && (board[dstpos]==0 || (board[dstpos]&0x20)!=c5)){
                    availableMoves[l][numMoves[l]].src=srcpos;
                    availableMoves[l][numMoves[l]++].dst=dstpos;
                }
            }
        }
        if((piece&0x5f)=='B' || (piece&0x5f)=='Q'){
            for(int i=0;i<20;i++){
                int d = bishopDeltas[i];
                int dstpos = srcpos+d;
                while(inBounds(dstpos-d,d,dstpos)){
                    if((board[dstpos]&0x20)==c5) break;
                    availableMoves[l][numMoves[l]].src=srcpos;
                    availableMoves[l][numMoves[l]++].dst=dstpos;
                    if(board[dstpos]!=0) break;
                    dstpos+=d;
                }
            }
        }
        if((piece&0x5f)=='R' || (piece&0x5f)=='Q'){
            for(int i=0;i<7;i++){
                int d = rookDeltas[i];
                int dstpos = srcpos+d;
                while(inBounds(dstpos-d,d,dstpos)){
                    if((board[dstpos]&0x20)==c5) break;
                    availableMoves[l][numMoves[l]].src=srcpos;
                    availableMoves[l][numMoves[l]++].dst=dstpos;
                    if(board[dstpos]!=0) break;
                    dstpos+=d;
                }
            }
        }
        if((piece&0x5f)=='U' || (piece&0x5f)=='Q'){
            for(int i=0;i<28;i++){
                int d = unicornDeltas[i];
                int dstpos = srcpos+d;
                while(inBounds(dstpos-d,d,dstpos)){
                    if((board[dstpos]&0x20)==c5) break;
                    availableMoves[l][numMoves[l]].src=srcpos;
                    availableMoves[l][numMoves[l]++].dst=dstpos;
                    if(board[dstpos]!=0) break;
                    dstpos+=d;
                }
            }
        }
        if((piece&0x5f)=='D' || (piece&0x5f)=='Q'){
            for(int i=0;i<16;i++){
                int d = dragonDeltas[i];
                int dstpos = srcpos+d;
                while(inBounds(dstpos-d,d,dstpos)){
                    if((board[dstpos]&0x20)==c5) break;
                    availableMoves[l][numMoves[l]].src=srcpos;
                    availableMoves[l][numMoves[l]++].dst=dstpos;
                    if(board[dstpos]!=0) break;
                    dstpos+=d;
                }
            }
        }
        if((piece&0x5f)=='P'){
            char moved = (piece>>7)&1;
            for(int i=0;i<2;i++){
                int d = pawnDeltas[c][i];
                int dstpos = srcpos+d;
                if (inBounds(srcpos,d,dstpos) && board[dstpos]==0){
                    availableMoves[l][numMoves[l]].src=srcpos;
                    availableMoves[l][numMoves[l]++].dst=dstpos;
                    if(!moved){
                        dstpos+=d;
                        if(inBounds(srcpos,d*2,dstpos) && board[dstpos]==0){
                            availableMoves[l][numMoves[l]].src=srcpos;
                            availableMoves[l][numMoves[l]++].dst=dstpos;
                        }
                    }
                }
            }
            for(int i=0;i<4;i++){
                int d = pawnCaptureDeltas[c][i];
                int dstpos = srcpos+d;
                if (inBounds(srcpos,d,dstpos){
                    if (board[dstpos]!=0 && (board[dstpos]&0x20)!=c5){
                        availableMoves[l][numMoves[l]].src=srcpos;
                        availableMoves[l][numMoves[l]++].dst=dstpos;
                    }
                    else if (board[dstpos]==0 && (dstpos&(UC-UT))==(srcpos&(UC-UT))){// en passant
                        //[othercol,t-1][dsty+dy][dstx]==opponent's unmoved pawn
                        //[t][dsty+dy][dstx]==empty
                        //[t][dsty-dy][dstx] == opponent's pawn
                        //[othercol,t-1][dsty-dy][dstx] == empty
                    }
                }
            }
        }
    }
}

/* Generate moves for all timelines a given player can play on*/
void genAll(int player){

}