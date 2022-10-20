unsigned long hash_a = 6364136223846793005;
unsigned long c = 1;
// https://en.wikipedia.org/wiki/Linear_congruential_generator
// apparently these are the constants used by musl
unsigned long  hashTimeline(int l){
  unsigned long state = timelineStarts[l];
  for(int t = timelineStarts[l]; t<=timelineEnds[l]; t++){
    state = state*a+c;
    for (int y=0;y<8;y++){
      state^=*((unsigned long*)(&BRD(t&1,l,t>>1,0,y));
      state = state*a+c;
    }
  }
  return (state>>31)|(state<<33);
}
