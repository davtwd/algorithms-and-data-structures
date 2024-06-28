#pragma once

struct Route {
	unsigned int network;	// network address in decimal
	unsigned int prefix;	// prefix length of the network
	unsigned int nextHop;	// next-hop address in decimal
	unsigned int lifetime;	// lifetime in seconds
};

struct Address {

    unsigned int octetA, octetB, octetC, octetD;

    unsigned int& operator[](size_t index) {
        static unsigned int invalid = 0;
        switch (index) {
            case 0: return octetA;
            case 1: return octetB;
            case 2: return octetC;
            case 3: return octetD;
            default: return invalid;
        }
    }
};