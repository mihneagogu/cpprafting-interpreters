#include <vector>


class Scanner {
    private:
        const char *src;

    public:
        Scanner(const char *content) {
            this->src = content;
        }

        std::vector<int> scan_tokens() {
            std::vector<int> res;
            return res;
        }
};
