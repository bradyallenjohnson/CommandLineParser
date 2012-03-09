
#include <CmdLineParser.h>
#include <iostream>
#include <stdlib.h>

using namespace std;

CmdLineParser::CmdLineParser() : minNumberArgs_(0), maxNumberArgs_(0)
{
  // Load the help options
  helpStrList_.push_back("-h");
  helpStrList_.push_back("-help");
  helpStrList_.push_back("--h");
  helpStrList_.push_back("--help");
  helpStrList_.push_back("-?");
}

CmdLineParser::~CmdLineParser()
{
}

bool CmdLineParser::isHelpOption(const string &str)
{
  bool foundIt(false);
  for(list<string>::iterator iter = helpStrList_.begin();
      iter != helpStrList_.end() && !foundIt;
      iter++)
  {
    if(str == *iter)
    {
      foundIt = true;
    }
  }

  return foundIt;
}

void CmdLineParser::addMutExclCmdLineOption(CmdLineOption *option)
{
  if(option)
  {
    cmdLineOptionMutExclList_.push_back(option);
    CmdLineParser::addCmdLineOption(option);
  }
}

void CmdLineParser::addCmdLineOption(CmdLineOption *option)
{
  if(option)
  {
    cmdLineOptionMap_[option->getCmdLine()] = option;
  }
}

CmdLineOption *CmdLineParser::getCmdLineOption(const string &key)
{
  CmdLineOptionMapIteratorType iter = cmdLineOptionMap_.find(key);
  if(iter == getCmdLineOptionMapIterEnd())
  {
    return NULL;
  }

  return iter->second;
}

CmdLineOption *CmdLineParser::getMutExclOption()
{
  for(CmdLineOptionListIteratorType iter = getCmdLineOptionListIterBegin();
      iter != getCmdLineOptionListIterEnd();
      iter++)
  {
    if((*iter)->isValueSet())
    {
      return *iter;
    }
  }

  return NULL;
}

void CmdLineParser::printUsage()
{
  cout << "\nUsage:\n"
       << "Input values:\n";

  for(CmdLineOptionMapIteratorType iter = getCmdLineOptionMapIterBegin();
      iter != getCmdLineOptionMapIterEnd();
      iter++)
  {
    CmdLineOption *option = iter->second;
    cout << "\t " << option->getCmdLine() << " " << option->getHelpText();
    if(option->isMandatory())
    {
      cout << ", Mandatory";
    }
    cout << endl;
  }

  // The mutually exclusive options, if there are any
  if(getCmdLineOptionListSize() > 0)
  {
    cout << endl;
    if(!mutExclUsageText_.empty())
    {
      cout << mutExclUsageText_ << ": ";
    }
    cout << "Mutually Exclusive options, one and only one can be set:\n\t";
    for(CmdLineOptionListIteratorType iter = getCmdLineOptionListIterBegin();
        iter != getCmdLineOptionListIterEnd();
        iter++)
    {
      cout << (*iter)->getCmdLine() << " ";
    }
    cout << endl;
  }

  // help text
  cout << "\nUse one of the following options to display this message:\n\t";
  for(list<string>::iterator iter = helpStrList_.begin(); iter != helpStrList_.end(); iter++)
  {
    cout << " " << *iter;
  }
  cout << endl;

}

bool CmdLineParser::parseCmdLine(int argc, char **argv)
{
  if(minNumberArgs_ > 0) // checking if it was set
  {
    if(argc < minNumberArgs_)
    {
      // argc = 2 would be the name of the binary and the help option
      if(argc == 2 && isHelpOption(argv[1]))
      {
        return false;
      }
      else
      {
        cerr << "Invalid number of command line arguments: " << argc
             << ", must be at least " << minNumberArgs_ << endl;
        return false;
      }
    }
  }

  if(maxNumberArgs_ > 0) // checking if it was set
  {
    if(argc > maxNumberArgs_)
    {
      cerr << "Invalid number of command line arguments: " << argc
           << ", cant be more than " << maxNumberArgs_ << endl;
      return false;
    }
  }

  // Parse each of the command line options
  for(int i = 1; i < argc; i++)
  {
     string argvStr = argv[i];
     CmdLineOption *option = getCmdLineOption(argvStr);
     if(option == NULL)
     {
       if(!isHelpOption(argvStr))
       {
         cerr << "Unrecognized argument: " << argvStr << endl;
       }
       return false;
     }

     switch(option->getOptionType())
     {
       case CmdLineOption::OPTION_FLAG:
         ((CmdLineOptionFlag*) option)->setValue(true);
         break;

       case CmdLineOption::OPTION_STR:
         ((CmdLineOptionStr*) option)->setValue(argv[++i]);
         break;

       case CmdLineOption::OPTION_INT:
         ((CmdLineOptionInt*) option)->setValue(atoi(argv[++i]));
         break;

       case CmdLineOption::OPTION_FLOAT:
         ((CmdLineOptionFloat*) option)->setValue(atof(argv[++i]));
         break;

       default:
         cerr << "Unrecognized option type: " << option->getOptionType() << ", for arg: " << argvStr << endl;
         return false;
         break;
     }
  }

  // Now check that all the mandatory args have been set
  for(CmdLineOptionMapIteratorType iter = getCmdLineOptionMapIterBegin();
      iter != getCmdLineOptionMapIterEnd();
      iter++)
  {
    CmdLineOption *option = iter->second;
    if(option->isMandatory() && !option->isValueSet())
    {
      cerr << "Mandatory option: " << option->getCmdLine() << " is not present" << endl;
      return false;
    }
  }

  // Now check that at least one, and only one of the mutually exclusive options has been set
  CmdLineOption *prevOption(NULL);
  for(CmdLineOptionListIteratorType iter = getCmdLineOptionListIterBegin();
      iter != getCmdLineOptionListIterEnd();
      iter++)
  {
    CmdLineOption *option = *iter;
    if(option->isValueSet())
    {
      if(prevOption)
      {
        cerr << "More than one mutually exclusive option set: "
             << prevOption->getCmdLine() << " and: " << option->getCmdLine()
             << endl;

        return false;
      }
      else
      {
        prevOption = option;
      }
    }
  }

  if(!prevOption)
  {
    cerr << "At least one of the mutually exclusive options must be set" << endl;
    return false;
  }

  return true;
}

