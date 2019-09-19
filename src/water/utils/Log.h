#pragma once

//-------------------------------------------------------------------------------------------------
// Custom ostream manipulator to simplify colorizing

class StreamColors
{
public:
    typedef std::ostream& (*ManipPtr)(std::ostream&);

    StreamColors(ManipPtr foreground, bool bold = false) :
        foreground_(foreground),
        bold_(bold)
    {}

    void apply(std::ostream& stream) const;

protected:
    ManipPtr foreground_;
    bool bold_;
};

std::ostream& operator << (std::ostream& stream, const StreamColors& m);

//-------------------------------------------------------------------------------------------------
// Predicate to remove ANSI color codes

class SkipColorCodes
{
public:
    SkipColorCodes():
        skippingMode_(false)
    {}

    bool operator()(char& c);

private:
    bool skippingMode_;
};


// Custom plog formatter (to simplify message format)
namespace plog
{
    template<bool useUtcTime, bool removeColorCodes>
    class CustomFormatterImpl
    {
    public:
        static util::nstring header()
        {
            return util::nstring();
        }

        static util::nstring format(const Record& record)
        {
            tm t;
            (useUtcTime ? util::gmtime_s : util::localtime_s)(&t, &record.getTime().time);

            util::nostringstream ss;
            ss << t.tm_year + 1900 << "-" << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_mon + 1 << PLOG_NSTR("-") << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_mday << PLOG_NSTR(" ");
            ss << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_hour << PLOG_NSTR(":") << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_min << PLOG_NSTR(":") << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_sec << PLOG_NSTR(".") << std::setfill(PLOG_NSTR('0')) << std::setw(3) << record.getTime().millitm << PLOG_NSTR(" ");
            ss << std::setfill(PLOG_NSTR(' ')) << std::setw(5) << std::left << PLOG_NSTR("[") << severityToString(record.getSeverity()) << PLOG_NSTR("] ");
            //ss << PLOG_NSTR("[") << record.getTid() << PLOG_NSTR("] ");
            //ss << PLOG_NSTR("[") << record.getFunc() << PLOG_NSTR("@") << record.getLine() << PLOG_NSTR("] ");
            ss << record.getMessage() << PLOG_NSTR("\n");

            if (removeColorCodes)
            {
            	// As I want to have several color codes in one log line in console I have to remove color codes while writing to file.

                auto in = ss.str();
                util::nstring out;

                SkipColorCodes predicate;
                std::copy_if(in.begin(), in.end(), std::back_inserter(out), predicate);

                return out;
            }

            return ss.str();
        }
    };

    template<bool removeColorCodes>
    class CustomFormatter : public CustomFormatterImpl<false, removeColorCodes> {};

    template<bool removeColorCodes>
    class CustomFormatterUtcTime : public CustomFormatterImpl<true, removeColorCodes> {};
}
