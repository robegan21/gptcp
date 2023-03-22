gptcp : gptcp.cpp
	g++ -O3 -fopenmp -o gptcp gptcp.cpp

clean : 
	rm gptcp
