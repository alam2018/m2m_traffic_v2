# Machine to Machine (M2M) Traffic Generator 
This projects creates simulated traffic from Machine to Machine (M2M) users.
We have also used the source traffic modelling as proposed in the 3GPP paper [1]. This 3GPPpaper recommended two different traffic model for the characterization two different m2m communication behaviour
Model 1 (m1) is proposed for the environment, where MTC devices produce traffic in a non synchronized pattern
Model 2 (m2) is proposed for the environment, where MTC devices produce traffic in a synchronized pattern (e.g. after power outage)

A number of users/machines (according to [1]) can be introduced in the simulation
We have used the aggregated traffic model 
The size of packets are normally distributed. The distribution cut-off is in between 1500 and 50 (maximum and minimum packet size)
The distribution period is 60s. During this time for each observation period (1s) number of packets are uniformly generated
At each observation period N random number of packets are drawn and for each packet gets  normal distributed (m1) or beta distributed (m2) packet size
The total amount of data during each observation period is dependent on the spread of packet size (e.g. max, min and average packet size) and also the spread of number of packets (e.g. max, min and average number of packets)



Source : 3rd Generation Partnership Project; Technical Specification Group Radio Access Network; Study on RAN Improvements for Machine-type Communications (3GPP TR 37.868 V0.8.1 (2011-08)
