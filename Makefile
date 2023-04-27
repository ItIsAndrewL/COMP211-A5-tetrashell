default: tetrashell recover check modify rank

tetrashell: tetrashell.c tetris.h
	gcc tetrashell.c -o tetrashell -ggdb3

recover: /playpen/a5/recover
	cp /playpen/a5/recover .

check: /playpen/a5/check
	cp /playpen/a5/check .

modify: /playpen/a5/modify
	cp /playpen/a5/modify .

rank: /playpen/a5/rank
	cp /playpen/a5/rank .

clean: 
	rm tetrashell
	rm -R -f recovered
