// Transaction 클래스에 서명 및 검증 로직 추가
class Transaction
{

private:
    std::string sender;
    std::string recipient;
    int amount;
    std::string data;
    // std::string signature; // 서명 추가

    friend class boost::serialization::access;

    template <class Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
        ar & sender;
        ar & recipient;
        ar & amount;
        ar & data;
        // ar & signature;
    }

public:
    std::string getData()
    {
        return data;
    }
    void setData(std::string data)
    {
        this->data = data;
    }
    Transaction(const std::string &sender, const std::string &recipient, int amount)
        // Transaction( std::string& sender,  std::string& recipient, int amount)
        : sender(sender), recipient(recipient), amount(amount), data("Transaction")
    {
        // signTransaction(); // 트랜잭션 생성 시 자동으로 서명
    }

    Transaction()
    {
        // signTransaction(); // 트랜잭션 생성 시 자동으로 서명
    }

    // // 서명 생성 메서드
    // void signTransaction() {
    //     std::string message = sender + recipient + std::to_string(amount);
    //     // OpenSSL을 사용하여 서명 생성
    //     signature = generateSignature(message);
    // }

    // // 서명 생성 메서드
    // std::string generateSignature(const std::string& message) {
    //     EVP_PKEY* privateKey; // 개인키 생성
    //     // 개인키 생성 및 초기화 (실제로는 적절한 키 생성 및 관리 과정이 필요)
    //     // ...

    //     EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    //     EVP_MD_CTX_init(ctx);

    //     // 서명 초기화
    //     EVP_SignInit_ex(ctx, EVP_sha256(), nullptr);
    //     // 서명 데이터에 대한 업데이트
    //     EVP_SignUpdate(ctx, reinterpret_cast<const unsigned char*>(message.c_str()), message.size());

    //     unsigned int sigLen;
    //     unsigned char* signatureBuffer = new unsigned char[EVP_PKEY_size(privateKey)];

    //     // 서명 생성
    //     EVP_SignFinal(ctx, signatureBuffer, &sigLen, privateKey);

    //     EVP_MD_CTX_free(ctx);

    //     std::stringstream ss;
    //     for (unsigned int i = 0; i < sigLen; ++i) {
    //         ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(signatureBuffer[i]);
    //     }
    //     delete[] signatureBuffer;

    //     return ss.str();
    // }

    // // 서명 검증 메서드
    // bool verifySignature() {
    //     EVP_PKEY* publicKey; // 공개키 생성
    //     // 공개키 생성 및 초기화 (실제로는 적절한 키 생성 및 관리 과정이 필요)
    //     // ...

    //     EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    //     EVP_MD_CTX_init(ctx);

    //     // 서명 초기화
    //     EVP_VerifyInit_ex(ctx, EVP_sha256(), nullptr);
    //     // 서명 데이터에 대한 업데이트
    //     EVP_VerifyUpdate(ctx, reinterpret_cast<const unsigned char*>(data.c_str()), data.size());

    //     // 서명 검증
    //     int result = EVP_VerifyFinal(ctx, reinterpret_cast<const unsigned char*>(signature.c_str()), signature.size(), publicKey);

    //     EVP_MD_CTX_free(ctx);

    //     return (result == 1);
    // }
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
};

// class TransactionProcessor {// 실제 은행이랑 연결된다고 가정
// private:
//     std::unordered_map<std::string, Wallet> wallets;
//     Blockchain blockchain;

// public:
//     void addWallet(const std::string& address, int balance) {
//         wallets.emplace(address, Wallet(address, balance));
//     }

//     Wallet* getWallet(const std::string& address) {
//         auto it = wallets.find(address);
//         if (it != wallets.end()) {
//             return &it->second;
//         }
//         return nullptr;
//     }

//     // bool transfer(const std::string& sender, const std::string& recipient, int amount) {
//     //     Wallet* senderWallet = getWallet(sender);
//     //     Wallet* recipientWallet = getWallet(recipient);

//     //     if (senderWallet == nullptr || recipientWallet == nullptr || senderWallet->balance < amount) {
//     //         return false; // Invalid transaction or insufficient balance
//     //     }

//     //     Transaction transaction(sender, recipient, amount);
//     //     Block newBlock(blockchain.getLatestBlock().index + 1, blockchain.getLatestBlock().hash, transaction.getData());
//     //     blockchain.addBlock(newBlock);

//     //     senderWallet->balance -= amount;
//     //     recipientWallet->balance += amount;

//     //     std::cout << "Transaction Successful: " << sender << " transferred " << amount << " to " << recipient << std::endl;

//     //     return true;
//     // }
// };

class Transactions
{
private:
    std::vector<Transaction> *transactions;

public:
    Transactions()
    {
        transactions = new std::vector<Transaction>();
    }

    ~Transactions()
    {
        delete transactions;
    }

    void push_back(const Transaction &transaction)
    {
        transactions->push_back(transaction);
    }

    void resetTransactions()
    {
        transactions->clear();
        delete transactions;
        transactions = new std::vector<Transaction>();
    }

    const std::vector<Transaction> *getTransactions() const
    { // Getter 함수 추가
        return transactions;
    }
};