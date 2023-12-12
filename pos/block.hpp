class Block
{
public:
    std::string serializeToString() const
    {
        std::ostringstream oss;
        boost::archive::text_oarchive oa(oss);
        oa << *this;
        return oss.str();
    }
    void deserializeFromString(const std::string &serializedData)
    {
        std::istringstream iss(serializedData);
        boost::archive::text_iarchive ia(iss);
        ia >> *this;
    }

    std::string getHash()
    {
        return hash;
    }

    Block(std::string previousHash, std::string data)
        : previousHash(previousHash), data(data)
    {
        timestamp = getCurrentTimestamp();
        hash = calculateHash();
    }

    Block()
    {
        timestamp = "";
        previousHash = "0";
        data = "Genesis Block";
        hash = calculateHash();
        //  Block( "0", "Genesis Block")
    }
    void modifyData(std::string modified)
    { // chain 에서 반환 함수가 const면 이것도 const로 바꿔줘야함
        data = modified;
    }
    std::string getData()
    { // chain 에서 반환 함수가 const면 이것도 const로 바꿔줘야함
        return data;
    }
    std::string getPreviousHash()
    { // chain 에서 반환 함수가 const면 이것도 const로 바꿔줘야함
        return previousHash;
    }

    void printBlockData()
    {
        std::cout << "Updated Block" << std::endl;
        std::cout << "Hash: " << hash << std::endl;
        std::cout << "Previous Hash: " << previousHash << std::endl;
        std::cout << "Timestamp: " << timestamp << std::endl;
        std::cout << data << std::endl;
    }

    std::string calculateHash() const
    {
        std::stringstream ss;
        ss << previousHash << timestamp << data; // 모든 데이터 모아 해쉬함수에 입력

        EVP_MD_CTX *mdctx;
        const EVP_MD *md;
        unsigned char hash[SHA256_DIGEST_LENGTH];
        unsigned int md_len;

        // 해시 함수 계산
        md = EVP_sha256();                                            // sha-256 초기화
        mdctx = EVP_MD_CTX_new();                                     // message digest context 생성
        EVP_DigestInit_ex(mdctx, md, NULL);                           // hash contest 초기화
        EVP_DigestUpdate(mdctx, ss.str().c_str(), ss.str().length()); // 데이터 업데이트
        EVP_DigestFinal_ex(mdctx, hash, &md_len);                     // hash 완성
        EVP_MD_CTX_free(mdctx);

        std::stringstream hashStream;
        for (int i = 0; i < md_len; ++i)
        {
            hashStream << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
        }

        return hashStream.str();
    }

    std::string getCurrentTimestamp() const
    {
        auto now = std::time(nullptr);
        return std::to_string(now);
    }

private:
    // int index;
    std::string previousHash;
    std::string timestamp;
    std::string data;
    std::string hash;
    friend class boost::serialization::access;

    template <class Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        // ar & index;
        ar & previousHash;
        ar & timestamp;
        ar & data;
        ar & hash;
    }
};

class Blockchain
{
private:
    std::vector<Block> chain;

public:
    Blockchain()
    {
        // Genesis Block
        chain.emplace_back(Block());
    }

    // const Block& getLatestBlock() const { //const로 하면 반환 block 변경x
    Block &getLatestBlock()
    { // const로 하면 반환 block 변경x
        return chain.back();
    }

    void addBlock(const Block &newBlock)
    {
        chain.push_back(newBlock);
    }
    void deleteBlock()
    {
        chain.pop_back();
    }

    const std::vector<Block> &getChain() const
    { // Getter 함수 추가
        return chain;
    }

    bool isChainValid()
    {
        if (chain.size() > 1)
            for (size_t i = 1; i < chain.size(); i++)
            {
                // printf("for: %ld\nchain size: %ld\n",i,chain.size());
                // std::cout<<chain[i].getPreviousHash()<<std::endl;
                if (chain[i].getPreviousHash() != chain[i - 1].calculateHash())
                { // calculate = hash?
                    std::cout << "this data is modified : " << chain[i - 1].getData() << std::endl;
                    return false;
                }
            }
        return true;
    }
};