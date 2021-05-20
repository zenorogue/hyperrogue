#!/bin/sh
# compact honeycombs

./mymake -O3 devmods/honeycomb-rule-generator

./hyper -geo 534h -gen-rule honeycomb-rules-534.dat -quit
./hyper -geo 535h -gen-rule honeycomb-rules-535.dat -quit
./hyper -geo 435h -gen-rule honeycomb-rules-435.dat -quit
./hyper -geo 353h -gen-rule honeycomb-rules-353.dat -quit

# exh 2 optimized
./hyper -geo 436h -exh 2 -gen-rule honeycomb-rules-436.dat -quit
./hyper -geo 344h -exh 2 -gen-rule honeycomb-rules-344.dat -quit
./hyper -geo 354h -exh 2 -gen-rule honeycomb-rules-354.dat -quit
./hyper -geo 536h -exh 2 -gen-rule honeycomb-rules-536.dat -quit

# still generates conflicts on exh 2:
# ./hyper -geo 355h -exh 2 -gen-rule honeycomb-rules-355.dat -quit #
# ./hyper -geo 345h -exh 2 -gen-rule honeycomb-rules-345.dat -quit #
