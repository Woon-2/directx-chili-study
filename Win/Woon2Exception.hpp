#ifndef __Woon2Exception
#define __Woon2Exception

#include <string>
#include <exception>

template < class CharT, class Traits = std::char_traits<CharT>,
    class Allocator = std::allocator<CharT> >
class Woon2Exception : public std::exception
{
public:
    using String = std::basic_string<CharT, Traits, Allocator>;

    Woon2Exception( int lineNum, const CharT* fileStr ) noexcept;
    const CharT* what() const noexcept override;
    virtual const CharT* getType() const noexcept;
    int getLineNum() const noexcept;
    const String& getFileStr() const noexcept;
    String getMetaStr() const noexcept;

private:
    int lineNum_;
    String fileStr_;
protected:
    mutable String whatBuffer_;
};

#endif  // __Woon2Exception