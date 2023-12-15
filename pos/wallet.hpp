class Wallet
{
private:
    int number_node = 0;      // 연결 노드 수
    int share;                // 소유 지분
    std::vector<int> *shares; // 다른 노드들의 지분 저장
public:
    void storeOtherNodeShares(int share)
    {
        shares->push_back(share);
    }

    void resetShares()
    {
        shares->clear();
        delete shares;
        shares = new std::vector<int>();
    }
    void saveShare(int share)
    {
        printf("your stake : %d\n", share);
        this->share = share;
    }
    void slashShare(int minus)
    {
        printf("slash your stake : -%d\n", minus);
        share -= minus;
        printf("your stake : %d\n", share);
    }

    int getShare()
    {
        return share;
    }
    void increaseShare()
    {
        share += 5;
        std::cout << "you gain 5 stake" << std::endl;
        std::cout << "your stake : " << share << std::endl;
    }
    void increaseNode()
    {
        number_node++;
    }
    void decreaseNode()
    {
        number_node--;
    }

    bool checkIfreceiveAllShare()
    {
        if (number_node == shares->size())
        {
            return true;
        }
        return false;
    }
    bool selectValidator()
    {
        for (int share : *shares)
        {
            if (this->share < share)
            {
                std::cout << "You(" << this->share << ") are not selected as validator\n"
                          << std::endl;
                return false;
            }
        }

        return true; // selected validator
    }

    Wallet()
    {
        shares = new std::vector<int>();
    }
    ~Wallet()
    {
        delete shares;
    }
};