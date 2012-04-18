
#include <list>
#include <map>
#include <string>

//--------------------------------------------------
// Command line options
//--------------------------------------------------

using namespace std;

class CmdLineOption
{
public:
  // Constructors defined below as protected and private
  ~CmdLineOption();

  enum OPTION_TYPE
  {
    OPTION_UNKNOWN = 0,
    OPTION_FLAG,
    OPTION_STR,
    OPTION_INT,
    OPTION_FLOAT
  };

  // Getters/Setters
  inline void setCmdLine(const string &cmdLine) { cmdLine_ = cmdLine; }
  inline string getCmdLine() const { return cmdLine_; }

  inline void setHelpText(const string &helpText) { helpText_ = helpText; }
  inline string getHelpText() const { return helpText_; }

  inline void setMandatory(bool m) { isMandatory_ = m; }
  inline bool isMandatory() const { return isMandatory_; }

  inline OPTION_TYPE getOptionType() const { return optionType_; }
  inline bool isValueSet() const { return isValueSet_; }

protected:
  // Can only be instantiated by derived classes
  CmdLineOption(const string &cmdLine, const string &helpText, bool isMandatory, OPTION_TYPE type) :
      cmdLine_(cmdLine), helpText_(helpText), isMandatory_(isMandatory), isValueSet_(false), optionType_(type) {}

  string cmdLine_;
  string helpText_;
  bool isMandatory_;
  bool isValueSet_;
  OPTION_TYPE optionType_;

private:
  CmdLineOption(); // Cant call the default version
};

//--------------------------------------------------
// Value Classes, maybe these should be templatized
//--------------------------------------------------

class CmdLineOptionStr : public CmdLineOption
{
public:
  CmdLineOptionStr(const string &cmdLine, const string &helpText, const string &defaultValue) :
      CmdLineOption(cmdLine, helpText, false, OPTION_STR) { setValue(defaultValue); }
  CmdLineOptionStr(const string &cmdLine, const string &helpText, bool isMandatory=false) :
      CmdLineOption(cmdLine, helpText, isMandatory, OPTION_STR) {}
  ~CmdLineOptionStr();
  inline string getValue() const { return valueStr_; }
  inline void setValue(const string &value) { valueStr_ = value; isValueSet_ = true; }

private:
  CmdLineOptionStr(); // Cant call the default version
  string valueStr_;
};

//--------------------------------------------------

class CmdLineOptionInt : public CmdLineOption
{
public:
  CmdLineOptionInt(const string &cmdLine, const string &helpText, int defaultValue) :
      CmdLineOption(cmdLine, helpText, false, OPTION_INT) { setValue(defaultValue); }
  CmdLineOptionInt(const string &cmdLine, const string &helpText, bool isMandatory=false) :
      CmdLineOption(cmdLine, helpText, isMandatory, OPTION_INT) {}
  ~CmdLineOptionInt();
  inline int getValue() const { return valueInt_; }
  inline void setValue(int value) { valueInt_ = value; isValueSet_ = true; }

private:
  CmdLineOptionInt(); // Cant call the default version
  int valueInt_;
};

//--------------------------------------------------

class CmdLineOptionFloat : public CmdLineOption
{
public:
  CmdLineOptionFloat(const string &cmdLine, const string &helpText, float defaultValue) :
      CmdLineOption(cmdLine, helpText, false, OPTION_FLOAT) { setValue(defaultValue); }
  CmdLineOptionFloat(const string &cmdLine, const string &helpText, bool isMandatory=false) :
      CmdLineOption(cmdLine, helpText, isMandatory, OPTION_FLOAT) {}
  ~CmdLineOptionFloat();
  inline float getValue() const { return valueFloat_; }
  inline void setValue(float value) { valueFloat_ = value; isValueSet_ = true; }

private:
  CmdLineOptionFloat(); // Cant call the default version
  float valueFloat_;
};

//--------------------------------------------------

class CmdLineOptionFlag : public CmdLineOption
{
public:
  CmdLineOptionFlag(const string &cmdLine, const string &helpText, bool isMandatory=false, int valueKey=0) :
      CmdLineOption(cmdLine, helpText, isMandatory, OPTION_FLAG), valueKey_(valueKey) {}
  ~CmdLineOptionFlag();
  inline bool getValue() const { return isValueSet_; }
  inline int getValueKey() const { return valueKey_; }
  inline void setValue(bool value) { isValueSet_ = true; }

private:
  CmdLineOptionFlag(); // Cant call the default version
  // Value key useful when using Mutually Exclusive CmdLineOptionFlags
  // You can call CmdLineParser.getMutExclOption()->getValueKey();
  int valueKey_;
};


//--------------------------------------------------
// Command line Parser
//--------------------------------------------------

class CmdLineParser
{
public:
  CmdLineParser();
  ~CmdLineParser();

  void addCmdLineOption(CmdLineOption *clo);
  CmdLineOption *getCmdLineOption(const string &key);

  // mutually exclusive options, only one of these can be set
  void addMutExclCmdLineOption(CmdLineOption *clo);
  // Get the mutually exclusive option that was set, if any
  CmdLineOption *getMutExclOption();

  // Additional help text displayed first
  inline void setMainHelpText(const string &text) { mainHelpText_ = text; }
  inline string &getMainHelpText() { return mainHelpText_; }

  // Additional help text displayed at the end, after the options
  inline void setMainHelpTextEnd(const string &text) { mainHelpTextEnd_ = text; }
  inline string &getMainHelpTextEnd() { return mainHelpTextEnd_; }

  inline void setMutExclUsageText(const string &text) { mutExclUsageText_ = text; }
  inline string &getMutExclUsageText() { return mutExclUsageText_; }

  inline void setMinNumberArgs(uint16_t min) { minNumberArgs_ = min; }
  inline uint16_t getMinNumberArgs() { return minNumberArgs_; }

  inline void setMaxNumberArgs(uint16_t max) { maxNumberArgs_ = max; }
  inline uint16_t getMaxNumberArgs() { return maxNumberArgs_; }

  bool parseCmdLine(int argc, char **argv);
  void printUsage();

  // Option map methods
  typedef map<string, CmdLineOption*> CmdLineOptionMapType;
  typedef CmdLineOptionMapType::iterator CmdLineOptionMapIteratorType;
  inline CmdLineOptionMapIteratorType getCmdLineOptionMapIterBegin() { return cmdLineOptionMap_.begin(); }
  inline CmdLineOptionMapIteratorType getCmdLineOptionMapIterEnd()   { return cmdLineOptionMap_.end(); }
  inline int getCmdLineOptionMapSize() const { return cmdLineOptionMap_.size(); }

  // Mutually Exclusive list methods
  typedef list<CmdLineOption*> CmdLineOptionListType;
  typedef CmdLineOptionListType::iterator CmdLineOptionListIteratorType;
  inline CmdLineOptionListIteratorType getCmdLineOptionListIterBegin() { return cmdLineOptionMutExclList_.begin(); }
  inline CmdLineOptionListIteratorType getCmdLineOptionListIterEnd()   { return cmdLineOptionMutExclList_.end(); }
  inline int getCmdLineOptionListSize() const { return cmdLineOptionMutExclList_.size(); }

private:
  bool isHelpOption(const string &str);
  CmdLineOptionMapType cmdLineOptionMap_;
  // TODO it would be nice to have groups of mut excl options,
  //       keyed by an int id: map<int, CmdLineOptionListType>
  CmdLineOptionListType cmdLineOptionMutExclList_;
  list<string> helpStrList_;
  uint16_t minNumberArgs_;
  uint16_t maxNumberArgs_;
  string mutExclUsageText_;
  string mainHelpText_;
  string mainHelpTextEnd_;
};

