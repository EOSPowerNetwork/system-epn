#include <string_view>

// Do not modify once contract is live
namespace fixedProps
{
    namespace memo
    {
        using std::string_view;

        constexpr string_view memoDelimiter = " -- ";
        constexpr string_view marketingMemo = "[EOS Power Network] ";

        constexpr uint16_t totalMemoBytes = 256;
        constexpr uint16_t marketingMemoBytes = 20;
        constexpr uint16_t delimiterBytes = 4;
        constexpr uint16_t remainingBytes = totalMemoBytes - marketingMemoBytes - delimiterBytes;
        constexpr uint16_t memoSize = remainingBytes / 2;  // 116 bytes for each memo

        // Marketing memo and memo delimiter must have fixed size.
        // If they were to grow after an org/signer established their memos, the total size may exceed the 256 byte memo limit
        static_assert(memoDelimiter.size() == delimiterBytes);
        static_assert(marketingMemo.size() == marketingMemoBytes);
        static_assert(memoDelimiter.size() + marketingMemo.size() + 2 * memoSize == totalMemoBytes);
    }  // namespace memo

};  // namespace fixedProps
