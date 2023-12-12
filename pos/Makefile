all: 
	# g++ pos.cpp -o pos -lssl -lcrypto
	# g++ agree.cpp -o agree -lssl -lcrypto
	g++ p2p.cpp -o p2p -lboost_system -pthread -lssl -lcrypto -lboost_serialization
	g++ p2p_seed.cpp -o p2p_seed -lboost_system -pthread -lssl -lcrypto -lboost_serialization


clean:
	# rm pos
	# rm agree
	rm p2p
	rm p2p_seed