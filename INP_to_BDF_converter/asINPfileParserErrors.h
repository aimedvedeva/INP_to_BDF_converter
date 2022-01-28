#ifndef ASINPFILEPARSERERRORS
#define ASINPFILEPARSERERRORS

typedef enum
{
  INPfileParserErrors_RIGHT_FILE,
  INPfileParserErrors_FILE_WITH_WRONG_ELEMENTS,
  INPfileParserErrors_INCLUDE_OR_INSTANCE_FILE
}INPfileParserErrors;

bool isErrorExisted();
void SetINPfileParsingError(INPfileParserErrors &error);
INPfileParserErrors GetLastParserError();

#endif /*ASINPFILEPARSERERRORS*/