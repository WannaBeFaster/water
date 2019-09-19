#include "../../headers.h"

#include "Log.h"

// I wanted to use ColorConsoleAppender and to be able to colorize some substrings inside the message differently as well.
// Certainly, all the color codes must be eliminated from the file logging.

namespace termcolor
{
    // Forward declaration of the `_internal` namespace.
    // All comments are below.
    namespace _internal
    {
        // An index to be used to access a private storage of I/O streams. See
        // colorize / nocolorize I/O manipulators for details.

        // it must be put out of the header to avoid more than one calls to xalloc.
        int colorize_index = std::ios_base::xalloc();
    }
}

//-------------------------------------------------------------------------------------------------
std::ostream& operator << (std::ostream& stream, const StreamColors& m)
{
    m.apply(stream);
    return stream;
}

//-------------------------------------------------------------------------------------------------
bool SkipColorCodes::operator()(char& c)
{
    if (c == '\x1B')
    {
        skippingMode_ = true;
        return false;
    }

    if (skippingMode_ && c == 'm')
    {
        skippingMode_ = false;
        return false;
    }

    if (skippingMode_)
        return false;

    return true;
}

//-------------------------------------------------------------------------------------------------
void StreamColors::apply(std::ostream& stream) const
{
    stream << termcolor::colorize << foreground_;
    if (bold_)
        stream << termcolor::bold;
}
