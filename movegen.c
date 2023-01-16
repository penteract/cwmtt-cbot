
struct Move availableMoves [MAXL*2][64*(MAXL*2+MAXT*3+23)]; // this is a bound on the number of available moves if every square is filled with queens
int numMoves[MAXL*2];

int knightDeltas[40] = {UX+2*UY, UX-2*UY, -UX+2*UY, -UX-2*UY, UX-2*UT, -UX-2*UT, UX+2*UL, UX-2*UL, -UX+2*UL, -UX-2*UL, UY+2*UX, UY-2*UX, -UY+2*UX, -UY-2*UX, UY-2*UT, -UY-2*UT, UY+2*UL, UY-2*UL, -UY+2*UL, -UY-2*UL, -UT+2*UX, -UT-2*UX, -UT+2*UY, -UT-2*UY, UT+2*UL, UT-2*UL, -UT+2*UL, -UT-2*UL, UL+2*UX, UL-2*UX, -UL+2*UX, -UL-2*UX, UL+2*UY, UL-2*UY, -UL+2*UY, -UL-2*UY, UL+2*UT, UL-2*UT, -UL+2*UT, -UL-2*UT};

int rookDeltas[71] = {+UX, -UX, +UY, -UY, -UT, +UL, -UL,
                     +UX+UY, -UX+UY, +UX-UY, -UX-UY, +UX-UT, -UX-UT, +UY-UT, -UY-UT, +UX+UL, -UX+UL, +UY+UL, -UY+UL, +UT+UL, -UT+UL, +UX-UL, -UX-UL, +UY-UL, -UY-UL, +UT-UL, -UT-UL,
                     +UX+UY-UT, -UX+UY-UT, +UX-UY-UT, -UX-UY-UT, +UX+UY+UL, -UX+UY+UL, +UX-UY+UL, -UX-UY+UL, +UX+UT+UL, -UX+UT+UL, +UY+UT+UL, -UY+UT+UL, +UX-UT+UL, -UX-UT+UL, +UY-UT+UL, -UY-UT+UL, +UX+UY-UL, -UX+UY-UL, +UX-UY-UL, -UX-UY-UL, +UX+UT-UL, -UX+UT-UL, +UY+UT-UL, -UY+UT-UL, +UX-UT-UL, -UX-UT-UL, +UY-UT-UL, -UY-UT-UL,
                     +UX+UY+UT+UL, -UX+UY+UT+UL, +UX-UY+UT+UL, -UX-UY+UT+UL, +UX+UY-UT+UL, -UX+UY-UT+UL, +UX-UY-UT+UL, -UX-UY-UT+UL, +UX+UY+UT-UL, -UX+UY+UT-UL, +UX-UY+UT-UL, -UX-UY+UT-UL, +UX+UY-UT-UL, -UX+UY-UT-UL, +UX-UY-UT-UL, -UX-UY-UT-UL};

const int* bishopDeltas = rookDeltas+7;
const int* unicornDeltas = rookDeltas+7+20;
const int* dragonDeltas = rookDeltas+7+20+28;

int pawnDeltas[2][2] = {{UY,-UL}, {-UY,UL}};
int pawnCaptureDeltas[2][4] = {{UY+UX, UY-UX, -UL+UT, -UL-UT}, {-UY+UX, -UY-UX, UL+UT, UL-UT}};

/* Test whether a delta can be applied to a move without taking it out of bounds */
bool inBounds(piecepos src, int delta, piecepos dst){
    // dst=src+delta
    //using the idea from https://stackoverflow.com/a/44017503/1779797
    int carry = src^delta^dst;
    int shouldcarry = (delta<<1)&(UY|UT|UL|UC);
    if (((UY|UT|UL|UC)&carry) == shouldcarry){ //there has been no overflow when adding signed components of delta to unsigned conmponents of src (dst is unsigned)
        int dstl = (dst&(UC-UL))>>13;
        if(minL<=dstl && dstl<=maxL){
            int dstt = ((dst&(UL-UT))>>5) + ((dst&UC)>>21);
            if (timelineEnds[dstl]>=dstt && timelineStarts[dstl]<=dstt){
                return true;
            }
        }
    }
    return false;
}

/*Generate moves from a given timeline*/
void genMoves(int l){
    numMoves[l]=0;
    int t = timelineEnds[l];
    int c = t&1;
    int c5=c<<5;
    t = t>>1;
    int start = c*UC|t*UT|l*UL;
    for(piecepos srcpos=start;srcpos<start+64;srcpos++){ //int x=0;x<8;x++)for(int y=0;y<8;y++){
        //int srcpos = c*UC|t*UT|l*UL|y*UY|x*UX;
        int piece = board[srcpos];
        if((piece&0x60) != (c5|0x40)) continue;
        if((piece&0x5f)=='N'){
            for(int i=0;i<40;i++){
                int d = knightDeltas[i];
                piecepos dstpos = d+srcpos;
                if (inBounds(srcpos,d,dstpos) && (board[dstpos]==0 || (board[dstpos]&0x20)!=c5)){
                    availableMoves[l][numMoves[l]].src=srcpos;
                    availableMoves[l][numMoves[l]++].dst=dstpos;
                }
            }
        }
        if((piece&0x5f)=='B' || (piece&0x5f)=='Q'){
            for(int i=0;i<20;i++){
                int d = bishopDeltas[i];
                piecepos dstpos = srcpos+d;
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
                piecepos dstpos = srcpos+d;
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
                piecepos dstpos = srcpos+d;
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
                piecepos dstpos = srcpos+d;
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
                piecepos dstpos = srcpos+d;
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
                piecepos dstpos = srcpos+d;
                if (inBounds(srcpos,d,dstpos)){
                    if (board[dstpos]!=0 && (board[dstpos]&0x20)!=c5){
                        availableMoves[l][numMoves[l]].src=srcpos;
                        availableMoves[l][numMoves[l]++].dst=dstpos;
                    }
                    else if (inBounds(srcpos,d+(d&-UY),dstpos+(d&-UY)) && board[dstpos]==0 && (d&(UC-UT))==0){// en passant
                        piecepos tsub1 = (dstpos^UC)-(c?0:UT);
                        if(timelineStarts[l]>timelineEnds[l] && //(tsub1 is in bounds)
                                //[othercol,t-1][dsty+dy][dstx]==opponent's unmoved pawn
                                board[tsub1+(d&-UY)]==('p'^c5) &&
                                //[t][dsty+dy][dstx]==empty
                                board[dstpos+(d&-UY)]==0 &&
                                //[t][dsty-dy][dstx] == opponent's moved pawn
                                board[dstpos-(d&-UY)]==(0xf0^c5) &&
                                //[othercol,t-1][dsty-dy][dstx] == empty
                                board[tsub1-(d&-UY)]==0){
                            //
                            availableMoves[l][numMoves[l]].src=srcpos;
                            availableMoves[l][numMoves[l]++].dst=dstpos;
                        }
                    }
                }
            }
        }
        if((piece&0x5f)=='K'){
            for(int i=0;i<80-9;i++){
                int d = rookDeltas[i];
                piecepos dstpos = d+srcpos;
                if (inBounds(srcpos,d,dstpos) && (board[dstpos]==0 || (board[dstpos]&0x20)!=c5)){
                    availableMoves[l][numMoves[l]].src=srcpos;
                    availableMoves[l][numMoves[l]++].dst=dstpos;
                }
            }
            if (!(piece&0x80)){//castling
                // TODO: can't castle through check
                if ((board[srcpos-4]&0xdf)=='R'
                  && board[srcpos-1]==0 && board[srcpos-2]==0 &&board[srcpos-3]==0){
                    availableMoves[l][numMoves[l]].src=srcpos;
                    availableMoves[l][numMoves[l]++].dst=srcpos-2;
                }
                if ((board[srcpos+3]&0xdf)=='R'
                  && board[srcpos+1]==0 && board[srcpos+2]==0){
                    availableMoves[l][numMoves[l]].src=srcpos;
                    availableMoves[l][numMoves[l]++].dst=srcpos+2;
                }
            }
        }
    }
}

/* Generate moves for all timelines a given player can play on*/
void genAll(int player){

}