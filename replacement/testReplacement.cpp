#include <iostream>
#include "FIFO.h"
#include "LRU.h"
#include "Optimal.h"
int main(){std::vector<int> refs={1,2,3,1,4,2,5,1,2,3,4,5};FIFO f(3);LRU l(3);Optimal o(3);f.simulate(refs);l.simulate(refs);o.simulate(refs);std::cout<<f.getFaults()<<' '<<l.getFaults()<<' '<<o.getFaults()<<'\n';}
