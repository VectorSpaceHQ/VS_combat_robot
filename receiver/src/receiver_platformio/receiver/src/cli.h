#ifndef CLI_H
#define CLI_H

#include "Arduino.h"
#include <PreferencesCLI.h>
#include "common.h"
#include "comms.h"

void cliErrorCallback(cmd_error* e);
void helpCommandCallback(cmd* commandPointer);
bool cliSetup();
void ParseCLI();

#endif
