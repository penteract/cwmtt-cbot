void printBoard(){
  int t = MAXT;
  int maxt = 0;
  for(int i=minL;i<=maxL;i++){
    if(timelineStarts[i]<t) t=timelineStarts[i];
    if(timelineEnds[i]>maxt) maxt=timelineEnds[i];
  }
  printf("%d %d\n",t,maxt);
  for(;t<=maxt;t++){
    for(int x=0;x<8;x++){
      if(x==0 && !(t&1))printf("T%.3d ", t>>1);
      else printf("     ");
      for(int l=minL;l<=maxL;l++){
        if (timelineStarts[l]<=t && t<=timelineEnds[l]){
          //printf("%d %d ",t,l);
          for(int y=0;y<8;y++){
//            printf("%d %d %d %d %d\n",t&1,l,t>>1,x,y);
            char c = BRD(t&1,l,t>>1,x,y)&0x7f;
            printf("%c",c=='\0'?' ':c);
          }
        }
        else{
          printf("        ");
        }
        printf(" ");
      }
      printf("\n");
    }
    printf("\n");
  }
}

void printMove(struct Move mv){
  int srcl=((mv.src&(UC-UL))>>13) - MAXL;
  int srct=(mv.src&(UL-UT))>>6;
  int srcx=(mv.src&(UY-UX))>>0;
  int srcy=(mv.src&(UT-UY))>>3;

  int dstl=((mv.dst&(UC-UL))>>13) - MAXL;
  int dstt=(mv.dst&(UL-UT))>>6;
  int dstx=(mv.dst&(UY-UX))>>0;
  int dsty=(mv.dst&(UT-UY))>>3;
  if(srct==dstt && srcl==dstl) printf("(%dT%d)%c%d%c%d", srcl,srct,'a'+srcx,srcy+1,'a'+dstx,dsty+1);
  else printf("(%dT%d)%c%d>>(%dT%d)%c%d", srcl,srct,'a'+srcx,srcy+1,dstl,dstt,'a'+dstx,dsty+1);
}