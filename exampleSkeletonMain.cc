/*
 * skeletonExampleMain.cc
 *
 *  Created on: Jul 19, 2012
 *      Author: ebrjohn
 */

#include <string>
#include <iostream>

#include "CmdLineParser.h"

using namespace std;

const string ARG_STR_MANDATORY          = "-sm";
const string ARG_STR_MANDATORY_ALIAS    = "-strMand";
const string ARG_STR_MUTEX1             = "-smtx1";
const string ARG_STR_MUTEX2             = "-smtx2";
const string ARG_STR_OPTIONAL           = "-so";
const string ARG_INT_OPTIONAL           = "-si";
const string ARG_FLOAT_OPTIONAL         = "-sf";
const string ARG_FLAG_OPTIONAL          = "-sflag";

struct ConfigInput
{
  // Command line options
  string argStrMandatory;
  string argStrMutex1;
  string argStrMutex2;
  string argStrOptional;
  uint32_t argIntOptional;
  float argFloatOptional;
  bool argFlagOptional;
};

void loadCmdLine(CmdLineParser &clp)
{
  clp.setMainHelpText("A simple OTP C-Diameter prototype server");

  //
  // Mandatory args
  //
     // argStrMandatory
  CmdLineOption *option = new CmdLineOptionStr(ARG_STR_MANDATORY,
                                               string("a mandatory string option"),
                                               true);
  option->setCmdLineAlias(ARG_STR_MANDATORY_ALIAS);
  clp.addCmdLineOption(option);

  //
  // Mutually Exclusive options
  // Either the host address or the service config file must be set
  //
    // argStrMutex1
  clp.addMutExclCmdLineOption(new CmdLineOptionStr(ARG_STR_MUTEX1,
                              string("a mutually exclusive string option"),
                              false));

    // argStrMutex2
  clp.addMutExclCmdLineOption(new CmdLineOptionStr(ARG_STR_MUTEX2,
                              string("a mutually exclusive string option"),
                              string()));

  //
  // Optional args
  //
     // argStrOptional
  clp.addCmdLineOption(new CmdLineOptionStr(ARG_STR_OPTIONAL,
                       string("an optional string option"),
                       string("ericsson.com")));

     // argIntOptional
  clp.addCmdLineOption(new CmdLineOptionInt(ARG_INT_OPTIONAL,
                       string("an optional integer option"),
                       1));

  // argFloatOptional
  clp.addCmdLineOption(new CmdLineOptionFloat(ARG_FLOAT_OPTIONAL,
                       string("an optional float option"),
                       (float) 1.0));

     // argFlagOptional
  clp.addCmdLineOption(new CmdLineOptionFlag(ARG_FLAG_OPTIONAL,
                       string("an optional flag option"),
                       1));

  clp.setMinNumberArgs(2);
}

bool parseCommandLine(int argc, char **argv, CmdLineParser &clp, ConfigInput &config)
{
  if(!clp.parseCmdLine(argc, argv))
  {
    clp.printUsage();
    return false;
  }

  config.argStrMandatory    = ((CmdLineOptionStr*)    clp.getCmdLineOption(ARG_STR_MANDATORY))->getValue();
  config.argStrMutex1       = ((CmdLineOptionStr*)    clp.getCmdLineOption(ARG_STR_MUTEX1))->getValue();
  config.argStrMutex2       = ((CmdLineOptionStr*)    clp.getCmdLineOption(ARG_STR_MUTEX2))->getValue();
  config.argStrOptional     = ((CmdLineOptionStr*)    clp.getCmdLineOption(ARG_STR_OPTIONAL))->getValue();
  config.argIntOptional     = ((CmdLineOptionInt*)    clp.getCmdLineOption(ARG_INT_OPTIONAL))->getValue();
  config.argFloatOptional   = ((CmdLineOptionFloat*)  clp.getCmdLineOption(ARG_FLOAT_OPTIONAL))->getValue();
  config.argFlagOptional    = ((CmdLineOptionFlag*)   clp.getCmdLineOption(ARG_FLAG_OPTIONAL))->getValue();

  return true;
}


int main(int argc, char **argv)
{
   // Handle the Command line args
   CmdLineParser clp;
   loadCmdLine(clp);

   ConfigInput input;
   if(!parseCommandLine(argc, argv, clp, input))
   {
     cerr << "Error parsing command line arguments, exiting" << endl;
     return 1;
   }

   return 0;
}
