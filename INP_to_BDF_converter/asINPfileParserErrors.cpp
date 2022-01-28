#include "asINPfileParserErrors.h"
static INPfileParserErrors CurrentParserError;

bool isErrorExisted()
{
  if (CurrentParserError == INPfileParserErrors_FILE_WITH_WRONG_ELEMENTS || CurrentParserError == INPfileParserErrors_INCLUDE_OR_INSTANCE_FILE)
  {
    return true;
  }
  return false;
}

INPfileParserErrors GetLastParserError()
{
  return CurrentParserError;
}

void SetINPfileParsingError(INPfileParserErrors &error)
{
  CurrentParserError = error;
}