
bool givesCheck(piecepos srcpos){
    int piece = board[srcpos];
    int c = srcpos&UC;
    //if it is not a piece belonging to the player who's turn it is, the piece does not give check
    if (piece==0 || ( (c>>(21-5)) ^ (piece&0x20)) != 0 ) return false;
    char otherKing = (piece&0x20)?'K':'k';
    if((piece&0x5f)=='N'){
        for(int i=0;i<40;i++){
            int d = knightDeltas[i];
            piecepos dstpos = d+srcpos;
            if (inBounds(srcpos,d,dstpos) && (board[dstpos]==otherKing)){
                return true;
            }
        }
    }
    if((piece&0x5f)=='B' || (piece&0x5f)=='Q'){
        for(int i=0;i<20;i++){
            int d = bishopDeltas[i];
            piecepos dstpos = srcpos+d;
            while(inBounds(dstpos-d,d,dstpos)){
                if(board[dstpos]==otherKing) return true;
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
                if(board[dstpos]==otherKing) return true;
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
                if(board[dstpos]==otherKing) return true;
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
                if(board[dstpos]==otherKing) return true;
                if(board[dstpos]!=0) break;
                dstpos+=d;
            }
        }
    }
    if((piece&0x5f)=='P'){
        for(int i=0;i<4;i++){
            int d = pawnCaptureDeltas[c][i];
            piecepos dstpos = srcpos+d;
            if (inBounds(srcpos,d,dstpos) && (board[dstpos]==otherKing)){
                return true;
            }
        }
    }
    if((piece&0x5f)=='K'){
        for(int i=0;i<80-9;i++){
            int d = rookDeltas[i];
            piecepos dstpos = d+srcpos;
            if (inBounds(srcpos,d,dstpos) && (board[dstpos]==otherKing)){
                return true;
            }
        }
    }
    return false;
}

// Return the location of a piece giving check if one exists, otherwise return -1
piecepos checkingFromL(int l){
    int t = timelineEnds[l];
    int c = t&1;
    t = t>>1;
    int c5=c<<5;
    int start = c*UC|t*UT|l*UL;
    for(piecepos srcpos=start;srcpos<start+64;srcpos++){
        if((board[srcpos]&0x20)==c5 && givesCheck(srcpos)){
            return srcpos;
        }
    }
    return -1;
}


// find out if the current moveset is illegal due to check
piecepos isCheck(int checkingPlayer, piecepos prevCheck){
    if (prevCheck!=-1 && givesCheck(prevCheck)) return prevCheck;
    for(int l=minL; l<=maxL; l++){
        if((timelineEnds[l]&1)==checkingPlayer){
            piecepos p = checkingFromL(l);
            if(p!=-1) return p;
        }
    }
    return -1;
}
