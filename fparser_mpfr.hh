/***************************************************************************\
|* Function Parser for C++ v5.0.0                                          *|
|*-------------------------------------------------------------------------*|
|* Copyright: Juha Nieminen                                                *|
\***************************************************************************/

#ifndef ONCE_FPARSER_MPFR_H_
#define ONCE_FPARSER_MPFR_H_

#include "fparser.hh"
#include "mpfr/MpfrFloat.hh"

class FunctionParser_mpfr: public FunctionParserBase<MpfrFloat> {};

#endif
