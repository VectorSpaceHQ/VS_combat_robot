#include "cli.h"


Preferences preferences;
SimpleCLI cli;
PreferencesCLI prefCli(preferences);

//globals for CLI
char cliResponseBuffer[256];
Command helpCommand;//command used to display CLI help
Command restartCommand;//command used to restart the transmitter or receiver
Command getVarCommand;//used to return values of certain internal variables




// Read command line inputs
void ParseCLI(){
    if(Serial.available())
    {
        String cliInput = Serial.readStringUntil('\n');
        Serial.println(cliInput);
        cli.parse(cliInput);
        if(cli.available())
        {
            Command command = cli.getCommand();
            prefCli.handleCommand(command,Serial);
        }
        Serial.println();
        Serial.print("> ");
    }
}

void cliErrorCallback(cmd_error* e)
{
  CommandError cmdError(e);
  Serial.print("ERROR: ");
  Serial.println(cmdError.toString());

  if (cmdError.hasCommand()) {
      Serial.print(cmdError.getCommand().toString());
  }
}

void helpCommandCallback(cmd* commandPointer)
{
  Command cmd(commandPointer);
  Argument commandName = cmd.getArg("commandName");
  if (commandName.isSet())
  {
    Command namedCommand = cli.getCommand(commandName.getValue());
    if (namedCommand.getName())
    {
      Serial.println(namedCommand.toString());
    } else {
      sprintf(cliResponseBuffer,"No matching command found for name '%s'\r\n%s",namedCommand.toString(),helpCommand.toString());
      Serial.println(cliResponseBuffer);
    }
  } else {
    Serial.println(helpCommand.toString());
  }
}

void restartCommandCallback(cmd* commandPointer)
{
    Command cmd(commandPointer);
    Argument transmitter = cmd.getArg("transmitter");
    Argument receiver = cmd.getArg("receiver");

    if(receiver.isSet())
    {
        // something isn't right here. Why are we passing pointer to this funcation and using cmd_msg global?
        //cmd_msg.command = (RemoteCommand)(cmd_msg.command | REMOTE_COMMAND_REBOOT);
        Serial.println("Remote restart queued.");
    }

    if(transmitter.isSet())
    {
        // can restartTime be stored in RemoteCommand?
        //restartTime = millis()+1000;
        Serial.println("Local restart queued.");
    }

}



void getVariableCommandCallback(cmd* commandPointer)
{
  Command cmd(commandPointer);
  Argument variable = cmd.getArg("variable");
  String variableName = variable.getValue();

  CommandMessage cmd_msg = GetCommandMessage();
  ResponseMessage rsp_msg = GetResponseMessage();

  if(variableName.equalsIgnoreCase("millis")) Serial.println(millis());
  else if (variableName.equalsIgnoreCase("commandMessageId")) Serial.println(cmd_msg.id);
  else if (variableName.equalsIgnoreCase("commandMessageTime")) Serial.println(cmd_msg.id);
  else if (variableName.equalsIgnoreCase("responseMessageId")) Serial.println(rsp_msg.command_id);
  else if (variableName.equalsIgnoreCase("leftSpeed")) Serial.println(cmd_msg.left_speed);
  else if (variableName.equalsIgnoreCase("rightSpeed")) Serial.println(cmd_msg.right_speed);
  // Adam breaking these 3 lines. Need Andrew to help bring them back without use of globals.
  // else if (variableName.equalsIgnoreCase("leftStickV")) Serial.println(leftJoystick.getVoltage());
  // else if (variableName.equalsIgnoreCase("rightStickV")) Serial.println(rightJoystick.getVoltage());
  else if (variableName.equalsIgnoreCase("weaponSpeed")) Serial.println(cmd_msg.weapon_speed);
  // else if (variableName.equalsIgnoreCase("batteryVoltage")) Serial.println(batteryVoltage);
  else if (variableName.equalsIgnoreCase("hornFreq")) Serial.println(cmd_msg.horn_frequency);
  else Serial.println("Variable is not supported by this command");
}


bool cliSetup()
{
    cli.setErrorCallback(cliErrorCallback);

    helpCommand = cli.addCommand("help", helpCommandCallback);
    helpCommand.setDescription("Use to display details on how to use a specific command\r\nAvailable Commands:\r\n\tsetPreference (setp)\r\n\tgetPreference (getp)\r\n\tclearPreference (clearp)\r\n\trestart\r\n\tgetVariable");
    helpCommand.addPositionalArgument("commandName,cmd","");

    prefCli.registerCommands(cli);

    restartCommand = cli.addCommand("restart", restartCommandCallback);
    restartCommand.setDescription("Used to restart the transmitter (-t) and/or the receiver (-r)");
    restartCommand.addFlagArgument("t/ransmitter");
    restartCommand.addFlagArgument("r/eceiver");

    getVarCommand = cli.addCommand("getV/ar/iable,getv", getVariableCommandCallback);
    getVarCommand.setDescription("Used to get the current value of the specified variable");
    getVarCommand.addPositionalArgument("v/ar/iable");

    return true;
}
