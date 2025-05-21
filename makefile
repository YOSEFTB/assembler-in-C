assembler: assembler.o assembleWithTwoPasses.o  analyzeLine.o validityCheckers.o codeLineValidity.o labelHandling.o dataValidity.o bitGeneratingOneOpNoOp.o bitGenerating2ops.o secondPassing.o macroHandling.o
	gcc -ansi -Wall -pedantic assembler.o assembleWithTwoPasses.o analyzeLine.o validityCheckers.o codeLineValidity.o labelHandling.o dataValidity.o bitGeneratingOneOpNoOp.o bitGenerating2ops.o secondPassing.o macroHandling.o -o assembler
assembler.o: assembler.c
	gcc -c -ansi -Wall -pedantic assembler.c -o assembler.o
assembleWithTwoPasses.o: assembleWithTwoPasses.c assembleHeader.h
	gcc -c -ansi -Wall -pedantic assembleWithTwoPasses.c -o assembleWithTwoPasses.o
analyzeLine.o: analyzeLine.c
	gcc -c -ansi -Wall -pedantic analyzeLine.c -o analyzeLine.o
validityCheckers.o: validityCheckers.c
	gcc -c -ansi -Wall -pedantic validityCheckers.c -o validityCheckers.o
codeLineValidity.o: codeLineValidity.c
	gcc -c -ansi -Wall -pedantic codeLineValidity.c -o codeLineValidity.o
labelHandling.o: labelHandling.c
	gcc -c -ansi -Wall -pedantic labelHandling.c -o labelHandling.o
dataValidity.o: dataValidity.c
	gcc -c -ansi -Wall -pedantic dataValidity.c -o dataValidity.o
bitGeneratingOneOpNoOp.o: bitGeneratingOneOpNoOp.c
	gcc -c -ansi -Wall -pedantic bitGeneratingOneOpNoOp.c -o bitGeneratingOneOpNoOp.o
bitGenerating2ops.o: bitGenerating2ops.c
	gcc -c -ansi -Wall -pedantic bitGenerating2ops.c -o bitGenerating2ops.o
secondPassing.o: secondPassing.c
	gcc -c -ansi -Wall -pedantic secondPassing.c -o secondPassing.o
macroHandling.o: macroHandling.c
	gcc -c -ansi -Wall -pedantic macroHandling.c -o macroHandling.o
