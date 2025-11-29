#include "Validator.h"


/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

/** Shutdown module's internal state. */
void _shutdownValidatorModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: Validator...");
		destroyLogger(_logger);
		_logger = NULL;
	}
}

ModuleDestructor initializeValidatorModule() {
	_logger = createLogger("Validator");
	return _shutdownValidatorModule;
}

ComputationResult executeValidator(CompilerState * compilerState) {
	ProgramBack program = {0};
	//...
	if (program.view == NULL){
		logError(_logger, "Undefinded view");
	}
}
