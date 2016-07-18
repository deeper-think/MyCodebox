cat frigate-8-*.0|grep TUNNEL| awk '{n[$10]++;f[$10]+=$11+$12;sum=sum+$12+$11;}END{for(i in n)print i"\t" n[i]"\t" f[i]"\t" f[i]/sum*100}'|sort -nrk 3|more
