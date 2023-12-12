# BlockChain_POS
- Simple  consensus algorithm ( Proof Of Stake) for blockchain

- Test OS Environment : Ubuntu 22.04.3 LTS 

- Installation : `g++`, `OpenSSL 3.0.2 15 Mar 2022`, `Boost Library`

```
jinsu@LAPTOP-UQUT2CSB:~/iot/blockchain/blockchain$ g++ --version
g++ (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0
Copyright (C) 2021 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```

- Ubuntu 22.04 버전에 기본으로 설치된 openSSL 3.0.2 버전을 사용했습니다.

```
jinsu@LAPTOP-UQUT2CSB:~/protection$ openssl version
OpenSSL 3.0.2 15 Mar 2022 (Library: OpenSSL 3.0.2 15 Mar 2022)
```

- 비동기 통신을 위해서 Boost.Asio를 사용했습니다.

- [Boost.Asio Overview](https://www.boost.org/doc/libs/1_81_0/doc/html/boost_asio/overview.html)

- [boost_1_81_0.tar.gz download](https://boostorg.jfrog.io/artifactory/main/release/1.81.0/source/boost_1_81_0.tar.gz)

## 실행 방법

- Compile을 위해 make파일을 실행합니다.

- 작업환경의 한계로 모든 노드들이 같은 local host 환경에서 테스트 되었습니다.

- 반드시 Seed Node를 먼저 실행시켜야 합니다.

- seed node는 처음 연결을 시도하는 node에게 peer node들의 ip와 port를 제공해주는 역할 이후에는 일반 노드와 같은 작업을 수행합니다.

- local에서 테스트 하다보니 아주 가끔씩 포트번호 겹침으로 인한 bind error가 발생합니다. 그럴 경우 모든 노드들을 끄고 다시 실행하면 됩니다.

- SeedNode(1개만 실행)

```
./p2p_seed <Port> <Stake>
```

- Node(2개 실행)

```
./p2p <SeedNode Ip> <SeedNode Port> <Stake>
```