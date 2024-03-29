#ifndef __Woon2Exception
#define __Woon2Exception

#include <string>
#include <exception>

class Woon2Exception : public std::exception
{
public:
    Woon2Exception( int lineNum, const char* fileStr ) noexcept;
    const char* what() const noexcept override;
    virtual const char* type() const noexcept;
    int lineNum() const noexcept;
    const std::string& fileStr() const noexcept;
    std::string metaStr() const noexcept;

private:
    int lineNum_;
    std::string fileStr_;
protected:
    mutable std::string whatBuffer_;
};

#endif  // __Woon2Exception