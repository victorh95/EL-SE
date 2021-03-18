all: Terminal Numeros_tests_PCR Numeros_tests_PCR_centre Validation Acquisition

Terminal: Terminal.c
	gcc -Wall Terminal.c ../libTestsPCR/lectureEcriture.o ../libTestsPCR/alea.o ../libTestsPCR/message.o -o Terminal

Numeros_tests_PCR: Numeros_tests_PCR.c
	gcc -Wall Numeros_tests_PCR.c ../libTestsPCR/lectureEcriture.o ../libTestsPCR/alea.o -o Numeros_tests_PCR

Numeros_tests_PCR_centre: Numeros_tests_PCR_centre.c
	gcc -Wall Numeros_tests_PCR_centre.c ../libTestsPCR/lectureEcriture.o ../libTestsPCR/alea.o -o Numeros_tests_PCR_centre

Validation: Validation.c
	gcc -Wall Validation.c ../libTestsPCR/lectureEcriture.o ../libTestsPCR/message.o -o Validation

Acquisition: Acquisition.c
	gcc -Wall Acquisition.c ../libTestsPCR/lectureEcriture.o -o Acquisition

clean:	
	rm -f Numeros_tests_PCR*.txt

cleanall: clean
	rm Terminal Numeros_tests_PCR Numeros_tests_PCR_centre Validation Acquisition
