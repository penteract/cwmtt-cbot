
struct Move moveset[2*MAXL];


struct Node{
  unsigned int weight;
  int value;
  int physical; // reference to a list of physical moves. Negative means not yet computed
};

// typedef struct HashEntry hashentry;

#define RUNLENGTH 10
#define NUMRUNS 100

void setup(){
}

struct Threat{
  piecepos s;
  piecepos target; //-1 if target is frozen
}

piecepos threat[2*MAXL];

void doRun(int numsteps){
  //find threats (places where a king could be captured
  int nthreats = getThreats();
  if(nthreats>0){
    int ttscore = findGoodTT();
    int avgAnsScore=0;
    if(!hardMate)for(threat in threats){
        avgAnsScore+=findBestRespose(threat);
    }
    avgAnsScore/=nthreats;
    if(hardMate || avgAnsScore < ttScore){
      dott();
    }
    else{
      doMoves();
    }
  }
  for(int l=minL;l<=maxL;l++){
    if(isCurrentPlayersTurn(l)){
      struct Move m = pickMove(l);
      apply(m);
    }
  }
  if (numsteps<0){
    unfold(score);
    return score;
  }
  else{
    score = doRun(numsteps);
    unfold(score);
  }
}

/**
returns the number of moves in the returned moveset
*/
int ai(){
  setup()
  for(int runNum=0;runNum<NUMRUNS;runNum++){
    doRun(RUNLENGTH);
  }
  return getBest();
}

