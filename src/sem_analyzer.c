#include <malloc.h>
#include "sem_analyzer.h"


SemAnalyzer* sem_an_init(semantic_action_f sem_action) {
	SemAnalyzer* sem_an = (SemAnalyzer*) malloc(sizeof(SemAnalyzer));
	if (sem_an == NULL)
		return NULL;

	sem_an->sem_action = sem_action;
	sem_an->state = SEM_STATE_START;

	return sem_an;
}

void sem_an_free(SemAnalyzer* sem_an) {
	free(sem_an);
}
