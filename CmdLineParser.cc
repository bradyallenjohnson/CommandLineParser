
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
  // The same Option is stored in both the cmdLineOptionMap_ and
  // the cmdLineOptionMutExclList_, so we need to delete them
  // from one and just empty the other
  CmdLineOptionMapIteratorType iter;
  for(iter = cmdLineOptionMap_.begin(); iter != cmdLineOptionMap_.end(); ++iter)
  {
    CmdLineOption *option(iter->second);
    delete option;
  }

  while(!cmdLineOptionMutExclList_.empty())
  {
    cmdLineOptionMutExclList_.pop_front();
  }
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
    // Add the cmd line aliases too
    if(option->hasCmdLineAlias())
    {
        for(CmdLineOption::CmdLineAliasListType::const_iterator iter = option->getCmdLineAliasIterBegin();
            iter != option->getCmdLineAliasIterEnd();
            ++iter)
        {
            cmdLineOptionMap_[*iter] = option;
        }
    }
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
  if(!mainHelpText_.empty())
  {
    cout << mainHelpText_ << "\n";
  }

  cout << "\nUsage:\n"
       << "Input values:\n";

  for(CmdLineOptionMapIteratorType iter = getCmdLineOptionMapIterBegin();
      iter != getCmdLineOptionMapIterEnd();
      iter++)
  {
    string key = iter->first;
    CmdLineOption *option = iter->second;
    if(key != option->getCmdLine())
    {
        // If there were alot of aliases, this may not be very efficient, but it should be ok
        // its an alias, which will be displayed with the main cmd line text
        continue;
    }

    cout << "\t " << option->getCmdLine();
    // If it has aliases, then print those too
    if(option->hasCmdLineAlias())
    {
      for(CmdLineOption::CmdLineAliasListType::const_iterator iter = option->getCmdLineAliasIterBegin();
          iter != option->getCmdLineAliasIterEnd();
          ++iter)
      {
        cout << ", " << *iter;
      }
    }

    // Display the help text
    cout << ": " << option->getHelpText();

    // Show if its a mandatory option
    if(option->isMandatory())
    {
      cout << ", Mandatory";
    }
    else
    {
      // Display the default value
      if(option->isValueSet())
      {
        cout << " (Default Value: ";
        if(option->getOptionType() == CmdLineOption::OPTION_STR)
        {
          cout << ((CmdLineOptionStr*) option)->getValue();
        }
        if(option->getOptionType() == CmdLineOption::OPTION_INT)
        {
          cout << ((CmdLineOptionInt*) option)->getValue();
        }
        if(option->getOptionType() == CmdLineOption::OPTION_FLOAT)
        {
          cout << ((CmdLineOptionFloat*) option)->getValue();
        }
        cout << ")";
      }
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

  if(!mainHelpTextEnd_.empty())
  {
    cout << endl << mainHelpTextEnd_ << endl;
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
  if(getCmdLineOptionListSize() > 0)
  {
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
  }

  return true;
}

