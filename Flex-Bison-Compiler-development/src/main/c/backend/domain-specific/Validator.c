#include "Validator.h"


/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

/** Shutdown module's internal state. */
void _shutdownLoggerModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: Calculator...");
		destroyLogger(_logger);
		_logger = NULL;
	}
}

ModuleDestructor initializeLoggerModule() {
	_logger = createLogger("Calculator");
	return _shutdownLoggerModule;
}
